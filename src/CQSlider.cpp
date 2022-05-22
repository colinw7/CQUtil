#include <CQSlider.h>

#include <QPainter>
#include <QStyleOptionSlider>

CQSlider::
CQSlider(QWidget *parent) :
 QSlider(Qt::Horizontal, parent)
{
  valFont_  = font(); valFont_ .setPointSizeF(0.9*valFont_ .pointSizeF());
  tickFont_ = font(); tickFont_.setPointSizeF(0.8*tickFont_.pointSizeF());
}

void
CQSlider::
setValueLabel(const QString &label)
{
  valueLabel_ = label;
}

int
CQSlider::
valueToPos(int val) const
{
  double border = 2.0;

  double w = width() - 2*border - 2*dx_;

  return int((w*(val - minimum()))/(maximum() - minimum()) + border + dx_);
}

int
CQSlider::
valueWidthToPos(int val, int w) const
{
  int pos = valueToPos(val) - w/2;

  if      (pos <  0              ) pos = 0;
  else if (pos >= width() - w - 1) pos = width() - w - 1;

  return pos;
}

void
CQSlider::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  QStyleOptionSlider opt;

  initStyleOption(&opt);

  opt.rect.adjust(dx_, dy_, -dx_, -dy_);

  opt.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;

  if (tickPosition() != NoTicks)
    opt.subControls |= QStyle::SC_SliderTickmarks;

#if 0
  if (pressedControl) {
    opt.activeSubControls = pressedControl;

    opt.state |= QStyle::State_Sunken;
  }
  else {
    opt.activeSubControls = hoverControl;
  }
#endif

  style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);

  // draw ticks
  int y = height()/2 + sliderHeight_/2 + 2;

  p.setPen(QColor(0,0,0));

  drawTick(&p, minimum(), y, 4);
  drawTick(&p, maximum(), y, 4);

  for (int i = minimum(); i <= maximum(); i += pageStep())
    drawTick(&p, i, y, 2);

  // draw labels
  QFont smallFont = font();

  smallFont.setPointSizeF(0.8*smallFont.pointSizeF());

  QFontMetrics fm1(valFont_);
  QFontMetrics fm2(tickFont_);

  auto minLabel = QString("%1").arg(minimum());
  auto maxLabel = QString("%1").arg(maximum());

  QString valLabel;

  if (valueLabel_.isEmpty())
    valLabel = QString("%1").arg(value());
  else
    valLabel = QString("%1 %2").arg(value()).arg(valueLabel_);

  p.setFont(tickFont_);

  p.drawText(valueWidthToPos(minimum(), fm2.width(minLabel)), height() - fm2.descent(), minLabel);
  p.drawText(valueWidthToPos(maximum(), fm2.width(maxLabel)), height() - fm2.descent(), maxLabel);

  p.setFont(valFont_);

  p.drawText(valueWidthToPos(value(), fm1.width(valLabel)), fm1.ascent() + 4, valLabel);
}

void
CQSlider::
drawTick(QPainter *p, int val, int y, int s)
{
  int x = valueToPos(val);

  p->drawLine(x, y, x, y + s);
}

QSize
CQSlider::
sizeHint() const
{
  QFontMetrics fm(valFont_);

  QSize s = QSlider::sizeHint();

  dx_ = fm.width("X")/2;
  dy_ = fm.height() + 4;

  sliderHeight_ = s.height();

  // add enough space for char above and below + tick space
  return s + QSize(2*dx_, 2*dy_);
}

//-----

CQRealSlider::
CQRealSlider(QWidget *parent) :
 QSlider(Qt::Horizontal, parent)
{
  valFont_  = font(); valFont_ .setPointSizeF(0.9*valFont_ .pointSizeF());
  tickFont_ = font(); tickFont_.setPointSizeF(0.8*tickFont_.pointSizeF());

  QSlider::setMinimum(0);
  QSlider::setMaximum(1000);
  QSlider::setSingleStep(1);

  connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
}

