#include <CQAlignEdit.h>
#include <CQWidgetMenu.h>
#include <CQUtil.h>

#include <QStylePainter>
#include <QStyleOptionComboBox>
#include <QMouseEvent>

namespace {

QColor blendColors(const QColor &c1, const QColor &c2, double f) {
  auto iclamp = [](double val, int low, int high) {
    if (val < low ) return low;
    if (val > high) return high;
    return int(val);
  };

  double f1 = 1.0 - f;

  double r = c1.redF  ()*f + c2.redF  ()*f1;
  double g = c1.greenF()*f + c2.greenF()*f1;
  double b = c1.blueF ()*f + c2.blueF ()*f1;

  return QColor(iclamp(255*r, 0, 255), iclamp(255*g, 0, 255), iclamp(255*b, 0, 255));
}

}

//------

CQAlignEdit::
CQAlignEdit(QWidget *parent) :
 QFrame(parent)
{
}

CQAlignEdit::
~CQAlignEdit()
{
}

void
CQAlignEdit::
setAlign(const Qt::Alignment &v)
{
  align_ = v;

  update();

  emit valueChanged(align_);
}

void
CQAlignEdit::
mousePressEvent(QMouseEvent *e)
{
  auto p = e->pos();

  QStyleOptionComboBox opt;

  initStyleOption(opt);

#if 0
  auto popupRect =
    style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

  if (! popupRect.contains(p))
    return;
#else
  if (! opt.rect.contains(p))
    return;
#endif

  auto *menu = new CQWidgetMenu;

  auto *widget = new CQAlignEditMenuWidget(this);

  menu->setWidget(widget);

  auto gpos = mapToGlobal(rect().bottomLeft());

  menu->exec(gpos);

  delete menu;
}

void
CQAlignEdit::
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

  auto str = toString();

  QFontMetrics fm(font());

  painter.drawText(r.left(), r.center().y() + (fm.ascent() - fm.descent())/2, str);
}

QString
CQAlignEdit::
toString() const
{
  auto align = this->align();

  return toString(align);
}

QString
CQAlignEdit::
toString(Qt::Alignment align)
{
  return CQUtil::alignToString(align);
}

Qt::Alignment
CQAlignEdit::
fromString(const QString &str)
{
  Qt::Alignment align;

  if (! CQUtil::stringToAlign(str, align))
    return Qt::Alignment();

  return align;
}

QSize
CQAlignEdit::
sizeHint() const
{
  auto str = toString();

  QFontMetrics fm(font());

  QSize fs(fm.horizontalAdvance(str) + 4, fm.height() + 4);

  QStyleOptionComboBox opt;

  initStyleOption(opt);

  auto popupRect =
    style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxArrow, this);

  return QSize(fs.width() + popupRect.width(), fs.height());
}

void
CQAlignEdit::
initStyleOption(QStyleOptionComboBox &opt) const
{
  opt.rect  = rect();
  opt.state = QStyle::State_Enabled;
}

//-----

CQAlignEditMenuWidget::
CQAlignEditMenuWidget(CQAlignEdit *edit) :
 edit_(edit)
{
  setMouseTracking(true);
}

void
CQAlignEditMenuWidget::
resizeEvent(QResizeEvent *)
{
  auto rect = this->rect();

  int m = 4;
  int s = 8;

  int x1 = rect.left  () + m;
  int y1 = rect.top   () + m;
  int x2 = rect.right () - m;
  int y2 = rect.bottom() - m;

  int xm = rect.center().x();
  int ym = rect.center().y();

  alignRect_[Qt::AlignLeft   ] = Rect(QRect(x1      , y1, s, y2 - y1));
  alignRect_[Qt::AlignRight  ] = Rect(QRect(x2 - s  , y1, s, y2 - y1));
  alignRect_[Qt::AlignHCenter] = Rect(QRect(xm - s/2, y1, s, y2 - y1));
//alignRect_[Qt::AlignJustify] = Rect(QRect(xm - s/2, y1, s, y2 - y1));

  alignRect_[Qt::AlignTop    ] = Rect(QRect(x1, y1      , x2 - x1, s));
  alignRect_[Qt::AlignBottom ] = Rect(QRect(x1, y2 - s  , x2 - x1, s));
  alignRect_[Qt::AlignVCenter] = Rect(QRect(x1, ym - s/2, x2 - x1, s));
}

void
CQAlignEditMenuWidget::
paintEvent(QPaintEvent *)
{
  auto bg = palette().color(QPalette::Window);

  auto rect = this->rect();

  QPainter painter(this);

  painter.setBrush(bg);

  painter.fillRect(rect, painter.brush());

  for (int i = 0; i < 2; ++i) {
    drawAlignRect(&painter, Qt::AlignLeft, i == 1);
    drawAlignRect(&painter, Qt::AlignRight, i == 1);
    drawAlignRect(&painter, Qt::AlignHCenter, i == 1);
  //drawAlignRect(&painter, Qt::AlignJustify, i == 1);

    drawAlignRect(&painter, Qt::AlignTop, i == 1);
    drawAlignRect(&painter, Qt::AlignBottom, i == 1);
    drawAlignRect(&painter, Qt::AlignVCenter, i == 1);
  }
}

void
CQAlignEditMenuWidget::
mousePressEvent(QMouseEvent *me)
{
  Qt::Alignment align;

  for (auto &rect : alignRect_) {
    if (rect.second.r.contains(me->pos()))
      align |= rect.first;
  }

  Qt::Alignment halign = (Qt::AlignLeft | Qt::AlignRight | Qt::AlignHCenter | Qt::AlignJustify);
  Qt::Alignment valign = (Qt::AlignTop | Qt::AlignBottom | Qt::AlignVCenter);

  if (! (align & halign))
    align |= edit_->align() & halign;

  if (! (align & valign))
    align |= edit_->align() & valign;

  if (align)
    edit_->setAlign(align);

  update();
}

void
CQAlignEditMenuWidget::
mouseMoveEvent(QMouseEvent *me)
{
  for (auto &rect : alignRect_)
    rect.second.inside = rect.second.r.contains(me->pos());

  update();
}

void
CQAlignEditMenuWidget::
drawAlignRect(QPainter *p, Qt::Alignment rectAlign, bool on)
{
  auto align = edit_->align();

  auto bg       = palette().color(QPalette::Window);
  auto onColor  = QColor("#586e75");
  auto offColor = blendColors(onColor, bg, 0.5);

  auto highlight = palette().color(QPalette::Highlight);

  if (! alignRect_[rectAlign].inside) {
    if (align & rectAlign) {
      if (! on)
        return;

      p->setBrush(onColor);
    }
    else {
      if (on)
        return;

      p->setBrush(offColor);
    }
  }
  else {
    p->setBrush(highlight);
  }

  p->fillRect(alignRect_[rectAlign].r, p->brush());
}

QSize
CQAlignEditMenuWidget::
sizeHint() const
{
  return QSize(128, 128);
}
