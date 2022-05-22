#ifndef CQSlider_H
#define CQSlider_H

#include <QSlider>

class CQSlider : public QSlider {
  Q_OBJECT

  Q_PROPERTY(QString valueLabel READ valueLabel WRITE setValueLabel)

 public:
  CQSlider(QWidget *parent=nullptr);

  const QString &valueLabel() const { return valueLabel_; }
  void setValueLabel(const QString &label);

  QSize sizeHint() const override;

 private:
  void paintEvent(QPaintEvent *) override;

  int valueToPos(int v) const;

  int valueWidthToPos(int v, int width) const;

  void drawTick(QPainter *p, int val, int y, int s);

 private:
  QString     valueLabel_;
  QFont       valFont_;
  QFont       tickFont_;
  mutable int sliderHeight_ { 0 };
  mutable int dx_           { 0 };
  mutable int dy_           { 0 };
};

//---

class CQRealSlider : public QSlider {
  Q_OBJECT

  Q_PROPERTY(QString valueLabel READ valueLabel WRITE setValueLabel)
  Q_PROPERTY(double  minimum    READ minimum    WRITE setMinimum   )
  Q_PROPERTY(double  maximum    READ maximum    WRITE setMaximum   )
  Q_PROPERTY(double  singleStep READ singleStep WRITE setSingleStep)

 public:
  CQRealSlider(QWidget *parent=nullptr);

  const QString &valueLabel() const { return valueLabel_; }
  void setValueLabel(const QString &label);

  double minimum() const { return minimum_; }
  void setMinimum(double r);

  double maximum() const { return maximum_; }
  void setMaximum(double r);

  void setRange(double minimum, double maximum);

  double singleStep() const { return singleStep_; }
  void setSingleStep(double r);

  double value() const { return value_; }
  void setValue(double r);

  QSize sizeHint() const override;

 private:
  void paintEvent(QPaintEvent *) override;

  int valueToPos(double v) const;

  int valueWidthToPos(double v, int width) const;

  void drawTick(QPainter *p, double val, int y, int s);

 signals:
  void valueChanged(double);
  void rangeChanged(double, double);

 private slots:
  void valueChangedSlot(int);

 private:
  QString valueLabel_;
  double  minimum_    { 0.0 };
  double  maximum_    { 1.0 };
  double  singleStep_ { 0.01 };
  double  value_      { 0.0 };
  QFont   valFont_;
  QFont   tickFont_;

  mutable int sliderHeight_ { 0 };
  mutable int dx_           { 0 };
  mutable int dy_           { 0 };
};

#endif
