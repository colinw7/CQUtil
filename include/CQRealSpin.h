#ifndef CQRealSpin_H
#define CQRealSpin_H

#include <QDoubleSpinBox>

/*!
 * \brief Extension of QDoubleSpinBox to allow sping box to increment the digit at the
 * current cursor position
 */
class CQRealSpin : public QDoubleSpinBox {
  Q_OBJECT

  Q_PROPERTY(bool   autoStep READ isAutoStep WRITE setAutoStep)
  Q_PROPERTY(double step     READ step)

 public:
  CQRealSpin(QWidget *parent, double value=0.0);
  CQRealSpin(double value=0.0);

  virtual ~CQRealSpin() { }

  //! get/set auto step
  bool isAutoStep() const { return autoStep_; }
  void setAutoStep(bool b) { autoStep_ = b; }

  //! get current step
  double step() const { return step_; }

  //! step by n
  void stepBy(int n) override;

  //! get/set cursor position
  int cursorPosition() const;
  void setCursorPosition(int pos);

  void setPlaceholderText(const QString &str);

  bool event(QEvent *event) override;

 Q_SIGNALS:
  void realValueChanged(double);

  //! emitted when step changed
  void stepChanged(double);

  void keyPress(int key, int modifiers);

 private Q_SLOTS:
  //! update step from cursor position
  void updateStep();

 private:
  //! init widget with current value
  void init(double value);

  //! calc step for cursor position
  double posToStep(int pos) const;

  //! check if value is negative
  bool isNegative() const;

  //! find position of decimal point
  int dotPos() const;

 private:
  bool   autoStep_ { true }; //!< support increment using cursor position
  double step_     { 1 };    //!< current step increment
};

#endif
