#include <CQSlider.h>
#include <CQRotatedText.h>

#include <QPainter>
#include <QStyleOptionSlider>

#include <cmath>

CQSliderBase::
CQSliderBase(QWidget *parent, Qt::Orientation orient) :
 QSlider(orient, parent)
{
  setTickPosition(TicksBelow);

  if (orient == Qt::Vertical)
    setInvertedAppearance(true);

  updateFonts();
}

void
CQSliderBase::
setLabel(const QString &label)
{
  label_ = label;

  update();
}

void
CQSliderBase::
setValueLabel(const QString &label)
{
  valueLabel_ = label;

  update();
}

void
CQSliderBase::
setValuePosition(const ValuePosition &v)
{
  valuePosition_ = v;

  update();
}

void
CQSliderBase::
setLabelSize(double r)
{
  labelSize_ = r;

  updateFonts();

  update();
}

void
CQSliderBase::
setTickLabelSize(double r)
{
  tickLabelSize_ = r;

  updateFonts();

  update();
}

void
CQSliderBase::
setLargeTickSize(int i)
{
  largeTickSize_ = i;

  update();
}

void
CQSliderBase::
setSmallTickSize(int i)
{
  smallTickSize_ = i;

  update();
}

void
CQSliderBase::
updateFonts()
{
  labelFont_     = font();
  tickLabelFont_ = font();

  labelFont_    .setPointSizeF(labelSize_    *labelFont_    .pointSizeF());
  tickLabelFont_.setPointSizeF(tickLabelSize_*tickLabelFont_.pointSizeF());
}

void
CQSliderBase::
drawLargeTick(QPainter *p, double val, int pos)
{
  drawTick(p, val, pos, largeTickSize_);
}

void
CQSliderBase::
drawSmallTick(QPainter *p, double val, int pos)
{
  drawTick(p, val, pos, smallTickSize_);
}

void
CQSliderBase::
drawTick(QPainter *p, double val, int pos, int s)
{
  int valPos = valueToPos(val);

  if (orientation() == Qt::Horizontal)
    p->drawLine(valPos, pos, valPos, pos + s);
  else
    p->drawLine(pos, valPos, pos + s, valPos);
}

//---

CQSlider::
CQSlider(QWidget *parent, Qt::Orientation orient) :
 CQSliderBase(parent, orient)
{
}

CQSlider::
CQSlider(Qt::Orientation orient) :
 CQSliderBase(nullptr, orient)
{
}

void
CQSlider::
setTickLabelDelta(int i)
{
  tickLabelDelta_ = i;

  update();
}

int
CQSlider::
valueToPos(double val) const
{
  double border = 2.0;

  if (orientation() == Qt::Horizontal) {
    double w = width() - 2*border - dx1_ - dx2_;

    return int((w*(val - minimum()))/(maximum() - minimum()) + border + dx1_ + dx2_);
  }
  else {
    double h = height() - 2*border - dy1_ - dy2_;

    return int((h*(val - minimum()))/(maximum() - minimum()) + border + dy1_ + dy2_);
  }
}

int
CQSlider::
valueWidthToPos(int val, int w) const
{
  int pos = valueToPos(val) - w/2;

  if (orientation() == Qt::Horizontal) {
    if      (pos <  0              ) pos = 0;
    else if (pos >= width() - w - 1) pos = width() - w - 1;
  }
  else {
    if      (pos <  0               ) pos = 0;
    else if (pos >= height() - w - 1) pos = height() - w - 1;
  }

  return pos;
}

