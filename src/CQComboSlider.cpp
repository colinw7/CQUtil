#include <CQComboSlider.h>
#include <CQRealSpin.h>
#include <CQIntegerSpin.h>

#include <QSlider>
#include <QHBoxLayout>

CQComboSlider::
CQComboSlider(QWidget *parent) :
 QWidget(parent)
{
  init();
}

CQComboSlider::
CQComboSlider(QWidget *parent, double defValue, double minimum, double maximum) :
 QWidget(parent), defValue_(defValue), minValue_(minimum), maxValue_(maximum)
{
  init();
}

void
CQComboSlider::
init()
{
  setObjectName("comboSlider");

  setLayout(new QHBoxLayout());
  layout()->setContentsMargins(0, 0, 0, 0);

  value_ = defValue_;

  slider_ = new QSlider(Qt::Horizontal, this);
  slider_->setRange(0, 100000);

  spinner_ = new CQRealSpin(this);
  spinner_->setDecimals(5);

  layout()->addWidget(slider_);
  layout()->addWidget(spinner_);

  connect(spinner_, SIGNAL(valueChanged(double)), this, SLOT(spinnerChanged(double)));
  connect(slider_ , SIGNAL(valueChanged(int))   , this, SLOT(sliderChanged(int)));

  updateWidgets();
}

void
CQComboSlider::
setRange(double minValue, double maxValue)
{
  minValue_ = minValue;
  maxValue_ = maxValue;

  updateWidgets();
}

void
CQComboSlider::
setValue(double d)
{
  value_ = d;

  updateWidgets();

  emit valueChanged(value_);
}

void
CQComboSlider::
updateWidgets()
{
  updateSlider ();
  updateSpinner();
}

void
CQComboSlider::
updateSlider()
{
  slider_->blockSignals(true);

  double val = 0.0;

  if (maxValue_ > minValue_)
    val = (value_ - minValue_)/(maxValue_ - minValue_);

  slider_->setValue(val*100000);

  slider_->blockSignals(false);
}

void
CQComboSlider::
updateSpinner()
{
  spinner_->blockSignals(true);

  spinner_->setMaximum(maxValue_);
  spinner_->setMinimum(minValue_);
  spinner_->setValue(value_);

  spinner_->blockSignals(false);
}

void
CQComboSlider::
setSliderValue(double d)
{
  value_ = d;

  updateSlider();

  emit valueChanged(value_);
}

void
CQComboSlider::
setSpinnerValue(double d)
{
  value_ = d;

  updateSpinner();

  emit valueChanged(value_);
}

void
CQComboSlider::
spinnerChanged(double)
{
  setSliderValue(spinner_->value());
}

void
CQComboSlider::
sliderChanged(int)
{
  double val = 0.0;

  if (maxValue_ > minValue_)
    val = (slider_->value()/100000.0)*(maxValue_ - minValue_) + minValue_;

  setSpinnerValue(val);
}

//------

CQIntComboSlider::
CQIntComboSlider(QWidget* parent) :
 QWidget(parent)
{
  init();
}

CQIntComboSlider::
CQIntComboSlider(QWidget* parent, int defValue, int minimum, int maximum) :
 QWidget(parent), defValue_(defValue), minValue_(minimum), maxValue_(maximum)
{
  init();
}

void
CQIntComboSlider::
init()
{
  setObjectName("intComboSlider");

  setLayout(new QHBoxLayout());
  layout()->setContentsMargins(0, 0, 0, 0);

  value_ = defValue_;

  slider_ = new QSlider(Qt::Horizontal, this);

  spinner_ = new CQIntegerSpin(this);

  layout()->addWidget(slider_);
  layout()->addWidget(spinner_);

  connect(spinner_, SIGNAL(valueChanged(int)), this, SLOT(spinnerChanged(int)));
  connect(slider_ , SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));

  updateWidgets();
}

void
CQIntComboSlider::
setRange(int minValue, int maxValue)
{
  minValue_ = minValue;
  maxValue_ = maxValue;

  updateWidgets();
}

void
CQIntComboSlider::
setValue(int i)
{
  value_ = i;

  updateWidgets();

  emit valueChanged(value_);
}

void
CQIntComboSlider::
updateWidgets()
{
  updateSlider ();
  updateSpinner();
}

void
CQIntComboSlider::
updateSlider()
{
  slider_->blockSignals(true);

  slider_->setRange(minValue_, maxValue_);
  slider_->setValue(value_);

  slider_->blockSignals(false);
}

void
CQIntComboSlider::
updateSpinner()
{
  spinner_->blockSignals(true);

  spinner_->setMaximum(maxValue_);
  spinner_->setMinimum(minValue_);
  spinner_->setValue(value_);

  spinner_->blockSignals(false);
}

void
CQIntComboSlider::
setSliderValue(int i)
{
  value_ = i;

  updateSlider();

  emit valueChanged(value_);
}

void
CQIntComboSlider::
setSpinnerValue(int i)
{
  value_ = i;

  updateSpinner();

  emit valueChanged(value_);
}

void
CQIntComboSlider::
spinnerChanged(int)
{
  setSliderValue(spinner_->value());
}

void
CQIntComboSlider::
sliderChanged(int)
{
  setSpinnerValue(slider_->value());
}
