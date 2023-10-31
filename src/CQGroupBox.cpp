#include <CQGroupBox.h>
#include <CQStyleMgr.h>

#include <QChildEvent>
#include <QLayout>
#include <QPainter>
#include <QPainterPath>
#include <QStylePainter>
#include <QStyleOption>
#include <QMouseEvent>
#include <cmath>

CQGroupBox::
CQGroupBox(QWidget *parent) :
 QWidget(parent), title_()
{
  init();
}

CQGroupBox::
CQGroupBox(const QString &title, QWidget *parent) :
 QWidget(parent), title_(title)
{
  init();
}

CQGroupBox::
~CQGroupBox()
{
}

void
CQGroupBox::
init()
{
  setObjectName("groupBox");

  updateStyle();

  //----

  //area_ = new CQGroupBoxArea;

  //area_->setParent(this);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  setMouseTracking(true);

  //----

  connect(CQStyleMgrInst, SIGNAL(fontChanged()), this, SLOT(updateSlot()));
  connect(CQStyleMgrInst, SIGNAL(colorsChanged()), this, SLOT(updateSlot()));
}

void
CQGroupBox::
setTitleFont()
{
  titleFont_ = font();

  titleFont_.setPointSizeF(titleFont_.pointSizeF()*titleScale_);

  titleFont_.setBold(titleBold_);

  calculateFrame();
}

void
CQGroupBox::
setTitle(const QString &title)
{
  title_ = title;

  calculateFrame();
}

void
CQGroupBox::
setTitleBold(bool bold)
{
  titleBold_ = bold;

  setTitleFont();

  calculateFrame();
}

void
CQGroupBox::
setTitleScale(double scale)
{
  titleScale_ = scale;

  setTitleFont();

  calculateFrame();
}

void
CQGroupBox::
setTitleColored(bool b)
{
  titleColored_ = b;

  update();
}

void
CQGroupBox::
setTitleAlignment(Qt::Alignment alignment)
{
  titleAlignment_ = alignment;

  calculateFrame();
}

void
CQGroupBox::
setHasLineTop(bool line)
{
  lineTop_ = line;

  calculateFrame();
}

void
CQGroupBox::
setHasLineBottom(bool line)
{
  lineBottom_ = line;

  calculateFrame();
}

void
CQGroupBox::
setLineTopAlignment(Qt::Alignment alignment)
{
  lineTopAlignment_ = alignment;

  calculateFrame();
}

void
CQGroupBox::
setLineBottomAlignment(Qt::Alignment alignment)
{
  lineBottomAlignment_ = alignment;

  calculateFrame();
}

void
CQGroupBox::
setMarginLeft(int margin)
{
  marginLeft_ = margin;

  calculateFrame();
}

void
CQGroupBox::
setMarginRight(int margin)
{
  marginRight_ = margin;

  calculateFrame();
}

void
CQGroupBox::
setMarginBottom(int margin)
{
  marginBottom_ = margin;

  calculateFrame();
}

void
CQGroupBox::
setMarginTop(int margin)
{
  marginTop_ = margin;

  calculateFrame();
}

void
CQGroupBox::
setCheckable(bool checkable)
{
  if (checkable != checkable_) {
    checkable_ = checkable;

    updateEnabled();

    calculateFrame();
  }
}

void
CQGroupBox::
setChecked(bool checked)
{
  if (checked != checked_) {
    checked_ = checked;

    if (isCheckable()) {
      updateEnabled();

      emit toggled(isChecked());

      update();
    }
  }
}

void
CQGroupBox::
setEnableChecked(bool b)
{
  if (b != enableChecked_) {
    enableChecked_ = b;

    if (isCheckable()) {
      updateEnabled();

      update();
    }
  }
}

void
CQGroupBox::
setCollapsible(bool collapsible)
{
  if (collapsible != collapsible_) {
    collapsible_ = collapsible;

    updateCollapsed();

    calculateFrame();
  }
}

void
CQGroupBox::
setCollapsed(bool collapsed)
{
  if (collapsed != collapsed_) {
    collapsed_ = collapsed;

    if (isCollapsible()) {
      updateCollapsed();

      calculateFrame();

      emit collapse(isCollapsed());
    }
  }
}

void
CQGroupBox::
changeEvent(QEvent *e)
{
  if (e->type() == QEvent::EnabledChange)
    updateEnabled();

  calculateFrame();

  QWidget::changeEvent(e);
}

void
CQGroupBox::
childEvent(QChildEvent *e)
{
  if (e->type() != QEvent::ChildAdded || ! e->child()->isWidgetType())
    return;

  calculateFrame();

  if (isCheckable())
    updateEnabled();

  QWidget::childEvent(e);
}