void
CQSlider::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  //---

  // draw basic slider
  QStyleOptionSlider opt;

  initStyleOption(&opt);

  opt.rect.adjust(dx1_, dy1_, -dx2_, -dy2_);

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

  //---

  p.setPen(palette().windowText().color());

  //---

  // draw ticks
  if (tickPosition() != NoTicks) {
    auto drawLargeTicks = [&](int pos) {
      if (tickLabelDelta() != 0) {
        int value = minimum();

        while (value < maximum() + tickLabelDelta()) {
          if (value < maximum())
            drawLargeTick(&p, value, pos);
          else
            drawLargeTick(&p, maximum(), pos);

          value += tickLabelDelta();
        }
      }
      else {
        drawLargeTick(&p, minimum(), pos);
        drawLargeTick(&p, maximum(), pos);
      }
    };

    int pos1, pos2;

    if (orientation() == Qt::Horizontal) {
      pos1 = height()/2 - sliderSize_.height()/2 + 2;
      pos2 = height()/2 + sliderSize_.height()/2 + 2;
    }
    else {
      pos1 = width()/2 - sliderSize_.width()/2 + 2;
      pos2 = width()/2 + sliderSize_.width()/2 + 2;
    }

    auto hasSmallTicks = [&]() {
      if (minimum() > maximum() || pageStep() <= 0) return false;
      int n = int((maximum() - minimum())/pageStep() + 0.5);
      return (n < 1000);
    };

    if      (tickPosition() == TicksAbove) {
      drawLargeTicks(pos1);

      if (hasSmallTicks()) {
        for (int i = minimum(); i <= maximum(); i += pageStep())
          drawSmallTick(&p, i, pos1);
      }
    }
    else if (tickPosition() == TicksBelow) {
      drawLargeTicks(pos2);

      if (hasSmallTicks()) {
        for (int i = minimum(); i <= maximum(); i += pageStep())
          drawSmallTick(&p, i, pos2);
      }
    }
    else if (tickPosition() == TicksBothSides) {
      drawLargeTicks(pos1);
      drawLargeTicks(pos2);

      if (hasSmallTicks()) {
        for (int i = minimum(); i <= maximum(); i += pageStep()) {
          drawSmallTick(&p, i, pos1);
          drawSmallTick(&p, i, pos2);
        }
      }
    }
  }

  //---

  // draw tick labels
  QFontMetrics fm2(tickLabelFont_);

  p.setFont(tickLabelFont_);

  auto drawValueLabel = [&](int value) {
    auto valueStr = QString("%1").arg(value);

    int tw = fm2.horizontalAdvance(valueStr);

    if (orientation() == Qt::Horizontal)
      p.drawText(valueWidthToPos(value, tw), height() - fm2.descent(), valueStr);
    else
      CQRotatedText::drawRotatedText(&p, width() - fm2.descent(), valueToPos(value),
                                     valueStr, 90.0);
  };

  if (tickLabelDelta() != 0.0) {
    int value = minimum();

    while (value < maximum() + tickLabelDelta()) {
      if (value < maximum())
        drawValueLabel(value);
      else
        drawValueLabel(maximum());

      value += tickLabelDelta();
    }
  }
  else {
    drawValueLabel(minimum());
    drawValueLabel(maximum());
  }

  if (valuePosition() == ValuePosition::BELOW && value() > minimum() && value() < maximum()) {
    auto label = QString("%1").arg(value());

    int tw = fm2.horizontalAdvance(label);

    p.setPen(palette().highlight().color());

    if (orientation() == Qt::Horizontal) {
      int tx = valueWidthToPos(value(), tw);

      auto trect = QRect(tx, height() - fm2.height(), tw, fm2.height());

      p.fillRect(trect, palette().window());

      p.drawText(tx, height() - fm2.descent(), label);
    }
    else {
      int ty = valueToPos(value());

      auto trect = QRect(width() - tw, ty, tw, fm2.height());

      p.fillRect(trect, palette().window());

      CQRotatedText::drawRotatedText(&p, width() - fm2.descent(), ty, label, 90.0);
    }

    p.setPen(palette().windowText().color());
  }

  //---

  // draw value (and label)
  int vpos = 0;

  if (valuePosition() == ValuePosition::ABOVE) {
    QFontMetrics fm1(labelFont_);

    p.setFont(labelFont_);

    QString valLabel;

    if (! valueLabel_.isEmpty()) {
      if      (valueLabel_[0] == '_')
        valLabel = QString("%1%2").arg(value()).arg(valueLabel_.mid(1));
      else if (valueLabel_[valueLabel_.length() - 1] == '_')
        valLabel = QString("%1%2").arg(valueLabel_.mid(0, valueLabel_.length() - 1)).arg(value());
      else
        valLabel = QString("%1 %2").arg(value()).arg(valueLabel_);
    }
    else
      valLabel = QString("%1").arg(value());

    p.setPen(palette().highlight().color());

    int tw = fm1.horizontalAdvance(valLabel);

    if (orientation() == Qt::Horizontal) {
      vpos = valueWidthToPos(value(), tw);

      p.drawText(vpos, fm1.ascent() + 4, valLabel);
    }
    else {
      vpos = valueToPos(value());

      CQRotatedText::drawRotatedText(&p, fm1.ascent() + 4, vpos, valLabel, 90.0);
    }

    p.setPen(palette().windowText().color());
  }

  //---

  // draw label
  if (! label_.isEmpty()) {
    QFontMetrics fm1(labelFont_);

    p.setFont(labelFont_);

    int tw = fm1.horizontalAdvance(label_);

    int pos1 = valueToPos(minimum());
    int pos2 = pos1 + tw;

    if (orientation() == Qt::Horizontal) {
      int tx;

      if (valuePosition() == ValuePosition::NONE || vpos > pos2)
        tx = pos1;
      else
        tx = valueToPos(maximum()) - tw;

      p.drawText(tx, fm1.ascent() + 4, label_);
    }
    else {
      int ty;

      if (valuePosition() == ValuePosition::NONE || vpos > pos2)
        ty = pos1;
      else
        ty = valueToPos(maximum()) - tw;

      CQRotatedText::drawRotatedText(&p, fm1.ascent() + 4, ty, label_, 90.0);
    }
  }
}

