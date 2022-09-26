#include <CQColorEdit.h>
#include <CQWidgetMenu.h>

#include <QHBoxLayout>
#include <QColorDialog>
#include <QStylePainter>
#include <QStyleOptionComboBox>

CQColorEdit::
CQColorEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("colorEdit");

  setFrameShape(QFrame::StyledPanel);
  setFrameShadow(QFrame::Sunken);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  //-----

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  //---

  edit_ = new CQColorEditEdit(this);

  connect(edit_, SIGNAL(editingFinished()), this, SLOT(textChangedSlot()));

  layout->addWidget(edit_);

  //---

  button_ = new CQColorEditMenuButton(this);

  connect(button_, SIGNAL(clicked()), this, SLOT(buttonSlot()));

  layout->addWidget(button_);

  //---

  menu_ = new CQWidgetMenu(this);

  connect(menu_, SIGNAL(menuShown ()), this, SLOT(showMenuSlot()));
  connect(menu_, SIGNAL(menuHidden()), this, SLOT(hideMenuSlot()));

  //---

  colorDlg_ = new QColorDialog;

  menu_->setWidget(colorDlg_);

  connect(colorDlg_, SIGNAL(colorSelected(const QColor &)), this, SLOT(colorSlot(const QColor &)));
  connect(colorDlg_, SIGNAL(finished(int)), this, SLOT(closeMenuSlot()));

  //---

  setFocusProxy(edit_);

  //---

  updateEdit();

  setColor(color_);
}

void
CQColorEdit::
setEditable(bool editable)
{
  editable_ = editable;

  edit_->setReadOnly(! editable);

  updateEdit();
}

void
CQColorEdit::
setNoFocus()
{
  setNoFocusHier(this);
}

void
CQColorEdit::
setNoFocusHier(QWidget *w)
{
  w->setFocusPolicy(Qt::NoFocus);

  const QObjectList &children = w->children();

  for (QObjectList::const_iterator po = children.begin(); po != children.end(); ++po) {
    QWidget *w1 = qobject_cast<QWidget *>(*po);

    if (w1)
      setNoFocusHier(w1);
  }
}

void
CQColorEdit::
updateEdit()
{
  edit_->setReadOnly(! editable_);
}

void
CQColorEdit::
setColor(const QColor &color)
{
  if (! color.isValid()) {
    if (! color_.isValid())
      return;

    color_     = color;
    colorName_ = "";
  }
  else {
    if (color_ == color)
      return;

    color_     = color;
    colorName_ = color_.name();
  }

  edit_->setText(colorName());
}

void
CQColorEdit::
setColorName(const QString &colorName)
{
  QColor color;

  if (colorName.length()) {
    color = QColor(colorName);

    if (! color.isValid())
      return;
  }

  if (color_ == color)
    return;

  color_     = color;
  colorName_ = colorName;

  edit_->setText(colorName_);
}

void
CQColorEdit::
textChangedSlot()
{
  setColorName(edit_->text());

  emitColorChanged();
}

void
CQColorEdit::
buttonSlot()
{
  colorDlg_->setCurrentColor(color());

  colorDlg_->show();

  //---

  // popup menu below or above the widget bounding box
  QPoint tl = edit_->mapToGlobal(edit_->rect().topLeft());

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect.bottomLeft());
}

void
CQColorEdit::
showMenuSlot()
{
  int w = std::max(menu_->sizeHint().width(), this->width());

  menu_->setFixedWidth(w);

  menu_->updateAreaSize();
}

void
CQColorEdit::
hideMenuSlot()
{
  emit menuHidden();
}

void
CQColorEdit::
colorSlot(const QColor &color)
{
  setColor(color);

  emitColorChanged();
}

void
CQColorEdit::
closeMenuSlot()
{
  menu_->hide();
}

void
CQColorEdit::
emitColorChanged()
{
  emit colorChanged(color_);
  emit colorChanged(colorName_);
}

void
CQColorEdit::
paintEvent(QPaintEvent *)
{
  QStylePainter painter(this);

  painter.setPen(palette().color(QPalette::Text));

  // draw the combobox frame, focusrect and selected etc.
  QStyleOptionComboBox opt;

  initStyle(opt);

  painter.drawComplexControl(QStyle::CC_ComboBox, opt);

  // draw text
  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

void
CQColorEdit::
resizeEvent(QResizeEvent *)
{
  button_->setFixedHeight(edit_->height() + 2);
}

void
CQColorEdit::
initStyle(QStyleOptionComboBox &opt)
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

//------

CQColorEditEdit::
CQColorEditEdit(CQColorEdit *edit) :
 QLineEdit(edit), edit_(edit)
{
  setObjectName("edit");

  setFrame(false);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void
CQColorEditEdit::
paintEvent(QPaintEvent *e)
{
  auto bwColor = [](const QColor &c) {
    int g = qGray(c.red(), c.green(), c.blue());

    return (g > 128 ? QColor(0, 0, 0) : QColor(255, 255, 255));
  };

  if (edit_->isEditable())
    QLineEdit::paintEvent(e);
  else {
    QPainter painter(this);

    painter.fillRect(rect(), QBrush(edit_->color()));

    QString text = edit_->color().name();

    QFontMetrics fm(font());

    QColor tc = bwColor(edit_->color());

    painter.setPen(tc);

    painter.drawText(rect().left() + 2, rect().center().y() + (fm.ascent() - fm.descent())/2, text);
  }
}

//------

CQColorEditMenuButton::
CQColorEditMenuButton(CQColorEdit *edit) :
 QPushButton(edit), edit_(edit)
{
  setObjectName("button");

  //---

  QStyleOptionComboBox opt;

  edit_->initStyle(opt);

  QRect r = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

  setFixedWidth(r.size().width());

  //---

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFocusPolicy(Qt::NoFocus);
}

void
CQColorEditMenuButton::
paintEvent(QPaintEvent *)
{
  // drawn by CQColorEdit
}
