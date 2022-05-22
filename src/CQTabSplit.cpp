#include <CQTabSplit.h>
#include <CQTabSplitSplitter.h>
#include <CQTabSplitSplitterTool.h>
#include <CQGroupBox.h>

#include <QApplication>
#include <QTabBar>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include <cassert>

CQTabSplit::
CQTabSplit(Qt::Orientation orient, QWidget *parent) :
 QFrame(parent), orient_(orient)
{
  init();
}

CQTabSplit::
CQTabSplit(QWidget *parent) :
 QFrame(parent)
{
  init();
}

void
CQTabSplit::
init()
{
  setObjectName("tabSplit");

  auto *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  splitter_  = new CQTabSplitSplitter (this);
  tabWidget_ = new CQTabSplitTabWidget(this);

  layout->addWidget(splitter_);
  layout->addWidget(tabWidget_);

  tabWidget_->setVisible(false);

  splitter_->setOrientation(orientation());

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(currentIndexSlot(int)));
}

void
CQTabSplit::
setOrientation(Qt::Orientation orient)
{
  if (state_ != State::TAB)
    setState(orient == Qt::Horizontal ? State::HSPLIT : State::VSPLIT);
  else
    orient_ = orient;
}

void
CQTabSplit::
setGrouped(bool b)
{
  if (b != grouped_) {
    grouped_ = b;

    // TODO: re-layout with/widthout groups
  }
}

void
CQTabSplit::
setGrouped(QWidget *w, bool b)
{
  w->setProperty("CQTabSplit::grouped", QVariant(b ? 1 : 0));
}

bool
CQTabSplit::
isGrouped(QWidget *w) const
{
  auto var = w->property("CQTabSplit::grouped");

  if (var.isValid()) {
    bool ok;
    auto i =var.toInt(&ok);
    if (ok) return i;
  }

  return isGrouped();
}

void
CQTabSplit::
setTabsClosable(bool b)
{
  if (b != tabsClosable_) {
    tabsClosable_ = b;

    tabWidget_->setTabsClosable(b);
  }
}

void
CQTabSplit::
setCurrentIndex(int i)
{
  if (i < 0 || i >= int(widgets_.size()))
    return;

  tabWidget_->setCurrentIndex(i);
}

void
CQTabSplit::
currentIndexSlot(int i)
{
  currentIndex_ = i;
}

void
CQTabSplit::
setSizes(const QList<int> &sizes)
{
  if (orientation() == Qt::Horizontal)
    hsizes_ = sizes;
  else
    vsizes_ = sizes;

  splitter_->setSizes(sizes);
}

void
CQTabSplit::
addWidget(QWidget *w, const QString &name)
{
  assert(w);

  w->setAutoFillBackground(true);

  WidgetData data(w, name);

  if (isGrouped(w)) {
    data.group = new CQGroupBox(name);
    data.group->setObjectName("group");

    data.layout = new QVBoxLayout(data.group);
    data.layout->setMargin(2); data.layout->setSpacing(2);

    data.layout->addWidget(w);
  }

  if (widgets_.empty())
    currentIndex_ = 0;

  widgets_.push_back(data);

  if (state_ == State::HSPLIT || state_ == State::VSPLIT) {
    assert(splitter_);

    if (isGrouped(w))
      splitter_->addWidget(data.group);
    else
      splitter_->addWidget(data.w);
  }
  else {
    tabWidget_->addTab(data.w, name);
  }

  w->setVisible(true); // has been reparented
}

void
CQTabSplit::
removeWidget(QWidget *w, bool deleteWidget)
{
  int i = 0;

  for (auto &data : widgets_) {
    if (w == data.w)
      break;

    ++i;
  }

  if (i >= int(widgets_.size())) {
    assert(false);
    return;
  }

  if (i == currentIndex_)
    i = currentIndex_ - 1;

  WidgetData data;

  if (i >= 0)
    data = widgets_[size_t(i)];

  for (auto j = size_t(i + 1); j < widgets_.size(); ++j)
    widgets_[j - 1] = widgets_[j];

  widgets_.pop_back();

  if (! deleteWidget)
    data.w->setParent(nullptr); // makes invisible

  delete data.group;

  if (deleteWidget)
    delete data.w;

  if (widgets_.empty())
    currentIndex_ = -1;
}

void
CQTabSplit::
removeAllWidgets()
{
  for (auto &data : widgets_) {
    delete data.group;
    delete data.w;
  }

  widgets_.clear();

  currentIndex_ = -1;
}

bool
CQTabSplit::
hasWidget(QWidget *w) const
{
  for (auto &data : widgets_) {
    if (w == data.w)
      return true;
  }

  return false;
}

QWidget *
CQTabSplit::
widget(int i) const
{
  if (i < 0 || i >= int(widgets_.size()))
    return nullptr;

  return widgets_[size_t(i)].w;
}

void
CQTabSplit::
setWidgetName(QWidget *w, const QString &name)
{
  WidgetData *data = nullptr;

  for (auto &data1 : widgets_) {
    if (w == data1.w) {
      data = &data1;
      break;
    }
  }

  if (! data)
    return;

  data->name = name;

  if      (state_ == State::TAB) {
    int i = tabWidget_->indexOf(data->w);

    if (i >= 0)
      tabWidget_->setTabText(i, data->name);
  }
  else if (state_ == State::HSPLIT || state_ == State::VSPLIT) {
    if (isGrouped(w))
      data->group->setTitle(data->name);
  }
}

int
CQTabSplit::
count() const
{
  return int(widgets_.size());
}