QSize
CQSlider::
sizeHint() const
{
  auto s = CQSliderBase::sizeHint();

  sliderSize_ = s;

  //---

  dx1_ = 0;
  dx2_ = 0;
  dy1_ = 0;
  dy2_ = 0;

  //---

  QFontMetrics fm1(tickLabelFont_);

  if (orientation() == Qt::Horizontal) {
    dx1_ = fm1.horizontalAdvance("X")/2;
    dx2_ = dx1_;
    dy2_ = fm1.height() + 4;
  }
  else {
    dy1_ = fm1.horizontalAdvance("X")/2;
    dy2_ = dy1_;
    dx2_ = fm1.height() + 4;
  }

  //---

  if (valuePosition() != ValuePosition::NONE || ! label_.isEmpty()) {
    QFontMetrics fm2(labelFont_);

    if (orientation() == Qt::Horizontal)
      dy1_ = fm2.height() + 4;
    else
      dx1_ = fm2.height() + 4;
  }

  //---

  // add enough space for char above and below + tick space
  return s + QSize(dx1_ + dx2_, dy1_ + dy2_);
}

//-----

CQRealSlider::
CQRealSlider(QWidget *parent, Qt::Orientation orient) :
 CQSliderBase(parent, orient)
{
  init();
}

CQRealSlider::
CQRealSlider(Qt::Orientation orient) :
 CQSliderBase(nullptr, orient)
{
  init();
}

void
CQRealSlider::
init()
{
  CQSliderBase::setMinimum(0);
  CQSliderBase::setMaximum(int(scale_));

  CQSliderBase::setSingleStep(1);
  CQSliderBase::setPageStep(1);

  connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
}

void
CQRealSlider::
setValue(double r)
{
  if (minimum() < maximum())
    value_ = std::min(std::max(r, minimum()), maximum());
  else
    value_ = std::min(std::max(r, maximum()), minimum());

  value_ = std::round(value_/singleStep_)*singleStep_;

  disconnect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));

  CQSliderBase::setValue(int(scale_*(value_ - minimum())/(maximum() - minimum())));

  connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));

  update();
}

