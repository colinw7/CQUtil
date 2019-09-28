#include <CQArrow.h>
#include <CMathGeom2D.h>

#include <QPainter>
#include <QPainterPath>
#include <cmath>

namespace {
  double Deg2Rad(double d) { return M_PI*d/180.0; }
//double Rad2Deg(double d) { return 180.0*d/M_PI; }
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
  frontData_.type     = type;
  frontData_.lineEnds = false;

  if      (frontData_.type == HeadType::TRIANGLE) {
    frontData_.angle = 30; frontData_.backAngle = 90;
  }
  else if (frontData_.type == HeadType::STEALTH) {
    frontData_.angle = 30; frontData_.backAngle = 45;
  }
  else if (frontData_.type == HeadType::DIAMOND) {
    frontData_.angle = 45; frontData_.backAngle = 135;
  }
  else if (frontData_.type == HeadType::LINE) {
    frontData_.lineEnds = true;
  }
  else if (frontData_.type == HeadType::NONE) {
  }
}

void
CQArrow::
setTailType(const HeadType &type)
{
  tailData_.type     = type;
  tailData_.lineEnds = false;

  if      (tailData_.type == HeadType::TRIANGLE) {
    tailData_.angle = 30; tailData_.backAngle = 90;
  }
  else if (tailData_.type == HeadType::STEALTH) {
    tailData_.angle = 30; tailData_.backAngle = 45;
  }
  else if (tailData_.type == HeadType::DIAMOND) {
    tailData_.angle = 45; tailData_.backAngle = 135;
  }
  else if (tailData_.type == HeadType::LINE) {
    tailData_.lineEnds = true;
  }
  else if (tailData_.type == HeadType::NONE) {
  }
}

