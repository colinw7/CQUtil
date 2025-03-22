#ifndef CQTOOLBAR_H
#define CQTOOLBAR_H

#include <QString>
#include <QSize>
#include <optional>

class CQMenuItem;
class QMainWindow;
class QToolBar;
class QWidget;
class QAction;

#define CQToolBarMgrInst CQToolBarMgr::instance()

class CQToolBarMgr {
 public:
  using OptInt = std::optional<int>;

 public:
  static CQToolBarMgr *instance();

  static void release();

  const OptInt &iconSize() const { return iconSize_; }
  void setIconSize(int i) { iconSize_ = i; }

 private:
  CQToolBarMgr();

 private:
  OptInt iconSize_;
};

//---

class CQToolBar {
 public:
  CQToolBar(QMainWindow *main_window, const QString &name,
            Qt::ToolBarArea area=Qt::TopToolBarArea);

  QMainWindow *getMainWindow() const { return main_window_; }

  QToolBar *getToolBar() const { return toolbar_; }

  void setIconSize(const QSize &iconSize);

  QAction *addItem(CQMenuItem *item);
  QAction *addItem(QAction *action);

  QAction *addWidget(QWidget *widget);
  QAction *addWidget(QWidget *widget, bool checked);

  QAction *addSeparator();

  void setAllowedAreas(Qt::ToolBarAreas areas);

  void setVisible(bool visible);

 private:
  QMainWindow *main_window_ { nullptr };
  QToolBar    *toolbar_ { nullptr };
};

#endif