void
CQRealSlider::
setMinimum(double r)
{
  minimum_ = r;

  update();

  Q_EMIT rangeChanged(minimum_, maximum_);
}

void
CQRealSlider::
setMaximum(double r)
{
  maximum_ = r;

  update();

  Q_EMIT rangeChanged(minimum_, maximum_);
}

void
CQRealSlider::
setRange(double minimum, double maximum)
{
  minimum_ = minimum;
  maximum_ = maximum;

  update();

  Q_EMIT rangeChanged(minimum_, maximum_);
}

void
CQRealSlider::
setSingleStep(double r)
{
  singleStep_ = r;

  int is = int(scale_*(singleStep_/(maximum() - minimum())));

  CQSliderBase::setSingleStep(is);
}

void
CQRealSlider::
setPageStep(double r)
{
  pageStep_ = r;

  int is = int(scale_*(pageStep_/(maximum() - minimum())));

  CQSliderBase::setPageStep(is);
}

void
CQRealSlider::
setPrecision(int i)
{
  precision_ = i;

  scale_ = std::pow(10, std::max(precision_, 0));

  CQSliderBase::setMaximum(int(scale_));
}

void
CQRealSlider::
setTickLabelDelta(double r)
{
  tickLabelDelta_ = r;

  update();
}

int
CQRealSlider::
valueToPos(double val) const
{
  double border = 2.0; // pixels

  if (orientation() == Qt::Horizontal) {
    double w = width() - 2*border - dx1_ - dx2_;

    return int((w*(val - minimum()))/(maximum() - minimum()) + border + dx1_ + dx2_);
  }
  else {
    double h = height() - 2*border - dy1_ - dy2_;

    return int((h*(val - minimum()))/(maximum() - minimum()) + border + dy1_ + dy2_);
  }
}

int
CQRealSlider::
valueWidthToPos(double val, int w) const
{
  int pos = valueToPos(val) - w/2;

  if (orientation() == Qt::Horizontal) {
    if      (pos <  0              ) pos = 0;
    else if (pos >= width() - w - 1) pos = width() - w - 1;
  }
  else {
    if      (pos <  0               ) pos = 0;
    else if (pos >= height() - w - 1) pos = height() - w - 1;
  }

  return pos;
}

double
CQRealSlider::
posToValue(int pos) const
{
  double border = 2.0; // pixels

  double value = 0.0;

  if (orientation() == Qt::Horizontal) {
    double w = width() - 2*border - dx1_ - dx2_;

    value = double(pos - dx1_ - dx2_ - border)*(maximum() - minimum())/w + minimum();
  }
  else {
    double h = height() - 2*border - dy1_ - dy2_;

    value = double(pos - dy1_ - dy2_ - border)*(maximum() - minimum())/h + minimum();
  }

  return value;
}

