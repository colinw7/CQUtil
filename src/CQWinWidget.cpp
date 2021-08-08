#include <CQWinWidget.h>
#include <CQDrawUtil.h>

#include <QImage>
#include <QMenu>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QBrush>
#include <QBitmap>
#include <QStyleOption>
#include <QFileDialog>

#include <svg/maximize_svg.h>
#include <svg/minimize_svg.h>
#include <svg/close_svg.h>

#include <cursors/select.xbm>
#include <cursors/selectmask.xbm>

namespace {

QColor mergedColors(const QColor &colorA, const QColor &colorB, double factor = 50.0) {
  const double maxFactor = 100.0;

  auto tmp = colorA;

  tmp.setRed  ((tmp.red  ()*factor)/maxFactor + (colorB.red  ()*(maxFactor - factor))/maxFactor);
  tmp.setGreen((tmp.green()*factor)/maxFactor + (colorB.green()*(maxFactor - factor))/maxFactor);
  tmp.setBlue ((tmp.blue ()*factor)/maxFactor + (colorB.blue ()*(maxFactor - factor))/maxFactor);

  return tmp;
}

enum Direction {
  DiagDown,
  DiagUp,
  TopDown,
  FromLeft,
  BottomUp,
  FromRight
};

void drawGradient(QPainter *painter, const QRect &rect, const QColor &gradientStart,
                  const QColor &gradientStop, Direction direction=TopDown,
                  QBrush bgBrush=QBrush()) {
  int x = rect.center().x();
  int y = rect.center().y();

  QLinearGradient *gradient;

  switch(direction) {
    case DiagDown:
      gradient = new QLinearGradient(rect.left(), rect.top(), rect.right(), rect.bottom());
      break;
    case DiagUp:
      gradient = new QLinearGradient(rect.left(), rect.bottom(), rect.right(), rect.top());
      break;
    case FromLeft:
      gradient = new QLinearGradient(rect.left(), y, rect.right(), y);
      break;
    case FromRight:
      gradient = new QLinearGradient(rect.right(), y, rect.left(), y);
      break;
    case BottomUp:
      gradient = new QLinearGradient(x, rect.bottom(), x, rect.top());
      break;
    case TopDown:
    default:
      gradient = new QLinearGradient(x, rect.top(), x, rect.bottom());
      break;
  }

  if (bgBrush.gradient())
    gradient->setStops(bgBrush.gradient()->stops());
  else {
    gradient->setColorAt(0, gradientStart);
    gradient->setColorAt(1, gradientStop);
  }

  painter->fillRect(rect, *gradient);

  delete gradient;
}

}

//---

CQWinWidget::
CQWinWidget(QWidget *parent, const char *name) :
 QWidget(parent), decoration_(HeaderBorderDecoration, SideTop, 10, 1,
                              QColor(80,80,100), QColor(200,200,240))
{
  if (name)
    setObjectName(name);

  setMouseTracking(true);

  setAutoFillBackground(true);
}

void
CQWinWidget::
setChild(QWidget *child)
{
  if (child_)
    child_->setParent(nullptr);

  child_ = child;

  child_->setParent(this);

  child_->show();

  child->setCursor(Qt::ArrowCursor);

  if (ops_ & ResizeOp)
    updateSize();
  else
    fitChild();
}

void
CQWinWidget::
fitChild()
{
  setChildSize(child_->sizeHint());
}

void
CQWinWidget::
setChildSize(const QSize &size)
{
  int w = size.width ();
  int h = size.height();

  if (w <= 0) w = 100;
  if (h <= 0) h = 100;

  int b = getBorder();

  w += 2*b;
  h += 2*b;

  if (decoration_.type & HeaderDecoration) {
    int hh = getHeaderHeight();

    if (decoration_.headerSide == SideTop || decoration_.headerSide == SideBottom)
      h += hh;
    else
      w += hh;
  }

  resize(w, h);
}

int
CQWinWidget::
getX() const
{
  int x = getBorder();

  if (decoration_.type & HeaderDecoration) {
    if (decoration_.headerSide == SideLeft)
      x += getHeaderHeight();
  }

  return x;
}

int
CQWinWidget::
getY() const
{
  int y = getBorder();

  if (decoration_.type & HeaderDecoration) {
    if (decoration_.headerSide == SideTop)
      y += getHeaderHeight();
  }

  return y;
}

