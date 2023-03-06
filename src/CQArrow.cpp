#include <CQArrow.h>
#include <CMathGeom2D.h>

#include <QPainter>
#include <QPainterPath>
#include <cmath>

namespace {

double Deg2Rad(double d) { return M_PI*d/180.0; }
//double Rad2Deg(double d) { return 180.0*d/M_PI; }

void headAngles(const CQArrow::HeadType &type, double &angle, double &backAngle,
                bool &lineEnds) {
  lineEnds = false;

  if      (type == CQArrow::HeadType::TRIANGLE) { angle = 30; backAngle =  90; }
  else if (type == CQArrow::HeadType::STEALTH ) { angle = 30; backAngle =  45; }
  else if (type == CQArrow::HeadType::DIAMOND ) { angle = 45; backAngle = 135; }
  else if (type == CQArrow::HeadType::LINE    ) { lineEnds = true; }
  else if (type == CQArrow::HeadType::NONE    ) { }
}

QPointF
movePointOnLine(const QPointF &p, const CQArrow::Angle &a, double d)
{
  return QPointF(p.x() + d*a.cos, p.y() + d*a.sin);
}

QPointF
movePointPerpLine(const QPointF &p, const CQArrow::Angle &a, double d)
{
  return QPointF(p.x() + d*a.sin, p.y() - d*a.cos);
}

void
addWidthToPoint(const QPointF &p, const CQArrow::Angle &a, double lw, QPointF &p1, QPointF &p2)
{
  double dx = lw*a.sin/2.0;
  double dy = lw*a.cos/2.0;

  p1 = QPointF(p.x() - dx, p.y() + dy);
  p2 = QPointF(p.x() + dx, p.y() - dy);
}

bool
intersectLine(const QPointF &l1s, const QPointF &l1e, const QPointF &l2s, const QPointF &l2e,
              QPointF &pi, bool &inside)
{
  double xi, yi, mu1, mu2;

  bool rc = CMathGeom2D::IntersectLine(l1s.x(), l1s.y(), l1e.x(), l1e.y(),
                                       l2s.x(), l2s.y(), l2e.x(), l2e.y(),
                                       &xi, &yi, &mu1, &mu2);

  inside = (mu1 >= 0.0 && mu1 <= 1.0);

  pi = QPointF(xi, yi);

  return rc;
}

}

//------

CQArrow::
CQArrow(const QPointF &from, const QPointF &to) :
 from_(from), to_(to)
{
  setTHead(true);
}

void
CQArrow::
setFrontType(const HeadType &type)
{
  frontData_.type = type;

  headAngles(type, frontData_.angle, frontData_.backAngle, frontData_.lineEnds);
}

void
CQArrow::
setTailType(const HeadType &type)
{
  tailData_.type = type;

  headAngles(type, tailData_.angle, tailData_.backAngle, tailData_.lineEnds);
}

void
CQArrow::
setMidType(const HeadType &type)
{
  midData_.type = type;

  headAngles(type, midData_.angle, midData_.backAngle, midData_.lineEnds);
}

//---

