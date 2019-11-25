#include <CQTabWidget.h>
#include <CQImageButton.h>
#include <CQFloatEdit.h>

#include <QApplication>
#include <QMouseEvent>
#include <QHBoxLayout>

#include <svg/new_svg.h>
#include <svg/left_svg.h>
#include <svg/right_svg.h>

#ifndef CQTAB_WIDGET_MOVABLE
#include <svg/tab_svg.h>
#endif

CQTabWidget::
CQTabWidget(QWidget *parent) :
 QTabWidget(parent), moveButtons_(false)
{
  setObjectName("tabWidget");

  tabBar_ = new CQTabBar();

  setTabBar(tabBar_);

  connect(tabBar_, SIGNAL(tabChanged(int)), this, SIGNAL(tabChanged(int)));

  connect(tabBar_, SIGNAL(tabMoveRequested(int, int)), this, SLOT(moveTab(int, int)));

#ifndef CQTAB_WIDGET_MOVABLE
  connect(tabBar_, SIGNAL(tabMoved(int,int)), this, SIGNAL(swapTabs(int,int)));
#endif

  CQImageButton *left_button  = new CQImageButton(CQPixmapCacheInst->getIcon("LEFT" ));
  CQImageButton *right_button = new CQImageButton(CQPixmapCacheInst->getIcon("RIGHT"));

  connect(left_button , SIGNAL(clicked()), this, SLOT(moveTabLeft ()));
  connect(right_button, SIGNAL(clicked()), this, SLOT(moveTabRight()));

  moveTabWidget_ = new QWidget;

  QHBoxLayout *moveTabLayout = new QHBoxLayout(moveTabWidget_);
  moveTabLayout->setMargin(2); moveTabLayout->setSpacing(2);

  moveTabLayout->addWidget(left_button);
  moveTabLayout->addWidget(right_button);
}

void
CQTabWidget::
setShowMoveButtons(bool show)
{
  moveButtons_ = show;

  setCornerWidget((show ? moveTabWidget_ : 0), Qt::TopRightCorner);

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

  QWidget *widget = QTabWidget::widget(fromIndex);

  QIcon   icon = tabIcon(fromIndex);
  QString text = tabText(fromIndex);

  removeTab(fromIndex);

  insertTab(toIndex, widget, icon, text);

  setCurrentIndex(toIndex);

  emit swapTabs(fromIndex, toIndex);
}

void
CQTabWidget::
addCreateButton()
{
  CQImageButton *new_button = new CQImageButton(CQPixmapCacheInst->getIcon("NEW" ));

  connect(new_button, SIGNAL(clicked()), this, SIGNAL(createTab()));

  setCornerWidget(new_button, Qt::TopLeftCorner);
}

//-----------

CQTabBar::
CQTabBar(QWidget *parent) :
 QTabBar(parent), edit_(nullptr), ind_(-1)
{
  setObjectName("tabBar");

  setAcceptDrops(true);

#ifdef CQTAB_WIDGET_MOVABLE
  setMovable(true);
#endif
}

#ifndef CQTAB_WIDGET_MOVABLE
void
CQTabBar::
mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    press_pos_ = event->pos();

  QTabBar::mousePressEvent(event);
}

void
CQTabBar::
mouseMoveEvent(QMouseEvent *event)
{
  // If the left button isn't pressed anymore then return
  if (! (event->buttons() & Qt::LeftButton))
    return;

  // If the distance is too small then return
  if ((event->pos() - press_pos_).manhattanLength() < QApplication::startDragDistance())
    return;

  // initiate Drag
  QDrag *drag = new QDrag(this);

  drag->setPixmap((CQPixmapCacheInst->getPixmap("TAB"));

  QMimeData *mimeData = new QMimeData;

  // a crude way to distinguish tab-reordering drops from other ones
  mimeData->setData("action", "tab-reordering") ;

  drag->setMimeData(mimeData);

  drag->exec();
}
#endif

void
CQTabBar::
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
CQTabBar::
dragEnterEvent(QDragEnterEvent *event)
{
  // Only accept if it's an tab-reordering request
  const QMimeData *m = event->mimeData();

  QStringList formats = m->formats();

  if (formats.contains("action") && (m->data("action") == "tab-reordering"))
    event->acceptProposedAction();
}

void
CQTabBar::
dropEvent(QDropEvent *event)
{
  int fromIndex = tabAt(press_pos_);
  int toIndex   = tabAt(event->pos());

  // Tell interested objects that
  if (fromIndex != toIndex)
    emit tabMoveRequested(fromIndex, toIndex);

  event->acceptProposedAction();
}
#endif

void
CQTabBar::
paintEvent(QPaintEvent *event)
{
  QTabBar::paintEvent(event);

  if (ind_ > 0 && edit_)
    edit_->raise();
}

void
CQTabBar::
stopEdit()
{
  edit_->hide();
}

void
CQTabBar::
tabEditFinished(const QString &text)
{
  setTabText(ind_, text);

  emit tabChanged(ind_);

  ind_ = -1;
}