int
CQWinWidget::
getWidth() const
{
  int b = getBorder();

  int h = 0;

  if (decoration_.type & HeaderDecoration) {
    if (decoration_.headerSide == SideLeft || decoration_.headerSide == SideRight)
      h = getHeaderHeight();
  }

  return width() - 2*b - h;
}

int
CQWinWidget::
getHeight() const
{
  int b = getBorder();

  int h = 0;

  if (decoration_.type & HeaderDecoration) {
    if (decoration_.headerSide == SideTop || decoration_.headerSide == SideBottom)
      h = getHeaderHeight();
  }

  return height() - 2*b - h;
}

int
CQWinWidget::
getHeaderHeight() const
{
  QFontMetrics fm(font());

  int h = qMax(decoration_.headerHeight, fm.height() + 4);

  return h;
}

int
CQWinWidget::
getXPos() const
{
  return this->x() + getX();
}

int
CQWinWidget::
getYPos() const
{
  return this->y() + getX();
}

void
CQWinWidget::
setPos(int x, int y)
{
  emit geometryChanging();

  move(x - getX(), y - getY());

  emit geometryChanged();
}

void
CQWinWidget::
setSize(int w, int h)
{
  emit geometryChanging();

  int b = getBorder();

  int hw = 0, hh = 0;

  if (decoration_.type & HeaderDecoration) {
    if (decoration_.headerSide == SideLeft || decoration_.headerSide == SideRight)
      hw = getHeaderHeight();

    if (decoration_.headerSide == SideTop || decoration_.headerSide == SideBottom)
      hh = getHeaderHeight();
  }

  resize(w + hw + 2*b, h + hh + 2*b);

  emit geometryChanged();
}

bool
CQWinWidget::
checkMove(QPoint &p) const
{
  // widget is fully visible
  if (constraint_ == VISIBLE_CONSTRAINT) {
    QRect r(p, this->size());

    int pw = parentWidget()->width();
    int ph = parentWidget()->height();

    if (p.x()            < 0 ) p = QPoint(0, p.y());
    if (p.y()            < 0 ) p = QPoint(p.x(), 0);
    if (p.x() + width () > pw) p = QPoint(pw - width(), p.y());
    if (p.y() + height() > ph) p = QPoint(p.x(), ph - height());
  }

  return true;
}

bool
CQWinWidget::
checkGeometry(const QRect &rect) const
{
  if (constraint_ == VISIBLE_CONSTRAINT) {
    if (rect.x()                 < 0 ||
        rect.y()                 < 0 ||
        rect.x() + rect.width () > parentWidget()->width() ||
        rect.y() + rect.height() > parentWidget()->height())
     return false;
  }

  return true;
}

