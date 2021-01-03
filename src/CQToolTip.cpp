#include <CQToolTip.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QCursor>
#include <QKeyEvent>
#include <QHelpEvent>
#include <QToolTip>
#include <QTimer>
#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionFrame>
#include <QBitmap>

#include <cassert>
#include <iostream>

CQToolTip *CQToolTip::s_instance;

CQToolTip *
CQToolTip::
instance()
{
  if (! s_instance)
    s_instance = new CQToolTip;

  return s_instance;
}

void
CQToolTip::
release()
{
  delete s_instance;

  s_instance = nullptr;
}

// the tooltip widget
CQToolTip::
CQToolTip() :
 QWidget(nullptr, Qt::Window | Qt::FramelessWindowHint),
 margin_ (style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth)),
 opacity_(style()->styleHint  (QStyle::SH_ToolTipLabel_Opacity)/255.0)
{
  setObjectName("tooltip");

  setAttribute(Qt::WA_TranslucentBackground);

  //setPalette(QToolTip::palette());
  setWindowOpacity(opacity_);

  setContentsMargins(margin_, margin_, margin_, margin_);

  qApp->installEventFilter(this);
}

CQToolTip::
~CQToolTip()
{
  qApp->removeEventFilter(this);
}

bool
CQToolTip::
show(const QPoint &pos, CQToolTipIFace *tooltip, QWidget *parent)
{
  if (isVisible()) return false;

  if (! tooltip->canTip(pos))
    return false;

  auto *desktop = QApplication::desktop();

  int snum;

  if (desktop->isVirtualDesktop())
    snum = desktop->screenNumber(pos);
  else
    snum = desktop->screenNumber(this);

  setParent(desktop->screen(snum));

  setWindowFlags(Qt::ToolTip);

  auto *tipWidget = tooltip->showWidget(pos);

  if (! tipWidget)
    return false;

  tooltip_ = tipWidget;
  parent_  = parent;

  int margin = calcMargin();

  setContentsMargins(margin, margin, margin, margin);

  showAtPos(pos);

  showPos_ = pos;

  updateOpacity(tooltip);

  assert(tooltip_);

  tooltip_->update();

  startHideTimer();

  return true;
}

void
CQToolTip::
updateSize()
{
  auto s = sizeHint();

  resize(s);

  int margin = calcMargin();

  tooltip_->setParent(this);

  auto pal = this->palette();
  pal.setColor(tooltip_->foregroundRole(), pal.color(QPalette::ToolTipText));
  tooltip_->setPalette(pal);

  tooltip_->move(margin, margin);

  tooltip_->resize(s.width() - 2*margin, s.height() - 2*margin);

  tooltip_->show();
}

QSize
CQToolTip::
sizeHint() const
{
  auto s      = calcSize();
  int  margin = calcMargin();

  return QSize(s.width() + 2*margin, s.height() + 2*margin);
}

void
CQToolTip::
updateOpacity(CQToolTipIFace *tooltip)
{
  double o = opacity_;

  if (tooltip->opacity() >= 0)
    o = tooltip->opacity();

  setWindowOpacity(o);

  //tooltip_->setWindowOpacity(o);
}

