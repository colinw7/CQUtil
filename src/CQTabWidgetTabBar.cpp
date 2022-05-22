#include <CQTabWidgetTabBar.h>
#include <CQTabWidget.h>
#include <CQFloatEdit.h>

#include <QMouseEvent>
#include <QMenu>

#ifndef CQTAB_WIDGET_MOVABLE
#include <svg/move_tab_light_svg.h>
#include <svg/move_tab_dark_svg.h>
#endif

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