void
CQWinWidget::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  // draw border
  int b = getBorder();

  qDrawPlainRect(&painter, 0, 0, width(), height(), decoration_.borderColor, b);

  // draw header
  if (decoration_.type & HeaderDecoration) {
    auto pal = palette();

    int hh = getHeaderHeight();

    // draw background
    auto bgBrush = QBrush(pal.color(QPalette::Window));

    if      (decoration_.headerSide == SideTop)
      decoration_.headerRect = QRect(b, b, width() - 2*b, hh);
    else if (decoration_.headerSide == SideBottom)
      decoration_.headerRect = QRect(b, height() - b - hh, width() - 2*b, hh);
    else if (decoration_.headerSide == SideLeft)
      decoration_.headerRect = QRect(b, b, hh, height() - 2*b);
    else if (decoration_.headerSide == SideRight)
      decoration_.headerRect = QRect(width() - b - hh, b, hh, height() - 2*b);

#if 1
  //auto barColor = palette().button().color();
    auto barColor = this->barColor();

    QColor dark;

    dark.setHsv(barColor.hue(),
                qMin(255, (int)(barColor.saturation()*1.9)),
                qMin(255, (int)(barColor.value()*0.7)));

    auto gradientStartColor = barColor.lighter(108);
    auto gradientStopColor  = mergedColors(barColor.darker(108), dark.lighter(150), 70);

    if (decoration_.headerSide == SideTop || decoration_.headerSide == SideBottom)
      drawGradient(&painter, decoration_.headerRect, gradientStartColor, gradientStopColor,
                   DiagDown, palette().button());
    else
      drawGradient(&painter, decoration_.headerRect, gradientStartColor, gradientStopColor,
                   DiagDown, palette().button());
#else
    painter.fillRect(decoration_.headerRect, bgBrush);
#endif

    int b1 = hh - 4;

    closeButton_   .displayed = (ops_ & CloseOp );
    expandButton_  .displayed = (ops_ & ExpandOp);
    collapseButton_.displayed = (ops_ & CollapseOp);

#if 0
    // draw title bar handle
    int panelSize = 3;
    int margin    = (decoration_.headerHeight - 2*panelSize - 1)/2;

    if (decoration_.headerSide == SideTop || decoration_.headerSide == SideBottom) {
      int x1 = width() - b + 2;

      int panelWidth = qMax(1, x1 - 2*b - 2*margin);

      if (closeButton_   .displayed) panelWidth -= b1 + 2;
      if (expandButton_  .displayed) panelWidth -= b1 + 2;
      if (collapseButton_.displayed) panelWidth -= b1 + 2;

      int pd = 2*panelSize - 1;

      int n  = (hh - 2*b - 2*margin)/pd;
      int dy = (hh - (n - 1)*pd)/2;

      int y = dy;

      for (int i = 0; i < n; ++i) {
        qDrawShadePanel(&painter, b + margin, y, panelWidth, panelSize, pal);

        y += pd;
      }
    }
    else {
      int y1 = b + 2;

      int panelWidth = qMax(1, height() - y1 - 2*b - 2*margin);

      if (closeButton_   .displayed) panelWidth -= b1 + 2;
      if (expandButton_  .displayed) panelWidth -= b1 + 2;
      if (collapseButton_.displayed) panelWidth -= b1 + 2;

      qDrawShadePanel(&painter, b + margin                , b + margin, panelSize, panelWidth, pal);
      qDrawShadePanel(&painter, b + margin + 1 + panelSize, b + margin, panelSize, panelWidth, pal);
    }
#endif

    // draw title
    auto text = windowTitle();

    if (text.length()) {
      painter.setPen(pal.color(QPalette::WindowText));

      QFontMetrics fm(font());

      int tw = fm.width(text);

      painter.save();

      if (decoration_.headerSide == SideTop || decoration_.headerSide == SideBottom)
        painter.fillRect(QRect(b, b, tw + 4, hh), bgBrush);
      else {
        painter.fillRect(QRect(b, height() - tw - b - 4, hh, tw + 4), bgBrush);

        painter.translate(0, height());
        painter.rotate(-90);
      }

      int ym = b + hh/2;

      painter.drawText(b + 2, b + ym - fm.height()/2 + fm.ascent(), text);

      painter.restore();
    }

    // draw header buttons
    painter.setPen(decoration_.borderColor);

    if (decoration_.headerSide == SideTop || decoration_.headerSide == SideBottom) {
      int x1 = width() - b + 2;
      int y1 = b + 2;

      drawHeaderButtonH(&painter, closeButton_   , x1, y1, b1);
      drawHeaderButtonH(&painter, expandButton_  , x1, y1, b1);
      drawHeaderButtonH(&painter, collapseButton_, x1, y1, b1);
    }
    else {
      int x1 = b - 2;
      int y1 = b + 2;

      drawHeaderButtonV(&painter, closeButton_   , x1, y1, b1);
      drawHeaderButtonV(&painter, expandButton_  , x1, y1, b1);
      drawHeaderButtonV(&painter, collapseButton_, x1, y1, b1);
    }
  }
}

void
CQWinWidget::
drawHeaderButtonH(QPainter *painter, HeaderButton &button, int &x1, int y1, int b1)
{
  if (! button.displayed) return;

  x1 -= b1 + 2;

  button.rect = QRect(x1, y1, b1, b1);

  button.draw(painter, this);
}

void
CQWinWidget::
drawHeaderButtonV(QPainter *painter, HeaderButton &button, int x1, int &y1, int b1)
{
  if (! button.displayed) return;

  y1 += b1 + 2;

  button.rect = QRect(x1, y1, b1, b1);

  button.draw(painter, this);
}

void
CQWinWidget::
moveEvent(QMoveEvent *)
{
  resizeEvent(nullptr);
}

void
CQWinWidget::
updateSize()
{
  resizeEvent(nullptr);
}

