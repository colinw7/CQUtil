#include <CQTableWidget.h>
#include <CQHeaderView.h>

#include <CQColorChooser.h>
#include <CQFontChooser.h>
#include <CQAlignEdit.h>
#include <CQUtil.h>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QItemDelegate>
#include <QHeaderView>
#include <QLineEdit>
#include <QCheckBox>
#include <QStringList>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QAbstractItemView>
#include <QScrollBar>

//---

class CQTableWidgetDelegate : public QItemDelegate {
 public:
  CQTableWidgetDelegate(CQTableWidget *table) :
   QItemDelegate(table), table_(table) {
  }

  // Override to create editor
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;

  // Override to get content from editor
  void setEditorData(QWidget *editor, const QModelIndex &index) const override;

  // Override to set editor from content
  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const override;

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  void drawBoolCheck(QPainter *painter, const QStyleOptionViewItem &option,
                     const QRect &rect, bool b) {
    Qt::CheckState checkState = (b ? Qt::Checked : Qt::Unchecked);

    QItemDelegate::drawCheck(painter, option, rect, checkState);
  }

  void drawDisplayText(QPainter *painter, const QStyleOptionViewItem &option,
                       const QRect &rect, const QString &str) {
    QItemDelegate::drawDisplay(painter, option, rect, str);
  }

 private:
  CQTableWidget *table_;
};

//---

uint CQTableWidgetItem::type_ = 0;

//---

CQTableWidget::
CQTableWidget(QWidget* parent) :
 QTableWidget(parent)
{
  setObjectName("table");

  //--

  verticalHeader()->hide();

  header_ = new CQHeaderView(this);

  header_->setInitFit(true);

  setHorizontalHeader(header_);

  //---

//setUniformRowHeights(true);
  setAlternatingRowColors(true);

  setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
//setEditTriggers(QAbstractItemView::AllEditTriggers);

  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);

  //---

  // Set Item Delegate
  delegate_ = new CQTableWidgetDelegate(this);

  setItemDelegate(delegate_);

//connect(delegate_, SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)),
//        this, SLOT(closeEditorSlot(QWidget*, QAbstractItemDelegate::EndEditHint)));

  //--

  connect(this, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(itemClickedSlot(const QModelIndex &)));

  //---

  // Set the value column to stretch to fit the table and other table attributes.
  auto *header = horizontalHeader();

  header->setStretchLastSection(true);

  //---

  //setSortingEnabled(false);

  //setRowMovingEnabled(false);
  //setColumnMovingEnabled(false);

  horizontalScrollBar()->setObjectName("hbar");
  verticalScrollBar  ()->setObjectName("vbar");

  //---

  registerType(CQTableWidgetBoolItem::TYPE  , new CQTableWidgetBoolItem  (this, false));
  registerType(CQTableWidgetStringItem::TYPE, new CQTableWidgetStringItem(this, QString()));
  registerType(CQTableWidgetColorItem::TYPE , new CQTableWidgetColorItem (this, QColor()));
  registerType(CQTableWidgetFontItem::TYPE  , new CQTableWidgetFontItem  (this, QFont()));
}

CQTableWidget::
~CQTableWidget()
{
  for (auto &pt : types_)
    delete pt.second;
}

void
CQTableWidget::
registerType(uint type, CQTableWidgetItem *item)
{
  assert(type > QTableWidgetItem::UserType);

  item->setType(type);

  types_[type] = item;
}

void
CQTableWidget::
setColumnLabel(int col, const QString &label)
{
  auto *item = horizontalHeaderItem(col);

  if (! item) {
    item = new QTableWidgetItem;

    setHorizontalHeaderItem(col, item);
  }

  item->setText(label);
}

void
CQTableWidget::
setRowLabel(int row, const QString &label)
{
  verticalHeader()->show();

  auto *item = verticalHeaderItem(row);

  if (! item) {
    item = new QTableWidgetItem;

    setVerticalHeaderItem(row, item);
  }

  item->setText(label);
}

void
CQTableWidget::
setColumnLabels(const QStringList &labels)
{
  setHorizontalHeaderLabels(labels);
}

void
CQTableWidget::
setRowLabels(const QStringList &labels)
{
  verticalHeader()->show();

  setVerticalHeaderLabels(labels);
}

