#include <CQLineDash.h>
#include <CQIconCombo.h>
#include <CQUtil.h>
#include <CQUtilLineDash.h>
#include <CQSwitchLineEdit.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QMenu>
#include <QPainter>
#include <QIconEngine>
#include <QKeyEvent>

#if 1
#include <QProxyStyle>

class CQLineDashProxyStyle : public QProxyStyle {
 public:
  CQLineDashProxyStyle(int is, int iconSize) :
   is_(is), iconSize_(iconSize) {
  }

  int pixelMetric(PixelMetric metric, const QStyleOption *option,
                  const QWidget *widget) const override {
    if      (metric == QStyle::PM_MenuHMargin  ) return 0;
    else if (metric == QStyle::PM_MenuVMargin  ) return 0;
    else if (metric == QStyle::PM_SmallIconSize) return iconSize_;

    return QProxyStyle::pixelMetric(metric, option, widget);
  }

  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size,
                         const QWidget *widget) const override {
    if (type == QStyle::CT_MenuItem)
      return QSize(iconSize_ + is_ + size.width(), 2*is_);

    return QProxyStyle::sizeFromContents(type, option, size, widget);
  }

 private:
  int is_       { 16 };
  int iconSize_ { 16 };
};
#endif

//---

// draw line icon pixmap
// TODO: cache pixmap ?
class CQLineDashIconEngine : public QIconEngine {
 public:
  CQLineDashIconEngine(const CLineDash &dash) :
   dash_(dash) {
  }

  QSize actualSize(const QSize & size, QIcon::Mode mode, QIcon::State state) override;

  QPixmap pixmap(const QSize & size, QIcon::Mode mode, QIcon::State state) override;

  void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;

  QIconEngine *clone() const override { return new CQLineDashIconEngine(dash_); }

 private:
  CLineDash dash_;
};

//------

CQLineDash::
CQLineDash(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("line_dash");

  setFrameStyle(uint(QFrame::NoFrame) | uint(QFrame::Plain));

  //---

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  int is = style()->pixelMetric(QStyle::PM_SmallIconSize);

  //---

  // editable controls

  edit_ = CQUtil::makeWidget<QLineEdit>("edit");

  edit_->setToolTip("Line Dash\n(List of Dash Lengths)");

  connect(edit_, SIGNAL(editingFinished()), this, SLOT(dashChangedSlot()));

  layout->addWidget(edit_);

  button_ = CQUtil::makeWidget<QToolButton>("button");

  menu_ = CQUtil::makeWidget<QMenu>();

  menu_->setStyle(new CQLineDashProxyStyle(is, 8*is));

  button_->setMenu(menu_);

  connect(menu_, SIGNAL(triggered(QAction *)), this, SLOT(menuItemActivated(QAction *)));

  button_->setPopupMode(QToolButton::InstantPopup);
  button_->setAutoRaise(true);
  button_->setFixedSize(QSize(is + 2, is + 6));

  layout->addWidget(button_);

  //---

  // combo control

  combo_ = CQUtil::makeWidget<CQIconCombo>("combo");

  combo_->setIconWidth(5*is);

  combo_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  layout->addWidget(combo_);

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboItemChanged()));

  //---

  addDashOption("solid" , CLineDash());
  addDashOption("1, 1"  , CLineDash("1, 1"));
  addDashOption("1, 2"  , CLineDash("1, 2"));
  addDashOption("1, 3"  , CLineDash("1, 3"));
  addDashOption("1, 4"  , CLineDash("1, 4"));
  addDashOption("1, 6"  , CLineDash("1, 6"));
  addDashOption("1, 8"  , CLineDash("1, 8"));
  addDashOption("1, 12" , CLineDash("1, 12"));
  addDashOption("1, 24" , CLineDash("1, 24"));
  addDashOption("1, 48" , CLineDash("1, 48"));
  addDashOption("2, 1"  , CLineDash("2, 1"));
  addDashOption("3, 1"  , CLineDash("3, 1"));
  addDashOption("4, 1"  , CLineDash("4, 1"));
  addDashOption("6, 1"  , CLineDash("6, 1"));
  addDashOption("8, 1"  , CLineDash("8, 1"));
  addDashOption("12, 1" , CLineDash("12, 1"));
  addDashOption("24, 1" , CLineDash("24, 1"));
  addDashOption("2, 2"  , CLineDash("2, 2"));
  addDashOption("3, 3"  , CLineDash("3, 3"));
  addDashOption("4, 4"  , CLineDash("4, 4"));
  addDashOption("6, 6"  , CLineDash("6, 6"));
  addDashOption("8, 8"  , CLineDash("8, 8"));
  addDashOption("12, 12", CLineDash("12, 12"));
  addDashOption("24, 24", CLineDash("24, 24"));
  addDashOption("2, 4"  , CLineDash("2, 4"));
  addDashOption("4, 2"  , CLineDash("4, 2"));
  addDashOption("2, 6"  , CLineDash("2, 6"));
  addDashOption("6, 2"  , CLineDash("6, 2"));
  addDashOption("4, 8"  , CLineDash("4, 8"));
  addDashOption("8, 4"  , CLineDash("8, 4"));

  addDashOption("2,1,0.5,1", CLineDash("2,1,0.5,1"));
  addDashOption("4,2,1,2"  , CLineDash("4,2,1,2"));
  addDashOption("8,2,1,2"  , CLineDash("8,2,1,2"));
  addDashOption("0.5,0.5"  , CLineDash("0.5,0.5"));
  addDashOption("0.25,0.25", CLineDash("0.25,0.25"));
  addDashOption("0.1,0.1"  , CLineDash("0.1,0.1"));

  //---

  updateState();

  //---

  combo_->installEventFilter(this);
  edit_ ->installEventFilter(this);
}

