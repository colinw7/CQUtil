#include <CQWindow.h>
#include <CQUtil.h>

#include <QApplication>

//#include <QX11Info>
#define QT_NO_X11 1

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#ifndef QT_NO_X11
namespace CQWindowUtil {
  void getWMNormalHints(Window xwin, XSizeHints **size_hints, int *supplied) {
    static XSizeHints size_hints1;

    long supplied1 = 0;

    if (! XGetWMNormalHints(QX11Info::display(), xwin, &size_hints1, &supplied1))
      size_hints1.flags = 0;

    *size_hints = &size_hints1;
    *supplied   = int(supplied1);
  }

  bool setWMNormalHints(Window xwin, XSizeHints *size_hints) {
    XSetWMNormalHints(QX11Info::display(), xwin, size_hints);

    return true;
  }
};
#endif

//-----------

CWindow *
CQWindowFactory::
createWindow(int x, int y, uint width, uint height)
{
  auto *window = new CQWindow;

  window->move  (x, y);
  window->resize(width, height);

  return window;
}

CWindow *
CQWindowFactory::
createWindow(CWindow *parent, int x, int y, uint width, uint height)
{
  auto *qparent = dynamic_cast<CQWindow *>(parent);

  assert(qparent);

  auto *window = new CQWindow(qparent);

  window->move  (x, y);
  window->resize(width, height);

  return window;
}

//-----------

void
CQWindow::
setFactory()
{
  CWindowMgrInst->setFactory(new CQWindowFactory);
}

void
CQWindow::
resetFactory()
{
  CWindowMgrInst->setFactory(0);
}

CQWindow::
CQWindow(CQWindow *parent) :
 QWidget(parent), CWindow(parent), pressed_(false)
{
}

CQWindow::
CQWindow(QWidget *parent) :
 QWidget(parent), CWindow(nullptr), pressed_(false)
{
}

CQWindow::
~CQWindow()
{
}

void
CQWindow::
setPressed(bool b)
{
  pressed_ = b;
}

void
CQWindow::
getPosition(int *x, int *y) const
{
  *x = QWidget::pos().x();
  *y = QWidget::pos().y();
}

void
CQWindow::
getSize(uint *w, uint *h) const
{
  *w = uint(QWidget::size().width ());
  *h = uint(QWidget::size().height());
}

void
CQWindow::
getScreenSize(uint *w, uint *h) const
{
  CQUtil::getScreenSize(w, h);
}

void
CQWindow::
destroy()
{
  std::cerr << "Unimplemented" << std::endl;
}

void
CQWindow::
move(int x, int y)
{
  auto *parent = CQUtil::getToplevelWidget(this);

  auto p = mapTo(parent, QPoint(0, 0));

  parent->move(x - p.x(), y - p.y());
}

void
CQWindow::
resize(uint width, uint height)
{
  QWidget::resize(int(width), int(height));
}

void
CQWindow::
map()
{
  QWidget::show();
}

void
CQWindow::
unmap()
{
  QWidget::hide();
}

bool
CQWindow::
isMapped()
{
  return QWidget::isVisible();
}

void
CQWindow::
iconize()
{
  auto *parent = CQUtil::getToplevelWidget(this);

  parent->setWindowState(parent->windowState() | Qt::WindowMinimized);
}

void
CQWindow::
deiconize()
{
  auto *parent = CQUtil::getToplevelWidget(this);

  parent->setWindowState(parent->windowState() & ~Qt::WindowMinimized);
}

void
CQWindow::
maximize()
{
  auto *parent = CQUtil::getToplevelWidget(this);

  parent->setWindowState(parent->windowState() | Qt::WindowMaximized);
}

void
CQWindow::
demaximize()
{
  auto *parent = CQUtil::getToplevelWidget(this);

  parent->setWindowState(parent->windowState() & ~Qt::WindowMaximized);
}

void
CQWindow::
lower()
{
  auto *parent = CQUtil::getToplevelWidget(this);

  parent->lower();
}

void
CQWindow::
raise()
{
  auto *parent = CQUtil::getToplevelWidget(this);

  parent->raise();
}

void
CQWindow::
setWindowTitle(const std::string &title)
{
  auto *parent = CQUtil::getToplevelWidget(this);

  parent->setWindowTitle(title.c_str());
}

void
CQWindow::
setIconTitle(const std::string &title)
{
  auto *parent = CQUtil::getToplevelWidget(this);

  parent->setWindowIconText(title.c_str());
}

void
CQWindow::
getWindowTitle(std::string &title) const
{
  auto *parent = CQUtil::getToplevelWidget(const_cast<CQWindow *>(this));

  title = parent->windowTitle().toStdString();
}

void
CQWindow::
getIconTitle(std::string &title) const
{
  auto *parent = CQUtil::getToplevelWidget(const_cast<CQWindow *>(this));

  title = parent->windowIconText().toStdString();
}

void
CQWindow::
expose()
{
  QWidget::update();
}

void
CQWindow::
setEventAdapter(CEventAdapter *)
{
  std::cerr << "Unimplemented" << std::endl;
}