void
CQArrow::
draw(QPainter *painter, CQArrowDevice *device)
{
  drawData_.reset();

  //---

#ifdef DEBUG_LABELS
  PointLabels pointLabels;

  auto addPointLabel = [&](const QPointF &point, const QString &text, bool above=true) {
    pointLabels.push_back(PointLabel(point, text, above));
  };
#endif

  //---

  painter_ = painter;
  device_  = device;

  //---

  auto from = from_;
  auto to   = (isRelative() ? from_ + to_ : to_);

  bool swapped = false;

  if (from.x() > to.x()) {
    std::swap(from, to);

    swapped = true;
  }

  //---

  // convert start/end point to pixel start/end point
  auto startPoint = windowToPixel(from);
  auto endPoint   = windowToPixel(to  );

#ifdef DEBUG_LABELS
  if (isDebugLabels()) {
    addPointLabel(startPoint, "startPoint", /*above*/false);
    addPointLabel(endPoint  , "endPoint"  , /*above*/true );
  }
#endif

  //---

  bool linePoly = (lineWidth() > 0);

  // convert line width, front/tail arrow length to pixel
  double linePixelWidth  = 0.0;
//double linePixelHeight = 0.0;

  if (linePoly) {
    linePixelWidth  = windowWidthToPixelWidth  (lineWidth());
  //linePixelHeight = windowHeightToPixelHeight(lineWidth());
  }

  // calc stroke width
  auto strokeWidth = (this->strokeWidth() >= 0 ? this->strokeWidth() : 4.0);

  //---

  GenHeadData frontData, midData, tailData;

  // set arrow len
  frontData.len = (frontLength() > 0 ? windowWidthToPixelWidth(frontLength()) : 8);
  midData  .len = (midLength  () > 0 ? windowWidthToPixelWidth(midLength  ()) : 8);
  tailData .len = (tailLength () > 0 ? windowWidthToPixelWidth(tailLength ()) : 8);

  // calc line angle (radians)
  Angle lineAngle(startPoint, endPoint);

  // calc front/tail arrow angles (radians)
  frontData.angle = Angle(Deg2Rad(frontAngle() > 0 ? frontAngle() : 45));
  midData  .angle = Angle(Deg2Rad(midAngle  () > 0 ? midAngle  () : 45));
  tailData .angle = Angle(Deg2Rad(tailAngle () > 0 ? tailAngle () : 45));

  // calc front/tail arrow back angles (radians)
  frontData.backAngle = Angle(Deg2Rad(frontBackAngle() > 0 ? frontBackAngle() : 90));
  midData  .backAngle = Angle(Deg2Rad(midBackAngle  () > 0 ? midBackAngle  () : 90));
  tailData .backAngle = Angle(Deg2Rad(tailBackAngle () > 0 ? tailBackAngle () : 90));

  // calc front/tail arrow length along line
  frontData.lineLen = frontData.len*frontData.angle.cos;
  midData  .lineLen = midData  .len*midData  .angle.cos;
  tailData .lineLen = tailData .len*tailData .angle.cos;

  //---

  midData.headMid =
    QPointF((startPoint.x() + endPoint.x())/2.0, (startPoint.y() + endPoint.y())/2.0);

  // calc front/tail arrow end point (along line)
  auto startPointI = movePointOnLine(startPoint, lineAngle,  frontData.lineLen);
  auto endPointI   = movePointOnLine(endPoint  , lineAngle, -tailData .lineLen);

  auto headMidL = movePointOnLine(midData.headMid, lineAngle, -midData.lineLen/2.0);
  auto headMidR = movePointOnLine(midData.headMid, lineAngle,  midData.lineLen/2.0);

#ifdef DEBUG_LABELS
  if (isDebugLabels()) {
    addPointLabel(startPointI, "startPointI", /*above*/false);
    addPointLabel(endPointI  , "endPointI"  , /*above*/true );

    addPointLabel(midData.headMid, "midData.headMid", /*above*/true );
    addPointLabel(headMidL, "headMidL", /*above*/true );
    addPointLabel(headMidR, "headMidR", /*above*/false);
  }
#endif

  //---

  frontData.isLineEnds = (this->isFrontLineEnds() && isFHead  ());
  midData  .isLineEnds = (this->isMidLineEnds  () && isMidHead());
  tailData .isLineEnds = (this->isTailLineEnds () && isTHead  ());

  frontData.isPoints = (! this->isFrontLineEnds() && isFHead());
  midData  .isPoints = (! this->isMidLineEnds  () && isMidHead());
  tailData .isPoints = (! this->isTailLineEnds () && isTHead());

  //---

  // calc front/tail head mid point (on line)
  frontData.headMid = startPoint;
  tailData .headMid = endPoint;

  if (isFHead()) {
    if (! frontData.isLineEnds)
      frontData.headMid = movePointOnLine(startPoint, lineAngle, strokeWidth);
  }

  if (isTHead()) {
    if (! tailData.isLineEnds)
      tailData.headMid = movePointOnLine(endPoint, lineAngle, -strokeWidth);
  }

  //---

  // create polygon for arrow shape if has width
  QPointF headMidR1, headMidR2;

  if (linePoly) {
    // calc front head mid point offset by line width
    addWidthToPoint(frontData.headMid, lineAngle, linePixelWidth,
                    frontData.headMid1, frontData.headMid2);

    // calc tail head mid point offset by line width
    addWidthToPoint(tailData.headMid, lineAngle, linePixelWidth,
                    tailData.headMid1, tailData.headMid2);

    // calc mid head mid point offset by line width
    addWidthToPoint(midData.headMid, lineAngle, linePixelWidth,
                    midData.headMid1, midData.headMid2);

    if (! swapped)
      addWidthToPoint(headMidR, lineAngle, linePixelWidth, headMidR1, headMidR2);
    else
      addWidthToPoint(headMidL, lineAngle, linePixelWidth, headMidR1, headMidR2);
  }

  //---

  if (isFHead())
    calcHeadPolyData(startPoint, startPointI, lineAngle, linePoly,
                     linePixelWidth, frontData, tailData);

  if (isTHead())
    calcTailPolyData(endPoint, endPointI, lineAngle, linePoly,
                     linePixelWidth, frontData, tailData);

  if (isMidHead())
    calcMidPolyData(startPoint, endPoint, lineAngle, linePoly, swapped,
                    linePixelWidth, headMidR1, headMidR2,
                    frontData, tailData, midData);

  //---

  if (frontData.isLineEnds && ! linePoly) {
    drawLine(startPoint, frontData.tipPoint1, strokeWidth);
    drawLine(startPoint, frontData.tipPoint2, strokeWidth);
  }

  if (tailData.isLineEnds && ! linePoly) {
    drawLine(endPoint, tailData.tipPoint1, strokeWidth);
    drawLine(endPoint, tailData.tipPoint2, strokeWidth);
  }

  if (midData.isLineEnds && ! linePoly) {
    drawLine(midData.headMid, midData.tipPoint1, strokeWidth);
    drawLine(midData.headMid, midData.tipPoint2, strokeWidth);
  }

  //---

  // draw line (no line width)
  if (! linePoly) {
    // draw line (no line width)
    drawData_.midLine = Line(frontData.headMid, tailData.headMid);

    drawLine(frontData.headMid, tailData.headMid, strokeWidth);
  }

  //---

  // draw arrows (no line width and not line ends)
  if (isFHead() && ! frontData.isLineEnds && ! linePoly)
    drawPolygon(frontData.headPoints1, strokeWidth, isFilled(), isStroked());

  if (isTHead() && ! tailData.isLineEnds && ! linePoly)
    drawPolygon(tailData.headPoints1, strokeWidth, isFilled(), isStroked());

  if (isMidHead() && ! midData.isLineEnds && ! linePoly)
    drawPolygon(midData.headPoints1, strokeWidth, isFilled(), isStroked());

  //---

  // update head and tail (non line) polygon for arrow shape with line width
  if (linePoly) {
    if (! frontData.headPoints1.empty() && ! frontData.isLineEnds)
      updateFrontLinePoly(frontData, tailData);

    if (! tailData.headPoints1.empty() && ! tailData.isLineEnds)
      updateTailLinePoly(frontData, tailData);

    if (! midData.headPoints1.empty() && ! midData.isLineEnds)
      updateMidLinePoly(midData, frontData, tailData);

    //---

#ifdef DEBUG_LABELS
    if (isDebugLabels()) {
      addPointLabel(frontData.headMid1, "frontData.headMid1", /*above*/false);
      addPointLabel(frontData.headMid2, "frontData.headMid2", /*above*/true );

      addPointLabel(tailData.headMid1, "tailData.headMid1", /*above*/false);
      addPointLabel(tailData.headMid2, "tailData.headMid2", /*above*/true );

      addPointLabel(midData.headMid1, "midData.headMid1", /*above*/false);
      addPointLabel(midData.headMid2, "midData.headMid2", /*above*/true );
    }
#endif

    //---

    // draw line polygon (has line width)
    Points points;

    // arrow front (line width)
    auto addFrontPoints = [&]() {
      points.push_back(frontData.headMid2);  // front head above mid line
      points.push_back(frontData.tipPoint2); // front head tip (above)
      points.push_back(startPoint);          // start tip (on line)
      points.push_back(frontData.tipPoint1); // front head tip (below)
      points.push_back(frontData.headMid1);  // front head below mid line
    };

    // arrow tail (line width)
    auto addTailPoints = [&]() {
      points.push_back(tailData.headMid1);  // tail head below mid line
      points.push_back(tailData.tipPoint1); // tail head tip (below)
      points.push_back(endPoint);           // end tip (on line)
      points.push_back(tailData.tipPoint2); // tail head tip (above)
      points.push_back(tailData.headMid2);  // tail head above mid line
    };

    // arrow mid (line width)
    auto addMidPoints1 = [&]() {
      if (! swapped) {
        points.push_back(midData.backLine1);
        points.push_back(midData.tipPoint1);
        points.push_back(headMidR1);
      }
      else {
        points.push_back(headMidR1);
        points.push_back(midData.tipPoint1);
        points.push_back(midData.backLine1);
      }
    };

    auto addMidPoints2 = [&]() {
      if (! swapped) {
        points.push_back(headMidR2);
        points.push_back(midData.tipPoint2);
        points.push_back(midData.backLine2);
      }
      else {
        points.push_back(midData.backLine2);
        points.push_back(midData.tipPoint2);
        points.push_back(headMidR2);
      }
    };

    // flat front
    auto addFrontLinePoints = [&]() {
      points.push_back(frontData.headMid2); // front head above mid line
      points.push_back(frontData.headMid1); // front head below mid line
    };

    // flat tail
    auto addTailLinePoints = [&]() {
      points.push_back(tailData.headMid1); // tail head below mid line
      points.push_back(tailData.headMid2); // tail head above mid line
    };

    // line front (no width)
    auto addFHeadPoints = [&]() {
      for (auto &p : frontData.headPoints1)
        points.push_back(p);
    };

    // line tail (no width)
    auto addTHeadPoints = [&]() {
      for (auto &p : tailData.headPoints1)
        points.push_back(p);
    };

    // line mid
    auto addMidHeadPoints1 = [&]() {
      for (auto &p : midData.headPoints1)
        points.push_back(p);
    };

    auto addMidHeadPoints2 = [&]() {
      for (auto &p : midData.headPoints2)
        points.push_back(p);
    };

    //---

    if      (frontData.isLineEnds)
      addFHeadPoints();
    else if (frontData.isPoints)
      addFrontPoints();
    else
      addFrontLinePoints();

    if      (midData.isLineEnds)
      addMidHeadPoints1();
    else if (midData.isPoints)
      addMidPoints1();

    if      (tailData.isLineEnds)
      addTHeadPoints();
    else if (tailData.isPoints)
      addTailPoints();
    else
      addTailLinePoints();

    if      (midData.isLineEnds)
      addMidHeadPoints2();
    else if (midData.isPoints)
      addMidPoints2();

    drawData_.arrowPoly = points;

    drawPolygon(points, strokeWidth, isFilled(), isStroked());
  }

  //---

#ifdef DEBUG_LABELS
  // draw debug labels
  for (const auto &pointLabel : pointLabels) {
    if (isDebugPoints())
      drawPointSymbol(pointLabel.point);

    if (isDebugLabels()) {
      drawPointLabel(pointLabel.point, pointLabel.text, pointLabel.above);
    }
  }
#endif
}

