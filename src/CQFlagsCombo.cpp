#include <CQFlagsCombo.h>

#include <CQWidgetUtil.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QStylePainter>
#include <QStyleOptionComboBox>

CQFlagsCombo::
CQFlagsCombo(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("flagsCombo");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 1, 1);

  edit_ = CQUtil::makeWidget<QLineEdit>(this, "edit");

  edit_->setFrame(false);

  connect(edit_, SIGNAL(returnPressed()), this, SLOT(returnPressedSlot()));

  layout->addWidget(edit_);

  button_ = new CQFlagsComboMenuButton(this);

  button_->setFocusPolicy(Qt::NoFocus);

  connect(button_, SIGNAL(clicked()), this, SLOT(showMenu()));

  layout->addWidget(button_);

  setFocusProxy(edit_);
}

CQFlagsCombo::
~CQFlagsCombo()
{
}

void
CQFlagsCombo::
setValue(uint value)
{
  if (value != value_) {
    value_ = value;

    updateEdit();

    Q_EMIT valueChanged(value_);
  }
}

void
CQFlagsCombo::
returnPressedSlot()
{
  auto strs = edit_->text().split("|");

  uint value = 0;

  for (const auto &str : strs) {
    auto lstr = str.toLower();

    auto po = litemId_.find(lstr);
    if (po == litemId_.end()) continue;

    value |= (*po).second;
  }

  setValue(value);
}

void
CQFlagsCombo::
updateEdit()
{
  QString str;

  for (uint i = 0; i < 32; ++i) {
    uint id = (1<<i);

    if (! (value_ & id))
      continue;

    auto pi = idItem_.find(id);
    if (pi == idItem_.end()) continue;

    if (str != "")
      str += "|";

    str += (*pi).second;
  }

  if (str != edit_->text())
    edit_->setText(str);
}

void
CQFlagsCombo::
addItem(const QString &name, uint id)
{
  auto lname = name.toLower();

  itemId_ [name ] = id;
  litemId_[lname] = id;
  idItem_ [id   ] = name;

  (void) getMenu();

  menu_->addItem(name, id);

  updateEdit();
}

CQFlagsComboMenu *
CQFlagsCombo::
getMenu() const
{
  if (menu_ == nullptr) {
    auto *th = const_cast<CQFlagsCombo *>(this);

    th->menu_ = new CQFlagsComboMenu(th);
  }

  return menu_;
}

// display item menu
void
CQFlagsCombo::
showMenu()
{
  (void) getMenu();

  // update menu to match current text's items
  menu_->showSlot();

  // popup menu below or above the widget bounding box
  auto tl = edit_->mapToGlobal(edit_->rect().topLeft()) + QPoint(-4, 0);

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect);
}

// hide menu
void
CQFlagsCombo::
hideMenu()
{
  if (menu_)
    menu_->hide();
}

void
CQFlagsCombo::
paintEvent(QPaintEvent *)
{
  QStylePainter painter(this);

  //---

  painter.setPen(palette().color(QPalette::Text));

  // draw the combobox frame, focus rect and selected etc.
  QStyleOptionComboBox opt;

  initStyleOption(opt);

  painter.drawComplexControl(QStyle::CC_ComboBox, opt);

  // draw the control (no text)
  opt.currentText = "";

  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);

  //---

  auto r = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);

  painter.setClipRect(r);

  //---

  auto str = edit_->text();

  QFontMetrics fm(font());

  painter.drawText(r.left(), r.center().y() + (fm.ascent() - fm.descent())/2, str);
}

