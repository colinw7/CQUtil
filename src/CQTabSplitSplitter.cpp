#include <CQTabSplitSplitter.h>
#include <CQTabSplitSplitterHandle.h>
#include <CQTabSplitSplitterTool.h>
#include <CQTabSplit.h>

//#include <iostream>

CQTabSplitSplitter::
CQTabSplitSplitter(CQTabSplit *split) :
 QSplitter(split), split_(split)
{
  setObjectName("splitter");
}

QSplitterHandle *
CQTabSplitSplitter::
createHandle()
{
  return new CQTabSplitSplitterHandle(orientation(), this);
}

int
CQTabSplitSplitter::
handleIndex(CQTabSplitSplitterHandle *handle) const
{
  for (int i = 0; i < count(); ++i) {
    auto *ihandle = this->handle(i);

    if (ihandle == handle)
      return i;
  }

  return -1;
}

void
CQTabSplitSplitter::
autoFit(int ind)
{
  int ind1 = ind - 1; // fit widget above splitter

  int n = count();
  if (n <= 1) return;

  if (ind1 < 0 || ind >= n - 1)
    return;

  auto sizes = this->sizes();

  auto s1 = sizes[ind1];

  auto *w = widget(ind1);

  auto sh = w->sizeHint();

  auto s2 = (orientation() == Qt::Vertical ? sh.height() : sh.width());

  auto ds = s2 - s1;

  sizes[ind1 ] = s2;
  sizes[n - 1] -= ds;

  setSizes(sizes);

  fixSizes();
}

void
CQTabSplitSplitter::
setTabbed()
{
  split()->setState(CQTabSplit::State::TAB);
}

void
CQTabSplitSplitter::
toggleSplit()
{
  if (split()->orientation() == Qt::Horizontal)
    split()->setOrientation(Qt::Vertical);
  else
    split()->setOrientation(Qt::Horizontal);
}

void
CQTabSplitSplitter::
fitAll()
{
  auto sizes = this->sizes();

  int n = count();
  if (n <= 1) return;

  int ds = 0;

  for (int i = 0; i < n - 1; ++i) {
    auto *w = widget(i);

    auto sh = w->sizeHint();

    auto s1 = sizes[i] - ds;
    auto s2 = (orientation() == Qt::Vertical ? sh.height() : sh.width());

    ds = s2 - s1;

    sizes[i] = s2;
  }

  sizes[n - 1] -= ds;

  setSizes(sizes);

  fixSizes();
}

void
CQTabSplitSplitter::
fixSizes()
{
  auto sizes = this->sizes();

  int n = count();
  if (n <= 1) return;

  int ds = 0;

  for (int i = 0; i < n - 1; ++i) {
    auto *w = widget(i);

    auto sh = w->minimumSizeHint();

    auto s1 = sizes[i] - ds;
    auto s2 = (orientation() == Qt::Vertical ? sh.height() : sh.width());

    if (s1 < s2) {
      ds = s2 - s1;

      sizes[i] = s2;
    }
  }

  sizes[n - 1] -= ds;

  setSizes(sizes);
}

void
CQTabSplitSplitter::
resizeEvent(QResizeEvent *e)
{
  QSplitter::resizeEvent(e);

  if (split()->isAutoFit()) {
    if (lastSizes_.empty())
      fitAll();

    lastSizes_ = sizes();
  }
}

void
CQTabSplitSplitter::
showTool(const QPoint &pos)
{
  auto *splitterTool = split_->getSplitterTool();

  splitterTool->setSplitter(this);
  splitterTool->showSlot();

  auto w = splitterTool->sizeHint().width();
  auto h = splitterTool->sizeHint().height();

  splitterTool->move(pos.x() - w/2, pos.y() - h/2);
}
