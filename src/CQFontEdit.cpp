#include <CQFontEdit.h>
#include <CQWidgetMenu.h>

#include <QHBoxLayout>
#include <QFontDialog>
#include <QStylePainter>
#include <QStyleOptionComboBox>

CQFontEdit::
CQFontEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("fontEdit");

  setFrameShape(QFrame::StyledPanel);
  setFrameShadow(QFrame::Sunken);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  //-----

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  //---

  edit_ = new CQFontEditEdit(this);

  connect(edit_, SIGNAL(editingFinished()), this, SLOT(textChangedSlot()));

  layout->addWidget(edit_);

  //---

  button_ = new CQFontEditMenuButton(this);

  connect(button_, SIGNAL(clicked()), this, SLOT(buttonSlot()));

  layout->addWidget(button_);

  //---

  menu_ = new CQWidgetMenu(this);

  connect(menu_, SIGNAL(menuShown ()), this, SLOT(showMenuSlot()));
  connect(menu_, SIGNAL(menuHidden()), this, SLOT(hideMenuSlot()));

  //---

  fontDlg_ = new QFontDialog;

  menu_->setWidget(fontDlg_);

  connect(fontDlg_, SIGNAL(fontSelected(const QFont &)), this, SLOT(fontSlot(const QFont &)));
  connect(fontDlg_, SIGNAL(finished(int)), this, SLOT(closeMenuSlot()));

  //---

  setFocusProxy(edit_);

  //---

  fontName_ = font_.toString();

  edit_->setText(fontName_);

  //---

  updateEdit();
}

void
CQFontEdit::
setEditable(bool editable)
{
  editable_ = editable;

  edit_->setReadOnly(! editable);

  updateEdit();
}

void
CQFontEdit::
setNoFocus()
{
  setNoFocusHier(this);
}

void
CQFontEdit::
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
CQFontEdit::
updateEdit()
{
  edit_->setReadOnly(! editable_);
}

void
CQFontEdit::
setFont(const QFont &font)
{
  if (font_ == font)
    return;

  font_     = font;
  fontName_ = font_.toString();

  edit_->setText(fontName());
}

void
CQFontEdit::
setFontName(const QString &fontName)
{
  QFont font;

  if (fontName.length())
    font = QFont(fontName);

  if (font_ == font)
    return;

  font_     = font;
  fontName_ = fontName;

  edit_->setText(fontName_);
}

void
CQFontEdit::
textChangedSlot()
{
  if (edit_->text() == fontName_)
    return;

  setFontName(edit_->text());

  emitFontChanged();
}

void
CQFontEdit::
buttonSlot()
{
  fontDlg_->setCurrentFont(font());

  fontDlg_->show();

  //---

  // popup menu below or above the widget bounding box
  QPoint tl = edit_->mapToGlobal(edit_->rect().topLeft());

  QRect rect(tl.x(), tl.y(), edit_->parentWidget()->rect().width(), edit_->rect().height());

  menu_->popup(rect.bottomLeft());
}

void
CQFontEdit::
showMenuSlot()
{
  int w = std::max(menu_->sizeHint().width(), this->width());

  menu_->setFixedWidth(w);

  menu_->updateAreaSize();
}

void
CQFontEdit::
hideMenuSlot()
{
  emit menuHidden();
}

void
CQFontEdit::
fontSlot(const QFont &font)
{
  setFont(font);

  emitFontChanged();
}

void
CQFontEdit::
closeMenuSlot()
{
  menu_->hide();
}

void
CQFontEdit::
emitFontChanged()
{
  emit fontChanged(font_);
  emit fontChanged(fontName_);
}

void
CQFontEdit::
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
CQFontEdit::
resizeEvent(QResizeEvent *)
{
  button_->setFixedHeight(edit_->height() + 2);
}

void
CQFontEdit::
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

CQFontEditEdit::
CQFontEditEdit(CQFontEdit *edit) :
 QLineEdit(edit), edit_(edit)
{
  setObjectName("edit");

  setFrame(false);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void
CQFontEditEdit::
paintEvent(QPaintEvent *e)
{
  if (edit_->isEditable())
    QLineEdit::paintEvent(e);
  else {
    QPainter painter(this);

    QColor c = palette().color(QPalette::Window);

    painter.fillRect(rect(), QBrush(c));

    QString text = "ABC abc";

    painter.setFont(edit_->font());

    QFontMetrics fm(painter.font());

    painter.drawText(rect().left() + 2, rect().center().y() + (fm.ascent() - fm.descent())/2, text);
  }
}

//------

CQFontEditMenuButton::
CQFontEditMenuButton(CQFontEdit *edit) :
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
CQFontEditMenuButton::
paintEvent(QPaintEvent *)
{
  // drawn by CQFontEdit
}
