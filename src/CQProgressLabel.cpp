#include <CQProgressLabel.h>

#include <QPainter>

CQProgressLabel::
CQProgressLabel(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("progressLabel");
}

void
CQProgressLabel::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  auto x = int(double(width())*double(progress())/100.0);

  auto lrect = QRect(0, 0, x, height());
  auto rrect = QRect(x, 0, width() - x, height());

  auto lcolor = progressColor();
  auto rcolor = palette().window().color();

  auto bwColor = [](const QColor &c) {
    int g = qGray(c.red(), c.green(), c.blue());

    return (g > 128 ? QColor(Qt::black) : QColor(Qt::white));
  };

  auto ltextColor = bwColor(lcolor);
  auto rtextColor = bwColor(rcolor);

  auto text = QString("%1%").arg(progress());

  painter.save();

  painter.setClipRect(lrect);

  painter.fillRect(rect(), QBrush(lcolor));

  painter.setPen(ltextColor);
  painter.drawText(rect(), Qt::AlignCenter, text);

  painter.setClipRect(rrect);

  painter.fillRect(rect(), QBrush(rcolor));

  painter.setPen(rtextColor);
  painter.drawText(rect(), Qt::AlignCenter, text);

  painter.restore();
}

QSize
CQProgressLabel::
sizeHint() const
{
  QFontMetrics fm(font());

  auto w = fm.horizontalAdvance("100%");
  auto h = fm.height();

  return QSize(2*w, h);
}