bool
CQGroupBox::
event(QEvent *e)
{
  if      (e->type() == QEvent::KeyPress) {
    auto *ke = static_cast<QKeyEvent*>(e);

    if (ke->key() == Qt::Key_Select || ke->key() == Qt::Key_Space) {
      if (isCheckable()) {
        checkPress_ = true;

        update();

        return true;
      }
    }
  }
  else if (e->type() == QEvent::KeyRelease) {
    auto *ke = static_cast<QKeyEvent*>(e);

    if (ke->key() == Qt::Key_Select || ke->key() == Qt::Key_Space) {
      if (isCheckable()) {
        setChecked(! isChecked());

        checkPress_ = false;

        emit clicked(isChecked());

        update();

        return true;
      }
    }
  }

  return QWidget::event(e);
}

void
CQGroupBox::
focusInEvent(QFocusEvent *e)
{
  QWidget::focusInEvent(e);
}

void
CQGroupBox::
leaveEvent(QEvent *)
{
  insideCheck_    = false;
  insideCollapse_ = false;
}

void
CQGroupBox::
enterEvent(QEvent *)
{
  insideCheck_    = false;
  insideCollapse_ = false;
}

void
CQGroupBox::
mouseMoveEvent(QMouseEvent *e)
{
  if (! pressed_) {
    bool insideCheck    = insideCheck_;
    bool insideCollapse = insideCollapse_;

    insideCheck_    = false;
    insideCollapse_ = false;

    if (isCheckable())
      insideCheck_ = checkRect_.contains(e->pos()) || titleRect_.contains(e->pos());

    if (isCollapsible())
      insideCollapse_ = collapseRect_.contains(e->pos());

    if (insideCheck != insideCheck_ || insideCollapse != insideCollapse_)
      update();
  }

  QWidget::mouseMoveEvent(e);
}

void
CQGroupBox::
mousePressEvent(QMouseEvent *e)
{
  pressed_ = true;

  if (isCheckable()) {
    bool inside = checkRect_.contains(e->pos()) || titleRect_.contains(e->pos());

    if (inside) {
      checkPress_ = true;

      update();

      return;
    }
  }

  if (isCollapsible()) {
    bool inside = collapseRect_.contains(e->pos());

    if (inside) {
      collapsePress_ = true;

      update();

      return;
    }
  }

  QWidget::mousePressEvent(e);
}

void
CQGroupBox::
mouseReleaseEvent(QMouseEvent *e)
{
  pressed_ = false;

  if (isCheckable()) {
    bool inside = checkRect_.contains(e->pos()) || titleRect_.contains(e->pos());

    if (inside) {
      setChecked(! isChecked());

      checkPress_ = false;

      emit clicked(isChecked());

      update();
    }
  }

  if (isCollapsible()) {
    bool inside = collapseRect_.contains(e->pos());

    if (inside) {
      setCollapsed(! isCollapsed());

      collapsePress_ = false;

      update();
    }
  }

  QWidget::mouseReleaseEvent(e);
}