void
CQFlagsCombo::
initStyleOption(QStyleOptionComboBox &opt)
{
  opt.initFrom(this);

  opt.editable = true;
  opt.frame    = true;

  if (hasFocus()) opt.state |= QStyle::State_Selected;

  opt.subControls = QStyle::SC_All;

  if      (button_ && button_->isDown()) {
    opt.activeSubControls = QStyle::SC_ComboBoxArrow;
    opt.state |= QStyle::State_Sunken;
  }
  else if (button_ && button_->underMouse()) {
    opt.activeSubControls = QStyle::SC_ComboBoxArrow;
  }
  else if (edit_ && edit_->underMouse()) {
    opt.activeSubControls = QStyle::SC_ComboBoxEditField;
  }
}

//---------

CQFlagsComboMenuButton::
CQFlagsComboMenuButton(CQFlagsCombo *combo) :
 combo_(combo)
{
  setObjectName("button");

  //---

  QStyleOptionComboBox opt;

  combo_->initStyleOption(opt);

  auto r = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

  setFixedWidth(r.size().width());

  //---

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFocusPolicy(Qt::NoFocus);
}

void
CQFlagsComboMenuButton::
paintEvent(QPaintEvent *)
{
  // drawn by CQFlagsCombo
}

//---------

CQFlagsComboMenu::
CQFlagsComboMenu(CQFlagsCombo *combo) :
 QMenu(combo), combo_(combo)
{
  setObjectName("flagsComboMenu");

  QFontMetrics fm(font());

  height_ = 10*(fm.height() + 1);

  //---

  // add list widget
  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 2, 2);

  menuList_ = new CQFlagsComboList(this);

  layout->addWidget(menuList_);

  connect(menuList_, SIGNAL(itemPressed(QListWidgetItem *)),
          this, SLOT(itemSelected(QListWidgetItem *)));
  connect(menuList_, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
          this, SLOT(itemActivated(QListWidgetItem *)));

  //---

  auto *toolbar = CQUtil::makeWidget<QFrame>("toolbar");

  layout->addWidget(toolbar);

  auto *tlayout = CQUtil::makeLayout<QVBoxLayout>(toolbar, 2, 2);

  auto *allButton  = CQUtil::makeLabelWidget<QPushButton>("All" , "all");
  auto *noneButton = CQUtil::makeLabelWidget<QPushButton>("None", "none");

  connect(allButton, SIGNAL(clicked()), menuList_, SLOT(checkAll()));
  connect(noneButton, SIGNAL(clicked()), menuList_, SLOT(checkNone()));

  tlayout->addWidget(allButton);
  tlayout->addWidget(noneButton);
  tlayout->addStretch(1);

  //---

  connect(this, SIGNAL(aboutToShow()), this, SLOT(showSlot()));
  connect(this, SIGNAL(aboutToHide()), this, SLOT(hideSlot()));
}

void
CQFlagsComboMenu::
calcSize()
{
  int num = std::min(std::max(menuList_->count(), 4), 20);

  QFontMetrics fm(font());

  height_ = (fm.height() + 1)*num;

  height_ += 8;
}

void
CQFlagsComboMenu::
addItem(const QString &item, uint id)
{
  auto *listItem = new QListWidgetItem(item);

  listItem->setFlags(listItem->flags() | Qt::ItemIsUserCheckable);
  listItem->setCheckState(Qt::Unchecked);

  listItem->setData(Qt::UserRole, QVariant(int(id)));

  menuList_->addItem(listItem);
}

void
CQFlagsComboMenu::
popup(const QRect &rect)
{
  calcSize();

  setMinimumHeight(height_);

  //---

  // calculate menu position
  // place below if possible. If that is off screen, place above
  QRect screenRect = CQWidgetUtil::desktopAvailableGeometry();

  QRect rect1;

  if (rect.bottom() + height_ <= screenRect.bottom()) {
    QPoint bl = rect.bottomLeft();

    rect1 = QRect(bl.x(), bl.y(), rect.width(), height_);
  }
  else {
    QPoint tl = rect.topLeft();

    rect1 = QRect(tl.x(), tl.y() - height_, rect.width(), height_);
  }

  setGeometry(rect1);

  //---

  show();

  setFocus();
}