void
CQArrow::
draw(QPainter *painter, CQArrowDevice *device)
{
  frontLine1_.reset();
  frontLine2_.reset();
  endLine1_  .reset();
  endLine2_  .reset();
  midLine_   .reset();
  frontPoly_ .reset();
  tailPoly_  .reset();
  arrowPoly_ .reset();

  //---

  struct Angle {
    double angle { 0.0 };
    double cos   { 0.0 };
    double sin   { 0.0 };

    Angle(double angle) :
     angle(angle) {
      init();
    }

    Angle(const QPointF &p1, const QPointF &p2) {
      angle = std::atan2(p2.y() - p1.y(), p2.x() - p1.x());

      init();
    }

    void init() {
      cos = std::cos(angle);
      sin = std::sin(angle);
    }
  };

  //---

  auto movePointOnLine = [](const QPointF &p, const Angle &a, double d) {
    return QPointF(p.x() + d*a.cos, p.y() + d*a.sin);
  };

  auto movePointPerpLine = [](const QPointF &p, const Angle &a, double d) {
    return QPointF(p.x() + d*a.sin, p.y() - d*a.cos);
  };

  auto addWidthToPoint = [](const QPointF &p, const Angle &a, double lw,
                            QPointF &p1, QPointF &p2) {
    double dx = lw*a.sin/2.0;
    double dy = lw*a.cos/2.0;

    p1 = QPointF(p.x() - dx, p.y() + dy);
    p2 = QPointF(p.x() + dx, p.y() - dy);
  };

  auto intersectLine = [](const QPointF &l1s, const QPointF &l1e,
                          const QPointF &l2s, const QPointF &l2e, QPointF &pi, bool &inside) {
    double xi, yi, mu1, mu2;

    bool rc = CMathGeom2D::IntersectLine(l1s.x(), l1s.y(), l1e.x(), l1e.y(),
                                         l2s.x(), l2s.y(), l2e.x(), l2e.y(),
                                         &xi, &yi, &mu1, &mu2);

    inside = (mu1 >= 0.0 && mu1 <= 1.0);

    pi = QPointF(xi, yi);

    return rc;
  };

  //---

  PointLabels pointLabels;

  auto addPointLabel = [&](const QPointF &point, const QString &text, bool above=true) {
    pointLabels.push_back(PointLabel(point, text, above));
  };

  //---

  painter_ = painter;
  device_  = device;

  //---

  QPointF from = from_;
  QPointF to   = (isRelative() ? from_ + to_ : to_);

  if (from.x() > to.x())
    std::swap(from, to);

  //---

  // convert start/end point to pixel start/end point
  QPointF p1 = windowToPixel(from);
  QPointF p4 = windowToPixel(to  );

  if (isDebugLabels()) {
    addPointLabel(p1, "p1", /*above*/false);
    addPointLabel(p4, "p4", /*above*/true );
  }

  //---

  // convert line width, front/tail arrow length to pixel
  double lpw = windowWidthToPixelWidth  (lineWidth());
//double lph = windowHeightToPixelHeight(lineWidth());

  double fl = (frontLength() > 0 ? windowWidthToPixelWidth(frontLength()) : 8);
  double tl = (tailLength () > 0 ? windowWidthToPixelWidth(tailLength ()) : 8);

  bool linePoly = (lineWidth() > 0);

  //---

  // calc stroke width
  double strokeWidth = (this->strokeWidth() >= 0 ? this->strokeWidth() : 4);

  //---

  // calc line angle (radians)
  Angle a(p1, p4);

  // calc front/tail arrow angles (radians)
  Angle faa(Deg2Rad(frontAngle() > 0 ? frontAngle() : 45));
  Angle taa(Deg2Rad(tailAngle () > 0 ? tailAngle () : 45));

  // calc front/tail arrow back angles (radians)
  Angle fba(Deg2Rad(frontBackAngle() > 0 ? frontBackAngle() : 90));
  Angle tba(Deg2Rad(tailBackAngle () > 0 ? tailBackAngle () : 90));

  //---

  // calc front/tail arrow length along line
  double fpl1 = fl*faa.cos;
  double tpl1 = tl*taa.cos;

  //---

  // calc front/tail arrow end point (along line)
  QPointF p2 = movePointOnLine(p1, a,  fpl1);
  QPointF p3 = movePointOnLine(p4, a, -tpl1);

  if (isDebugLabels()) {
    addPointLabel(p2, "p2", /*above*/false);
    addPointLabel(p3, "p3", /*above*/true );
  }

  //---

  bool isFrontLineEnds = (this->isFrontLineEnds() && isFHead());
  bool isTailLineEnds  = (this->isTailLineEnds () && isTHead());

  //---

  // calc front/tail head mid point (on line)
  QPointF fHeadMid = p1;
  QPointF tHeadMid = p4;

  if (isFHead()) {
    if (! isFrontLineEnds)
      fHeadMid = movePointOnLine(p1, a,  strokeWidth);
  }

  if (isTHead()) {
    if (! isTailLineEnds)
      tHeadMid = movePointOnLine(p4, a, -strokeWidth);
  }

  //---

  // create polygon for arrow shape if has width
  QPointF pl1, pl2, pl3, pl4;

  if (linePoly) {
    // calc front head mid point offset by line width
    addWidthToPoint(fHeadMid, a, lpw, pl1, pl2);

    // calc tail head mid point offset by line width
    addWidthToPoint(tHeadMid, a, lpw, pl3, pl4);
  }

  //---

  Points  fHeadPoints;
  QPointF pf1, pf2;

  if (isFHead()) {
    // calc front head angle (relative to line)
    Angle a1 = a.angle + faa.angle;
    Angle a2 = a.angle - faa.angle;

    //---

    // calc front head arrow tip points
    pf1 = movePointOnLine(p1, a1, fl);
    pf2 = movePointOnLine(p1, a2, fl);

    if (isDebugLabels()) {
      addPointLabel(pf1, "pf1", /*above*/false);
      addPointLabel(pf2, "pf2", /*above*/true );
    }

    //---

    if (isFrontLineEnds) {
      if (! linePoly) {
        frontLine1_ = Line(p1, pf1);
        frontLine2_ = Line(p1, pf2);

        drawLine(p1, pf1, strokeWidth);
        drawLine(p1, pf2, strokeWidth);
      }
      else {
        // calc front head angle (relative to line)
        Angle a1 = a.angle + faa.angle;
        Angle a2 = a.angle - faa.angle;

        // calc line points offset from end arrow lines (p1->pf1, p1->pf2)
        QPointF pf11 = movePointPerpLine(pf1, a1,  lpw);
        QPointF pf21 = movePointPerpLine(pf2, a2, -lpw);

        // calc point at line width from start point along line
        QPointF pf31 = movePointOnLine(p1, a, lpw);

        // intersect front head lines to line offset by width
        bool inside;

        QPointF pf41, pf51;

        intersectLine(pl2, pl4, pf21, pf31, pf41, inside);
        intersectLine(pl1, pl3, pf11, pf31, pf51, inside);

        //---

        if (isDebugLabels()) {
          addPointLabel(pf11, "pf11", /*above*/false);
          addPointLabel(pf21, "pf21", /*above*/true );
          addPointLabel(pf31, "pf31", /*above*/false);
          addPointLabel(pf41, "pf41", /*above*/true );
          addPointLabel(pf51, "pf51", /*above*/false);
        }

        fHeadPoints.push_back(pf41); // intersect with line top
        fHeadPoints.push_back(pf21); // top arrow line end bottom
        fHeadPoints.push_back(pf2);  // top arrow line end top
        fHeadPoints.push_back(p1);   // line start (left)
        fHeadPoints.push_back(pf1);  // bottom arrow line end bottom
        fHeadPoints.push_back(pf11); // bottom arrow line end top
        fHeadPoints.push_back(pf51); // intersect with line bottom
      }
    }
    else {
      QPointF pf3 = p2;

      // if valid back angle intersect arrow mid line (p1, p2) to back line
      if (fba.angle > faa.angle && fba.angle < M_PI) {
        Angle a3 = a.angle + fba.angle;

        QPointF pf1t = movePointOnLine(pf1, a3, -10);

        bool inside;
        intersectLine(p1, p2, pf1, pf1t, pf3, inside);

        if (isDebugLabels())
          addPointLabel(pf3, "pf3", /*above*/false);

        fHeadMid = pf3;
      }

      fHeadPoints.push_back(p1);  // tip (on line)
      fHeadPoints.push_back(pf1); // tip (below)
      fHeadPoints.push_back(pf3); // back line intersect or arrow right point (start + length)
      fHeadPoints.push_back(pf2); // tip (above)

      if (! linePoly) {
        frontPoly_ = fHeadPoints;

        drawPolygon(fHeadPoints, strokeWidth, isFilled(), isStroked());
      }
    }
  }

  //---

  Points  tHeadPoints;
  QPointF pt1, pt2;

  if (isTHead()) {
    // calc tail head angle (relative to line)
    Angle a1 = a.angle + M_PI - taa.angle;
    Angle a2 = a.angle + M_PI + taa.angle;

    //---

    // calc tail head arrow tip points
    pt1 = movePointOnLine(p4, a1, tl);
    pt2 = movePointOnLine(p4, a2, tl);

    if (isDebugLabels()) {
      addPointLabel(pt1, "pt1", /*above*/false);
      addPointLabel(pt2, "pt2", /*above*/true );
    }

    //---

    if (isTailLineEnds) {
      if (! linePoly) {
        endLine1_ = Line(p4, pt1);
        endLine2_ = Line(p4, pt2);

        drawLine(p4, pt1, strokeWidth);
        drawLine(p4, pt2, strokeWidth);
      }
      else {
        // calc tail head angle (relative to line)
        Angle a1 = a.angle + M_PI - taa.angle;
        Angle a2 = a.angle + M_PI + taa.angle;

        // calc line points offset from end arrow lines (p1->pf1, p1->pf2)
        QPointF pt11 = movePointPerpLine(pt1, a1, -lpw);
        QPointF pt21 = movePointPerpLine(pt2, a2,  lpw);

        // calc point at line width from start point along line
        QPointF pt31 = movePointOnLine(p4, a, -lpw);

        // intersect tail head lines to line offset by width
        bool inside;

        QPointF pt41, pt51;

        intersectLine(pl2, pl4, pt21, pt31, pt41, inside);
        intersectLine(pl1, pl3, pt11, pt31, pt51, inside);

        //---

        if (isDebugLabels()) {
          addPointLabel(pt11, "pt11", /*above*/false);
          addPointLabel(pt21, "pt21", /*above*/true );
          addPointLabel(pt31, "pt31", /*above*/false);
          addPointLabel(pt41, "pt41", /*above*/true );
          addPointLabel(pt51, "pt51", /*above*/false);
        }

        tHeadPoints.push_back(pt51); // intersect with line bottom
        tHeadPoints.push_back(pt11); // bottom arrow line end top
        tHeadPoints.push_back(pt1);  // bottom arrow line end bottom
        tHeadPoints.push_back(p4);   // line end (right)
        tHeadPoints.push_back(pt2);  // top arrow line end top
        tHeadPoints.push_back(pt21); // top arrow line end bottom
        tHeadPoints.push_back(pt41); // intersect with line top
      }
    }
    else {
      QPointF pt3 = p3;

      // if valid back angle intersect arrow mid line (p1, p2) to back line
      if (tba.angle > taa.angle && tba.angle < M_PI) {
        Angle a3 = a.angle + M_PI - tba.angle;

        QPointF pt1t = movePointOnLine(pt1, a3, -10);

        bool inside;
        intersectLine(p3, p4, pt1, pt1t, pt3, inside);

        if (isDebugLabels())
          addPointLabel(pt3, "pt3", /*above*/false);

        tHeadMid = pt3;
      }

      tHeadPoints.push_back(p4);  // tip (on line)
      tHeadPoints.push_back(pt1); // tip (below)
      tHeadPoints.push_back(pt3); // back line intersect or arrow left point (end - length)
      tHeadPoints.push_back(pt2); // tip (above)

      if (! linePoly) {
        tailPoly_ = tHeadPoints;

        drawPolygon(tHeadPoints, strokeWidth, isFilled(), isStroked());
      }
    }
  }

  //---

  // update head and tail (non line) polygon for arrow shape with line width
  if (linePoly) {
    // intersect front head point lines with arrow line (offset by width)
    if (! fHeadPoints.empty() && ! isFrontLineEnds) {
      QPointF fl1, fl2;

      int np = fHeadPoints.size();

      QPointF pi1 = pl1;
      QPointF pi2 = pl2;

      for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
        bool inside;

        intersectLine(pl1, pl3, fHeadPoints[i1], fHeadPoints[i2], fl1, inside);

        // if intersect inside, and more to right, update intersection (above)
        if (inside && (fl1.x() > pi1.x()))
          pi1 = fl1;

        intersectLine(pl2, pl4, fHeadPoints[i1], fHeadPoints[i2], fl2, inside);

        // if intersect inside, and more to right, update intersection (below)
        if (inside && (fl2.x() > pi2.x()))
          pi2 = fl2;
      }

      pl1 = pi1;
      pl2 = pi2;
    }

    //---

    // intersect front head point lines with arrow line (offset by width)
    if (! tHeadPoints.empty() && ! isTailLineEnds) {
      QPointF tl1, tl2;

      int np = tHeadPoints.size();

      QPointF pi3 = pl3;
      QPointF pi4 = pl4;

      for (int i1 = np - 1, i2 = 0; i2 < np; i1 = i2++) {
        bool inside;

        intersectLine(pl1, pl3, tHeadPoints[i1], tHeadPoints[i2], tl1, inside);

        // if intersect inside, and more to left, update intersection (above)
        if (inside && (tl1.x() < pi3.x()))
          pi3 = tl1;

        intersectLine(pl2, pl4, tHeadPoints[i1], tHeadPoints[i2], tl2, inside);

        // if intersect inside, and more to left, update intersection (below)
        if (inside && (tl2.x() < pi4.x()))
          pi4 = tl2;
      }

      pl3 = pi3;
      pl4 = pi4;
    }

    //---

    if (isDebugLabels()) {
      addPointLabel(pl1, "pl1", /*above*/false);
      addPointLabel(pl2, "pl2", /*above*/true );
      addPointLabel(pl3, "pl3", /*above*/false);
      addPointLabel(pl4, "pl4", /*above*/true );
    }
  }

  //---

  if (linePoly) {
    // draw line polygon (has line width)
    Points points;

    auto addFrontPoints = [&]() {
      points.push_back(pl2); // front head above mid line
      points.push_back(pf2); // front head tip (above)
      points.push_back(p1);  // start tip (on line)
      points.push_back(pf1); // front head tip (below)
      points.push_back(pl1); // front head below mid line
    };

    auto addTailPoints = [&]() {
      points.push_back(pl3); // tail head below mid line
      points.push_back(pt1); // tail head tip (below)
      points.push_back(p4);  // end tip (on line)
      points.push_back(pt2); // tail head tip (above)
      points.push_back(pl4); // tail head above mid line
    };

    auto addFrontLinePoints = [&]() {
      points.push_back(pl2); // front head above mid line
      points.push_back(pl1); // front head below mid line
    };

    auto addTailLinePoints = [&]() {
      points.push_back(pl3); // tail head below mid line
      points.push_back(pl4); // tail head above mid line
    };

    auto addLinePoints = [&]() {
      addFrontLinePoints();
      addTailLinePoints ();
    };

    auto addFHeadPoints = [&]() {
      for (auto &p : fHeadPoints)
        points.push_back(p);
    };

    auto addTHeadPoints = [&]() {
      for (auto &p : tHeadPoints)
        points.push_back(p);
    };

    //---

    if      (! isFrontLineEnds && ! isTailLineEnds) {
      if      (isFHead() && isTHead()) {
        addFrontPoints();
        addTailPoints ();
      }
      else if (isTHead()) {
        addFrontLinePoints();
        addTailPoints     ();
      }
      else if (isFHead()) {
        addFrontPoints   ();
        addTailLinePoints();
      }
      else {
        addLinePoints();
      }
    }
    else if (isFrontLineEnds && ! isTailLineEnds) {
      if (isTHead()) {
        addFHeadPoints();
        addTailPoints ();
      }
      else {
        addFHeadPoints   ();
        addTailLinePoints();
      }
    }
    else if (isTailLineEnds && ! isFrontLineEnds) {
      if (isFHead()) {
        addFrontPoints();
        addTHeadPoints();
      }
      else {
        addFrontLinePoints();
        addTHeadPoints    ();
      }
    }
    else {
      addFHeadPoints();
      addTHeadPoints();
    }

    arrowPoly_ = points;

    drawPolygon(points, strokeWidth, isFilled(), isStroked());
  }
  else {
    // draw line (no line width)
    midLine_ = Line(fHeadMid, tHeadMid);

    drawLine(fHeadMid, tHeadMid, strokeWidth);
  }

  //---

  // draw debug labels
  for (const auto &pointLabel : pointLabels) {
    if (isDebugPoints())
      drawPointSymbol(pointLabel.point);

    if (isDebugLabels())
      drawPointLabel(pointLabel.point, pointLabel.text, pointLabel.above);
  }
}