void
CQGroupBox::
paintEvent(QPaintEvent *)
{
  QStylePainter painter(this);

  painter.setRenderHints(QPainter::Antialiasing);

  painter.fillRect(rect(), QBrush(palette().color(QPalette::Window)));

  QFontMetrics fm(titleFont_);

  // set check size
  int checkSize  = (isCheckable() ? fm.height() + 4 : 0);
  int checkSize1 = (isCheckable() ? checkSize + dx_ : 0);

  // set collapse size
  int collapseSize  = (isCollapsible() ? fm.ascent() : 0);
  int collapseSize1 = (isCollapsible() ? collapseSize + dx_ : 0);

  // set text position
  int textX = 0;

  int tw = (title_ != "" ? fm.horizontalAdvance(title_) : 0);

  if      (titleAlignment_ & Qt::AlignRight)
    textX = width() - dx_ - tw - collapseSize1;
  else if (titleAlignment_ & Qt::AlignHCenter)
    textX = (width() - tw - checkSize1 - collapseSize1)/2 + checkSize1 + dx_;
  else if (titleAlignment_ & Qt::AlignLeft)
    textX = dx_ + checkSize1;

  int textY = 0;

  if      (titleAlignment_ & Qt::AlignBottom)
    textY = spaceTop() - fm.descent() - 2;
  else if (titleAlignment_ & Qt::AlignTop)
    textY = fm.ascent() + 2;
  else if (titleAlignment_ & Qt::AlignVCenter)
    textY = spaceTop()/2 + fm.descent() + 2;

  // set check position
  int checkX = textX - checkSize1;
  int checkY = textY - fm.ascent()/2;

  // set collapse position
  int collapseX = width() - collapseSize1;
  int collapseY = textY - fm.ascent()/2;

  // draw top line
  if (hasLineTop()) {
    int lineY;

    if      (lineTopAlignment_ & Qt::AlignTop)
      lineY = 2;
    else if (lineTopAlignment_ & Qt::AlignVCenter)
      lineY = spaceTop()/2;
    else
      lineY = spaceTop() - 2;

    painter.setPen(lineColor_);

    painter.drawLine(dx_, lineY, width() - 2*dx_, lineY);
  }

  // draw title
  if (title_ != "") {
    painter.setFont(titleFont_);

    int tw1 = width() - 2*dx_ - checkSize1 - collapseSize1;
    int tw2 = std::min(tw, tw1);

    titleRect_ = QRect(textX - dx_, textY - fm.ascent() + fm.descent(), tw2 + dx_, fm.height());

    painter.fillRect(titleRect_, QBrush(palette().color(QPalette::Window)));

    QColor titleColor;

    if (isEnabled()) {
      if (isTitleColored())
        titleColor = CQStyleMgrInst->calcBaseColor();
      else
        titleColor = palette().color(QPalette::Active, QPalette::WindowText);
    }
    else
      titleColor = palette().color(QPalette::Disabled, QPalette::WindowText);

    painter.setPen(titleColor);

    painter.drawText(textX, textY, title_);
  }

  // draw bottom line
  if (lineBottom_) {
    int lineY;

    if      (lineTopAlignment_ & Qt::AlignTop)
      lineY = height() - spaceBottom() + 2;
    else if (lineTopAlignment_ & Qt::AlignVCenter)
      lineY = height() - spaceBottom()/2;
    else
      lineY = height() - 2;

    painter.setPen(lineColor_);

    painter.drawLine(dx_, lineY, width() - 2*dx_, lineY);
  }

  //------

  // draw check (if checkable)
  if (isCheckable()) {
    bool isChecked = this->isChecked();

    //---

    int checkX1    = checkX + 2;
    int checkSize1 = checkSize - 4;
    int checkY1    = checkY - checkSize1/2;

    checkRect_ = QRect(checkX1, checkY1, checkSize1, checkSize1);

    painter.fillRect(QRect(checkX1 - 2, checkY1 - 2, checkSize1 + 4, checkSize1 + 4),
                     QBrush(palette().color(QPalette::Window)));

    QStyleOptionButton opt;

    opt.initFrom(this);

    opt.rect = checkRect_;

    opt.state |= (isChecked ? QStyle::State_On : QStyle::State_Off);

    if (checkPress_)
      opt.state |= QStyle::State_Sunken;

    if (insideCheck_)
      opt.state |= QStyle::State_MouseOver;
    else
      opt.state &= ~QStyle::State_MouseOver;

    painter.drawControl(QStyle::CE_CheckBox, opt);
  }

  //------

  // draw collpase state (if collapsible)
  if (isCollapsible()) {
    bool isCollapsed = this->isCollapsed();

    //---

    double collapseSize1 = collapseSize - 4;

    double collapseX1 = collapseX + 2;
    double collapseY1 = collapseY - collapseSize1/2.0;
    double collapseX2 = collapseX1 + collapseSize1;
    double collapseY2 = collapseY1 + collapseSize1;

    collapseRect_ = QRect(int(collapseX1), int(collapseY1), int(collapseSize1), int(collapseSize1));

    painter.fillRect(QRectF(collapseX1 - 2, collapseY1 - 2, collapseSize1 + 4, collapseSize1 + 4),
                            QBrush(palette().color(QPalette::Window)));

    double collapseXM = (collapseX1 + collapseX2)/2.0;
    double collapseYM = (collapseY1 + collapseY2)/2.0;

    painter.setPen(Qt::NoPen);

    if      (insideCollapse_)
      painter.setBrush(palette().color(QPalette::Active, QPalette::Highlight));
    else if (collapsePress_)
      painter.setBrush(palette().color(QPalette::Active, QPalette::Dark));
    else
      painter.setBrush(palette().color(QPalette::Active, QPalette::Text));

    if (! isCollapsed) {
      drawArcShape(&painter, collapseXM, collapseYM, collapseSize1/2.0, 90, 3);
    }
    else {
      drawArcShape(&painter, collapseXM, collapseYM, collapseSize1/2.0, -90, 3);
    }
  }
}