void
CQToolTip::
showAtPos(const QPoint &pos)
{
  auto drect = desktopRect(pos);

  //---

  auto c = parent_->cursor();

  auto *tooltip = getToolTip(parent_);

  //---

  // cursor size and hotspot
  QSize size(16, 16);

  const auto *bm = c.bitmap();

  if (bm)
    size = bm->size();

  int cw = size.width ();
  int ch = size.height();

  auto hs = c.hotSpot();

  //---

  // tip rect size

  auto ts = sizeHint();

  int tw = ts.width ();
  int th = ts.height();

  //---

  auto align = tooltip->alignment();

  QPoint pos1;

  if (! tooltip->outside()) {
    int dx = -hs.x();
    int dy = -hs.y();

    if      (align & Qt::AlignLeft)
      dx += cw;
    else if (align & Qt::AlignRight)
      dx -= tw;
    else
      dx -= tw/2;

    if      (align & Qt::AlignTop)
      dy += ch;
    else if (align & Qt::AlignBottom)
      dy -= th;
    else
      dy -= th/2;

    int x = pos.x() + dx;
    int y = pos.y() + dy;

    pos1 = QPoint(x, y);
  }
  else {
    int x, y;

    if      (align & Qt::AlignLeft)
      x = 0;
    else if (align & Qt::AlignLeft)
      x = parent_->width() - 1 - tw;
    else
      x = parent_->width()/2 - tw/2;

    if      (align & Qt::AlignTop)
      y = 0;
    else if (align & Qt::AlignBottom)
      y = parent_->height() - 1 - th;
    else
      y = parent_->height()/2 - th/2;

    pos1 = parent_->mapToGlobal(QPoint(x, y));
  }

  //---

  auto s = this->sizeHint();

  int w = s.width ();
  int h = s.height();

  if (pos1.x() + w > drect.right()) {
    if (w < drect.width())
      pos1.setX(drect.right() - w);
    else
      pos1.setX(drect.left());
  }

  if (pos1.y() + h > drect.bottom()) {
    if (h < drect.height())
      pos1.setY(drect.bottom() - h);
    else
      pos1.setY(drect.top());
  }

  //---

  move(pos1);

  updateSize();

  QWidget::show();

  //---

  if (tooltip->grabKey()) {
    //qApp->setActiveWindow(this);

    setFocus();
  }
}

void
CQToolTip::
enterEvent(QEvent *)
{
  if (isVisible())
    hideLater();
}

void
CQToolTip::
paintEvent(QPaintEvent *)
{
  auto *tooltip = getToolTip(parent_);

  if (tooltip && tooltip->isTransparent())
    return;

  QStylePainter painter(this);

  QStyleOptionFrame opt;

  opt.initFrom(this);

  painter.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
}

