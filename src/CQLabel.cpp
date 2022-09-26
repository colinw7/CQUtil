#include <CQLabel.h>

#include <QPainter>

CQLabel::
CQLabel(const QVariant &value, QWidget *parent) :
 QLabel("", parent)
{
  setValue(value);
}

CQLabel::
CQLabel(const QVariant &value, Qt::Orientation orient, QWidget *parent) :
 QLabel("", parent), orientation_(orient)
{
  setValue(value);
}

void
CQLabel::
setValue(const QVariant &value)
{
  value_ = value;

  setText(value_.toString());

  updateState();

  update();
}

void
CQLabel::
setOrientation(Qt::Orientation orient)
{
  orientation_ = orient;

  updateState();

  update();
}

void
CQLabel::
updateState()
{
  //if (orientation() == Qt::Horizontal)
  //  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  //else
  //  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
}

void
CQLabel::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  QRect r = rect();

  //painter.fillRect(r, QBrush(QColor(100, 100, 200)));

  if (orientation() == Qt::Vertical) {
    painter.translate(0, r.height());
    painter.rotate(-90);
  }

  auto str = value_.toString();

  QFontMetrics fm(font());

  painter.setPen(palette().color(QPalette::Active, QPalette::WindowText));

  int dx, dy;

  if (orientation() == Qt::Horizontal) {
    if      (alignment() & Qt::AlignLeft)
      dx = r.left();
    else if (alignment() & Qt::AlignRight)
      dx = r.width() - fm.horizontalAdvance(str);
    else
      dx = (r.width() - fm.horizontalAdvance(str))/2;

    if      (alignment() & Qt::AlignTop)
      dy = r.top() + fm.ascent();
    else if (alignment() & Qt::AlignBottom)
      dy = r.bottom() - fm.descent();
    else
      dy = r.height()/2 + (fm.height() - fm.descent())/2;
  }
  else {
    dx = r.height() - fm.horizontalAdvance(str);
    dy = r.width () - fm.height();
  }

  painter.drawText(dx, dy, str);
}

QSize
CQLabel::
sizeHint() const
{
  auto str = value_.toString();

  QFontMetrics fm(font());

  int w = fm.horizontalAdvance(str) + 2*margin();
  int h = fm.height()               + 2*margin();

  if (orientation() == Qt::Horizontal)
    return QSize(w, h);
  else
    return QSize(h, w);
}

QSize
CQLabel::
minimumSizeHint() const
{
  return sizeHint();
}