void
CQGroupBox::
drawArcShape(QPainter *painter, double xc, double yc, double r, double startAngle, int sides) const
{
  auto Deg2Rad = [](double d) -> double { return M_PI*d/180.0; };
//auto Rad2Deg = [](double r) -> double { return 180.0*r/M_PI; };

  double x1 = xc - r;
  double y1 = yc - r;
  double x2 = xc + r;
  double y2 = yc + r;

  double xm = (x1 + x2)/2;
  double ym = (y1 + y2)/2;

  double da = 360.0/sides;
  double dc = 360.0/40;

  QPainterPath path;

  for (int i = 0; i < sides; ++i) {
    double angle = startAngle + i*da;

    double a1 = Deg2Rad(angle - dc);
    double a2 = Deg2Rad(angle + dc);

    double c1 = cos(a1), s1 = sin(a1);
    double c2 = cos(a2), s2 = sin(a2);

    QPointF p1(xm + r*c1, ym + r*s1);
    QPointF p2(xm + r*c2, ym + r*s2);

    if (i == 0)
      path.moveTo(p1);
    else
      path.lineTo(p1);

    //---

    auto p12 = (p1 + p2)/2;

    double ar = 2*hypot(p1.x() - p12.x(), p1.y() - p12.y())/sides;

    double a = Deg2Rad(angle);

    double c = cos(a), s = sin(a);

    QPointF pq(xm + (r + ar)*c, ym + (r + ar)*s);

    path.quadTo(pq, p2);
  }

  path.closeSubpath();

  painter->drawPath(path);
}

void
CQGroupBox::
resizeEvent(QResizeEvent *)
{
  if (cornerWidget_)
    cornerWidget_->move(width() - cornerWidget_->width() - 2, 2);
}

int
CQGroupBox::
spaceTop() const
{
  if (marginTop_ >= 0)
    return marginTop_;

  int t = 0;

  if (title_ != "") {
    QFontMetrics fm(titleFont_);

    t = fm.height();
  }

  if (cornerWidget_)
    t = std::max(t, cornerWidget_->sizeHint().height());

  t += 4;

  return t;
}

int
CQGroupBox::
spaceBottom() const
{
  if (marginBottom_ >= 0)
    return marginBottom_;

  return 4;
}

void
CQGroupBox::
setCornerWidget(QWidget *w)
{
  delete cornerWidget_;

  cornerWidget_ = w;

  if (cornerWidget_) {
    cornerWidget_->setParent(this);

    cornerWidget_->resize(cornerWidget_->sizeHint());

    cornerWidget_->show();

    resizeEvent(nullptr);
  }

  calculateFrame();
}

QSize
CQGroupBox::
sizeHint() const
{
  return minimumSizeHint();
}

QSize
CQGroupBox::
minimumSizeHint() const
{
  QFontMetrics fm(titleFont_);

  int baseWidth  = fm.horizontalAdvance(title_) + 4;
  int baseHeight = fm.height();

  if (isCheckable())
    baseWidth += fm.height() + 4;

  if (isCollapsible())
    baseWidth += fm.height();

  if (cornerWidget_)
    baseWidth += cornerWidget_->minimumSizeHint().width();

  QSize size;

  if (! isCollapsed()) {
    QStyleOptionGroupBox option;

    size = style()->sizeFromContents(QStyle::CT_GroupBox, &option,
                                     QSize(baseWidth, baseHeight), this);
  }
  else {
    size = QSize(baseWidth, baseHeight);
  }

  return size.expandedTo(QWidget::minimumSizeHint());
}

void
CQGroupBox::
calculateFrame()
{
  auto *l = layout();
  if (! l) return;

  l->setContentsMargins(marginLeft(), spaceTop(), marginRight(), spaceBottom());

  update();
}

void
CQGroupBox::
updateEnabled()
{
  bool enabled = isEnabled();

  auto childList = children();

  for (int i = 0; i < childList.size(); ++i) {
    auto *o = childList.at(i);

    if (! o->isWidgetType()) continue;

    auto *w = static_cast<QWidget *>(o);

    if (isCheckable()) {
      if (isEnableChecked()) {
        if (isChecked()) {
//        if (! w->isEnabled()) {
//          if (! w->testAttribute(Qt::WA_ForceDisabled))
              w->setEnabled(true);
//        }
        }
        else {
//        if (w->isEnabled())
            w->setEnabled(false);
        }
      }
      else {
        w->setEnabled(enabled);
      }
    }
    else {
      w->setEnabled(enabled);
    }
  }
}

void
CQGroupBox::
updateCollapsed()
{
  //area_->setVisible(isCollapsed());

  auto childList = children();

  for (int i = 0; i < childList.size(); ++i) {
    auto *o = childList.at(i);

    if (! o->isWidgetType()) continue;

    auto *w = static_cast<QWidget *>(o);

    w->setVisible(! isCollapsed());
  }

  auto size = this->size();

  size.setHeight(minimumSizeHint().height());

  resize(size);
}

void
CQGroupBox::
updateSlot()
{
  updateStyle();

  update();
}

void
CQGroupBox::
updateStyle()
{
  setTitleFont();

  lineColor_ = palette().color(QPalette::Mid);
}

//-----------

#if 0
CQGroupBoxArea::
CQGroupBoxArea(QWidget *parent) :
 QWidget(parent)
{
}

void
CQGroupBoxArea::
paintEvent(QPaintEvent *)
{
  //QPainter painter(this);

  //painter.fillRect(rect(), QBrush(QColor(255, 0, 0)));
}
#endif