void
CQWinWidget::
resizeEvent(QResizeEvent *)
{
  int b = getBorder();

  int x = b;
  int y = b;
  int w = width () - 2*b;
  int h = height() - 2*b;

  if (decoration_.type & HeaderDecoration) {
    int hh = getHeaderHeight();

    if      (decoration_.headerSide == SideTop)
      y += hh;
    else if (decoration_.headerSide == SideLeft)
      x += hh;

    if (decoration_.headerSide == SideTop || decoration_.headerSide == SideBottom)
      h -= hh;
    else
      w -= hh;
  }

  if (child_) {
    child_->move  (x, y);
    child_->resize(w, h);

    emit geometryChanging();
  }
}

void
CQWinWidget::
mousePressEvent(QMouseEvent *event)
{
  pressed_ = true;

  if (editMode() == EDIT_MODE_CLICK) {
    if (state_.moving || state_.resizing) {
      editMode_ = EDIT_MODE_DRAG;

      mouseReleaseEvent(event);

      releaseMouse();

      editMode_ = EDIT_MODE_CLICK;

      return;
    }
  }

  //-----

  state_.initPos  = pos();
  state_.initSize = size();
  state_.pressPos = event->globalPos();

  if      (event->button() == Qt::LeftButton) {
    state_.moving   = false;
    state_.resizing = false;

    auto p = event->pos();

    if (decoration_.type & HeaderDecoration) {
      closeButton_   .updateActive(p, pressed_);
      expandButton_  .updateActive(p, pressed_);
      collapseButton_.updateActive(p, pressed_);

      update();
    }

    if (ops_ & MoveOp) {
      if (decoration_.type & HeaderDecoration) {
        if (decoration_.headerRect.contains(p)) {
          if (! closeButton_.active && ! expandButton_.active && ! collapseButton_.active)
            state_.moving = true;
        }
      }
      else
        state_.moving = true;
    }

    if ((ops_ & ResizeOp) && ! state_.moving) {
      int b = getBorder() + 2;

      state_.resizeL = (event->x() <= b);
      state_.resizeT = (event->y() <= b);
      state_.resizeR = (event->x() >= width () - b - 1);
      state_.resizeB = (event->y() >= height() - b - 1);

      if (state_.resizeL || state_.resizeT || state_.resizeR || state_.resizeB)
        state_.resizing = true;
    }

    if (ops_ & RaiseOp)
      raise();

    if (editMode() == EDIT_MODE_CLICK)
      grabMouse();
  }
  else if (event->button() == Qt::MidButton) {
    state_.moving   = false;
    state_.resizing = false;

    if (ops_ & MoveOp)
      state_.moving = true;

    if (ops_ & RaiseOp)
      raise();

    if (editMode() == EDIT_MODE_CLICK)
      grabMouse();
  }
}

void
CQWinWidget::
mouseMoveEvent(QMouseEvent *event)
{
  if      (state_.moving) {
    auto d = event->globalPos() - state_.pressPos;

    auto p = state_.initPos + d;

    if (checkMove(p))
      move(p);
  }
  else if (state_.resizing) {
    int dx = 0; int dy = 0;
    int dw = 0; int dh = 0;

    if (state_.resizeL) {
      dx = event->globalPos().x() - state_.pressPos.x();
      dw = -dx;
    }

    if (state_.resizeR)
      dw = event->globalPos().x() - state_.pressPos.x();

    if (state_.resizeT) {
      dy = event->globalPos().y() - state_.pressPos.y();
      dh = -dy;
    }

    if (state_.resizeB)
      dh = event->globalPos().y() - state_.pressPos.y();

    move  (pos () + QPoint(dx, dy));
    resize(size() + QSize (dw, dh));

    state_.pressPos = event->globalPos();
  }

  //------

  setCursor(select_bits, selectmask_bits, 2, 2);

  auto p = event->pos();

  if (decoration_.type & HeaderDecoration) {
    bool insideClose    = closeButton_   .rect.contains(p);
    bool insideExpand   = expandButton_  .rect.contains(p);
    bool insideCollapse = collapseButton_.rect.contains(p);

    bool changed = (insideClose    != closeButton_   .active ||
                    insideExpand   != expandButton_  .active ||
                    insideCollapse != collapseButton_.active);

    closeButton_   .updateActive(p, pressed_);
    expandButton_  .updateActive(p, pressed_);
    collapseButton_.updateActive(p, pressed_);

    if (changed)
      update();
  }

  if (ops_ & MoveOp) {
    if (decoration_.type & HeaderDecoration) {
      if (decoration_.headerRect.contains(p)) {
        if (closeButton_.active || expandButton_.active || collapseButton_.active)
          setCursor(select_bits, selectmask_bits, 2, 2);
        else
          QWidget::setCursor(Qt::SizeAllCursor);
      }
    }
    else
      QWidget::setCursor(Qt::SizeAllCursor);
  }

  if (ops_ & ResizeOp) {
    int b = getBorder() + 2;

    bool resizeL = (event->x() <= b);
    bool resizeT = (event->y() <= b);
    bool resizeR = (event->x() >= width () - b - 1);
    bool resizeB = (event->y() >= height() - b - 1);

    if      (resizeL && resizeT) QWidget::setCursor(Qt::SizeFDiagCursor);
    else if (resizeL && resizeB) QWidget::setCursor(Qt::SizeBDiagCursor);
    else if (resizeR && resizeT) QWidget::setCursor(Qt::SizeBDiagCursor);
    else if (resizeR && resizeB) QWidget::setCursor(Qt::SizeFDiagCursor);
    else if (resizeL           ) QWidget::setCursor(Qt::SizeHorCursor);
    else if (resizeR           ) QWidget::setCursor(Qt::SizeHorCursor);
    else if (resizeT           ) QWidget::setCursor(Qt::SizeVerCursor);
    else if (resizeB           ) QWidget::setCursor(Qt::SizeVerCursor);
  }
}