CQTableWidgetItem *
CQTableWidget::
createItem(int row, int column, uint type)
{
  assert(types_.find(type) != types_.end());

  auto *item = types_[type]->clone();

  QTableWidget::setItem(row, column, item);

  return item;
}

void
CQTableWidget::
setItem(int row, int column, CQTableWidgetItem *item)
{
  QTableWidget::setItem(row, column, item);

  auto index = model()->index(row, column);

  model()->setData(index, item->getString());
}

void
CQTableWidget::
setItem(int row, int column, QTableWidgetItem *item)
{
  QTableWidget::setItem(row, column, item);

  auto index = model()->index(row, column);

  model()->setData(index, item->text());
}

void
CQTableWidget::
setItem(int row, int column, const QString &str)
{
  auto *item = new QTableWidgetItem(str);

  item->setFlags(item->flags() & ~Qt::ItemIsEditable);

  QTableWidget::setItem(row, column, item);
}

void
CQTableWidget::
setItem(int row, int column, const QVariant &var)
{
  auto *item = new QTableWidgetItem(var.toString());

  item->setFlags(item->flags() & ~Qt::ItemIsEditable);

  QTableWidget::setItem(row, column, item);
}

void
CQTableWidget::
setItem(int row, int column, QWidget *w)
{
  QTableWidget::setCellWidget(row, column, w);
}

void
CQTableWidget::
showEvent(QShowEvent *)
{
  //resizeColumnsToContents();

  //fixTableColumnWidths(this, 16, true);
}

void
CQTableWidget::
itemClickedSlot(const QModelIndex &index)
{
  int row    = index.row();
  int column = index.column();

  auto *item = this->item(row, column);

  auto *boolItem = dynamic_cast<CQTableWidgetBoolItem *>(item);

  if (boolItem) {
    boolItem->click();

    Q_EMIT boolClicked(row, column, boolItem->value());
  }
}

void
CQTableWidget::
fitAll()
{
  header_->fitAllSlot();
}

void
CQTableWidget::
fixTableColumnWidths()
{
  resizeColumnsToContents();

  //fixTableColumnWidths(this, 16, false);
}

void
CQTableWidget::
fixTableColumnWidths(QTableWidget *table, int max_len, bool init)
{
  const auto &font = table->font();

  QFontMetrics fm(font);

  int num_cols = table->columnCount();

  for (int c = 0; c < num_cols - 1; ++c) {
    if (table->isColumnHidden(c)) continue;

    int width = (init ? 0 : table->columnWidth(c));

    if (table->horizontalHeader()->isVisible()) {
      auto *item = table->horizontalHeaderItem(c);

      if (item) {
        auto var = item->data(Qt::DisplayRole);

        auto str = var.toString();

        if (str.length() <= max_len)
          width = qMax(width, fm.horizontalAdvance(str) + 10);
      }
    }

    int num_rows = table->rowCount();

    for (int r = 0; r < num_rows; ++r) {
      auto *item = table->item(r, c);

      if (item) {
        auto var = item->data(Qt::DisplayRole);

        auto str = var.toString();

        if (str.length() <= max_len)
          width = qMax(width, fm.horizontalAdvance(str) + 28);

        auto *w = table->cellWidget(r, c);

        if (w)
          width = qMax(width, w->width());
      }
    }

    table->setColumnWidth(c, qMax(24, width));
  }
}

void
CQTableWidget::
emitValueChanged(int row, int col)
{
  Q_EMIT valueChanged(row, col);
}

//---

QWidget *
CQTableWidgetDelegate::
createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  auto *item = table_->item(index.row(), index.column());

  auto *item1 = dynamic_cast<CQTableWidgetItem *>(item);

  if (item1) {
    auto *w = item1->createEditor(parent);

    if (w)
      w->installEventFilter(const_cast<CQTableWidgetDelegate *>(this));

    return w;
  }
  else
    return QItemDelegate::createEditor(parent, option, index);
}

void
CQTableWidgetDelegate::
setEditorData(QWidget *, const QModelIndex &index) const
{
  auto *item = table_->item(index.row(), index.column());

  auto *item1 = dynamic_cast<CQTableWidgetItem *>(item);
  if (! item1) return;

  item1->setEditorData();
}