void
CQRealSlider::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  //---

  // draw basic slider
  QStyleOptionSlider opt;

  initStyleOption(&opt);

  opt.rect.adjust(dx1_, dy1_, -dx2_, -dy2_);

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

  //---

  p.setPen(palette().windowText().color());

  //---

  // draw ticks
  if (tickPosition() != NoTicks) {
    auto drawLargeTicks = [&](int y) {
      if (tickLabelDelta() != 0.0) {
        double value = minimum();

        while (value < maximum() + tickLabelDelta()) {
          if (value < maximum())
            drawLargeTick(&p, value, y);
          else
            drawLargeTick(&p, maximum(), y);

          value += tickLabelDelta();
        }
      }
      else {
        drawLargeTick(&p, minimum(), y);
        drawLargeTick(&p, maximum(), y);
      }
    };

    int pos1, pos2;

    if (orientation() == Qt::Horizontal) {
      pos1 = height()/2 - sliderSize_.height()/2 + 2;
      pos2 = height()/2 + sliderSize_.height()/2 + 2;
    }
    else {
      pos1 = width()/2 - sliderSize_.width()/2 + 2;
      pos2 = width()/2 + sliderSize_.width()/2 + 2;
    }

    auto hasSmallTicks = [&]() {
      if (minimum() > maximum() || pageStep() <= 0) return false;
      auto n = (maximum() - minimum())/pageStep() + 0.5;
      return (n < 1000);
    };

    if      (tickPosition() == TicksAbove) {
      drawLargeTicks(pos1);

      if (hasSmallTicks()) {
        for (double i = minimum(); i <= maximum(); i += pageStep())
          drawSmallTick(&p, i, pos1);
      }
    }
    else if (tickPosition() == TicksBelow) {
      drawLargeTicks(pos2);

      if (hasSmallTicks()) {
        for (double i = minimum(); i <= maximum(); i += pageStep())
          drawSmallTick(&p, i, pos2);
      }
    }
    else if (tickPosition() == TicksBothSides) {
      drawLargeTicks(pos1);
      drawLargeTicks(pos2);

      if (hasSmallTicks()) {
        for (double i = minimum(); i <= maximum(); i += pageStep()) {
          drawSmallTick(&p, i, pos1);
          drawSmallTick(&p, i, pos2);
        }
      }
    }
  }

  //---

  // draw tick labels
  QFontMetrics fm2(tickLabelFont_);

  p.setFont(tickLabelFont_);

  auto drawValueLabel = [&](double value) {
    auto valueStr = QString("%1").arg(value);

    int tw = fm2.horizontalAdvance(valueStr);

    if (orientation() == Qt::Horizontal)
      p.drawText(valueWidthToPos(value, tw), height() - fm2.descent(), valueStr);
    else
      CQRotatedText::drawRotatedText(&p, width() - fm2.descent(), valueToPos(value),
                                     valueStr, 90.0);
  };

  if (tickLabelDelta() != 0.0) {
    double value = minimum();

    while (value < maximum() + tickLabelDelta()) {
      if (value < maximum())
        drawValueLabel(value);
      else
        drawValueLabel(maximum());

      value += tickLabelDelta();
    }
  }
  else {
    drawValueLabel(minimum());
    drawValueLabel(maximum());
  }

  if (valuePosition() == ValuePosition::BELOW && value() > minimum() && value() < maximum()) {
    auto label = QString("%1").arg(value());

    int tw = fm2.horizontalAdvance(label);

    p.setPen(palette().highlight().color());

    if (orientation() == Qt::Horizontal) {
      int tx = valueWidthToPos(value(), tw);

      auto trect = QRect(tx, height() - fm2.height(), tw, fm2.height());

      p.fillRect(trect, palette().window());

      p.drawText(tx, height() - fm2.descent(), label);
    }
    else {
      int ty = valueToPos(value());

      auto trect = QRect(width() - tw, ty, tw, fm2.height());

      p.fillRect(trect, palette().window());

      CQRotatedText::drawRotatedText(&p, width() - fm2.descent(), ty, label, 90.0);
    }

    p.setPen(palette().windowText().color());
  }

  //---

  // draw value (and label)
  int vpos = 0;
  int vtw  = 0;

  if (valuePosition() == ValuePosition::ABOVE) {
    QFontMetrics fm1(labelFont_);

    p.setFont(labelFont_);

    QString valLabel;

    if (! valueLabel_.isEmpty()) {
      if      (valueLabel_[0] == '_')
        valLabel = QString("%1%2").arg(value()).arg(valueLabel_.mid(1));
      else if (valueLabel_[valueLabel_.length() - 1] == '_')
        valLabel = QString("%1%2").arg(valueLabel_.mid(0, valueLabel_.length() - 1)).arg(value());
      else
        valLabel = QString("%1 %2").arg(value()).arg(valueLabel_);
    }
    else
      valLabel = QString("%1").arg(value());

    p.setPen(palette().highlight().color());

    vtw = fm1.horizontalAdvance(valLabel);

    if (orientation() == Qt::Horizontal) {
      vpos = valueWidthToPos(value(), vtw);

      p.drawText(vpos, fm1.ascent() + 4, valLabel);
    }
    else {
      vpos = valueToPos(value());

      CQRotatedText::drawRotatedText(&p, fm1.ascent() + 4, vpos, valLabel, 90.0);
    }

    p.setPen(palette().windowText().color());
  }

  //---

  // draw label
  if (! label_.isEmpty()) {
    QFontMetrics fm1(labelFont_);

    p.setFont(labelFont_);

    int tw = fm1.horizontalAdvance(label_);

    int pos1 = valueToPos(minimum());
    int pos2 = pos1 + tw;
    int pos4 = valueToPos(maximum());
    int pos3 = pos4 - tw;

    if (orientation() == Qt::Horizontal) {
      bool draw = true;

      int tx;

      if      (valuePosition() == ValuePosition::BELOW) {
        if (pos2 < vpos)
          tx = pos1;
        else
          draw = false;
      }
      else if (valuePosition() == ValuePosition::BELOW) {
        if (pos3 > vpos + vtw)
          tx = pos3;
        else
          draw = false;
      }
      else {
        if      (pos2 < vpos)
          tx = pos1;
        else if (pos3 > vpos + vtw)
          tx = pos3;
        else
          draw = false;
      }

      if (draw)
        p.drawText(tx, fm1.ascent() + 4, label_);
    }
    else {
      int ty;

      if (valuePosition() == ValuePosition::NONE || vpos > pos2)
        ty = pos1;
      else
        ty = valueToPos(maximum()) - tw;

      CQRotatedText::drawRotatedText(&p, fm1.ascent() + 4, ty, label_, 90.0);
    }
  }
}

