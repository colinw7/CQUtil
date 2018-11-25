#include <CQRectEdit.h>

namespace CQRectEditUtil {

bool toReal(const std::string &s, double *r) {
  bool ok = true;

  *r = 0.0;

  try {
    *r = std::stod(s);
  }
  catch (...) {
    ok = false;
  }

  return ok;
}

}

//---

CQRectEdit::
CQRectEdit(QWidget *parent, const QRectF &value) :
 QFrame(parent)
{
  init(value);
}

CQRectEdit::
CQRectEdit(const QRectF &value) :
 QFrame(nullptr)
{
  init(value);
}

void
CQRectEdit::
init(const QRectF &value)
{
  setObjectName("edit");

  setFrameStyle(QFrame::NoFrame | QFrame::Plain);

  //---

  edit_ = new QLineEdit;

  edit_->setObjectName("edit");
  edit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  connect(edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));

  //---

  layout_ = new QHBoxLayout(this);
  layout_->setMargin(0); layout_->setSpacing(0);

  layout_->addWidget(edit_);

  //---

  setValue(value);
}

void
CQRectEdit::
setValue(const QRectF &rect)
{
  rect_ = rect;

  rectToWidget();
}

const QRectF &
CQRectEdit::
getValue() const
{
  return rect_;
}

void
CQRectEdit::
editingFinishedI()
{
  if (disableSignals_) return;

  widgetToPoint();

  emit valueChanged();
}

void
CQRectEdit::
rectToWidget()
{
  disableSignals_ = true;

  QString str = QString("%1 %2 %3 %4").arg(rect_.left ()).arg(rect_.bottom()).
                                       arg(rect_.right()).arg(rect_.top   ());

  edit_->setText(str);

  disableSignals_ = false;
}

bool
CQRectEdit::
widgetToPoint()
{
  QStringList strs = edit_->text().split(" ", QString::SkipEmptyParts);

  if (strs.length() != 4)
    return false;

  bool ok1, ok2, ok3, ok4;

  double x1 = strs[0].toDouble(&ok1);
  double y1 = strs[1].toDouble(&ok2);
  double x2 = strs[2].toDouble(&ok3);
  double y2 = strs[3].toDouble(&ok4);

  if (! ok1 || ! ok2 || ! ok3 || ! ok4)
    return false;

  rect_ = QRectF(x1, y1, x2 - x1, y2 - y1);

  return true;
}