void
CQArrow::
calcHeadPolyData(const QPointF &startPoint, const QPointF &startPointI,
                 const Angle &lineAngle, bool linePoly, double linePixelWidth,
                 GenHeadData &frontData, const GenHeadData &tailData)
{
  // calc front head angle (relative to line)
  auto frontAngle1 = Angle(lineAngle.angle + frontData.angle.angle);
  auto frontAngle2 = Angle(lineAngle.angle - frontData.angle.angle);

  //---

  // calc front head arrow tip points
  frontData.tipPoint1 = movePointOnLine(startPoint, frontAngle1, frontData.len);
  frontData.tipPoint2 = movePointOnLine(startPoint, frontAngle2, frontData.len);

#ifdef DEBUG_LABELS
  if (isDebugLabels()) {
    addPointLabel(frontData.tipPoint1, "frontData.tipPoint1", /*above*/false);
    addPointLabel(frontData.tipPoint2, "frontData.tipPoint2", /*above*/true );
  }
#endif

  //---

  if (frontData.isLineEnds) {
    if (! linePoly) {
      drawData_.frontLine1 = Line(startPoint, frontData.tipPoint1);
      drawData_.frontLine2 = Line(startPoint, frontData.tipPoint2);
    }
    else {
      // calc front head angle (relative to line)
      auto frontAngle1 = Angle(lineAngle.angle + frontData.angle.angle);
      auto frontAngle2 = Angle(lineAngle.angle - frontData.angle.angle);

      // calc line points offset from end arrow lines
      // (startPoint->frontData.tipPoint1, startPoint->frontData.tipPoint2)
      auto frontTip11 = movePointPerpLine(frontData.tipPoint1, frontAngle1,  linePixelWidth);
      auto frontTip21 = movePointPerpLine(frontData.tipPoint2, frontAngle2, -linePixelWidth);

      // calc point at line width from start point along line
      auto startPointI1 = movePointOnLine(startPoint, lineAngle, linePixelWidth);

      // intersect front head lines to line offset by width
      bool inside;

      QPointF startPointI11, startPointI21;

      intersectLine(frontData.headMid2, tailData.headMid2,
                    frontTip21, startPointI1, startPointI11, inside);
      intersectLine(frontData.headMid1, tailData.headMid1,
                    frontTip11, startPointI1, startPointI21, inside);

      //---

#ifdef DEBUG_LABELS
      if (isDebugLabels()) {
        addPointLabel(frontTip11, "frontTip11", /*above*/false);
        addPointLabel(frontTip21, "frontTip21", /*above*/true );

        addPointLabel(startPointI1 , "startPointI1" , /*above*/false);
        addPointLabel(startPointI11, "startPointI11", /*above*/true );
        addPointLabel(startPointI21, "startPointI21", /*above*/false);
      }
#endif

      frontData.headPoints1.push_back(startPointI11);       // intersect with line top
      frontData.headPoints1.push_back(frontTip21);          // top arrow line end bottom
      frontData.headPoints1.push_back(frontData.tipPoint2); // top arrow line end top
      frontData.headPoints1.push_back(startPoint);          // line start (left)
      frontData.headPoints1.push_back(frontData.tipPoint1); // bottom arrow line end bottom
      frontData.headPoints1.push_back(frontTip11);          // bottom arrow line end top
      frontData.headPoints1.push_back(startPointI21);       // intersect with line bottom
    }
  }
  else {
    auto backTip1 = startPointI;

    // if valid back angle intersect arrow mid line (startPoint, startPointI) to back line
    if (frontData.backAngle.angle > frontData.angle.angle && frontData.backAngle.angle < M_PI) {
      auto a3 = Angle(lineAngle.angle + frontData.backAngle.angle);

      auto backTip2 = movePointOnLine(frontData.tipPoint1, a3, -10);

      bool inside;
      intersectLine(startPoint, startPointI, frontData.tipPoint1, backTip2, backTip1, inside);

#ifdef DEBUG_LABELS
      if (isDebugLabels()) {
        addPointLabel(backTip1, "backTip1", /*above*/false);
      }
#endif

      frontData.headMid = backTip1;
    }

    frontData.headPoints1.push_back(startPoint);          // tip (on line)
    frontData.headPoints1.push_back(frontData.tipPoint1); // tip (below)
    frontData.headPoints1.push_back(backTip1);            // back line intersect or arrow right
                                                          // point (start + length)
    frontData.headPoints1.push_back(frontData.tipPoint2); // tip (above)

    if (! linePoly)
      drawData_.frontPoly = frontData.headPoints1;
  }
}

