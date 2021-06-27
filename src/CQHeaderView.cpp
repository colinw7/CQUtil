#include <CQHeaderView.h>
#include <QTreeWidget>
#include <QTableWidget>
#include <QSortFilterProxyModel>
#include <QContextMenuEvent>
#include <QMenu>

#include <iostream>
#include <cassert>

CQHeaderView::
CQHeaderView(QWidget *parent) :
 QHeaderView(Qt::Horizontal, parent)
{
  setObjectName("headerView");

  connect(this, SIGNAL(sectionResized(int, int, int)), this,
          SLOT(handleSectionResized(int, int, int)));
  connect(this, SIGNAL(sectionMoved(int, int, int)), this,
          SLOT(handleSectionMoved(int, int, int)));
  connect(this, SIGNAL(sectionClicked(int)), this,
          SLOT(handleSectionClicked(int)));

  setSectionsMovable(true);
  setSectionsClickable(true);

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQHeaderView::
setSectionsMovable(bool movable)
{
#if QT_VERSION < 0x050000
  QHeaderView::setMovable(movable);
#else
  QHeaderView::setSectionsMovable(movable);
#endif
}

void
CQHeaderView::
setSectionsClickable(bool clickable)
{
#if QT_VERSION < 0x050000
  QHeaderView::setClickable(clickable);
#else
  QHeaderView::setSectionsClickable(clickable);
#endif
}

void
CQHeaderView::
setWidgetFactory(CQHeaderViewWidgetFactory *factory)
{
  factory_ = factory;
}

QWidget *
CQHeaderView::
widget(int i) const
{
  return widgets_[i];
}

void
CQHeaderView::
showEvent(QShowEvent *e)
{
  shown_ = true;

  initWidgets();

  QHeaderView::showEvent(e);

  doInitFit();
}

void
CQHeaderView::
resizeEvent(QResizeEvent *e)
{
  resized_ = true;

  QHeaderView::resizeEvent(e);

  doInitFit();
}

void
CQHeaderView::
doInitFit()
{
  if (shown_ && resized_ && initFit_) {
    fitAllSlot();

    initFit_ = false;
  }
}

void
CQHeaderView::
contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu(this);

  menu.addAction("Fit Column", this, SLOT(fitColumnSlot()));
  menu.addAction("Fit All"   , this, SLOT(fitAllSlot()));

  menu.addSeparator();

  auto *stretchAction = menu.addAction("Stretch Last", this, SLOT(stretchLastSlot(bool)));
  stretchAction->setCheckable(true);
  stretchAction->setChecked(stretchLastSection());

  auto *sortAction = menu.addAction("Sort Indicator", this, SLOT(sortIndicatorSlot(bool)));
  sortAction->setCheckable(true);
  sortAction->setChecked(isSortIndicatorShown());

  menu.addAction("Reset Sort", this, SLOT(resetSortSlot()));

  menu.addAction("Sort Increasing", this, SLOT(sortIncreasingSlot()));
  menu.addAction("Sort Decreasing", this, SLOT(sortDecreasingSlot()));

  menuSection_ = logicalIndexAt(event->pos());

  menu.exec(event->globalPos());
}

void
CQHeaderView::
fitColumnSlot()
{
  if (menuSection_ < 0)
    return;

  auto *tree  = qobject_cast<QTreeView  *>(parentWidget());
  auto *table = qobject_cast<QTableView *>(parentWidget());

  if      (tree) {
    int nr = tree->model()->rowCount();

    int w = 0;

    for (int r = 0; r < nr; ++r) {
      auto ind = tree->model()->index(r, menuSection_);

      auto s = tree->model()->data(ind, Qt::SizeHintRole).toSize();

      w = std::max(w, s.width());
    }

    if (w > 0)
      resizeSection(menuSection_, w);
  }
  else if (table) {
    int nr = table->model()->rowCount();

    int w = 0;

    for (int r = 0; r < nr; ++r) {
      auto ind = table->model()->index(r, menuSection_);

      auto s = table->model()->data(ind, Qt::SizeHintRole).toSize();

      w = std::max(w, s.width());
    }

    if (w > 0)
      resizeSection(menuSection_, w);
  }
}

void
CQHeaderView::
fitAllSlot()
{
  auto *tree  = qobject_cast<QTreeView  *>(parentWidget());
  auto *table = qobject_cast<QTableView *>(parentWidget());

  ColumnWidths columnWidths;

  QHeaderView        *header = nullptr;
  QAbstractItemModel *model  = nullptr;

  if      (tree) {
    header = tree->header();
    model  = tree->model();
  }
  else if (table) {
    header = table->horizontalHeader();
    model  = table->model();
  }

  if (! model)
    return;

  // calc header max column widths
  int nc = model->columnCount();

  for (int c = 0; c < nc; ++c) {
    auto s = model->headerData(c, Qt::Horizontal, Qt::SizeHintRole).toSize();

    if (! s.isValid() && header)
      s = QSize(header->sectionSizeHint(c), 0);

    if (s.isValid())
      columnWidths[c] = std::max(columnWidths[c], s.width());
  }

  if      (tree) {
    calcTreeWidths(tree, QModelIndex(), 0, columnWidths);
  }
  else if (table) {
    calcTableWidths(table, columnWidths);
  }

  for (int c = 0; c < nc; ++c) {
    if (columnWidths[c] > 0) {
      resizeSection(c, columnWidths[c] + 24);
    }
  }
}

