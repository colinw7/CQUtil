#ifndef CQSlider_H
#define CQSlider_H

#include <QSlider>

class CQSliderBase : public QSlider {
  Q_OBJECT

  Q_PROPERTY(QString label     READ label     WRITE setLabel    )
  Q_PROPERTY(double  labelSize READ labelSize WRITE setLabelSize)

  Q_PROPERTY(QString       valueLabel    READ valueLabel    WRITE setValueLabel)
  Q_PROPERTY(ValuePosition valuePosition READ valuePosition WRITE setValuePosition)

  Q_PROPERTY(double tickLabelSize READ tickLabelSize WRITE setTickLabelSize)

  Q_PROPERTY(int largeTickSize READ largeTickSize WRITE setLargeTickSize)
  Q_PROPERTY(int smallTickSize READ smallTickSize WRITE setSmallTickSize)

  Q_ENUMS(ValuePosition)

 public:
  enum class ValuePosition {
    NONE,
    ABOVE,
    BELOW
  };

  CQSliderBase(QWidget *parent=nullptr, Qt::Orientation orient=Qt::Horizontal);

  //---

  const QString &label() const { return label_; }
  void setLabel(const QString &label);

  double labelSize() const { return labelSize_; }
  void setLabelSize(double r);

  //---

  const QString &valueLabel() const { return valueLabel_; }
  void setValueLabel(const QString &label);

  const ValuePosition &valuePosition() const { return valuePosition_; }
  void setValuePosition(const ValuePosition &v);

  //---

  double tickLabelSize() const { return tickLabelSize_; }
  void setTickLabelSize(double r);

  //---

  int largeTickSize() const { return largeTickSize_; }
  void setLargeTickSize(int i);

  int smallTickSize() const { return smallTickSize_; }
  void setSmallTickSize(int i);

 protected:
  void updateFonts();

  virtual int valueToPos(double v) const = 0;

  void drawLargeTick(QPainter *p, double val, int y);
  void drawSmallTick(QPainter *p, double val, int y);
  void drawTick     (QPainter *p, double val, int y, int s);

 protected:
  QString label_;
  double  labelSize_ { 0.9 };

  QString       valueLabel_;
  ValuePosition valuePosition_ { ValuePosition::ABOVE };

  double tickLabelSize_ { 0.8 };

  int largeTickSize_ { 4 };
  int smallTickSize_ { 2 };

  mutable QFont labelFont_;
  mutable QFont tickLabelFont_;

  mutable QSize sliderSize_;

  mutable int dx1_ { 0 };
  mutable int dx2_ { 0 };
  mutable int dy1_ { 0 };
  mutable int dy2_ { 0 };
};

//---

class CQSlider : public CQSliderBase {
  Q_OBJECT

  Q_PROPERTY(int tickLabelDelta READ tickLabelDelta WRITE setTickLabelDelta)

 public:
  CQSlider(QWidget *parent=nullptr, Qt::Orientation orient=Qt::Horizontal);
  CQSlider(Qt::Orientation orient=Qt::Horizontal);

  int tickLabelDelta() const { return tickLabelDelta_; }
  void setTickLabelDelta(int i);

  QSize sizeHint() const override;

 private:
  void paintEvent(QPaintEvent *) override;

  int valueToPos(double v) const override;

  int valueWidthToPos(int v, int width) const;

 private:
  int tickLabelDelta_ { 0 };
};

//---

class CQRealSlider : public CQSliderBase {
  Q_OBJECT

  Q_PROPERTY(double minimum    READ minimum    WRITE setMinimum   )
  Q_PROPERTY(double maximum    READ maximum    WRITE setMaximum   )
  Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep)
  Q_PROPERTY(double pageStep   READ pageStep   WRITE setPageStep  )
  Q_PROPERTY(int    precision  READ precision  WRITE setPrecision )

  Q_PROPERTY(double tickLabelDelta READ tickLabelDelta WRITE setTickLabelDelta)

 public:
  CQRealSlider(QWidget *parent=nullptr, Qt::Orientation orient=Qt::Horizontal);
  CQRealSlider(Qt::Orientation orient=Qt::Horizontal);

  //---

  double minimum() const { return minimum_; }
  void setMinimum(double r);

  double maximum() const { return maximum_; }
  void setMaximum(double r);

  void setRange(double minimum, double maximum);

  //---

  double singleStep() const { return singleStep_; }
  void setSingleStep(double r);

  double pageStep() const { return pageStep_; }
  void setPageStep(double r);

  int precision() const { return precision_; }
  void setPrecision(int i);

  //---

  double value() const { return value_; }
  void setValue(double r);

  double tickLabelDelta() const { return tickLabelDelta_; }
  void setTickLabelDelta(double r);

  //---

  int valueToPos(double v) const override;
  double posToValue(int pos) const;

  //---

  QSize sizeHint() const override;

 private:
  void init();

  void paintEvent(QPaintEvent *) override;

  int valueWidthToPos(double v, int width) const;

 Q_SIGNALS:
  void valueChanged(double);
  void rangeChanged(double, double);

 private Q_SLOTS:
  void valueChangedSlot(int);


  double integerToReal(int ivalue, bool snap) const;
  int realToInteger(double rvalue) const;

 private:
  double minimum_    { 0.0 };
  double maximum_    { 1.0 };
  double singleStep_ { 0.01 };
  double pageStep_   { 0.1 };
  double value_      { 0.0 };

  double tickLabelDelta_ { 0.0 };

  int    precision_ { 3 };
  double scale_     { 1000.0 };
};

#endif
