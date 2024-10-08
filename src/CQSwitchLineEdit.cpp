#include <CQSwitchLineEdit.h>
#include <CQUtil.h>
#include <CQIconButton.h>

#include <QLineEdit>
#include <QKeyEvent>
#include <iostream>

#include <svg/switch_edit_svg.h>

CQSwitchLineEdit::
CQSwitchLineEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("switch");

  edit_   = CQUtil::makeWidget<QLineEdit>(this, "edit");
  button_ = CQUtil::makeWidget<CQIconButton>(this, "switch");

  button_->setAutoRaise(true);
  button_->setIcon("SWITCH_EDIT");
  button_->setFocusPolicy(Qt::NoFocus);
  button_->setToolTip("Switch between custom edit to line edit");

  connect(edit_, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
  connect(button_, SIGNAL(clicked()), this, SLOT(toggleAltEdit()));

  setFocusProxy(edit_);

  updatePlacement();
}

QString
CQSwitchLineEdit::
text() const
{
  return edit_->text();
}

void
CQSwitchLineEdit::
setText(const QString &text)
{
  edit_->setText(text);
}

void
CQSwitchLineEdit::
setAltEdit(QWidget *edit)
{
  altEdit_ = edit;

  if (altEdit_) {
    altEdit_->setParent(this);

    if (altEdit_->focusProxy())
      altEdit_->focusProxy()->installEventFilter(this);
    else
      altEdit_->installEventFilter(this);
  }

  updatePlacement();
}

void
CQSwitchLineEdit::
toggleAltEdit()
{
  setShowAltEdit(! isAlt_);
}

void
CQSwitchLineEdit::
setShowAltEdit(bool isAlt)
{
  //std::cerr << "Show Alt Edit" << isAlt << "\n";

  if (! altEdit_)
    isAlt = false;

  if (isAlt != isAlt_) {
    isAlt_ = isAlt;

    updatePlacement();

    Q_EMIT editSwitched(isAlt_);
  }
}

void
CQSwitchLineEdit::
updatePlacement()
{
  if (! altEdit_)
    isAlt_ = false;

  edit_->setVisible(! isShowAltEdit());

  if (altEdit_)
    altEdit_->setVisible(isShowAltEdit());

  int bw = 0;

  if (altEdit_)
    bw = button_->sizeHint().width();

  button_->setVisible(altEdit_);

  if (! isShowAltEdit()) {
    edit_->move(0, 0);
    edit_->resize(width() - bw, height());

    setFocusProxy(edit_);

    edit_->setFocus();

    //edit_->raise();
  }
  else {
    altEdit_->move(0, 0);
    altEdit_->resize(width() - bw, height());

    setFocusProxy(altEdit_);

    altEdit_->setFocus();

    //altEdit_->raise();
  }

  button_->move(width() - bw, 0);
  button_->resize(bw, height());
}

bool
CQSwitchLineEdit::
event(QEvent *event)
{
  switch (event->type()) {
    case QEvent::KeyPress: {
      auto *ke = static_cast<QKeyEvent *>(event);

      if (isSwitchKey(ke))
        setShowAltEdit(! isShowAltEdit());

      break;
    }
    case QEvent::Resize: {
      updatePlacement();
      break;
    }
    default:
      //std::cerr << event->type() << "\n";
      break;
  }

  return QFrame::event(event);
}

bool
CQSwitchLineEdit::
eventFilter(QObject *o , QEvent *e)
{
  if      (e->type() == QEvent::KeyPress) {
    auto *ke = static_cast<QKeyEvent *>(e);

    if (isSwitchKey(ke)) {
      setShowAltEdit(! isShowAltEdit());
      return true;
    }
  }
  else if (e->type() == QEvent::KeyRelease) {
    auto *ke = static_cast<QKeyEvent *>(e);

    if (isSwitchKey(ke)) {
      return true;
    }
  }

  return QObject::eventFilter(o, e);
}

bool
CQSwitchLineEdit::
isSwitchKey(QKeyEvent *ke)
{
  if (ke->modifiers() & Qt::AltModifier) {
    if (ke->key() == Qt::Key_Right)
      return true;
  }

  return false;
}

QSize
CQSwitchLineEdit::
sizeHint() const
{
  return edit_->sizeHint();
}
