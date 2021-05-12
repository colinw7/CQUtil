#ifndef CQComboSlider_H
#define CQComboSlider_H

#include <QWidget>

class CQRealSpin;
class CQIntegerSpin;

class QSlider;
class QSpinBox;

// combined double slider+spinner
class CQComboSlider : public QWidget {
  Q_OBJECT

  Q_PROPERTY(double value READ value WRITE setValue)

 public:
  CQComboSlider(QWidget *parent=nullptr);
  CQComboSlider(QWidget *parent, double defValue, double minimum, double maximum);

  void setRange(double minValue, double maxValue);

  double value() const { return value_; }
  void setValue(double d);

  double getSpan() const { return maxValue_ - minValue_; }

 private:
  void init();

  void updateWidgets();
  void updateSlider ();
  void updateSpinner();

  void setSliderValue (double d);
  void setSpinnerValue(double d);

 signals:
  void valueChanged(double);

 protected slots:
  void spinnerChanged(double);

  void sliderChanged(int);

 private:
  double      defValue_ { 0.0 };
  double      minValue_ { 0.0 };
  double      maxValue_ { 1.0 };
  double      value_    { 0.0 };
  QSlider*    slider_   { nullptr };
  CQRealSpin* spinner_  { nullptr };
};

//------

// combined int slider+spinner
class CQIntComboSlider : public QWidget {
  Q_OBJECT

  Q_PROPERTY(int value READ value WRITE setValue)

 public:
  CQIntComboSlider(QWidget* parent=nullptr);
  CQIntComboSlider(QWidget* parent, int defValue, int minimum, int maximum);

  void setRange(int minValue, int maxValue);

  int value() const { return value_; }
  void setValue(int i);

 private:
  void init();

  void updateWidgets();
  void updateSlider ();
  void updateSpinner();

  void setSliderValue (int i);
  void setSpinnerValue(int i);

 signals:
  void valueChanged(int);

 protected slots:
  void spinnerChanged(int);

  void sliderChanged(int);

 private:
  int            defValue_ { 0 };
  int            minValue_ { 0 };
  int            maxValue_ { 100 };
  int            value_    { 0 };
  QSlider*       slider_   { nullptr };
  CQIntegerSpin* spinner_  { nullptr };
};

#endif
