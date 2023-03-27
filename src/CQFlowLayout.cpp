#include <CQFlowLayout.h>

#include <QWidget>

CQFlowLayout::
CQFlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing) :
 QLayout(parent), hSpace_(hSpacing), vSpace_(vSpacing)
{
  setContentsMargins(margin, margin, margin, margin);
}

CQFlowLayout::
CQFlowLayout(int margin, int hSpacing, int vSpacing) :
 hSpace_(hSpacing), vSpace_(vSpacing)
{
  setContentsMargins(margin, margin, margin, margin);
}

CQFlowLayout::
~CQFlowLayout()
{
  QLayoutItem *item;

  while ((item = takeAt(0)))
    delete item;
}

void
CQFlowLayout::
addItem(QLayoutItem *item)
{
  itemList_.append(item);
}

int
CQFlowLayout::
horizontalSpacing() const
{
  if (hSpace_ >= 0)
    return hSpace_;

  return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int
CQFlowLayout::
verticalSpacing() const
{
  if (vSpace_ >= 0)
    return vSpace_;

  return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

int
CQFlowLayout::
count() const
{
  return itemList_.size();
}

QLayoutItem *
CQFlowLayout::
itemAt(int index) const
{
  return itemList_.value(index);
}

QLayoutItem *
CQFlowLayout::
takeAt(int index)
{
  if (index >= 0 && index < itemList_.size())
    return itemList_.takeAt(index);

  return nullptr;
}

Qt::Orientations
CQFlowLayout::
expandingDirections() const
{
  return { };
}

bool
CQFlowLayout::
hasHeightForWidth() const
{
  return true;
}

int
CQFlowLayout::
heightForWidth(int width) const
{
  int height = doLayout(QRect(0, 0, width, 0), true);
  return height;
}

void
CQFlowLayout::
setGeometry(const QRect &rect)
{
  QLayout::setGeometry(rect);
  doLayout(rect, false);
}

QSize
CQFlowLayout::
sizeHint() const
{
  return minimumSize();
}

QSize
CQFlowLayout::
minimumSize() const
{
  QSize size;
  for (const QLayoutItem *item : std::as_const(itemList_))
    size = size.expandedTo(item->minimumSize());

  const QMargins margins = contentsMargins();
  size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
  return size;
}

int
CQFlowLayout::
doLayout(const QRect &rect, bool testOnly) const
{
  int left, top, right, bottom;
  getContentsMargins(&left, &top, &right, &bottom);
  QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
  int x = effectiveRect.x();
  int y = effectiveRect.y();
  int lineHeight = 0;

  for (QLayoutItem *item : std::as_const(itemList_)) {
    const QWidget *wid = item->widget();

    int spaceX = horizontalSpacing();
    if (spaceX == -1)
      spaceX = wid->style()->layoutSpacing(
        QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);

    int spaceY = verticalSpacing();
    if (spaceY == -1)
      spaceY = wid->style()->layoutSpacing(
        QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);

    int nextX = x + item->sizeHint().width() + spaceX;
    if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
      x = effectiveRect.x();
      y = y + lineHeight + spaceY;
      nextX = x + item->sizeHint().width() + spaceX;
      lineHeight = 0;
    }

    if (! testOnly)
      item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

    x = nextX;
    lineHeight = qMax(lineHeight, item->sizeHint().height());
  }
  return y + lineHeight - rect.y() + bottom;
}

int
CQFlowLayout::
smartSpacing(QStyle::PixelMetric pm) const
{
  QObject *parent = this->parent();
  if (! parent) return -1;

  if (parent->isWidgetType()) {
    QWidget *pw = static_cast<QWidget *>(parent);
    return pw->style()->pixelMetric(pm, nullptr, pw);
  }

  return static_cast<QLayout *>(parent)->spacing();
}
