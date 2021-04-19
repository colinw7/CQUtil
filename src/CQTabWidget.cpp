#include <CQTabWidget.h>
#include <CQIconButton.h>
#include <CQFloatEdit.h>

#include <QApplication>
#include <QMouseEvent>
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

#ifndef CQTAB_WIDGET_MOVABLE
#include <svg/move_tab_light_svg.h>
#include <svg/move_tab_dark_svg.h>
#endif

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
  connect(tabBar_, SIGNAL(tabMoved(int,int)), this, SIGNAL(swapTabs(int,int)));
#endif

  auto *leftButton  = new CQIconButton; leftButton ->setIcon("LEFT" );
  auto *rightButton = new CQIconButton; rightButton->setIcon("RIGHT");

  leftButton ->setObjectName("left");
  rightButton->setObjectName("right");

  leftButton ->setToolTip("Scroll left");
  rightButton->setToolTip("Scroll right");

  connect(leftButton , SIGNAL(clicked()), this, SLOT(moveTabLeft ()));
  connect(rightButton, SIGNAL(clicked()), this, SLOT(moveTabRight()));

  moveTabWidget_ = new QWidget;

  moveTabWidget_->setObjectName("moveTab");

  auto *moveTabLayout = new QHBoxLayout(moveTabWidget_);
  moveTabLayout->setMargin(2); moveTabLayout->setSpacing(2);

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

//-----------

CQTabWidgetTabBar::
CQTabWidgetTabBar(CQTabWidget *tabWidget) :
 QTabBar(tabWidget), tabWidget_(tabWidget)
{
  setObjectName("tabBar");

  setAcceptDrops(true);

#ifdef CQTAB_WIDGET_MOVABLE
  setMovable(true);
#endif

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

#ifndef CQTAB_WIDGET_MOVABLE
void
CQTabWidgetTabBar::
mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    pressPos_ = event->pos();

  QTabBar::mousePressEvent(event);
}

void
CQTabWidgetTabBar::
mouseMoveEvent(QMouseEvent *event)
{
  // If the left button isn't pressed anymore then return
  if (! (event->buttons() & Qt::LeftButton))
    return;

  // If the distance is too small then return
  if ((event->pos() - pressPos_).manhattanLength() < QApplication::startDragDistance())
    return;

  // initiate Drag
  auto *drag = new QDrag(this);

  drag->setPixmap((CQPixmapCacheInst->getPixmap("MOVE_TAB"));

  auto *mimeData = new QMimeData;

  // a crude way to distinguish tab-reordering drops from other ones
  mimeData->setData("action", "tab-reordering") ;

  drag->setMimeData(mimeData);

  drag->exec();
}
#endif

void
CQTabWidgetTabBar::
mouseDoubleClickEvent(QMouseEvent *event)
{
  ind_ = tabAt(event->pos());

  QRect rect = tabRect(ind_);

  if (! edit_) {
    edit_ = new CQFloatEdit(this);

    connect(edit_, SIGNAL(valueChanged(const QString &)),
            this, SLOT(tabEditFinished(const QString &)));
  }

  QPoint gpos = mapToGlobal(rect.topLeft());

  edit_->display(QRect(gpos, rect.size()), tabText(ind_));
}

#ifndef CQTAB_WIDGET_MOVABLE
void
CQTabWidgetTabBar::
dragEnterEvent(QDragEnterEvent *event)
{
  // Only accept if it's an tab-reordering request
  const QMimeData *m = event->mimeData();

  QStringList formats = m->formats();

  if (formats.contains("action") && (m->data("action") == "tab-reordering"))
    event->acceptProposedAction();
}

void
CQTabWidgetTabBar::
dropEvent(QDropEvent *event)
{
  int fromIndex = tabAt(pressPos_);
  int toIndex   = tabAt(event->pos());

  // Tell interested objects that
  if (fromIndex != toIndex)
    emit tabMoveRequested(fromIndex, toIndex);

  event->acceptProposedAction();
}
#endif

void
CQTabWidgetTabBar::
paintEvent(QPaintEvent *event)
{
  QTabBar::paintEvent(event);

  if (ind_ > 0 && edit_)
    edit_->raise();
}

void
CQTabWidgetTabBar::
stopEdit()
{
  edit_->hide();
}

void
CQTabWidgetTabBar::
tabEditFinished(const QString &text)
{
  setTabText(ind_, text);

  emit tabChanged(ind_);

  ind_ = -1;
}

void
CQTabWidgetTabBar::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = tabWidget_->createTabMenu();

  (void) menu->exec(e->globalPos());

  delete menu;
}