void
CQWinWidget::
mouseReleaseEvent(QMouseEvent *event)
{
  pressed_ = false;

  if (editMode() == EDIT_MODE_DRAG) {
    if (state_.moving || state_.resizing) {
      mouseMoveEvent(event);

      if (! checkGeometry(geometry())) {
        move  (state_.initPos );
        resize(state_.initSize);
      }
      else {
        emit geometryChanged();
      }

      state_.moving   = false;
      state_.resizing = false;
    }
  }

  auto p = event->pos();

  if (decoration_.type & HeaderDecoration) {
    closeButton_   .updateActive(p, pressed_);
    expandButton_  .updateActive(p, pressed_);
    collapseButton_.updateActive(p, pressed_);

    update();

    if (closeButton_.active)
      closeSlot();

    if (expandButton_.active)
      expandSlot();

    if (collapseButton_.active)
      collapseSlot();
  }
}

//---

void
CQWinWidget::
contextMenuEvent(QContextMenuEvent *event)
{
  showContextMenu(event->globalPos());
}

void
CQWinWidget::
showContextMenu(const QPoint &gpos)
{
  auto *menu = new QMenu(this);

  auto *headerAction = menu->addAction("Show Header");

  headerAction->setCheckable(true);
  headerAction->setChecked  (isHeaderVisible());

  connect(headerAction, SIGNAL(triggered(bool)), this, SLOT(setHeaderSlot(bool)));

  menu->exec(gpos);

  delete menu;
}

void
CQWinWidget::
setHeaderSlot(bool b)
{
  setHeaderVisible(b);
}

//---

void
CQWinWidget::
enterEvent(QEvent *)
{
  active_ = true;

  update();
}

void
CQWinWidget::
leaveEvent(QEvent *)
{
  active_ = false;

  update();
}

void
CQWinWidget::
showEvent(QShowEvent *)
{
}

void
CQWinWidget::
hideEvent(QHideEvent *)
{
}

int
CQWinWidget::
getBorder() const
{
  int b = (decoration_.type & BorderDecoration ? decoration_.border : 0);

  return b;
}

void
CQWinWidget::
closeSlot()
{
  emit closing();

  hide();

  emit closed();
}

void
CQWinWidget::
expandSlot()
{
  emit expand();
}

void
CQWinWidget::
collapseSlot()
{
  emit collapse();
}

void
CQWinWidget::
setCursor(const uchar *bits, const uchar *mask, int xo, int yo)
{
  QCursor cursor(QBitmap::fromData(QSize(16,16), bits),
                 QBitmap::fromData(QSize(16,16), mask),
                 xo, yo);

  QWidget::setCursor(cursor);
}

//------

CQWinWidget::HeaderButton::
HeaderButton(const QString &iconName)
{
  icon = CQPixmapCacheInst->getIcon(iconName);
}

void
CQWinWidget::HeaderButton::
updateActive(const QPoint &p, bool pressed1)
{
  active = rect.contains(p);

  if (active)
    pressed = pressed1;
  else
    pressed = false;
}

