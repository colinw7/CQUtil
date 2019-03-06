#include <CQLineEdit.h>

#include <QEvent>
#include <QAction>

#include <iostream>

// create line edit
CQLineEdit::
CQLineEdit(QWidget *parent) :
 QLineEdit(parent), placeColor_(), placeItalic_(true), state_(STATE_NONE), ignore_(false)
{
#if QT_VERSION >= 0x050000
  setClearButtonEnabled(true);
#endif

  updateState();

  connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(updateState()));

  //---

  for (int i = 0; i < children().size(); ++i) {
    QAction *action = qobject_cast<QAction *>(children().at(i));

    if (action)
      connect(action, SIGNAL(triggered()), this, SIGNAL(returnPressed()), Qt::QueuedConnection);
  }
}

// set color of placeholder text
void
CQLineEdit::
setPlaceholderColor(const QColor &c)
{
  placeColor_ = c;

  updateState(true);
}

// set is placeholder italic
void
CQLineEdit::
setPlaceholderItalic(bool italic)
{
  placeItalic_ = italic;

  updateState(true);
}

// update placeholder state
void
CQLineEdit::
updateState(bool force)
{
  State state = STATE_NONE;

  if (! hasFocus() && text().isEmpty())
    state = STATE_PLACE;
  else
    state = STATE_NORMAL;

  if (force || state != state_) {
    state_ = state;

    updateStyle();
  }
}

// update style sheet strings
void
CQLineEdit::
updateStyle()
{
  static QString styleFmt = "QLineEdit { color: %1; font: %2 }";

  // reset style so we get normal colors
  setStyleSheet("");

  QColor fg = palette().text().color();

  // set style
  QString style;

  if (state_ == STATE_PLACE) {
    QColor placeColor;

    if (! placeColor_.isValid()) {
      // if no place color defined then blend background and foreground
      QColor bg = palette().color(backgroundRole());

      double f1 = (isEnabled() ? 0.6 : 0.4);
      double f2 = 1.0 - f1;

      int r = int(f1*fg.red  () + f2*bg.red  ());
      int g = int(f1*fg.green() + f2*bg.green());
      int b = int(f1*fg.blue () + f2*bg.blue ());

      placeColor = QColor(r, g, b);
    }
    else {
      // use defined color
      placeColor = placeColor_;

      if (! isEnabled())
        placeColor = placeColor.lighter();
    }

    // set place style
    style  = QString(styleFmt).arg(colorString(placeColor)).
                               arg(placeItalic_ ? "italic" : "normal");

    setStyleSheet(style);
  }
}

// update style on focus in event
void
CQLineEdit::
focusInEvent(QFocusEvent *e)
{
  updateState();

  QLineEdit::focusInEvent(e);
}

// update style on focus out event
void
CQLineEdit::
focusOutEvent(QFocusEvent *e)
{
  updateState();

  QLineEdit::focusOutEvent(e);
}

// update style on change event
void
CQLineEdit::
changeEvent(QEvent *e)
{
  if (ignore_) return;

  ignore_ = true;

  if (e->type() == QEvent::EnabledChange)
    updateState(true);

  ignore_ = false;

  QLineEdit::changeEvent(e);
}

// get stylesheet color string from color
QString
CQLineEdit::
colorString(const QColor &color)
{
  return QString("rgb(%1,%2,%3)").arg(color.red()).arg(color.green()).arg(color.blue());
}