void
CQLineDash::
setEditable(bool edit)
{
  if (edit == editable_)
    return;

  editable_ = edit;

  updateState();
}

void
CQLineDash::
setLineDash(const CLineDash &dash)
{
  dash_ = dash;

  updateState();
}

void
CQLineDash::
updateState()
{
  edit_  ->setVisible(editable_);
  button_->setVisible(editable_);
  combo_ ->setVisible(! editable_);

  edit_->setText(QString::fromStdString(dash_.toString()));

  for (int i = 0; i < combo_->count(); ++i) {
    QVariant var = combo_->itemData(i);

    CLineDash dash = var.value<CLineDash>();

    if (dash_ == dash) {
      if (i != combo_->currentIndex())
        combo_->setCurrentIndex(i);

      break;
    }
  }

  if (editable_) {
    setFocusProxy(edit_);

    edit_->setFocus();

    layout()->setMargin(0);
  }
  else {
    setFocusProxy(combo_);

    combo_->setFocus();

    layout()->setMargin(3);
  }
}

void
CQLineDash::
dashChangedSlot()
{
  // line edit changed
  CLineDash dash;

  QString str = edit_->text();

  if (dash.fromString(str.toStdString())) {
    dash_ = dash;

    emit valueChanged(dash_);
  }
}

void
CQLineDash::
menuItemActivated(QAction *action)
{
  // edit menu changed
  edit_->setText(action->text());

  dashChangedSlot();
}

void
CQLineDash::
comboItemChanged()
{
  // combo (non editable) changed
  int ind = combo_->currentIndex();

  QVariant var = combo_->itemData(ind);

  dash_ = var.value<CLineDash>();

  emit valueChanged(dash_);
}

void
CQLineDash::
addDashOption(const std::string &id, const CLineDash &dash)
{
  auto p = actions_.find(id);
  if (p != actions_.end()) return;

  QIcon icon = dashIcon(dash);

  auto *action = new CQLineDashAction(this, id, dash, icon);

  actions_[id] = action;

  menu_->addAction(action);

  combo_->addItem(icon, action->text(), QVariant::fromValue(dash));
}

QIcon
CQLineDash::
dashIcon(const CLineDash &dash)
{
  return QIcon(new CQLineDashIconEngine(dash));
}

bool
CQLineDash::
event(QEvent *event)
{
  switch (event->type()) {
    case QEvent::KeyPress: {
      auto *ke = static_cast<QKeyEvent *>(event);

      if (CQSwitchLineEdit::isSwitchKey(ke))
        setEditable(! isEditable());

      break;
    }
    default:
      break;
  }

  return QFrame::event(event);
}

bool
CQLineDash::
eventFilter(QObject *o , QEvent *e)
{
  if      (e->type() == QEvent::KeyPress) {
    auto *ke = static_cast<QKeyEvent *>(e);

    if (CQSwitchLineEdit::isSwitchKey(ke)) {
      setEditable(! isEditable());
      return true;
    }
  }
  else if (e->type() == QEvent::KeyRelease) {
    auto *ke = static_cast<QKeyEvent *>(e);

    if (CQSwitchLineEdit::isSwitchKey(ke)) {
      return true;
    }
  }

  return QObject::eventFilter(o, e);
}

//---

CQLineDashAction::
CQLineDashAction(CQLineDash *parent, const std::string &id,
                 const CLineDash &dash, const QIcon &icon) :
 QAction(parent), parent_(parent), id_(id), dash_(dash)
{
  setIcon(icon);
  setText(QString::fromStdString(id_));

  setIconVisibleInMenu(true);
}

//---

QSize
CQLineDashIconEngine::
actualSize(const QSize &size, QIcon::Mode /*mode*/, QIcon::State /*state*/)
{
  return size;
}

QPixmap
CQLineDashIconEngine::
pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
  QPixmap pixmap(size);

  QPainter painter(&pixmap);

  paint(&painter, QRect(QPoint(0, 0), size), mode, state);

  return pixmap;
}

void
CQLineDashIconEngine::
paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
  painter->setRenderHints(QPainter::Antialiasing);

  QColor bg, fg;

  if      (state == QIcon::On) {
    bg = qApp->palette().highlight().color();
    fg = QColor(255, 255, 255);
  }
  else if (mode == QIcon::Active || mode == QIcon::Selected) {
    bg = qApp->palette().highlight().color();
    fg = QColor(255, 255, 255);
  }
  else {
  //bg = QColor(255, 255, 255); //qApp->palette().base().color();
    bg = qApp->palette().window().color();
    fg = qApp->palette().text().color();
  }

  painter->fillRect(rect, bg);

  QPen pen(fg);

  pen.setWidthF(1.5);

  CQUtil::penSetLineDash(pen, dash_);

  int y = rect.height()/2;

  painter->setPen(pen);

  painter->drawLine(2, y, rect.width() - 2, y);
}