bool
CQArrow::
contains(const QPointF &p) const
{
  if (arrowPoly_.valid && arrowPoly_.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (frontPoly_.valid && frontPoly_.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (tailPoly_.valid && tailPoly_.points.containsPoint(p, Qt::OddEvenFill))
    return true;

  if (frontLine1_.valid && (frontLine1_.distance(p) < 4))
    return true;
  if (frontLine2_.valid && (frontLine2_.distance(p) < 4))
    return true;

  if (endLine1_.valid && (endLine1_.distance(p) < 4))
    return true;
  if (endLine2_.valid && (endLine2_.distance(p) < 4))
    return true;

  if (midLine_.valid && (midLine_.distance(p) < 4))
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

  painter_->drawLine(point.x() - 4, point.y()    , point.x() + 4, point.y()    );
  painter_->drawLine(point.x()    , point.y() - 4, point.x()    , point.y() + 4);
}

void
CQArrow::
drawPointLabel(const QPointF &point, const QString &text, bool above)
{
  QPen pen(textColor_);

  pen.setWidthF(1.0);

  painter_->setPen(pen);

  QFontMetrics fm(painter_->font());

  int fw = fm.width(text);
  int fa = fm.ascent();
  int fd = fm.descent();

  painter_->drawText(point.x() - fw/2, point.y() + (above ? -fd : fa), text);
}

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