bool
CQWindow::
setProperty(const std::string &name, const std::string &value)
{
#ifndef QT_NO_X11
  auto *parent = CQUtil::getToplevelWidget(this);

  Window xwin = parent->winId();

  Atom xatom = XInternAtom(QX11Info::display(), name.c_str(), False);

  XChangeProperty(QX11Info::display(), xwin, xatom, XA_STRING,
                  8, PropModeReplace,
                  reinterpret_cast<uchar *>(const_cast<char *>(value.c_str())),
                  int(value.size()));

  return true;
#else
  assert(name != "" && value != "");

  return false;
#endif
}

bool
CQWindow::
setSelectText(const std::string &text)
{
  CQUtil::setSelectText(text);

  return true;
}

void
CQWindow::
redraw()
{
}

void
CQWindow::
mousePressEvent(QMouseEvent *event)
{
  setPressed(true);

  buttonPressEvent(*CQUtil::convertEvent(event));
}

void
CQWindow::
mouseReleaseEvent(QMouseEvent *event)
{
  buttonReleaseEvent(*CQUtil::convertEvent(event));

  setPressed(false);
}

void
CQWindow::
mouseDoubleClickEvent(QMouseEvent *event)
{
  CMouseEvent *e = CQUtil::convertEvent(event);

  e->setCount(2);

  buttonReleaseEvent(*e);
}

void
CQWindow::
mouseMoveEvent(QMouseEvent *event)
{
  if (! isPressed())
    pointerMotionEvent(*CQUtil::convertEvent(event));
  else
    buttonMotionEvent(*CQUtil::convertEvent(event));
}

void
CQWindow::
keyPressEvent(QKeyEvent *event)
{
  keyPressEvent(*CQUtil::convertEvent(event));
}

bool
CQWindow::
keyPressEvent(const CKeyEvent &)
{
  return false;
}

void
CQWindow::
keyReleaseEvent(QKeyEvent *event)
{
  keyReleaseEvent(*CQUtil::convertEvent(event));
}

bool
CQWindow::
keyReleaseEvent(const CKeyEvent &)
{
  return false;
}

void
CQWindow::
paintEvent(QPaintEvent *)
{
  exposeEvent();
}

void
CQWindow::
resizeEvent(QResizeEvent *)
{
  resizeEvent();
}

bool
CQWindow::
resizeEvent()
{
  return false;
}

void
CQWindow::
hideEvent(QHideEvent *)
{
  hideEvent();
}

bool
CQWindow::
hideEvent()
{
  return false;
}

void
CQWindow::
showEvent(QShowEvent *)
{
  showEvent();
}

bool
CQWindow::
showEvent()
{
  return false;
}

void
CQWindow::
enterEvent(QEvent *)
{
  enterEvent();
}

bool
CQWindow::
enterEvent()
{
  return false;
}

void
CQWindow::
leaveEvent(QEvent *)
{
  leaveEvent();
}

bool
CQWindow::
leaveEvent()
{
  return false;
}

void
CQWindow::
beep()
{
  QApplication::beep();
}

void
CQWindow::
setMinSize(int width, int height)
{
#ifndef QT_NO_X11
  auto *parent = CQUtil::getToplevelWidget(this);

  Window xwin = parent->winId();
  if (xwin == 0) return;

  parent->setMinimumSize(width, height);

  XSizeHints *hints { nullptr };
  int         supplied { 0 };

  CQWindowUtil::getWMNormalHints(xwin, &hints, &supplied);

  if ((supplied & PMinSize) && hints->min_width == width && hints->min_height == height)
    return;

  hints->flags |= PMinSize;

  hints->min_width  = width;
  hints->min_height = height;

  CQWindowUtil::setWMNormalHints(xwin, hints);
#else
  assert(width && height);
#endif
}

void
CQWindow::
setBaseSize(int width, int height)
{
#ifndef QT_NO_X11
  auto *parent = CQUtil::getToplevelWidget(this);

  Window xwin = parent->winId();
  if (xwin == 0) return;

  parent->setBaseSize(width, height);

  XSizeHints *hints { nullptr };
  int         supplied { 0 };

  CQWindowUtil::getWMNormalHints(xwin, &hints, &supplied);

  if ((supplied & PBaseSize) && hints->base_width == width && hints->base_height == height)
    return;

  hints->flags |= PBaseSize;

  hints->base_width  = width;
  hints->base_height = height;

  CQWindowUtil::setWMNormalHints(xwin, hints);
#else
  assert(width && height);
#endif
}

void
CQWindow::
setResizeInc(int width, int height)
{
#ifndef QT_NO_X11
  auto *parent = CQUtil::getToplevelWidget(this);

  Window xwin = parent->winId();
  if (xwin == 0) return;

  parent->setSizeIncrement(width, height);

  XSizeHints *hints { nullptr };
  int         supplied { 0 };

  CQWindowUtil::getWMNormalHints(xwin, &hints, &supplied);

  if ((supplied & PResizeInc) && hints->width_inc == width && hints->height_inc == height)
    return;

  hints->flags |= PResizeInc;

  hints->width_inc  = width;
  hints->height_inc = height;

  CQWindowUtil::setWMNormalHints(xwin, hints);
#else
  assert(width && height);
#endif
}
