#include <CQTreeView.h>
#include <CQHeaderView.h>
#include <QMenu>

CQTreeView::
CQTreeView(QWidget *parent) :
 QTreeView(parent)
{
  setObjectName("treeView");

  //--

  header_ = new CQHeaderView(this);

  setHeader(header_);

  //---

  setSelectionMode(ExtendedSelection);

  setUniformRowHeights(true);

  setAlternatingRowColors(true);

  //---

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(customContextMenuSlot(const QPoint&)));
}

CQTreeView::
~CQTreeView()
{
}

void
CQTreeView::
customContextMenuSlot(const QPoint &pos)
{
  // Map point to global from the viewport to account for the header.
  QPoint mpos = viewport()->mapToGlobal(pos);

  auto *menu = new QMenu;

  addMenuActions(menu);

  menuIndex_ = indexAt(pos);

  menu->exec(mpos);

  delete menu;
}

void
CQTreeView::
addMenuActions(QMenu *menu)
{
  auto *expandAction   = new QAction("Expand All"  , menu);
  auto *collapseAction = new QAction("Collapse All", menu);
  auto *setRootAction  = new QAction("Set Root"    , menu);

  connect(expandAction  , SIGNAL(triggered()), this, SLOT(expandAll()));
  connect(collapseAction, SIGNAL(triggered()), this, SLOT(collapseAll()));
  connect(setRootAction , SIGNAL(triggered()), this, SLOT(setRootSlot()));

  menu->addAction(expandAction);
  menu->addAction(collapseAction);
  menu->addAction(setRootAction);
}

void
CQTreeView::
expandAll(const QModelIndex &ind)
{
  setExpanded(ind, true);

  for (int r = 0; r < model()->rowCount(ind); ++r) {
    QModelIndex ind1 = model()->index(r, 0, ind);

    expandAll(ind1);
  }
}

void
CQTreeView::
collapseAll(const QModelIndex &ind)
{
  setExpanded(ind, false);

  for (int r = 0; r < model()->rowCount(ind); ++r) {
    QModelIndex ind1 = model()->index(r, 0, ind);

    collapseAll(ind1);
  }
}

void
CQTreeView::
setRootSlot()
{
  setRootIndex(menuIndex_);
}