void
CQFlagsComboMenu::
popup(const QPoint &pos)
{
  QMenu::popup(pos);
}

void
CQFlagsComboMenu::
itemSelected(QListWidgetItem *item)
{
  item->setCheckState(item->checkState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);

  combo_->setValue(menuList_->calcValue());
}

void
CQFlagsComboMenu::
itemActivated(QListWidgetItem *)
{
}

// hide menu
void
CQFlagsComboMenu::
hideMenu()
{
  combo_->hideMenu();
}

void
CQFlagsComboMenu::
showSlot()
{
  origValue_ = combo_->value();

  menuList_->setValue(combo_->value());
}

void
CQFlagsComboMenu::
hideSlot()
{
  if (isCancelled())
    combo_->setValue(origValue_);

  setCancelled(false);
}

void
CQFlagsComboMenu::
updateValue()
{
}

//---

CQFlagsComboList::
CQFlagsComboList(CQFlagsComboMenu *menu) :
 QListWidget(nullptr), menu_(menu)
{
  setObjectName("flagsComboList");

  setSelectionMode(SingleSelection);
}

CQFlagsComboList::
~CQFlagsComboList()
{
}

void
CQFlagsComboList::
setValue(uint value)
{
  for (int i = 0; i < count(); ++i) {
    auto *item = this->item(i);

    auto ivalue = uint(item->data(Qt::UserRole).toInt());

    auto checked = (value & ivalue);

    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
  }
}

uint
CQFlagsComboList::
calcValue() const
{
  uint value = 0;

  for (int i = 0; i < count(); ++i) {
    auto *item = this->item(i);

    if (item->checkState() == Qt::Checked) {
      auto ivalue = uint(item->data(Qt::UserRole).toInt());

      value |= ivalue;
    }
  }

  return value;
}

void
CQFlagsComboList::
checkAll()
{
  for (int i = 0; i < count(); ++i) {
    auto *item = this->item(i);

    item->setCheckState(Qt::Checked);
  }

  menu_->combo()->setValue(calcValue());
}

void
CQFlagsComboList::
checkNone()
{
  for (int i = 0; i < count(); ++i) {
    auto *item = this->item(i);

    item->setCheckState(Qt::Unchecked);
  }

  menu_->combo()->setValue(0);
}

bool
CQFlagsComboList::
event(QEvent *e)
{
  if (e->type() == QEvent::KeyPress ||
      e->type() == QEvent::KeyRelease ||
      e->type() == QEvent::ShortcutOverride) {
    auto *ke = dynamic_cast<QKeyEvent *>(e);
    if (! ke) return false;

    // ignore if modifier pressed
    Qt::KeyboardModifiers mask = Qt::ShiftModifier | Qt::ControlModifier |
                                 Qt::AltModifier   | Qt::MetaModifier;

    int modifiers = int(ke->modifiers() & mask);

    int key = ke->key();

    // return pressed so accept current selection (if any)
    if      (key == Qt::Key_Return || key == Qt::Key_Enter) {
      menu_->hideMenu();

      return true;
    }
    // if up or down pressed and no item selected force selection of first item
    else if (key == Qt::Key_Up || key == Qt::Key_Down) {
      if (! modifiers) {
        auto *item = currentItem();

        if (! item || ! item->isSelected()) {
          setCurrentRow(0);

          return true;
        }
      }

      return QListWidget::event(e);
    }
    else if (key == Qt::Key_Select || (key == Qt::Key_Space && modifiers)) {
      return QListWidget::event(e);
    }
    else if (key == Qt::Key_Escape) {
      menu_->setCancelled(true);

      return QListWidget::event(e);
    }
    // other key pressed - send to line edit
    else {
      return QApplication::sendEvent(menu_->combo()->edit(), e);
    }
  }
  else {
    // all other events handled as normal
    return QListWidget::event(e);
  }
}