void
CQArrow::
calcTailPolyData(const QPointF &endPoint, const QPointF &endPointI,
                 const Angle &lineAngle, bool linePoly, double linePixelWidth,
                 const GenHeadData &frontData, GenHeadData &tailData)
{
  // calc tail head angle (relative to line)
  auto tailAngle1 = Angle(lineAngle.angle + M_PI - tailData.angle.angle);
  auto tailAngle2 = Angle(lineAngle.angle + M_PI + tailData.angle.angle);

  //---

  // calc tail head arrow tip points
  tailData.tipPoint1 = movePointOnLine(endPoint, tailAngle1, tailData.len);
  tailData.tipPoint2 = movePointOnLine(endPoint, tailAngle2, tailData.len);

#ifdef DEBUG_LABELS
  if (isDebugLabels()) {
    addPointLabel(tailData.tipPoint1, "tailData.tipPoint1", /*above*/false);
    addPointLabel(tailData.tipPoint2, "tailData.tipPoint2", /*above*/true );
  }
#endif

  //---

  if (tailData.isLineEnds) {
    if (! linePoly) {
      drawData_.endLine1 = Line(endPoint, tailData.tipPoint1);
      drawData_.endLine2 = Line(endPoint, tailData.tipPoint2);
    }
    else {
      // calc tail head angle (relative to line)
      auto tailAngle1 = Angle(lineAngle.angle + M_PI - tailData.angle.angle);
      auto tailAngle2 = Angle(lineAngle.angle + M_PI + tailData.angle.angle);

      // calc line points offset from end arrow lines
      // (startPoint->frontData.tipPoint1, startPoint->frontData.tipPoint2)
      auto tailTip11 = movePointPerpLine(tailData.tipPoint1, tailAngle1, -linePixelWidth);
      auto tailTip21 = movePointPerpLine(tailData.tipPoint2, tailAngle2,  linePixelWidth);

      // calc point at line width from start point along line
      auto endPointI1 = movePointOnLine(endPoint, lineAngle, -linePixelWidth);

      // intersect tail head lines to line offset by width
      bool inside;

      QPointF endPointI11, endPointI12;

      intersectLine(frontData.headMid2, tailData.headMid2,
                    tailTip21, endPointI1, endPointI11, inside);
      intersectLine(frontData.headMid1, tailData.headMid1,
                    tailTip11, endPointI1, endPointI12, inside);

      //---

#ifdef DEBUG_LABELS
      if (isDebugLabels()) {
        addPointLabel(tailTip11  , "tailTip11"  , /*above*/false);
        addPointLabel(tailTip21  , "tailTip21"  , /*above*/true );
        addPointLabel(endPointI1 , "endPointI1" , /*above*/false);
        addPointLabel(endPointI11, "endPointI11", /*above*/true );
        addPointLabel(endPointI12, "endPointI12", /*above*/false);
      }
#endif

      tailData.headPoints1.push_back(endPointI12);        // intersect with line bottom
      tailData.headPoints1.push_back(tailTip11);          // bottom arrow line end top
      tailData.headPoints1.push_back(tailData.tipPoint1); // bottom arrow line end bottom
      tailData.headPoints1.push_back(endPoint);           // line end (right)
      tailData.headPoints1.push_back(tailData.tipPoint2); // top arrow line end top
      tailData.headPoints1.push_back(tailTip21);          // top arrow line end bottom
      tailData.headPoints1.push_back(endPointI11);        // intersect with line top
    }
  }
  else {
    auto pt3 = endPointI;

    // if valid back angle intersect arrow mid line (endPointI, endPoint) to back line
    if (tailData.backAngle.angle > tailData.angle.angle && tailData.backAngle.angle < M_PI) {
      auto a3 = Angle(lineAngle.angle + M_PI - tailData.backAngle.angle);

      auto tailTip11 = movePointOnLine(tailData.tipPoint1, a3, -10);

      bool inside;
      intersectLine(endPointI, endPoint, tailData.tipPoint1, tailTip11, pt3, inside);

#ifdef DEBUG_LABELS
      if (isDebugLabels()) {
        addPointLabel(pt3, "pt3", /*above*/false);
      }
#endif

      tailData.headMid = pt3;
    }

    tailData.headPoints1.push_back(endPoint);           // tip (on line)
    tailData.headPoints1.push_back(tailData.tipPoint1); // tip (below)
    tailData.headPoints1.push_back(pt3);                // back line intersect or arrow left
                                                        // point (end - length)
    tailData.headPoints1.push_back(tailData.tipPoint2); // tip (above)

    if (! linePoly)
      drawData_.tailPoly = tailData.headPoints1;
  }
}

