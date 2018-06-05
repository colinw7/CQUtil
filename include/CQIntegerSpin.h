#ifndef CQINTEGER_SPIN_H
#define CQINTEGER_SPIN_H

#include <QSpinBox>

class CQIntegerSpin : public QSpinBox {
  Q_OBJECT

  Q_PROPERTY(bool autoStep READ isAutoStep WRITE setAutoStep)
  Q_PROPERTY(int  step     READ step)

 public:
  CQIntegerSpin(QWidget *parent, int value=0);
  CQIntegerSpin(int value=0);

  virtual ~CQIntegerSpin() { }

  bool isAutoStep() const { return autoStep_; }
  void setAutoStep(bool b) { autoStep_ = b; }

  int cursorPosition() const;
  void setCursorPosition(int pos);

  int step() const { return step_; }

  void stepBy(int n);

 signals:
  void stepChanged(int);

 private slots:
  void updateStep();

 private:
  void init(int value);

  int posToStep(int pos) const;

  bool isNegative() const;

 private:
  bool autoStep_ { true };
  int  step_     { 1 };
};

#endif
