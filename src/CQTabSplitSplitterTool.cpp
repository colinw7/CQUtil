#include <CQTabSplitSplitterTool.h>
#include <CQTabSplitSplitter.h>
#include <CQTabSplit.h>

#include <CQUtil.h>
#include <CQIconButton.h>
#include <QTimer>

#include <svg/hsplit_svg.h>
#include <svg/vsplit_svg.h>
#include <svg/tab_svg.h>

CQTabSplitSplitterTool::
CQTabSplitSplitterTool(CQTabSplit *split) :
 QFrame(split, Qt::Tool | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint), split_(split)
{
  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  auto makeIcon = [&](const char *iconName, const char *slotName, const char *tip) {
    auto *button = new CQIconButton(this);
    button->setIcon(iconName);
    button->setAutoRaise(true);
    button->setToolTip(tip);
    connect(button, SIGNAL(clicked()), this, slotName);
    return button;
  };

  hbutton_ = makeIcon("HSPLIT", SLOT(hsplitSlot()), "Horizontal Split");
  vbutton_ = makeIcon("VSPLIT", SLOT(vsplitSlot()), "Vertical Split");
  tbutton_ = makeIcon("TAB"   , SLOT(tabbedSlot()), "Tabbed");

  layout->addWidget(hbutton_);
  layout->addWidget(vbutton_);
  layout->addWidget(tbutton_);
}

void
CQTabSplitSplitterTool::
startHideTimer()
{
  if (! timer_) {
    timer_ = new QTimer;

    timer_->setSingleShot(true);

    connect(timer_, SIGNAL(timeout()), this, SLOT(hideSlot()));
  }

  timer_->start(250);

  //std::cerr << "startHideTimer\n";
}

void
CQTabSplitSplitterTool::
resetHideTimer()
{
  delete timer_;

  timer_ = nullptr;

  //std::cerr << "resetHideTimer\n";
}

void
CQTabSplitSplitterTool::
showSlot()
{
  hbutton_->setVisible(split_->state() != CQTabSplit::State::HSPLIT);
  vbutton_->setVisible(split_->state() != CQTabSplit::State::VSPLIT);
  tbutton_->setVisible(split_->state() != CQTabSplit::State::TAB);

  layout()->invalidate();

  show();
}

void
CQTabSplitSplitterTool::
hideSlot()
{
  hide();

  //std::cerr << "hide\n";
}

bool
CQTabSplitSplitterTool::
event(QEvent *event)
{
  switch (event->type()) {
    case QEvent::Leave:
    case QEvent::FocusOut:
      startHideTimer(); break;

    case QEvent::Enter:
    case QEvent::FocusIn:
    case QEvent::KeyPress:
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
      resetHideTimer(); break;

    default: break;
  }

  return QFrame::event(event);
}

void
CQTabSplitSplitterTool::
hsplitSlot()
{
  if (splitter_)
    splitter_->toggleSplit();
  else
    split_->setState(CQTabSplit::State::HSPLIT);

  hideSlot();
}

void
CQTabSplitSplitterTool::
vsplitSlot()
{
  if (splitter_)
    splitter_->toggleSplit();
  else
    split_->setState(CQTabSplit::State::VSPLIT);

  hideSlot();
}

void
CQTabSplitSplitterTool::
tabbedSlot()
{
  if (splitter_)
    splitter_->setTabbed();

  hideSlot();
}
