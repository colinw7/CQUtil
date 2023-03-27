#include <CQTabWidget.h>
#include <CQTabWidgetTabBar.h>

#include <CQIconButton.h>
#include <CQFloatEdit.h>
#include <CQUtil.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include <svg/new_tab_light_svg.h>
#include <svg/new_tab_dark_svg.h>

#include <svg/left_light_svg.h>
#include <svg/left_dark_svg.h>

#include <svg/right_light_svg.h>
#include <svg/right_dark_svg.h>

CQTabWidget::
CQTabWidget(QWidget *parent) :
 QTabWidget(parent)
{
  setObjectName("tabWidget");

  tabBar_ = new CQTabWidgetTabBar(this);

  setTabBar(tabBar_);

  connect(tabBar_, SIGNAL(tabChanged(int)), this, SIGNAL(tabChanged(int)));
  connect(tabBar_, SIGNAL(tabMoveRequested(int, int)), this, SLOT(moveTab(int, int)));

#ifndef CQTAB_WIDGET_MOVABLE
  connect(tabBar_, SIGNAL(tabMoved(int, int)), this, SIGNAL(swapTabs(int, int)));
#endif

  auto *leftButton  = CQUtil::makeWidget<CQIconButton>("left" ); leftButton ->setIcon("LEFT" );
  auto *rightButton = CQUtil::makeWidget<CQIconButton>("right"); rightButton->setIcon("RIGHT");

  leftButton ->setToolTip("Scroll left");
  rightButton->setToolTip("Scroll right");

  connect(leftButton , SIGNAL(clicked()), this, SLOT(moveTabLeft ()));
  connect(rightButton, SIGNAL(clicked()), this, SLOT(moveTabRight()));

  moveTabWidget_ = CQUtil::makeWidget<QWidget>("moveTab");

  auto *moveTabLayout = CQUtil::makeLayout<QHBoxLayout>(moveTabWidget_, 2, 2);

  moveTabLayout->addWidget(leftButton);
  moveTabLayout->addWidget(rightButton);

  //---

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

CQTabWidget::
~CQTabWidget()
{
  if (! moveTabWidget_->parent())
    delete moveTabWidget_;
}

void
CQTabWidget::
setShowMoveButtons(bool show)
{
  moveButtons_ = show;

  setCornerWidget((show ? moveTabWidget_ : nullptr), Qt::TopRightCorner);

  moveTabWidget_->setVisible(show);
}

void
CQTabWidget::
moveTabLeft()
{
  int fromIndex = currentIndex();
  int toIndex   = fromIndex - 1;

  moveTab(fromIndex, toIndex);
}

void
CQTabWidget::
moveTabRight()
{
  int fromIndex = currentIndex();
  int toIndex   = fromIndex + 1;

  moveTab(fromIndex, toIndex);
}

void
CQTabWidget::
moveTab(int fromIndex, int toIndex)
{
  if (fromIndex < 0 || fromIndex > count() - 1) return;
  if (toIndex   < 0 || toIndex   > count() - 1) return;

  auto *widget = QTabWidget::widget(fromIndex);

  auto icon = tabIcon(fromIndex);
  auto text = tabText(fromIndex);

  removeTab(fromIndex);

  insertTab(toIndex, widget, icon, text);

  setCurrentIndex(toIndex);

  emit swapTabs(fromIndex, toIndex);
}

void
CQTabWidget::
addCreateButton()
{
  auto *newButton = new CQIconButton; newButton->setIcon("NEW_TAB");

  connect(newButton, SIGNAL(clicked()), this, SIGNAL(createTab()));

  setCornerWidget(newButton, Qt::TopLeftCorner);
}

void
CQTabWidget::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = createTabMenu();

  (void) menu->exec(e->globalPos());

  delete menu;
}

QMenu *
CQTabWidget::
createTabMenu() const
{
  auto *menu = new QMenu;

  menu->setObjectName("menu");

  //---

  for (int i = 0; i < count(); ++i) {
    auto icon = tabIcon(i);
    auto text = tabText(i);

    auto *action = menu->addAction(icon, text);

    action->setCheckable(true);
    action->setChecked(i == currentIndex());

    action->setData(i);

    connect(action, SIGNAL(triggered()), this, SLOT(tabSlot()));
  }

  return menu;
}

void
CQTabWidget::
tabSlot()
{
  auto *action = qobject_cast<QAction *>(sender());
  if (! action) return;

  int i = action->data().toInt();

  setCurrentIndex(i);
}
