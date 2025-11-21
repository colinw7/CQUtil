#include <CQToolBar.h>
#include <CQMenu.h>

#include <QToolBar>
#include <QAbstractButton>

static CQToolBarMgr *s_instance;

CQToolBarMgr *
CQToolBarMgr::
instance()
{
  if (! s_instance)
    s_instance = new CQToolBarMgr;

  return s_instance;
}

void
CQToolBarMgr::
release()
{
  delete s_instance;

  s_instance = nullptr;
}

CQToolBarMgr::
CQToolBarMgr()
{
}

//------

CQToolBar::
CQToolBar(QMainWindow *mainWindow, const QString &name, Qt::ToolBarArea area) :
 mainWindow_(mainWindow)
{
  toolbar_ = new QToolBar(name);

  toolbar_->setObjectName("toolbar");

  mainWindow->addToolBar(area, toolbar_);

#if 0
  auto *mgr = CQToolBarMgrInst;

  if (mgr->iconSize().isValid()) {
    int is = mgr->iconSize().getValue();

    setIconSize(QSize(is, is));
  }
#endif
}

void
CQToolBar::
setIconSize(const QSize &iconSize)
{
  toolbar_->setIconSize(iconSize);
}

QAction *
CQToolBar::
addItem(CQMenuItem *item)
{
  auto *action = item->getAction();

  return addItem(action);
}

QAction *
CQToolBar::
addItem(QAction *action)
{
  toolbar_->addAction(action);

  return action;
}

QAction *
CQToolBar::
addWidget(QWidget *widget)
{
  return toolbar_->addWidget(widget);
}

QAction *
CQToolBar::
addWidget(QWidget *widget, bool checked)
{
  auto *action = toolbar_->addWidget(widget);

  auto *button = qobject_cast<QAbstractButton *>(widget);

  if (button) {
    button->setCheckable(true);
    button->setChecked(checked);
  }

  return action;
}

QAction *
CQToolBar::
addSeparator()
{
  return toolbar_->addSeparator();
}

void
CQToolBar::
setAllowedAreas(Qt::ToolBarAreas areas)
{
  toolbar_->setAllowedAreas(areas);
}

void
CQToolBar::
setVisible(bool visible)
{
  toolbar_->setVisible(visible);
}
