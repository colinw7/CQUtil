#include <CQTableWidget.h>
#include <CQHeaderView.h>

#include <CQColorChooser.h>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QItemDelegate>
#include <QHeaderView>
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
                        const QModelIndex &index) const;

  // Override to get content from editor
  void setEditorData(QWidget *editor, const QModelIndex &index) const;

  // Override to set editor from content
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const;

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const;

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

  registerType(CQTableWidgetColorItem::TYPE, new CQTableWidgetBoolItem (this, false));
  registerType(CQTableWidgetColorItem::TYPE, new CQTableWidgetColorItem(this, QColor()));
}

CQTableWidget::
~CQTableWidget()
{
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

    emit boolClicked(row, column, boolItem->value());
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
          width = qMax(width, fm.width(str) + 10);
      }
    }

    int num_rows = table->rowCount();

    for (int r = 0; r < num_rows; ++r) {
      auto *item = table->item(r, c);

      if (item) {
        auto var = item->data(Qt::DisplayRole);

        auto str = var.toString();

        if (str.length() <= max_len)
          width = qMax(width, fm.width(str) + 28);

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
  emit valueChanged(row, col);
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
      w->installEventFilter(const_cast<CQTableWidgetDelegate*>(this));

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

  if (item1)
    item1->setEditorData();
}

void
CQTableWidgetDelegate::
setModelData(QWidget *, QAbstractItemModel *model, const QModelIndex &index) const
{
  auto *item = table_->item(index.row(), index.column());

  auto *item1 = dynamic_cast<CQTableWidgetItem *>(item);

  if (item1) {
    QString str;

    item1->getEditorData(str);

    model->setData(index, str);

    table_->emitValueChanged(index.row(), index.column());
  }
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
    QPalette::ColorGroup cg =
     (option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled);

    painter->fillRect(option.rect, option.palette.brush(cg, QPalette::Highlight));
  }
}

void
CQTableWidgetItem::
setTextPen(QPainter *painter, const QStyleOptionViewItem &option) const
{
  QPalette::ColorGroup cg =
   (option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled);

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

void
CQTableWidgetBoolItem::
setEditorData()
{
}

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
CQTableWidgetColorItem::
createEditor(QWidget *parent) const
{
  uint styles = CQColorChooser::Text | CQColorChooser::ColorButton | CQColorChooser::AlphaButton;

  edit_ = new CQColorChooser(styles, parent);

  return edit_;
}

void
CQTableWidgetColorItem::
setEditorData()
{
  if (edit_.isNull())
    return;

  edit_->setColor(c_);
}

void
CQTableWidgetColorItem::
getEditorData(QString &str)
{
  if (edit_.isNull()) return;

  c_ = edit_->color();

  str = getString();
}

bool
CQTableWidgetColorItem::
paint(QPainter *painter, const QStyleOptionViewItem &option) const
{
  if (edit_.isNull()) {
    paintBackground(painter, option);

    QFontMetrics fm(option.font);

    auto rect = option.rect.adjusted(2, 2, -4, -2);

    painter->fillRect(rect, QBrush(c_));

    setTextPen(painter, option);

    painter->drawText(QRect(QPoint(rect.right() + 2, rect.top()),
                            QPoint(rect.right() + 2, rect.bottom())), getString());
  }

  return true;
}
