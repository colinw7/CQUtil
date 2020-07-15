#include <CQDragLabel.h>

#include <QMouseEvent>
#include <iostream>

CQDragLabel::
CQDragLabel(QWidget *parent) :
 QLabel(parent)
{
  init();
}

CQDragLabel::
CQDragLabel(const QString &label, QWidget *parent) :
 QLabel(label, parent)
{
  init();
}

void
CQDragLabel::
init()
{
  //setCursor(Qt::SizeHorCursor);
}

void
CQDragLabel::
mousePressEvent(QMouseEvent *e)
{
  pressed_ = true;
  lastDx_  = 0;
  pos_     = e->globalPos();

  setCursor(Qt::SizeHorCursor);
}

void
CQDragLabel::
mouseMoveEvent(QMouseEvent *e)
{
  if (! pressed_)
    return;

  auto pos = e->globalPos();

  int s1 = dragSize()/2;
  int s2 = dragSize() - s1;

  int dx = pos.x() - pos_.x();

  dx = std::min(std::max(dx, -s1), s2);

  if (dx == lastDx_)
    return;

  double f = 1.0*(dx + s1)/dragSize();

  double x = dragMin() + f*(dragMax() - dragMin());

  emit dragValueChanged(x);

  lastDx_ = dx;
}

void
CQDragLabel::
mouseReleaseEvent(QMouseEvent *)
{
  pressed_ = false;

  setCursor(Qt::ArrowCursor);
}
