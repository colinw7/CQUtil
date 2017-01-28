#include <CQToolBar.h>
#include <CQMenu.h>

#include <QToolBar>
#include <QAbstractButton>

CQToolBarMgr *
CQToolBarMgr::
instance()
{
  static CQToolBarMgr *inst;

  if (! inst)
    inst = new CQToolBarMgr;

  return inst;
}

CQToolBarMgr::
CQToolBarMgr()
{
}

//------

CQToolBar::
CQToolBar(QMainWindow *main_window, const QString &name, Qt::ToolBarArea area) :
 main_window_(main_window)
{
  toolbar_ = new QToolBar(name);

  toolbar_->setObjectName("toolbar");

  main_window->addToolBar(area, toolbar_);

  if (CQToolBarMgrInst->iconSize().isValid()) {
    int is = CQToolBarMgrInst->iconSize().getValue();

    setIconSize(QSize(is, is));
  }
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
  QAction *action = item->getAction();

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
  QAction *action = toolbar_->addWidget(widget);

  QAbstractButton *button = qobject_cast<QAbstractButton *>(widget);

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