void
CQArrow::
calcMidPolyData(const QPointF &startPoint, const QPointF &endPoint,
                const Angle &lineAngle, bool linePoly, bool swapped, double linePixelWidth,
                const QPointF &headMidR1, const QPointF &headMidR2,
                const GenHeadData &frontData, const GenHeadData &tailData,
                GenHeadData &midData)
{
  // calc mid head angle (relative to line)
  Angle midAngle1, midAngle2;

  if (! swapped) {
    midAngle1 = Angle(lineAngle.angle + M_PI - midData.angle.angle);
    midAngle2 = Angle(lineAngle.angle + M_PI + midData.angle.angle);
  }
  else {
    midAngle1 = Angle(lineAngle.angle + midData.angle.angle);
    midAngle2 = Angle(lineAngle.angle - midData.angle.angle);
  }

  //---

  // calc mid head arrow tip points
  if (! midData.isLineEnds && linePoly) {
#ifdef DEBUG_LABELS
    if (isDebugLabels()) {
      addPointLabel(headMidR1, "headMidR1", /*above*/false);
      addPointLabel(headMidR2, "headMidR2", /*above*/true );
    }
#endif

    midData.tipPoint1 = movePointOnLine(headMidR1, midAngle1, midData.len);
    midData.tipPoint2 = movePointOnLine(headMidR2, midAngle2, midData.len);
  }
  else {
    midData.tipPoint1 = movePointOnLine(midData.headMid, midAngle1, midData.len);
    midData.tipPoint2 = movePointOnLine(midData.headMid, midAngle2, midData.len);
  }

#ifdef DEBUG_LABELS
  if (isDebugLabels()) {
    addPointLabel(midData.tipPoint1, "midData.tipPoint1", /*above*/false);
    addPointLabel(midData.tipPoint2, "midData.tipPoint2", /*above*/true );
  }
#endif

  //---

  if (midData.isLineEnds) {
    if (! linePoly) {
      drawData_.endLine1 = Line(midData.headMid, midData.tipPoint1);
      drawData_.endLine2 = Line(midData.headMid, midData.tipPoint2);
    }
    else {
      // calc mid head angle (relative to line)
      Angle midAngle1, midAngle2;

      if (! swapped) {
        midAngle1 = Angle(lineAngle.angle + M_PI - midData.angle.angle);
        midAngle2 = Angle(lineAngle.angle + M_PI + midData.angle.angle);
      }
      else {
        midAngle1 = Angle(lineAngle.angle + midData.angle.angle);
        midAngle2 = Angle(lineAngle.angle - midData.angle.angle);
      }

      // calc line points offset from end arrow lines
      // (headMidL->midData.tipPoint1, headMidL->midData.tipPoint2)
      auto pmh11 = movePointPerpLine(midData.tipPoint1, midAngle1, -linePixelWidth/2.0);
      auto pmh12 = movePointPerpLine(midData.tipPoint1, midAngle1,  linePixelWidth/2.0);
      auto pmh21 = movePointPerpLine(midData.tipPoint2, midAngle2,  linePixelWidth/2.0);
      auto pmh22 = movePointPerpLine(midData.tipPoint2, midAngle2, -linePixelWidth/2.0);

      // intersect mid head lines to line offset by width
      bool inside;

      QPointF pmh4, pmh5;

      intersectLine(frontData.headMid1, tailData.headMid1,
                    midData.tipPoint1, midData.headMid, pmh4, inside);
      intersectLine(frontData.headMid2, tailData.headMid2,
                    midData.tipPoint2, midData.headMid, pmh5, inside);

      auto pmh41 = movePointPerpLine(pmh4, midAngle1, -linePixelWidth/2.0);
      auto pmh42 = movePointPerpLine(pmh4, midAngle1,  linePixelWidth/2.0);
      auto pmh51 = movePointPerpLine(pmh5, midAngle2,  linePixelWidth/2.0);
      auto pmh52 = movePointPerpLine(pmh5, midAngle2, -linePixelWidth/2.0);

      QPointF pmh43, pmh44, pmh53, pmh54;

      intersectLine(frontData.headMid1, tailData.headMid1, pmh11, pmh41, pmh43, inside);
      intersectLine(frontData.headMid1, tailData.headMid1, pmh12, pmh42, pmh44, inside);
      intersectLine(frontData.headMid2, tailData.headMid2, pmh21, pmh51, pmh53, inside);
      intersectLine(frontData.headMid2, tailData.headMid2, pmh22, pmh52, pmh54, inside);

      //---

#ifdef DEBUG_LABELS
      if (isDebugLabels()) {
        addPointLabel(pmh11, "pmh11", /*above*/false);
        addPointLabel(pmh12, "pmh12", /*above*/false);
        addPointLabel(pmh21, "pmh21", /*above*/true );
        addPointLabel(pmh22, "pmh22", /*above*/true );

        addPointLabel(pmh41, "pmh41", /*above*/true );
        addPointLabel(pmh42, "pmh42", /*above*/true );
        addPointLabel(pmh51, "pmh51", /*above*/false);
        addPointLabel(pmh52, "pmh52", /*above*/false);

        addPointLabel(pmh43, "pmh43", /*above*/false);
        addPointLabel(pmh44, "pmh44", /*above*/false);
        addPointLabel(pmh53, "pmh53", /*above*/false);
        addPointLabel(pmh54, "pmh54", /*above*/false);
      }
#endif

      midData.headPoints1.push_back(pmh43); // intersect with line bottom
      midData.headPoints1.push_back(pmh11); // bottom arrow line end top
      midData.headPoints1.push_back(pmh12); // bottom arrow line end bottom
      midData.headPoints1.push_back(pmh44); // bottom arrow line end bottom

      midData.headPoints2.push_back(pmh54); // bottom arrow line end bottom
      midData.headPoints2.push_back(pmh22); // top arrow line end top
      midData.headPoints2.push_back(pmh21); // top arrow line end bottom
      midData.headPoints2.push_back(pmh53); // intersect with line top
    }
  }
  else {
    // arrow tip (right)
    // if valid back angle intersect arrow mid line (startPoint, startPointI) to back line
    QPointF pmh30;

    if (midData.backAngle.angle > midData.angle.angle && midData.backAngle.angle < M_PI) {
      auto a3 = Angle(lineAngle.angle + M_PI - midData.backAngle.angle);
      auto a4 = Angle(lineAngle.angle        + midData.backAngle.angle);

      auto dl = 20;

      QPointF midTip11, midTip21;

      if (! swapped) {
        midTip11 = movePointOnLine(midData.tipPoint1, a3, -dl);
        midTip21 = movePointOnLine(midData.tipPoint2, a4,  dl);
      }
      else {
        midTip11 = movePointOnLine(midData.tipPoint1, a4, -dl);
        midTip21 = movePointOnLine(midData.tipPoint2, a3,  dl);
      }

#ifdef DEBUG_LABELS
      if (isDebugLabels()) {
        addPointLabel(midTip11, "midTip11", /*above*/false);
        addPointLabel(midTip21, "midTip21", /*above*/true);
      }
#endif

      bool inside;
      intersectLine(startPoint, endPoint, midData.tipPoint1, midTip11, pmh30, inside);

      intersectLine(frontData.headMid1, tailData.headMid1,
                    midData.tipPoint1, midTip11, midData.backLine1, inside);
      intersectLine(frontData.headMid2, tailData.headMid2,
                    midData.tipPoint2, midTip21, midData.backLine2, inside);
    }
    else {
      bool inside;
      intersectLine(startPoint, endPoint, midData.tipPoint1, midData.tipPoint2, pmh30, inside);

      intersectLine(frontData.headMid1, tailData.headMid1,
                    midData.tipPoint1, midData.tipPoint2, midData.backLine1, inside);
      intersectLine(frontData.headMid2, tailData.headMid2,
                    midData.tipPoint1, midData.tipPoint2, midData.backLine2, inside);
    }

#ifdef DEBUG_LABELS
    if (isDebugLabels()) {
      addPointLabel(midData.backLine1, "midData.backLine1", /*above*/false);
      addPointLabel(midData.backLine2, "midData.backLine2", /*above*/true);
    }
#endif

    midData.headPoints1.push_back(pmh30);
    midData.headPoints1.push_back(midData.tipPoint1);
    midData.headPoints1.push_back(midData.headMid);
    midData.headPoints1.push_back(midData.tipPoint2);

    if (! linePoly)
      drawData_.midPoly = midData.headPoints1;
  }
}

