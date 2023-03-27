#ifndef CQWORKSPACE_H
#define CQWORKSPACE_H

#include <QObject>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSignalMapper>

#include <CQMenu.h>

class CQWorkspaceUpdateWindowsMenuIFace {
 public:
  virtual ~CQWorkspaceUpdateWindowsMenuIFace() { }

  virtual void exec() = 0;
};

class CQWorkspaceBase {
 public:
  CQWorkspaceBase() { }

  virtual ~CQWorkspaceBase() { }

  virtual void setActiveWidget(QWidget *w) = 0;
};

class CQWorkspaceArea : public QMdiArea {
  Q_OBJECT

 public:
  CQWorkspaceArea(CQWorkspaceBase *base) :
   QMdiArea(nullptr), base_(base) {
    setObjectName("area");
  }

 public Q_SLOTS:
  void setActiveWidget(QWidget *w) {
    base_->setActiveWidget(w);

    emit widgetActivated(w);
  }

 signals:
  void widgetActivated(QWidget *w);

 private:
  CQWorkspaceBase *base_;
};

template<typename W>
class CQWorkspace;

template<typename W>
class CQWorkspaceUpdateWindowsMenu : public CQWorkspaceUpdateWindowsMenuIFace {
 private:
  CQWorkspace<W> *workspace_;

 public:
  CQWorkspaceUpdateWindowsMenu(CQWorkspace<W> *workspace) :
   workspace_(workspace) {
  }

  void exec() override;
};

class CQWorkspaceUpdateWindowsMenuObject : public QObject {
  Q_OBJECT

 private:
  CQWorkspaceUpdateWindowsMenuIFace *iface_;

 public:
  CQWorkspaceUpdateWindowsMenuObject(CQMenu *menu, CQWorkspaceUpdateWindowsMenuIFace *iface);

 private Q_SLOTS:
  void updateWindowsMenu();
};

template<typename W>
class CQWorkspace : public CQWorkspaceBase {
 public:
  typedef QList<W *> WindowList;

 public:
  CQWorkspace(QMainWindow *mainWindow, const QString &name) :
   mainWindow_(mainWindow) {
    workspace_ = new CQWorkspaceArea(this);

    workspace_->setObjectName(name);

    mainWindow_->setCentralWidget(workspace_);

    windowMapper_ = new QSignalMapper(mainWindow_);

    QObject::connect(windowMapper_, SIGNAL(mapped(QWidget *)),
                     workspace_, SLOT(setActiveWidget(QWidget *)));
  }

  virtual ~CQWorkspace() { }

  QMainWindow *getMainWindow() const { return mainWindow_; }

  CQWorkspaceArea *getWorkspace() const { return workspace_; }

  QSignalMapper *getMapper() const { return windowMapper_; }

  W *addWindow(const QString &name) {
    W *window = new W(mainWindow_);

    window->setObjectName(name);

    window->setWindowTitle(name);

    window->setAttribute(Qt::WA_DeleteOnClose);

    /*QMdiSubWindow *subWindow =*/ workspace_->addSubWindow(window);

    window->show();

    return window;
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
    QMdiSubWindow *subWindow = workspace_->activeSubWindow();

    return dynamic_cast<W *>(subWindow ? subWindow->widget() : nullptr);
  }

  const WindowList &windowList() const {
    CQWorkspace *th = const_cast<CQWorkspace *>(this);

    QList<QMdiSubWindow *> widgets = workspace_->subWindowList();

    QList<QMdiSubWindow *>::iterator p1 = widgets.begin();
    QList<QMdiSubWindow *>::iterator p2 = widgets.end  ();

    th->windows_.clear();

    for ( ; p1 != p2; ++p1) {
      W *w = dynamic_cast<W *>((*p1)->widget());

      th->windows_.push_back(w);
    }

    return windows_;
  }

  void setActiveWidget(QWidget *w) override {
    QList<QMdiSubWindow *> widgets = workspace_->subWindowList();

    QList<QMdiSubWindow *>::iterator p1 = widgets.begin();
    QList<QMdiSubWindow *>::iterator p2 = widgets.end  ();

    for ( ; p1 != p2; ++p1) {
      if ((*p1)->widget() == w)
        workspace_->setActiveSubWindow(*p1);
    }
  }

  void updateWindowsMenu() {
    QMenu *menu = windowsMenu_->getMenu();

    menu->clear();

    menu->addAction(closeItem_->getAction());
    menu->addAction(closeAllItem_->getAction());
    menu->addSeparator();

    menu->addAction(tileItem_->getAction());
    menu->addAction(cascadeItem_->getAction());
    //menu->addAction(arrangeItem_->getAction());

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
                     workspace_, SLOT(closeActiveSubWindow()));

    closeAllItem_ = new CQMenuItem(windowsMenu_, "Close &All");

    closeAllItem_->setStatusTip("Close all the windows");

    QObject::connect(closeAllItem_->getAction(), SIGNAL(triggered()),
                     workspace_, SLOT(closeAllSubWindows()));

    tileItem_ = new CQMenuItem(windowsMenu_, "&Tile");

    tileItem_->setStatusTip("Tile the windows");

    QObject::connect(tileItem_->getAction(), SIGNAL(triggered()),
                     workspace_, SLOT(tileSubWindows()));

    cascadeItem_ = new CQMenuItem(windowsMenu_, "&Cascade");

    cascadeItem_->setStatusTip("Cascade the windows");

    QObject::connect(cascadeItem_->getAction(), SIGNAL(triggered()),
                     workspace_, SLOT(cascadeSubWindows()));

#if 0
    arrangeItem_ = new CQMenuItem(windowsMenu_, "Arrange &icons");

    arrangeItem_->setStatusTip("Arrange the icons");

    QObject::connect(arrangeItem_->getAction(), SIGNAL(triggered()),
                     workspace_, SLOT(arrangeIcons()));
#endif

    nextItem_ = new CQMenuItem(windowsMenu_, "Ne&xt");

    nextItem_->setStatusTip("Move the focus to the next window");

    QObject::connect(nextItem_->getAction(), SIGNAL(triggered()),
                     workspace_, SLOT(activateNextSubWindow()));

    prevItem_ = new CQMenuItem(windowsMenu_, "Pre&vious");

    prevItem_->setStatusTip("Move the focus to the prev window");

    QObject::connect(prevItem_->getAction(), SIGNAL(triggered()),
                     workspace_, SLOT(activatePreviousSubWindow()));

    //------

    CQWorkspaceUpdateWindowsMenu<W> *update =
      new CQWorkspaceUpdateWindowsMenu<W>(this);

    new CQWorkspaceUpdateWindowsMenuObject(windowsMenu_, update);

    return windowsMenu_;
  }

 private:
  QMainWindow     *mainWindow_;
  CQWorkspaceArea *workspace_;
  WindowList       windows_;
  QSignalMapper   *windowMapper_;
  CQMenu          *windowsMenu_;
  CQMenuItem      *closeItem_;
  CQMenuItem      *closeAllItem_;
  CQMenuItem      *tileItem_;
  CQMenuItem      *cascadeItem_;
//CQMenuItem      *arrangeItem_;
  CQMenuItem      *nextItem_;
  CQMenuItem      *prevItem_;
  CQMenuItem      *separatorItem_;
};

template<typename W>
void
CQWorkspaceUpdateWindowsMenu<W>::
exec() {
  workspace_->updateWindowsMenu();
}

#endif