void
CQWinWidget::HeaderButton::
draw(QPainter *painter, CQWinWidget *preview)
{
  auto *style = preview->style();

  QStyleOptionButton opt;

  opt.init(preview);

  opt.rect = rect;

  opt.state    |= QStyle::State_AutoRaise;
  opt.features |= QStyleOptionButton::Flat;

  if (! active || ! pressed)
    opt.state |= QStyle::State_Raised;

  if (active && pressed)
    opt.state |= QStyle::State_Sunken;

  style->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, painter, preview);

  int shiftHorizontal = (opt.state & QStyle::State_Sunken ?
                         style->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &opt, preview) : 0);
  int shiftVertical   = (opt.state & QStyle::State_Sunken ?
                         style->pixelMetric(QStyle::PM_ButtonShiftVertical  , &opt, preview) : 0);

  auto r = rect;

  r.adjust(2, 2, -2, -2);

  r.translate(shiftHorizontal, shiftVertical);

  auto pm = icon.pixmap(r.size(),
                        active  ? QIcon::Active : QIcon::Normal,
                        pressed ? QIcon::On     : QIcon::Off);

  style->drawItemPixmap(painter, r, Qt::AlignCenter, pm);
}

//---------------

CQWinWidgetBase::
CQWinWidgetBase(QWidget *parent)
{
  widget_ = new CQWinWidget(parent);
}

CQWinWidgetBase::
~CQWinWidgetBase()
{
}

void
CQWinWidgetBase::
setEditMode(CQWinWidget::EditMode editMode)
{
  widget_->setEditMode(editMode);
}

//---------------

CQWinTextEdit::
CQWinTextEdit(QWidget *parent, const char *name) :
 CQWinWidgetBase(parent)
{
  QTextEdit::setObjectName(name);

  widget_->setChild(this);

  connect(this, SIGNAL(textChanged()), this, SIGNAL(valueChanged()));

  connect(widget_, SIGNAL(geometryChanging()), this, SIGNAL(geometryChanging()));
  connect(widget_, SIGNAL(geometryChanged()), this, SIGNAL(geometryChanged()));
}

//---------------

CQWinImage::
CQWinImage(QWidget *parent, const char *name) :
 CQWinWidgetBase(parent)
{
  QLabel::setObjectName(name);

  widget_->setChild(this);

  widget_->setDecorationType(CQWinWidget::BorderDecoration);

  widget_->setResizable(false);

  connect(widget_, SIGNAL(geometryChanging()), this, SIGNAL(geometryChanging()));
  connect(widget_, SIGNAL(geometryChanged()), this, SIGNAL(geometryChanged()));

  //------

  menu_ = new QMenu(this);

  auto *loadAction = menu_->addAction("Load Image");

  connect(loadAction, SIGNAL(triggered()), this, SLOT(loadImage()));
}

void
CQWinImage::
setImageName(const QString &fileName)
{
  fileName_ = fileName;

  QImage image(fileName);

  setImage(image);
}

void
CQWinImage::
setImage(const QImage &image)
{
  image_ = image;

  auto pixmap = QPixmap::fromImage(image_);

  if (! pixmap.isNull()) {
    setPixmap(pixmap);

    widget_->setResizable(true);

    widget_->setChildSize(pixmap.size());

    widget_->setResizable(false);
  }
}

void
CQWinImage::
contextMenuEvent(QContextMenuEvent *event)
{
  menu_->exec(event->globalPos());
}

void
CQWinImage::
loadImage()
{
  auto fileName = QFileDialog::getOpenFileName(this,
    "Open Image", "", "Image Files (*.png *.gif *.jpg)");

  if (fileName.length())
    setImageName(fileName);

  emit valueChanged();
}

//---------------

CQWinTable::
CQWinTable(QWidget *parent, const char *name) :
 CQWinWidgetBase(parent)
{
  QTableWidget::setObjectName(name);

  widget_->setChild(this);

  connect(widget_, SIGNAL(geometryChanging()), this, SIGNAL(geometryChanging()));
  connect(widget_, SIGNAL(geometryChanged()), this, SIGNAL(geometryChanged()));
}

//---------------

CQWinTree::
CQWinTree(QWidget *parent, const char *name) :
 CQWinWidgetBase(parent)
{
  QTreeWidget::setObjectName(name);

  widget_->setChild(this);

  connect(widget_, SIGNAL(geometryChanging()), this, SIGNAL(geometryChanging()));
  connect(widget_, SIGNAL(geometryChanged()), this, SIGNAL(geometryChanged()));
}