void
CQTableWidgetDelegate::
setModelData(QWidget *, QAbstractItemModel *model, const QModelIndex &index) const
{
  auto *item = table_->item(index.row(), index.column());

  auto *item1 = dynamic_cast<CQTableWidgetItem *>(item);
  if (! item1) return;

  QString str;

  item1->getEditorData(str);

  model->setData(index, str);

  Q_EMIT item1->valueChanged();

  table_->emitValueChanged(index.row(), index.column());
}

QSize
CQTableWidgetDelegate::
sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  auto *item = table_->item(index.row(), index.column());

  auto *item1 = dynamic_cast<CQTableWidgetItem *>(item);

  if (item1) {
    QSize size;

    if (item1->sizeHint(option, size))
      return size;
  }

  return QItemDelegate::sizeHint(option, index);
}

void
CQTableWidgetDelegate::
updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                     const QModelIndex &/* index */) const
{
  editor->setGeometry(option.rect);
}

void
CQTableWidgetDelegate::
paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  auto *item = table_->item(index.row(), index.column());

  auto *item1 = dynamic_cast<CQTableWidgetItem *>(item);

  if (! item1 || ! item1->paint(painter, option))
    QItemDelegate::paint(painter, option, index);
}

//---

CQTableWidgetItem::
CQTableWidgetItem(CQTableWidget *table) :
 table_(table)
{
}

void
CQTableWidgetItem::
paintBackground(QPainter *painter, const QStyleOptionViewItem &option) const
{
  if (option.state & QStyle::State_Selected) {
    auto cg = (option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled);

    painter->fillRect(option.rect, option.palette.brush(cg, QPalette::Highlight));
  }
}

void
CQTableWidgetItem::
setTextPen(QPainter *painter, const QStyleOptionViewItem &option) const
{
  auto cg = (option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled);

  if (option.state & QStyle::State_Selected)
    painter->setPen(option.palette.color(cg, QPalette::HighlightedText));
  else
    painter->setPen(option.palette.color(cg, QPalette::Text));
}

//---

#if 0
CQTableWidgetStdItem::
CQTableWidgetStdItem(const QString &label) :
 CQTableWidgetItem(CQOBJ_EDIT_STD_ITEM_ID)
{
  setText(label);

  setFlags(flags() | Qt::ItemIsEditable);
}

CQTableWidgetStdItem::
CQTableWidgetStdItem(const CQTableWidgetStdItem &item) :
 QTreeWidgetItem(item)
{
  for (int i = 0; i < item.childCount(); ++i)
    setText(i, item.text(i));

  setFlags(flags() | Qt::ItemIsEditable);
}

bool
CQTableWidgetStdItem::
paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const
{
  return false;
}
#endif

//---

QWidget *
CQTableWidgetBoolItem::
createEditor(QWidget *) const
{
  return nullptr;
}

// set editor for table widget item
void
CQTableWidgetBoolItem::
setEditorData()
{
}

// set table widget item from editor
void
CQTableWidgetBoolItem::
getEditorData(QString &str)
{
  str = getString();
}

bool
CQTableWidgetBoolItem::
paint(QPainter *painter, const QStyleOptionViewItem &option) const
{
  paintBackground(painter, option);

  auto rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  table_->delegate()->drawBoolCheck(painter, option, rect, b_);

  int x = rect.right() + 4;

  QRect rect1;

  rect1.setCoords(x, option.rect.top(), option.rect.right(), option.rect.bottom());

  table_->delegate()->drawDisplayText(painter, option, rect1, getString());

  return true;
}

//---

QWidget *
CQTableWidgetStringItem::
createEditor(QWidget *parent) const
{
  edit_ = new QLineEdit(parent);

  return edit_;
}

// set editor for table widget item
void
CQTableWidgetStringItem::
setEditorData()
{
  if (edit_.isNull())
    return;

  edit_->setText(s_);
}

// set table widget item from editor
void
CQTableWidgetStringItem::
getEditorData(QString &str)
{
  if (edit_.isNull()) return;

  s_ = edit_->text();

  str = getString();
}

bool
CQTableWidgetStringItem::
paint(QPainter *, const QStyleOptionViewItem &) const
{
  return false;
}

//---