void
CQRealSlider::
setValueLabel(const QString &label)
{
  valueLabel_ = label;
}

void
CQRealSlider::
setValue(double r)
{
  value_ = std::min(std::max(r, minimum()), maximum());

  QSlider::setValue(int(1000.0*(value_ - minimum())/(maximum() - minimum())));
}

void
CQRealSlider::
setMinimum(double r)
{
  minimum_ = r;

  emit rangeChanged(minimum_, maximum_);
}

void
CQRealSlider::
setMaximum(double r)
{
  maximum_ = r;

  emit rangeChanged(minimum_, maximum_);
}

void
CQRealSlider::
setRange(double minimum, double maximum)
{
  minimum_ = minimum;
  maximum_ = maximum;

  emit rangeChanged(minimum_, maximum_);
}

void
CQRealSlider::
setSingleStep(double r)
{
  singleStep_ = r;

  QSlider::setSingleStep(int(singleStep_*1000.0));
}

int
CQRealSlider::
valueToPos(double val) const
{
  double border = 2.0; // pixels

  double w = width() - 2*border - 2*dx_;

  return int((w*(val - minimum()))/(maximum() - minimum()) + border + dx_);
}

int
CQRealSlider::
valueWidthToPos(double val, int w) const
{
  int pos = valueToPos(val) - w/2;

  if      (pos <  0              ) pos = 0;
  else if (pos >= width() - w - 1) pos = width() - w - 1;

  return pos;
}

void
CQRealSlider::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  QStyleOptionSlider opt;

  initStyleOption(&opt);

  opt.rect.adjust(dx_, dy_, -dx_, -dy_);

  opt.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;

  if (tickPosition() != NoTicks)
    opt.subControls |= QStyle::SC_SliderTickmarks;

#if 0
  if (pressedControl) {
    opt.activeSubControls = pressedControl;

    opt.state |= QStyle::State_Sunken;
  }
  else {
    opt.activeSubControls = hoverControl;
  }
#endif

  style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);

  // draw ticks
  int y = height()/2 + sliderHeight_/2 + 2;

  p.setPen(QColor(0,0,0));

  drawTick(&p, minimum(), y, 4);
  drawTick(&p, maximum(), y, 4);

  for (double i = minimum(); i <= maximum(); i += pageStep())
    drawTick(&p, i, y, 2);

  // draw labels
  QFont smallFont = font();

  smallFont.setPointSizeF(0.8*smallFont.pointSizeF());

  QFontMetrics fm1(valFont_);
  QFontMetrics fm2(tickFont_);

  QString minLabel = QString("%1").arg(minimum());
  QString maxLabel = QString("%1").arg(maximum());

  QString valLabel;

  if (valueLabel_.isEmpty())
    valLabel = QString("%1").arg(value());
  else
    valLabel = QString("%1 %2").arg(value()).arg(valueLabel_);

  p.setFont(tickFont_);

  p.drawText(valueWidthToPos(minimum(), fm2.width(minLabel)), height() - fm2.descent(), minLabel);
  p.drawText(valueWidthToPos(maximum(), fm2.width(maxLabel)), height() - fm2.descent(), maxLabel);

  p.setFont(valFont_);

  p.drawText(valueWidthToPos(value(), fm1.width(valLabel)), fm1.ascent() + 4, valLabel);
}

void
CQRealSlider::
drawTick(QPainter *p, double val, int y, int s)
{
  int x = valueToPos(val);

  p->drawLine(x, y, x, y + s);
}

void
CQRealSlider::
valueChangedSlot(int v)
{
  value_ = minimum() + (maximum() - minimum())*v/1000.0;

  emit valueChanged(value_);
}

QSize
CQRealSlider::
sizeHint() const
{
  QFontMetrics fm(valFont_);

  QSize s = QSlider::sizeHint();

  dx_ = fm.width("X")/2;
  dy_ = fm.height() + 4;

  sliderHeight_ = s.height();

  // add enough space for char above and below + tick space
  return s + QSize(2*dx_, 2*dy_);
}
