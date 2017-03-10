#ifndef CQMdiArea_H
#define CQMdiArea_H

#include <QObject>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSignalMapper>

#include <CQMenu.h>

class CQMdiAreaUpdateWindowsMenuIFace {
 public:
  virtual ~CQMdiAreaUpdateWindowsMenuIFace() { }

  virtual void exec() = 0;
};

template<typename W>
class CQMdiArea;

template<typename W>
class CQMdiAreaUpdateWindowsMenu : public CQMdiAreaUpdateWindowsMenuIFace {
 public:
  CQMdiAreaUpdateWindowsMenu(CQMdiArea<W> *area) :
   area_(area) {
  }

  void exec();

 private:
  CQMdiArea<W> *area_;
};

class CQMdiAreaUpdateWindowsMenuObject : public QObject {
  Q_OBJECT

 private:
  CQMdiAreaUpdateWindowsMenuIFace *iface_;

 public:
  CQMdiAreaUpdateWindowsMenuObject(CQMenu *menu, CQMdiAreaUpdateWindowsMenuIFace *iface);

 private slots:
  void updateWindowsMenu();
};

template<typename W>
class CQMdiArea {
 public:
  typedef QList<W *> WindowList;

 public:
  CQMdiArea(QMainWindow *mainWindow, const QString &name) :
   mainWindow_(mainWindow) {
    area_ = new QMdiArea;

    area_->setObjectName(name);

    mainWindow_->setCentralWidget(area_);

    windowMapper_ = new QSignalMapper(mainWindow_);

//  QObject::connect(windowMapper_, SIGNAL(mapped(QWidget *)),
//                   area_, SLOT(setActiveSubWindow(QMdiSubWindow *)));
  }

  virtual ~CQMdiArea() { }

  QMainWindow *getMainWindow() const { return mainWindow_; }

  QMdiArea *getArea() const { return area_; }

  QSignalMapper *getMapper() const { return windowMapper_; }

  W *addWindow(const QString &name) {
    W *w = new W(mainWindow_);

    w->setObjectName(name);

    w->setWindowTitle(name);

    w->setAttribute(Qt::WA_DeleteOnClose);

    QMdiSubWindow *window = area_->addSubWindow(w);

    window->show();

    return w;
  }

  W *getWindow(uint i) const {
    const WindowList &windows = windowList();

    return windows.at(i);
  }

  int getNumWindows() const {
    const WindowList &windows = windowList();

    return windows.size();
  }

  W *activeWindow() const {
    return dynamic_cast<W *>(area_->activeSubWindow()->widget());
  }

  const WindowList &windowList() const {
    CQMdiArea *th = const_cast<CQMdiArea *>(this);

    QList<QMdiSubWindow *> widgets = area_->subWindowList();

    QList<QWidget *>::iterator p1 = widgets.begin();
    QList<QWidget *>::iterator p2 = widgets.end  ();

    th->windows_.clear();

    for ( ; p1 != p2; ++p1) {
      QMdiSubWindow *window = *p1;

      th->windows_.push_back(static_cast<W *>(window->widget()));
    }

    return windows_;
  }

  void updateWindowsMenu() {
    QMenu *menu = windowsMenu_->getMenu();

    menu->clear();

    menu->addAction(closeItem_->getAction());
    menu->addAction(closeAllItem_->getAction());
    menu->addSeparator();

    menu->addAction(tileItem_->getAction());
    menu->addAction(cascadeItem_->getAction());
    menu->addAction(arrangeItem_->getAction());

    menu->addSeparator();

    menu->addAction(nextItem_->getAction());
    menu->addAction(prevItem_->getAction());

    WindowList windows = windowList();

    if (! windows.isEmpty()) {
      menu->addSeparator();

      W *current = activeWindow();

      for (int i = 0; i < windows.size(); ++i) {
        W *window = windows.at(i);

        QAction *action = menu->addAction(window->objectName());

        action->setCheckable(true);

        action->setChecked(window == current);

        QObject::connect(action, SIGNAL(triggered()), windowMapper_, SLOT(map()));

        windowMapper_->setMapping(action, window);
      }
    }
  }

  CQMenu *addWindowsMenu() {
    windowsMenu_ = new CQMenu(mainWindow_, "&Windows");

    closeItem_ = new CQMenuItem(windowsMenu_, "Cl&ose");

    closeItem_->setShortcut("Ctrl+F4");
    closeItem_->setStatusTip("Close the active window");

    QObject::connect(closeItem_->getAction(), SIGNAL(triggered()),
                     area_, SLOT(closeActiveSubWindow()));

    closeAllItem_ = new CQMenuItem(windowsMenu_, "Close &All");

    closeAllItem_->setStatusTip("Close all the windows");

    QObject::connect(closeAllItem_->getAction(), SIGNAL(triggered()),
                     area_, SLOT(closeAllSubWindows()));

    tileItem_ = new CQMenuItem(windowsMenu_, "&Tile");

    tileItem_->setStatusTip("Tile the windows");

    QObject::connect(tileItem_->getAction(), SIGNAL(triggered()),
                     area_, SLOT(tileSubWindows()));

    cascadeItem_ = new CQMenuItem(windowsMenu_, "&Cascade");

    cascadeItem_->setStatusTip("Cascade the windows");

    QObject::connect(cascadeItem_->getAction(), SIGNAL(triggered()),
                     area_, SLOT(cascadeSubWindows()));

    arrangeItem_ = new CQMenuItem(windowsMenu_, "Arrange &icons");

    arrangeItem_->setStatusTip("Arrange the icons");

    QObject::connect(arrangeItem_->getAction(), SIGNAL(triggered()),
                     area_, SLOT(arrangeIcons()));

    nextItem_ = new CQMenuItem(windowsMenu_, "Ne&xt");

    nextItem_->setStatusTip("Move the focus to the next window");

    QObject::connect(nextItem_->getAction(), SIGNAL(triggered()),
                     area_, SLOT(activateNextSubWindow()));

    prevItem_ = new CQMenuItem(windowsMenu_, "Pre&vious");

    prevItem_->setStatusTip("Move the focus to the prev window");

    QObject::connect(prevItem_->getAction(), SIGNAL(triggered()),
                     area_, SLOT(activatePreviousSubWindow()));

    //------

    CQMdiAreaUpdateWindowsMenu<W> *update =
      new CQMdiAreaUpdateWindowsMenu<W>(this);

    new CQMdiAreaUpdateWindowsMenuObject(windowsMenu_, update);

    return windowsMenu_;
  }

 private:
  QMainWindow   *mainWindow_;
  QMdiArea      *area_;
  WindowList     windows_;
  QSignalMapper *windowMapper_;
  CQMenu        *windowsMenu_;
  CQMenuItem    *closeItem_;
  CQMenuItem    *closeAllItem_;
  CQMenuItem    *tileItem_;
  CQMenuItem    *cascadeItem_;
  CQMenuItem    *arrangeItem_;
  CQMenuItem    *nextItem_;
  CQMenuItem    *prevItem_;
  CQMenuItem    *separatorItem_;
};

template<typename W>
void
CQMdiAreaUpdateWindowsMenu<W>::
exec() {
  area_->updateWindowsMenu();
}

#endif
