#include <CQIntegerSpin.h>
#include <QLineEdit>

CQIntegerSpin::
CQIntegerSpin(QWidget *parent, int value) :
 QSpinBox(parent)
{
  init(value);
}

CQIntegerSpin::
CQIntegerSpin(int value) :
 QSpinBox(0)
{
  init(value);
}

void
CQIntegerSpin::
init(int value)
{
  setRange(-INT_MAX, INT_MAX);

  setValue(value);

  connect(lineEdit(), SIGNAL(cursorPositionChanged(int,int)), this, SLOT(updateStep()));

  updateStep();
}

void
CQIntegerSpin::
updateStep()
{
  int pos = lineEdit()->cursorPosition();

  int s = posToStep(pos);

  if (step() != s) {
    step_ = s;

    emit stepChanged(step());
  }
}

int
CQIntegerSpin::
posToStep(int pos) const
{
  bool negative = isNegative();

  if (! negative) {
    if (pos < 1)
      pos = 1;
  }
  else {
    if (pos < 2)
      pos = 2;
  }

  int d = text().length() - pos;

  return pow(10, d);
}

void
CQIntegerSpin::
stepBy(int n)
{
  int v = value();
  int s = step();

  int l   = text().length();
  int pos = lineEdit()->cursorPosition();

  bool negative = isNegative();

  if (! negative) {
    if (pos < 1)
      pos = 1;
  }
  else {
    if (pos < 2)
      pos = 2;
  }

  setValue(v + n*s);

  int l1 = text().length();

  int pos1 = l1 - l + pos;

  if (pos1 != pos)
    lineEdit()->setCursorPosition(pos1);

  updateStep();
}

bool
CQIntegerSpin::
isNegative() const
{
  const QString &text = lineEdit()->text();

  return (text.length() && text[0] == '-');
}
