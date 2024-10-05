#include <CQAutoHide.h>

#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <QMenu>

//#include <iostream>

CQAutoHide::
CQAutoHide(QWidget *w)
{
  setWidget(w);
}

void
CQAutoHide::
setWidget(QWidget *w)
{
  w_ = w;

  clear();

  addWidget(w_);
}

void
CQAutoHide::
setActive(bool active)
{
  if (active_ == active)
    return;

  active_ = active;

  if (active_)
    qApp->installEventFilter(this);
  else
    qApp->removeEventFilter(this);
}

void
CQAutoHide::
clear()
{
  widgets_.clear();
  rects_  .clear();
}

void
CQAutoHide::
addWidget(QWidget *w)
{
  widgets_.insert(w);
}

void
CQAutoHide::
addRect(const QRect &r)
{
  rects_.push_back(r);
}

bool
CQAutoHide::
eventFilter(QObject *obj, QEvent *event)
{
  bool ok = processEvent(obj, event);

  if (! ok) {
    if (w_)
      w_->hide();

    emit autoHide();
  }

  return QObject::eventFilter(obj, event);
}

bool
CQAutoHide::
processEvent(QObject *obj, QEvent *event)
{
  auto type = event->type();

  // ignore if not a widget
  auto *w = qobject_cast<QWidget *>(obj);
  if (! w) return true;

  bool showValid = true;

  // ignore if not a button event
  if      (type == QEvent::MouseButtonPress || type == QEvent::MouseButtonDblClick) {
    // hide if event is not in an allowable widget or rectangle
    auto *me = static_cast<QMouseEvent *>(event);

    pressWidget_   = w;
    hideOnRelease_ = false;

    if (type == QEvent::MouseButtonPress)
      hideOnRelease_ = ! isPreviewValid(w, me->globalPos());
    else
      showValid = isPreviewValid(w, me->globalPos());
  }
  else if (type == QEvent::MouseButtonRelease) {
    if (pressWidget_ != w) return true;

    // stop preview if event is not in an allowable widget or rectangle
    auto *me = static_cast<QMouseEvent *>(event);

    if (hideOnRelease_) {
      if (! menuActive_ && ! QWidget::mouseGrabber())
        showValid = false;

      hideOnRelease_ = false;
    }
    else
      showValid = isPreviewValid(w, me->globalPos());

    pressWidget_ = nullptr;
  }
  else if (type == QEvent::Show) {
    if (qobject_cast<QMenu *>(w) != nullptr) {
      menuActive_ = true;
      //std::cerr << "menu active" << std::endl;
    }
  }
  else if (type == QEvent::Hide) {
    if (qobject_cast<QMenu *>(w) != nullptr) {
      menuActive_ = false;
      //std::cerr << "menu inactive" << std::endl;
    }

    if (w == w_)
      setActive(false);
  }
  else
    return true;

  if (! showValid) {
    //std::cerr << "Hide for ";
    //if      (type == QEvent::MouseButtonPress   ) std::cerr << "Press ";
    //else if (type == QEvent::MouseButtonDblClick) std::cerr << "DblClick ";
    //else if (type == QEvent::MouseButtonRelease ) std::cerr << "Release ";
    //else if (type == QEvent::Show               ) std::cerr << "Show ";
    //else if (type == QEvent::Hide               ) std::cerr << "Hide ";
    //std::cerr << " on '" << w->objectName().toStdString() << "'" << std::endl;
    return false;
  }

  return true;
}

// check if mouse in an allowable widget or rectangle
bool
CQAutoHide::
isPreviewValid() const
{
  auto gp = QCursor::pos();

  return isPreviewValid(qApp->widgetAt(gp), gp);
}

// check if event widget and position are allowable
bool
CQAutoHide::
isPreviewValid(QWidget *w, const QPoint &gp) const
{
  // mouse is grab (menu) so assume ok
  if (menuActive_ || QWidget::mouseGrabber())
    return true;

  //-----

  // check widgets and rects
  bool showValid = true;

  bool found = false;

  if (isPreviewWidget(w))
    found = true;

  if (! found)
    found = checkPreviewRects(gp);

  if (! found && isModalDialogWidget(w))
    found = true;

  if (! found)
    showValid = false;

  return showValid;
}

bool
CQAutoHide::
checkPreviewRects(const QPoint &gp, int tol) const
{
  bool found = false;

  int n = rects_.size();

  for (int i = 0; i < n; ++i) {
    const auto &r = rects_[i];

    auto r1 = r.adjusted(-tol, -tol, tol, tol);

    if (r1.contains(gp)) {
      found = true;
      break;
    }
  }

  return found;
}

// check if widget is a preview widget or a child of it
bool
CQAutoHide::
isPreviewWidget(QWidget *w) const
{
  auto *w1 = w;

  while (w1) {
    if (widgets_.contains(w1))
      return true;

    w1 = w1->parentWidget();
  }

  return false;
}

// check if widget is a child of a modal dialog
bool
CQAutoHide::
isModalDialogWidget(QWidget *w) const
{
  auto *w1 = w;

  while (w1) {
    auto *dialog = qobject_cast<QDialog *>(w1);

    if (dialog && dialog->isModal())
      return true;

    w1 = w1->parentWidget();
  }

  return false;
}
