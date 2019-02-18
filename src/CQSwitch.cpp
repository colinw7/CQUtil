#include <CQSwitch.h>
#include <QPainter>

namespace {

template<typename T>
T clamp(T val, T low, T high) {
  if (val < low ) return low;
  if (val > high) return high;
  return val;
}

QColor blendColors(const QColor &c1, const QColor &c2, double f) {
  double f1 = 1.0 - f;

  double r = c1.redF  ()*f + c2.redF  ()*f1;
  double g = c1.greenF()*f + c2.greenF()*f1;
  double b = c1.blueF ()*f + c2.blueF ()*f1;

  return QColor(clamp(int(255*r), 0, 255),
                clamp(int(255*g), 0, 255),
                clamp(int(255*b), 0, 255));
}

}

//---

CQSwitch::
CQSwitch(QWidget *parent) :
 QAbstractButton(parent)
{
  init();
}

CQSwitch::
CQSwitch(QWidget *parent, const QString &onLabel, const QString &offLabel) :
 QAbstractButton(parent), onLabel_(onLabel), offLabel_(offLabel)
{
  init();
}

CQSwitch::
CQSwitch(const QString &onLabel, const QString &offLabel, QWidget *parent) :
 QAbstractButton(parent), onLabel_(onLabel), offLabel_(offLabel)
{
  init();
}

void
CQSwitch::
init()
{
  setCheckable(true);
}

void
CQSwitch::
mousePressEvent(QMouseEvent*)
{
  emit pressed();
}

void
CQSwitch::
mouseReleaseEvent(QMouseEvent*)
{
  click();

  emit released();
}

void
CQSwitch::
click()
{
  toggle();

  emit clicked();
}

void
CQSwitch::
toggle()
{
  setChecked(! isChecked());

  update();

  emit toggled(isChecked());
}

void
CQSwitch::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QColor fc  = palette().color(QPalette::Background);
  QColor hc  = palette().color(QPalette::Highlight);
  QColor tc  = palette().color(QPalette::Text);
  QColor htc = palette().color(QPalette::HighlightedText);
  QColor bc  = palette().color(QPalette::Button);

  painter.fillRect(rect(), fc);

  QString text = (isChecked() ? onLabel() : offLabel());

//int w = width ();
  int h = height();

  QFontMetrics fm(painter.font());

//int tw = fm.width(text);
  int ta = fm.ascent();
  int td = fm.descent();
  int th = ta + td;

  int m = margin();

  int cs = th/2;

  // draw border
  QSize sh = sizeHint();

  int cw = std::min(rect().width (), sh.width ());
  int ch = std::min(rect().height(), sh.height());

  QRect brect(0, 0, cw - 1, ch - 1);

  QColor borderFillColor   = (isHighlightOn() && isChecked() ? hc : blendColors(bc, tc, 0.9));
  QColor borderStrokeColor = blendColors(tc, fc, 0.5);

  painter.setPen(borderStrokeColor);
  painter.setBrush(borderFillColor);

  painter.drawRoundedRect(brect, cs, cs);

  // draw text
  QColor textColor = (isHighlightOn() && isChecked() ? htc : tc);

  painter.setPen(textColor);

  int tx = (isChecked() ? m : ta + 2*m);

  int tym = (h - th)/2;

  painter.drawText(tx, tym + ta, text);

  // draw toggle
  QColor toggleStrokeColor = tc;

  toggleStrokeColor.setAlphaF(0.5);

  painter.setPen(toggleStrokeColor);
  painter.setBrush(bc);

  int ex = (isChecked() ? cw - ta - m : m);

  int eym = (h - ta)/2;

  QRect erect(ex, eym, ta - 1, ta - 1);

  painter.drawRoundedRect(erect, cs, cs);
}

QSize
CQSwitch::
sizeHint() const
{
  QFontMetrics fm(font());

  int tw1 = fm.width(onLabel ());
  int tw2 = fm.width(offLabel());

  int th = fm.height();

  int m = margin();

  return QSize(std::max(tw1, tw2) + th + 3*m, th + 2*m);
}