void
CQTabSplit::
setState(State state)
{
  if (state_ == state)
    return;

  std::swap(state_, state);

  splitter_ ->setVisible(state_ == State::HSPLIT || state_ == State::VSPLIT);
  tabWidget_->setVisible(state_ == State::TAB);

  // from tab -> splitter
  if      (state == State::TAB) {
    // remove widgets from tab
    int n = tabWidget_->count();

    assert(n == int(widgets_.size()));

    for (int i = 0; i < n; ++i)
      tabWidget_->removeTab(0);

    //---

    //add widgets to splitter
    for (auto &data : widgets_) {
      if (! data.w)
        continue;

      if (isGrouped(data.w))
        splitter_->addWidget(data.group);
      else
        splitter_->addWidget(data.w);

      if (isGrouped(data.w)) {
        data.layout->addWidget(data.w);

        data.group->setVisible(true);
      }

      data.w->setVisible(true);
    }

    //---

    // restore saved sizes
    if (orientation() == Qt::Horizontal) {
      if (hsizes_.length())
        splitter_->setSizes(hsizes_);
    }
    else {
      if (vsizes_.length())
        splitter_->setSizes(vsizes_);
    }
  }
  // from splitter -> tab
  else if (state_ == State::TAB) {
    // save sizes
    if (orientation() == Qt::Horizontal)
      hsizes_ = splitter_->sizes();
    else
      vsizes_ = splitter_->sizes();

    //---

    // remove widgets from splitter
    int n = splitter_->count();

    assert(n == int(widgets_.size()));

    for (auto &data : widgets_) {
      if (! data.w)
        continue;

      if (isGrouped(data.w))
        data.group->setParent(this);
      else
        data.w->setParent(this);
    }

    //---

    // add widgets to tab
    for (auto &data : widgets_) {
      if (! data.w)
        continue;

      tabWidget_->addTab(data.w, data.name);
    }
  }
  // new splitter direction
  else {
    orient_ = (state_ == State::HSPLIT ? Qt::Horizontal : Qt::Vertical);

    splitter_->setOrientation(orientation());
  }
}

void
CQTabSplit::
tabCloseSlot(int i)
{
  emit widgetCloseRequested(i);
}

//---

CQTabSplitSplitterTool *
CQTabSplit::
getSplitterTool()
{
  if (! splitterTool_)
    splitterTool_ = new CQTabSplitSplitterTool(this);

  return splitterTool_;
}

//---

QSize
CQTabSplit::
sizeHint() const
{
  if (state_ == State::TAB)
    return tabWidget_->sizeHint();
  else
    return splitter_->sizeHint();
}

//------

CQTabSplitTabWidget::
CQTabSplitTabWidget(CQTabSplit *split) :
 split_(split)
{
  setObjectName("tab");

  setContextMenuPolicy(Qt::DefaultContextMenu);

  connect(this, SIGNAL(tabCloseRequested(int)), split_, SLOT(tabCloseSlot(int)));

  //---

  tabBar_ = new CQTabSplitTabBar(this);

  setTabBar(tabBar_);
}

void
CQTabSplitTabWidget::
contextMenuEvent(QContextMenuEvent *e)
{
  if (! count())
    return;

  auto p = e->pos();
  auto r = tabBar()->rect();

  if (p.y() >= r.height())
    return;

  auto *menu = new QMenu(this);

  menu->setObjectName("menu");

  //---

  auto *hsplitAction = menu->addAction("Horizontal Split");
  auto *vsplitAction = menu->addAction("Vertical Split");

  connect(hsplitAction, SIGNAL(triggered()), this, SLOT(hsplitSlot()));
  connect(vsplitAction, SIGNAL(triggered()), this, SLOT(vsplitSlot()));

  //---

  menu->addSeparator();

  for (int i = 0; i < count(); ++i) {
    auto icon = tabIcon(i);
    auto text = tabText(i);

    auto *action = menu->addAction(icon, text);

    action->setCheckable(true);
    action->setChecked(i == currentIndex());

    action->setData(i);

    connect(action, SIGNAL(triggered()), this, SLOT(tabSlot()));
  }

  //---

  (void) menu->exec(e->globalPos());

  delete menu;
}

void
CQTabSplitTabWidget::
hsplitSlot()
{
  split()->setState(CQTabSplit::State::HSPLIT);
}

void
CQTabSplitTabWidget::
vsplitSlot()
{
  split()->setState(CQTabSplit::State::VSPLIT);
}

void
CQTabSplitTabWidget::
tabSlot()
{
  auto *action = qobject_cast<QAction *>(sender());
  if (! action) return;

  int i = action->data().toInt();

  setCurrentIndex(i);
}

bool
CQTabSplitTabWidget::
event(QEvent *event)
{
  switch (event->type()) {
    case QEvent::ToolTip: {
      auto *he = static_cast<QHelpEvent*>(event);

      auto gpos = mapToGlobal(he->pos());

      auto *widget = qApp->widgetAt(gpos);

      if (widget == this)
        showTool(gpos);

      break;
    }

    default: break;
  }

  return QTabWidget::event(event);
}

void
CQTabSplitTabWidget::
showTool(const QPoint &pos)
{
  auto *splitterTool = split()->getSplitterTool();

  splitterTool->setSplitter(nullptr);
  splitterTool->showSlot();

  auto w = splitterTool->sizeHint().width();
  auto h = splitterTool->sizeHint().height();

  splitterTool->move(pos.x() - w/2, pos.y() - h/2);
}

//---

CQTabSplitTabBar::
CQTabSplitTabBar(CQTabSplitTabWidget *tabWidget) :
 QTabBar(tabWidget), tabWidget_(tabWidget)
{
  setObjectName("tabBar");
}