void
CQHeaderView::
calcTreeWidths(QTreeView *tree, const QModelIndex &ind, int depth, ColumnWidths &columnWidths)
{
  int indent = depth*tree->indentation();

  QFontMetrics fm(font());

  auto *model = tree->model();

  int nc = model->columnCount();

  // calc row max column widths
  int nr = model->rowCount(ind);

  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      auto ind1 = model->index(r, c, ind);

      auto s = model->data(ind1, Qt::SizeHintRole).toSize();

      if (! s.isValid())
        s = tree->sizeHintForIndex(ind1);

      if (! s.isValid()) {
        auto str = model->data(ind1, Qt::DisplayRole).toString();

        s = QSize(fm.width(str), fm.height());
      }

      columnWidths[c] = std::max(columnWidths[c], s.width() + indent);
    }
  }

  // process children (column 0)
  for (int r = 0; r < nr; ++r) {
    auto ind1 = model->index(r, 0, ind);

    calcTreeWidths(tree, ind1, depth + 1, columnWidths);
  }
}

void
CQHeaderView::
calcTableWidths(QTableView *table, ColumnWidths &columnWidths)
{
  QFontMetrics fm(font());

  auto *model = table->model();

  int nc = model->columnCount();

  // calc row max widths
  int nr = model->rowCount();

  for (int r = 0; r < nr; ++r) {
    for (int c = 0; c < nc; ++c) {
      auto ind = model->index(r, c);

      auto s = model->data(ind, Qt::SizeHintRole).toSize();

      if (! s.isValid())
        s = table->sizeHintForIndex(ind);

      if (! s.isValid()) {
        auto str = model->data(ind, Qt::DisplayRole).toString();

        s = QSize(fm.width(str), fm.height());
      }

      columnWidths[c] = std::max(columnWidths[c], s.width());
    }
  }
}

void
CQHeaderView::
stretchLastSlot(bool b)
{
  setStretchLastSection(b);
}

void
CQHeaderView::
sortIndicatorSlot(bool b)
{
  setSortIndicatorShown(b);
}

void
CQHeaderView::
resetSortSlot()
{
  setSortIndicator(-1, Qt::AscendingOrder);

  auto *proxyModel = this->proxyModel();

  if (proxyModel) {
    sortRole_ = proxyModel->sortRole();

    proxyModel->setSortRole(Qt::InitialSortOrderRole);

    proxyModel->invalidate();

    //proxyModel->setSortRole(role);
  }
}

void
CQHeaderView::
sortIncreasingSlot()
{
  setSortIndicator(menuSection_, Qt::AscendingOrder);
}

void
CQHeaderView::
sortDecreasingSlot()
{
  setSortIndicator(menuSection_, Qt::DescendingOrder);
}

void
CQHeaderView::
initWidgets()
{
  if (! factory_)
    return;

  int n = count();

  while (widgets_.size() < n)
    widgets_.push_back(0);

  while (widgets_.size() > n) {
    auto *w = widgets_.back();

    delete w;

    widgets_.pop_back();
  }

  for (int i = 0; i < n; ++i) {
    auto *w = widgets_[i];

    if (! w) {
      w = factory_->createWidget(i);

      w->setParent(this);

      widgets_[i] = w;
    }

    w->setGeometry(sectionViewportPosition(i), 0, sectionSize(i) - 5, height());

    w->show();
  }
}

void
CQHeaderView::
handleSectionResized(int section, int oldSize, int newSize)
{
  Q_UNUSED(oldSize);
  Q_UNUSED(newSize);

//std::cerr << "handleSectionResized " << section << " " << oldSize << " " << newSize << "\n";
  for (int j = visualIndex(section); j < count(); ++j) {
    int logical = logicalIndex(j);

    if (logical < 0 || logical >= widgets_.size())
      continue;

    auto *w = widgets_[logical];

    w->setGeometry(sectionViewportPosition(logical), 0, sectionSize(logical) - 5, height());
  }
}

void
CQHeaderView::
handleSectionMoved(int /*logical*/, int oldVisualIndex, int newVisualIndex)
{
  for (int i = qMin(oldVisualIndex, newVisualIndex); i < count(); ++i) {
    int logical = logicalIndex(i);

    if (logical < 0 || logical >= widgets_.size())
      continue;

    auto *w = widgets_[logical];

    w->setGeometry(sectionViewportPosition(logical), 0, sectionSize(logical) - 5, height());
  }
}

void
CQHeaderView::
handleSectionClicked(int /*logical*/)
{
  if (sortRole_ >= 0) {
    auto *proxyModel = this->proxyModel();

    if (proxyModel)
      proxyModel->setSortRole(sortRole_);

    sortRole_ = -1;
  }
}

#if 0
void
CQHeaderView::
scrollContentsBy(QTableWidget *table, int dx, int dy)
{
  table->scrollContentsBy(dx, dy);

  if (dx != 0)
    fixWidgetPositions();
}
#endif

void
CQHeaderView::
fixWidgetPositions()
{
  for (int i = 0; i < count(); ++i) {
    auto *w = widgets_[i];

    w->setGeometry(sectionViewportPosition(i), 0, sectionSize(i) - 5, height());
  }
}

QSortFilterProxyModel *
CQHeaderView::
proxyModel()
{
  auto *tree  = qobject_cast<QTreeView  *>(parentWidget());
  auto *table = qobject_cast<QTableView *>(parentWidget());

  QSortFilterProxyModel *proxyModel = nullptr;

  if      (tree) {
    proxyModel = qobject_cast<QSortFilterProxyModel *>(tree->model());
  }
  else if (table) {
    proxyModel = qobject_cast<QSortFilterProxyModel *>(table->model());
  }

  return proxyModel;
}

void
CQHeaderView::
update()
{
  viewport()->update();

  QHeaderView::update();
}
