#ifndef CQMENU_H
#define CQMENU_H

#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QMainWindow>
#include <QMenuBar>

#include <map>

class CQMenuItem;

class CQMenu {
 public:
  CQMenu(QMainWindow *mainWindow, const QString &name) :
   mainWindow_(mainWindow) {
    auto *menuBar = mainWindow->menuBar();

    if (menuBar->objectName() == "")
      menuBar->setObjectName("menubar");

    if (name == "|")
      menuBar->addSeparator();
    else {
      menu_ = menuBar->addMenu(name);

      if (name.length())
        menu_->setObjectName(name);

      menu_->setFont(qApp->font());
    }
  }

  CQMenu(QWidget *widget, const QString &name="") {
    menu_ = new QMenu(widget);

    if (name.length())
      menu_->setObjectName(name);

    menu_->setFont(qApp->font());
  }

  QMainWindow *getMainWindow() const { return mainWindow_; }

  QWidget *getParent() const {
    return (mainWindow_ ? mainWindow_ : menu_->parentWidget());
  }

  QMenu *getMenu() const { return menu_; }

  void startGroup() {
    actionGroup_ = new QActionGroup(mainWindow_);
  }

  void endGroup() {
    actionGroup_ = nullptr;
  }

  void addMenuItem(CQMenuItem *menuItem);

  void addAction(QAction *action) {
    if (actionGroup_)
      actionGroup_->addAction(action);

    menu_->addAction(action);

    actionMap_[action->text()] = action;
  }

  CQMenuItem *getMenuItem(const QString &name) {
    auto p = menuItemMap_.find(name);

    if (p != menuItemMap_.end())
      return (*p).second;

    return nullptr;
  }

  QAction *getAction(const QString &name) {
    auto p = actionMap_.find(name);

    if (p != actionMap_.end())
      return (*p).second;

    return nullptr;
  }

  void addSeparator() {
    menu_->addSeparator();
  }

  void exec(const QPoint &pos) {
    (void) menu_->exec(pos);
  }

 private:
  using MenuItemMap = std::map<QString, CQMenuItem *>;
  using ActionMap   = std::map<QString, QAction *>;

  QMainWindow*  mainWindow_  { nullptr };
  QMenu*        menu_        { nullptr };
  QActionGroup* actionGroup_ { nullptr };
  MenuItemMap   menuItemMap_;
  ActionMap     actionMap_;
};

//------

class CQMenuItem {
 public:
  enum Type {
    NORMAL    = 0,
    CHECKABLE = (1<<0),
    CHECKED   = (1<<1)
  };

  CQMenuItem(CQMenu *menu, const QString &name, Type type = NORMAL) :
   menu_(menu) {
    action_ = new QAction(name, menu->getParent());

    menu_->addMenuItem(this);

    if ((type & CHECKABLE) || (type & CHECKED))
      setCheckable(true);

    if (type & CHECKED)
      setChecked(true);

    action_->setFont(qApp->font());
  }

  CQMenuItem(CQMenu *menu, const QIcon &icon, const QString &name, Type type = NORMAL) :
   menu_(menu) {
    action_ = new QAction(icon, name, menu->getParent());

    menu_->addMenuItem(this);

    if ((type & CHECKABLE) || (type & CHECKED))
      setCheckable(true);

    if (type & CHECKED)
      setChecked(true);

    action_->setFont(qApp->font());
  }

  CQMenuItem(CQMenu *menu, const QString &name, const QIcon &icon, Type type = NORMAL) :
   menu_(menu) {
    action_ = new QAction(icon, name, menu->getParent());

    menu_->addMenuItem(this);

    if ((type & CHECKABLE) || (type & CHECKED))
      setCheckable(true);

    if (type & CHECKED)
      setChecked(true);

    action_->setFont(qApp->font());
  }

  QString getName() const { return action_->text(); }

  void setName(const std::string &name) { action_->setText(name.c_str()); }

  QAction *getAction() const { return action_; }

  void setShortcut(const QKeySequence &key) {
    action_->setShortcut(key);
  }

  void setShortcut(const QString &shortcut) {
    action_->setShortcut(shortcut);
  }

  void setStatusTip(const QString &tip) {
    action_->setStatusTip(tip);
  }

  void setIcon(uchar *data, uint len) {
    QPixmap pixmap;

    pixmap.loadFromData(data, len);

    QIcon icon(pixmap);

    action_->setIcon(icon);
  }

  void setIcon(const QIcon &icon) {
    action_->setIcon(icon);
  }

  void setXPMIcon(const char **xpmData) {
    action_->setIcon(QIcon(QPixmap(xpmData)));
  }

  bool isChecked() const {
    return action_->isChecked();
  }

  void setCheckable(bool checkable) {
    action_->setCheckable(checkable);
  }

  void setChecked(bool checked) {
    action_->setChecked(checked);
  }

  bool isEnabled() const {
    return action_->isEnabled();
  }

  void setEnabled(bool enabled) {
    action_->setEnabled(enabled);
  }

  bool connect(const char *signal, const QObject *recv, const char *method) {
    return QObject::connect(getAction(), signal, recv, method);
  }

  bool connect(const QObject *recv, const char *method) {
    if (strstr(method, "(bool)"))
      return QObject::connect(getAction(), SIGNAL(triggered(bool)), recv, method);
    else
      return QObject::connect(getAction(), SIGNAL(triggered()), recv, method);
  }

 private:
  CQMenu*  menu_ { nullptr };
  QAction* action_ { nullptr };
};

//------

inline void
CQMenu::
addMenuItem(CQMenuItem *menuItem)
{
  addAction(menuItem->getAction());

  menuItemMap_[menuItem->getName()] = menuItem;
}

#endif