void
CQArrow::
updateFrontLinePoly(GenHeadData &frontData, const GenHeadData &tailData)
{
  // intersect front head point lines with arrow line (offset by width)
  QPointF headPoint11, headPoint12;

  int np = frontData.headPoints1.size();

  auto headMid11 = frontData.headMid1;
  auto headMid21 = frontData.headMid2;

  for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
    bool inside;

    intersectLine(frontData.headMid1, tailData.headMid1,
                  frontData.headPoints1[i1], frontData.headPoints1[i2], headPoint11, inside);

    // if intersect inside, and more to right, update intersection (above)
    if (inside && (headPoint11.x() > headMid11.x()))
      headMid11 = headPoint11;

    intersectLine(frontData.headMid2, tailData.headMid2,
                  frontData.headPoints1[i1], frontData.headPoints1[i2], headPoint12, inside);

    // if intersect inside, and more to right, update intersection (below)
    if (inside && (headPoint12.x() > headMid21.x()))
      headMid21 = headPoint12;
  }

  frontData.headMid1 = headMid11;
  frontData.headMid2 = headMid21;
}

void
CQArrow::
updateTailLinePoly(const GenHeadData &frontData, GenHeadData &tailData)
{
  // intersect tail head point lines with arrow line (offset by width)
  QPointF tailPoint11, tailPoint12;

  int np = tailData.headPoints1.size();

  auto headMid11 = tailData.headMid1;
  auto headMid21 = tailData.headMid2;

  for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
    bool inside;

    intersectLine(frontData.headMid1, tailData.headMid1,
                  tailData.headPoints1[i1], tailData.headPoints1[i2], tailPoint11, inside);

    // if intersect inside, and more to left, update intersection (above)
    if (inside && (tailPoint11.x() < headMid11.x()))
      headMid11 = tailPoint11;

    intersectLine(frontData.headMid2, tailData.headMid2,
                  tailData.headPoints1[i1], tailData.headPoints1[i2], tailPoint12, inside);

    // if intersect inside, and more to left, update intersection (below)
    if (inside && (tailPoint12.x() < headMid21.x()))
      headMid21 = tailPoint12;
  }

  tailData.headMid1 = headMid11;
  tailData.headMid2 = headMid21;
}

