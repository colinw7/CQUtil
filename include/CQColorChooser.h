#ifndef CQColorChooser_H
#define CQColorChooser_H

#include <QWidget>

class CQIconButton;
class CQAlphaButton;

class QLineEdit;
class QPushButton;
class QToolButton;
class QLabel;

class CQColorChooser : public QWidget {
  Q_OBJECT

  Q_PROPERTY(bool    editable  READ getEditable WRITE setEditable )
  Q_PROPERTY(uint    style     READ getStyles   WRITE setStyles   )
  Q_PROPERTY(QColor  color     READ color       WRITE setColor    )
  Q_PROPERTY(QString colorName READ colorName   WRITE setColorName)

  Q_ENUMS(Style);

 public:
  enum Style {
    Text        = (1<<0),
    ColorButton = (1<<1),
    ColorLabel  = (1<<2),
    ImageButton = (1<<3),
    AlphaButton = (1<<4)
  };

 public:
  CQColorChooser(QWidget *parent=0);

  CQColorChooser(uint styles, QWidget *parent=0);

  bool getEditable() const { return editable_; }
  void setEditable(bool editable=true);

  uint getStyles() const { return styles_; }
  void setStyles(uint styles);

  //---

  void setTextStyle(bool b) {
    uint styles = getStyles();

    if (b) styles |=  Text;
    else   styles &= ~Text;

    setStyles(styles);
  }

  bool hasTextStyle() const { return styles_ & Text; }

  //---

  void setColorButtonStyle(bool b) {
    uint styles = getStyles();

    if (b) styles |=  ColorButton;
    else   styles &= ~ColorButton;

    setStyles(styles);
  }

  bool hasColorButtonStyle() const { return styles_ & ColorButton; }

  //---

  void setColorLabelStyle(bool b) {
    uint styles = getStyles();

    if (b) styles |=  ColorLabel;
    else   styles &= ~ColorLabel;

    setStyles(styles);
  }

  bool hasColorLabelStyle() const { return styles_ & ColorLabel; }

  //---

  void setImageButtonStyle(bool b) {
    uint styles = getStyles();

    if (b) styles |=  ImageButton;
    else   styles &= ~ImageButton;

    setStyles(styles);
  }

  bool hasImageButtonStyle() const { return styles_ & ImageButton; }

  //---

  void setAlphaButtonStyle(bool b) {
    uint styles = getStyles();

    if (b) styles |=  AlphaButton;
    else   styles &= ~AlphaButton;

    setStyles(styles);
  }

  bool hasAlphaButtonStyle() const { return styles_ & AlphaButton; }

  //---

  const QColor &color() const;
  void setColor(const QColor &color, double alpha=1.0);

  QString colorName() const;
  void setColorName(const QString &colorName, double alpha=1.0);

  double getAlpha() const;
  void setAlpha(double alpha);

  QLineEdit *getTextWidget() const { return cedit_; }

 private:
  void init();
  void update();
  void changeColor();
  void applyColor();

  void setWidgetBackground(QWidget *widget, const QColor &color);

 private slots:
  void textColorChanged();
  void imageButtonClicked();
  void colorButtonClicked();
  void alphaButtonChanged();

 signals:
  void colorChanged(const QColor &color);
  void colorChanged(const QString &colorName);

  void colorApplied(const QColor &color);
  void colorApplied(const QString &colorName);

 private:
  uint           styles_    { 0 };
  bool           editable_  { true };
  bool           has_alpha_ { false };
  QColor         color_;
  QString        colorName_;
  double         alpha_     { 1.0 };
  QLineEdit     *cedit_     { nullptr }; // text color name
  QToolButton   *cbutton_   { nullptr }; // click color button
  QLabel        *clabel_    { nullptr }; // static color button
  CQAlphaButton *alphab_    { nullptr }; // alpha slider
  CQIconButton  *button_    { nullptr }; // image click button
};

#endif