QWidget *
CQTableWidgetColorItem::
createEditor(QWidget *parent) const
{
  auto *frame  = CQUtil::makeWidget<QFrame>(parent, "frame");
  auto *layout = CQUtil::makeLayout<QHBoxLayout>(frame, 0, 0);

  uint styles = CQColorChooser::Text | CQColorChooser::ColorButton | CQColorChooser::AlphaButton;

  edit_ = new CQColorChooser(styles);

  frame->setFocusProxy(edit_);

  layout->addWidget(edit_);

  if (isOptional()) {
    check_ = CQUtil::makeWidget<QCheckBox>("check");

    connect(check_, SIGNAL(stateChanged(int)), this, SLOT(checkSlot(int)));

    layout->addWidget(check_);
  }

  return frame;
}

void
CQTableWidgetColorItem::
checkSlot(int state)
{
  if (! state)
    edit_->getTextWidget()->setText("");
}

// set editor for table widget item
void
CQTableWidgetColorItem::
setEditorData()
{
  if (edit_.isNull()) return;

  if (isOptional())
    check_->setChecked(true);

  if (hasValue())
    edit_->setColor(value());
}

// set table widget item from editor
void
CQTableWidgetColorItem::
getEditorData(QString &str)
{
  if (edit_.isNull()) return;

  if (check_->isChecked() && edit_->getTextWidget()->text() != "")
    setValue(edit_->color());
  else
    set_ = false;

  str = getString();
}

bool
CQTableWidgetColorItem::
paint(QPainter *painter, const QStyleOptionViewItem &option) const
{
  if (! hasValue())
    return true;

  if (edit_.isNull()) {
    paintBackground(painter, option);

    auto rect = option.rect.adjusted(2, 2, -4, -2);

    painter->fillRect(rect, QBrush(value()));

    setTextPen(painter, option);

    painter->drawText(QRect(QPoint(rect.right() + 2, rect.top()),
                            QPoint(rect.right() + 2, rect.bottom())), getString());
  }

  return true;
}

//---

QWidget *
CQTableWidgetFontItem::
createEditor(QWidget *parent) const
{
  auto *frame  = CQUtil::makeWidget<QFrame>(parent, "frame");
  auto *layout = CQUtil::makeLayout<QHBoxLayout>(frame, 0, 0);

  edit_ = new CQFontChooser;

  frame->setFocusProxy(edit_);

  layout->addWidget(edit_);

  if (isOptional()) {
    check_ = CQUtil::makeWidget<QCheckBox>("check");

    layout->addWidget(check_);
  }

  return frame;
}

// set editor for table widget item
void
CQTableWidgetFontItem::
setEditorData()
{
  if (edit_.isNull()) return;

  if (isOptional())
    check_->setChecked(hasValue());

  if (hasValue())
    edit_->setFont(value());
}

// set table widget item from editor
void
CQTableWidgetFontItem::
getEditorData(QString &str)
{
  if (edit_.isNull()) return;

  if (check_->isChecked())
    setValue(edit_->font());
  else
    set_ = false;

  str = getString();
}

bool
CQTableWidgetFontItem::
paint(QPainter *, const QStyleOptionViewItem &) const
{
  if (! hasValue())
    return true;

  return false;
}

//---

QWidget *
CQTableWidgetAlignItem::
createEditor(QWidget *parent) const
{
  auto *frame  = CQUtil::makeWidget<QFrame>(parent, "frame");
  auto *layout = CQUtil::makeLayout<QHBoxLayout>(frame, 0, 0);

  edit_ = new CQAlignEdit;

  frame->setFocusProxy(edit_);

  layout->addWidget(edit_);

  if (isOptional()) {
    check_ = CQUtil::makeWidget<QCheckBox>("check");

    layout->addWidget(check_);
  }

  return frame;
}

QString
CQTableWidgetAlignItem::
getString() const
{
  return CQAlignEdit::toString(align_);
}

// set editor for table widget item
void
CQTableWidgetAlignItem::
setEditorData()
{
  if (edit_.isNull()) return;

  if (isOptional())
    check_->setChecked(hasValue());

  if (hasValue())
    edit_->setAlign(align_);
}

// set table widget item from editor
void
CQTableWidgetAlignItem::
getEditorData(QString &str)
{
  if (edit_.isNull()) return;

  if (check_->isChecked())
    setValue(edit_->align());
  else
    set_ = false;

  str = getString();
}

bool
CQTableWidgetAlignItem::
paint(QPainter *, const QStyleOptionViewItem &) const
{
  if (! hasValue())
    return true;

  return false;
}