bool
CQToolTip::
eventFilter(QObject *o, QEvent *e)
{
  switch (e->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease: {
      if (isVisible()) {
        int  key = static_cast<QKeyEvent *>(e)->key();
        auto mod = static_cast<QKeyEvent *>(e)->modifiers();

      //auto *parent = static_cast<QWidget *>(o);

        auto *tooltip = getToolTip(parent_);

        // ignore modifier key presses
        if (key == Qt::Key_Shift || key == Qt::Key_Control ||
            key == Qt::Key_Alt   || key == Qt::Key_Meta)
          break;

        if (tooltip) {
          if (tooltip->isHideKey(key, mod)) {
            hideLater();

            // don't propagate escape
            if (key == Qt::Key_Escape)
              return true;
          }
          else {
            if (e->type() == QEvent::KeyRelease) {
              if (tooltip->keyPress(key, mod)) {
                auto gpos = QCursor::pos();

                if (! tooltip->trackMouse())
                  gpos = showPos_;

                if (! updateTip(parent_, gpos)) {
                  hideLater();

                  return false;
                }

                startHideTimer();

                return true; // don't propagate key
              }
              else {
                hideLater();

                return false;
              }
            }
          }
        }
      }

      break;
    }
    case QEvent::Leave: {
      if (isVisible()) {
        hideLater();
      }

      break;
    }
    case QEvent::WindowActivate:
    case QEvent::WindowDeactivate: {
      if (isVisible()) {
        auto *tooltip = getToolTip(parent_);

        if (tooltip && ! tooltip->grabKey()) {
          hideLater();
        }
      }

      break;
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Wheel: {
      if (isVisible()) {
        hideLater();
      }

      break;
    }
    case QEvent::FocusIn:
    case QEvent::FocusOut: {
      if (isVisible()) {
      //auto *parent     = static_cast<QWidget *>(o);
      //auto *focusEvent = static_cast<QFocusEvent *>(e);

        auto *tooltip = getToolTip(parent_);

        if (tooltip && ! tooltip->grabKey()) {
          hideLater();
        }
      }

      break;
    }
    case QEvent::MouseMove: {
      if (isVisible()) {
      //auto *parent     = static_cast<QWidget *>(o);
        auto *mouseEvent = static_cast<QMouseEvent *>(e);

        auto *tooltip = getToolTip(parent_);

        if (tooltip && ! tooltip->grabKey()) {
          auto gpos = mouseEvent->globalPos();

          if (! tooltip->trackMouse())
            gpos = showPos_;

          if (! updateTip(parent_, gpos)) {
            //hideLater();

            return false;
          }

          startHideTimer();
        }
      }

      break;
    }
    case QEvent::ToolTip: {
      auto *parent    = static_cast<QWidget *>(o);
      auto *helpEvent = static_cast<QHelpEvent *>(e);

      if (showTip(parent, helpEvent->globalPos()))
        return true;

      break;
    }
    default:
      break;
  }

  return false;
}

bool
CQToolTip::
showTip(QWidget *parent, const QPoint &gpos)
{
  auto *tooltip = getToolTip(parent);

  if (tooltip && ! isVisible()) {
    if (! show(gpos, tooltip, parent))
      return true;

    if (tooltip->trackMouse()) {
      setMouseTracking(true);
    }

    if (tooltip->grabKey()) {
      setFocusPolicy(Qt::StrongFocus);
    }

    return true;
  }

  return false;
}

bool
CQToolTip::
updateTip(QWidget *parent, const QPoint &gpos)
{
  auto *tooltip = getToolTip(parent);

  if (tooltip) {
    if (! tooltip->updateWidget(gpos))
      return false;

    showAtPos(gpos);

    updateOpacity(tooltip);

    assert(tooltip_);

    tooltip_->update();

    return true;
  }

  return false;
}

void
CQToolTip::
timerEvent(QTimerEvent *event)
{
  if (event->timerId() == hideTimer_)
    hideLater();
}

void
CQToolTip::
hideLater()
{
  if (! isVisible())
    return;

  stopTimer();

  QTimer::singleShot(0, this, SLOT(hideSlot()));
}

void
CQToolTip::
startHideTimer()
{
  stopTimer();

  double hideSecs = hideSecs_;

  auto *tooltip = getToolTip(parent_);

  if (tooltip && tooltip->hideSecs() > 0)
    hideSecs = tooltip->hideSecs();

  hideTimer_ = startTimer(hideSecs*1000);
}

void
CQToolTip::
stopTimer()
{
  if (hideTimer_) {
    killTimer(hideTimer_);

    hideTimer_ = 0;
  }
}

void
CQToolTip::
hideSlot()
{
  this->hide();

  auto *tooltip = getToolTip(parent_);

  //if (tooltip && tooltip->trackMouse())
  setMouseTracking(false);

  if (tooltip)
    tooltip->hideWidget();

  if (tooltip->grabKey()) {
    if (parent_) {
      //qApp->setActiveWindow(parent_);

      parent_->setFocus();
    }
  }
}

CQToolTipIFace *
CQToolTip::
getToolTip(QWidget *parent)
{
  auto *inst = CQToolTipInst;

  if (! inst->tooltips_.contains(parent))
    return 0;

  return inst->tooltips_.value(parent);
}

void
CQToolTip::
setToolTip(QWidget *parent, QWidget *tooltip)
{
  setToolTip(parent, new CQToolTipWidgetIFace(tooltip));
}

void
CQToolTip::
setToolTip(QWidget *parent, CQToolTipIFace *tooltip)
{
  assert(parent);

  auto *inst = CQToolTipInst;

  if (inst->tooltips_.contains(parent)) {
    delete inst->tooltips_.value(parent);

    inst->tooltips_.remove(parent);
  }

  if (tooltip) {
    inst->hide();

    inst->tooltips_[parent] = tooltip;
  }
}

void
CQToolTip::
unsetToolTip(QWidget *parent)
{
  setToolTip(parent, (CQToolTipIFace *) 0);
}

int
CQToolTip::
calcMargin() const
{
  auto *tooltip = getToolTip(parent_);

  if (tooltip->margin() >= 0)
    return tooltip->margin();
  else
    return margin_;
}

QSize
CQToolTip::
calcSize() const
{
  auto *tooltip = getToolTip(parent_);

  auto s = tooltip->sizeHint();

  if (! s.isValid()) {
    assert(tooltip_);

    s = tooltip_->sizeHint();
  }

  return s;
}

QRect
CQToolTip::
desktopRect(const QPoint &pos) const
{
  auto *desktop = QApplication::desktop();

  int snum;

  if (desktop->isVirtualDesktop())
    snum = desktop->screenNumber(pos);
  else
    snum = desktop->screenNumber(this);

  return desktop->availableGeometry(snum);
}