void
CQArrow::
updateMidLinePoly(GenHeadData &midData, const GenHeadData &frontData, const GenHeadData &tailData)
{
  // intersect mid head point lines with arrow line (offset by width)
  QPointF midPoint11, midPoint12;

  int np = midData.headPoints1.size();

  auto headMid11 = midData.headMid1;
  auto headMid21 = midData.headMid2;

  for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
    bool inside;

    intersectLine(frontData.headMid1, tailData.headMid1,
                  midData.headPoints1[i1], midData.headPoints1[i2], midPoint11, inside);

    // if intersect inside, and more to left, update intersection (above)
    if (inside && (midPoint11.x() < headMid11.x()))
      headMid11 = midPoint11;

    intersectLine(frontData.headMid2, tailData.headMid2,
                  midData.headPoints1[i1], midData.headPoints1[i2], midPoint12, inside);

    // if intersect inside, and more to left, update intersection (below)
    if (inside && (midPoint12.x() < headMid21.x()))
      headMid21 = midPoint12;
  }

  midData.headMid1 = headMid11;
  midData.headMid2 = headMid21;
}

bool
CQArrow::
contains(const QPointF &p) const
{
  if (drawData_.arrowPoly.valid && drawData_.arrowPoly.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (drawData_.frontPoly.valid && drawData_.frontPoly.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (drawData_.tailPoly.valid && drawData_.tailPoly.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (drawData_.frontLine1.valid && (drawData_.frontLine1.distance(p) < 4))
    return true;
  if (drawData_.frontLine2.valid && (drawData_.frontLine2.distance(p) < 4))
    return true;

  if (drawData_.endLine1.valid && (drawData_.endLine1.distance(p) < 4))
    return true;
  if (drawData_.endLine2.valid && (drawData_.endLine2.distance(p) < 4))
    return true;

  if (drawData_.midLine.valid && (drawData_.midLine.distance(p) < 4))
    return true;

  return false;
}

void
CQArrow::
drawPolygon(const Points &points, double w, bool filled, bool stroked)
{
  QPainterPath path;

  path.moveTo(points[0]);

  for (int i = 1; i < points.length(); ++i) {
    path.lineTo(points[i]);
  }

  path.closeSubpath();

  //---

  if (filled) {
    QBrush brush(! isInside() ? fillColor_ : Qt::red);

    painter_->fillPath(path, brush);
  }

  if (stroked) {
    QPen pen(! isInside() ? strokeColor_ : Qt::red);

    pen.setWidthF(w);

    painter_->strokePath(path, pen);
  }
}

void
CQArrow::
drawLine(const QPointF &point1, const QPointF &point2, double width)
{
  QPen p = painter_->pen();

  p.setColor(! isInside() ? strokeColor_ : Qt::red);
  p.setWidthF(width);

  painter_->setPen(p);

  painter_->drawLine(point1, point2);
}

#ifdef DEBUG_LABELS
void
CQArrow::
drawPointSymbolLabel(const QPointF &point, const QString &text, bool above)
{
  drawPointSymbol(point);
  drawPointLabel (point, text, above);
}

void
CQArrow::
drawPointSymbol(const QPointF &point)
{
  QPen pen(textColor_);

  pen.setWidthF(1.0);

  painter_->setPen(pen);

  painter_->drawLine(int(point.x() - 4), int(point.y()    ),
                     int(point.x() + 4), int(point.y()    ));
  painter_->drawLine(int(point.x()    ), int(point.y() - 4),
                     int(point.x()    ), int(point.y() + 4));
}

void
CQArrow::
drawPointLabel(const QPointF &point, const QString &text, bool above)
{
  QPen pen(textColor_);

  pen.setWidthF(1.0);

  painter_->setPen(pen);

  QFontMetrics fm(painter_->font());

  int fw = fm.horizontalAdvance(text);
  int fa = fm.ascent();
  int fd = fm.descent();

  painter_->drawText(int(point.x() - fw/2), int(point.y() + (above ? -fd : fa)), text);
}
#endif

double
CQArrow::
windowWidthToPixelWidth(double w)
{
  QPointF w1 = windowToPixel(QPointF(0, 0));
  QPointF w2 = windowToPixel(QPointF(w, w));

  return std::abs(w2.x() - w1.x());
}

double
CQArrow::
windowHeightToPixelHeight(double h)
{
  QPointF w1 = windowToPixel(QPointF(0, 0));
  QPointF w2 = windowToPixel(QPointF(h, h));

  return std::abs(w2.y() - w1.y());
}

QPointF
CQArrow::
windowToPixel(const QPointF &w)
{
  if (device_)
    return device_->windowToPixel(w);
  else
    return w;
}

double
CQArrow::
pointLineDistance(const QPointF &p, const QPointF &p1, const QPointF &p2)
{
  double xmin = std::min(p1.x(), p2.x());
  double ymin = std::min(p1.y(), p2.y());
  double xmax = std::max(p1.x(), p2.x());
  double ymax = std::max(p1.y(), p2.y());

  if (p.x() < xmin || p.y() < ymin)
    return std::hypot(xmin - p.x(), ymin - p.y());

  if (p.x() > xmax || p.y() > ymax)
    return std::hypot(xmax - p.x(), ymax - p.y());

  double dx = p2.x() - p1.x();
  double dy = p2.y() - p1.y();

  double l = std::hypot(dx, dy);
  if (l <= 0.0) return 0.0;

  return std::abs(dy*p.x() - dx*p.y() + p2.x()*p1.y() - p2.y()*p1.x())/l;
}
