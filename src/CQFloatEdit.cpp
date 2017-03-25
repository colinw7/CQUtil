#include <CQFloatEdit.h>
#include <CQAutoHide.h>

#include <QApplication>
#include <QKeyEvent>

CQFloatEdit::
CQFloatEdit(QWidget *parent) :
 QLineEdit(parent)
{
  connect(this, SIGNAL(editingFinished()), this, SLOT(acceptSlot()));

  hider_ = new CQAutoHide(this);

  hide();
}

CQFloatEdit::
~CQFloatEdit()
{
  delete hider_;
}

void
CQFloatEdit::
display(const QRect &rect, const QString &text)
{
  valid_ = true;

  setText(text);

  saveText_ = text;

  setParent(0, Qt::Tool | Qt::FramelessWindowHint);

  setGeometry(rect);

  show();
  raise();

  setFocus();

  qApp->setActiveWindow(this);

  selectAll();

  hider_->setActive(true);
}

bool
CQFloatEdit::
event(QEvent *e)
{
  if      (e->type() == QEvent::KeyPress) {
    QKeyEvent *ke = dynamic_cast<QKeyEvent *>(e);

    if (ke->key() == Qt::Key_Escape) {
      setText(saveText_);

      valid_ = false;

      hide();
    }
  }
  else if (e->type() == QEvent::Hide) {
    hider_->setActive(false);
  }

  return QLineEdit::event(e);
}

void
CQFloatEdit::
acceptSlot()
{
  if (valid_)
    emit valueChanged(text());

  hide();
}