void
CQRealSlider::
valueChangedSlot(int v)
{
  value_ = integerToReal(v, /*snap*/true);

  int v1 = realToInteger(value_);

  if (v != v1) {
    disconnect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));

    CQSliderBase::setValue(v1);

    connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
  }

  Q_EMIT valueChanged(value_);
}

QSize
CQRealSlider::
sizeHint() const
{
  auto s = CQSliderBase::sizeHint();

  sliderSize_ = s;

  //---

  dx1_ = 0;
  dx2_ = 0;
  dy1_ = 0;
  dy2_ = 0;

  //---

  QFontMetrics fm1(tickLabelFont_);

  if (orientation() == Qt::Horizontal) {
    dx1_ = fm1.horizontalAdvance("X")/2;
    dx2_ = dx1_;
    dy2_ = fm1.height() + 4;
  }
  else {
    dy1_ = fm1.horizontalAdvance("X")/2;
    dy2_ = dy1_;
    dx2_ = fm1.height() + 4;
  }

  //---

  if (valuePosition() != ValuePosition::NONE || ! label_.isEmpty()) {
    QFontMetrics fm2(labelFont_);

    if (orientation() == Qt::Horizontal)
      dy1_ = fm2.height() + 4;
    else
      dx1_ = fm2.height() + 4;
  }

  //---

  // add enough space for char above and below + tick space
  return s + QSize(dx1_ + dx2_, dy1_ + dy2_);
}

double
CQRealSlider::
integerToReal(int ivalue, bool snap) const
{
  // integer value (0 - scale_)
  // real value (minimum() -> maximum())

  double f      = double(ivalue)/scale_;
  double rvalue = minimum() + f*(maximum() - minimum());

  if (snap)
    rvalue = std::round(rvalue/singleStep_)*singleStep_;

  return rvalue;
}

int
CQRealSlider::
realToInteger(double rvalue) const
{
  double f = (rvalue - minimum())/(maximum() - minimum());

  int ivalue = std::round(scale_*f);

  return ivalue;
}
