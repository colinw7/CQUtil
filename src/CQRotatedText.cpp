#include <CQRotatedText.h>

#include <QPainter>
#include <cmath>

namespace CQRotatedText {

void
drawRotatedText(QPainter *painter, double x, double y, const QString &text,
                double angle, Qt::Alignment align, bool alignBBox)
{
  painter->save();

  QFontMetrics fm(painter->font());

  int th = fm.height();
  int tw = fm.horizontalAdvance(text);

  double a1 = M_PI*angle/180.0;

  double c = cos(-a1);
  double s = sin(-a1);

  double tx = 0.0, ty = 0.0;
  double ax = 0.0, ay = 0.0;

  if (! alignBBox) {
    double dx = 0.0, dy = 0.0;

    if      (align & Qt::AlignLeft)
      dx = 0.0;
    else if (align & Qt::AlignRight)
      dx = -tw;
    else if (align & Qt::AlignHCenter)
      dx = -tw/2.0;

    if      (align & Qt::AlignBottom)
      dy = 0.0;
    else if (align & Qt::AlignTop)
      dy = th;
    else if (align & Qt::AlignVCenter)
      dy = th/2.0;

    tx = c*dx - s*dy;
    ty = s*dx + c*dy;

    ax = -s*fm.descent();
    ay =  c*fm.descent();
  }
  else {
    if      (align & Qt::AlignLeft)
      tx = -th*s;
    else if (align & Qt::AlignRight)
      tx = -tw*c;
    else if (align & Qt::AlignHCenter)
      tx = -(th*s + tw*c)/2.0;

    if      (align & Qt::AlignBottom)
      ty = 0.0;
    else if (align & Qt::AlignTop)
      ty = -(tw*s - th*c);
    else if (align & Qt::AlignVCenter)
      ty = -(tw*s - th*c)/2;

    ax = -s*fm.descent();
    ay =  c*fm.descent();
  }

  //------

  QTransform t;

  t.translate(x + tx - ax, y + ty - ay);
  t.rotate(-angle);
//t.translate(0, -fm.descent());

  painter->setTransform(t);

  painter->drawText(0, 0, text);

  painter->restore();
}

QRectF
bbox(double x, double y, const QString &text, const QFont &font, double angle, double border,
     Qt::Alignment align, bool alignBBox)
{
  QRectF bbox;
  Points points;

  bboxData(x, y, text, font, angle, border, bbox, points, align, alignBBox);

  return bbox;
}

Points
bboxPoints(double x, double y, const QString &text, const QFont &font, double angle, double border,
           Qt::Alignment align, bool alignBBox)
{
  QRectF bbox;
  Points points;

  bboxData(x, y, text, font, angle, border, bbox, points, align, alignBBox);

  return points;
}

void
bboxData(double x, double y, const QString &text, const QFont &font, double angle,
         double border, QRectF &bbox, Points &points, Qt::Alignment align, bool alignBBox)
{
  QFontMetrics fm(font);

  //------

  int th = int(fm.height()                + 2*border);
  int tw = int(fm.horizontalAdvance(text) + 2*border);

  double a1 = M_PI*angle/180.0;

  double c = cos(-a1);
  double s = sin(-a1);

  //---

  double tx = 0.0, ty = 0.0;

  if (! alignBBox) {
    double dx = 0.0, dy = 0.0;

    if      (align & Qt::AlignLeft)
      dx = -border;
    else if (align & Qt::AlignRight)
      dx = -tw + border;
    else if (align & Qt::AlignHCenter)
      dx = -tw/2.0;

    if      (align & Qt::AlignBottom)
      dy = border;
    else if (align & Qt::AlignTop)
      dy = th - border;
    else if (align & Qt::AlignVCenter)
      dy = th/2.0;

    //---

    tx = c*dx - s*dy;
    ty = s*dx + c*dy;
  }
  else {
    if      (align & Qt::AlignLeft)
      tx = -th*s - border;
    else if (align & Qt::AlignRight)
      tx = -tw*c + border;
    else if (align & Qt::AlignHCenter)
      tx = -(th*s + tw*c)/2.0;

    if      (align & Qt::AlignBottom)
      ty = border;
    else if (align & Qt::AlignTop)
      ty = -(tw*s - th*c) - border;
    else if (align & Qt::AlignVCenter)
      ty = -(tw*s - th*c)/2.0;
  }

  //------

  //x -= c*border - s*border;
  //y -= s*border + c*border;

  double x1 = x + tx, x2 = x + tw*c + tx, x3 = x + tw*c + th*s + tx, x4 = x + th*s + tx;
  double y1 = y + ty, y2 = y + tw*s + ty, y3 = y + tw*s - th*c + ty, y4 = y - th*c + ty;

  points.clear();

  points.push_back(QPointF(x1, y1));
  points.push_back(QPointF(x2, y2));
  points.push_back(QPointF(x3, y3));
  points.push_back(QPointF(x4, y4));

  //---

  double xmin = points[0].x(); double xmax = xmin;
  double ymin = points[0].y(); double ymax = ymin;

  for (uint i = 1; i < 4; ++i) {
    xmin = std::min(xmin, points[i].x());
    ymin = std::min(ymin, points[i].y());
    xmax = std::max(xmax, points[i].x());
    ymax = std::max(ymax, points[i].y());
  }

  bbox = QRectF(xmin, ymin, xmax - xmin, ymax - ymin);
}

}
