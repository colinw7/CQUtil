#ifndef CQColorEdit_H
#define CQColorEdit_H

#include <QFrame>

class CQWidgetMenu;
class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QStyleOptionComboBox;
class QColorDialog;

class CQColorEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QColor  color     READ color      WRITE setColor    )
  Q_PROPERTY(QString colorName READ colorName  WRITE setColorName)
  Q_PROPERTY(bool    editable  READ isEditable WRITE setEditable )

 public:
  CQColorEdit(QWidget *parent=nullptr);

  //---

  const QColor &color() const { return color_; }
  void setColor(const QColor &color);

  QString colorName() const { return colorName_; }
  void setColorName(const QString &colorName);

  bool isEditable() const { return editable_; }
  void setEditable(bool editable=true);

  void setNoFocus();

  //---

  QLineEdit *lineEdit() const { return edit_; }

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;

 private:
  friend class CQColorEditMenuButton;

  void updateEdit();

  void setNoFocusHier(QWidget *w);

  void emitColorChanged();

  void initStyle(QStyleOptionComboBox &opt);

 private Q_SLOTS:
  void textChangedSlot();

  void buttonSlot();

  void showMenuSlot();
  void hideMenuSlot();

  void colorSlot(const QColor &color);

  void closeMenuSlot();

 Q_SIGNALS:
  void colorChanged(const QColor &color);
  void colorNameChanged(const QString &colorName);

  void menuHidden();

 private:
  bool          editable_   { true };
  QColor        color_;
  QString       colorName_;
  QLineEdit*    edit_       { nullptr }; // text edit
  QPushButton*  button_     { nullptr }; // menu button
  CQWidgetMenu* menu_       { nullptr };
  QColorDialog* colorDlg_   { nullptr };
};

//---

#include <QLineEdit>

class CQColorEditEdit : public QLineEdit {
  Q_OBJECT

 public:
  CQColorEditEdit(CQColorEdit *edit);

  void paintEvent(QPaintEvent *) override;

 private:
  CQColorEdit *edit_ { nullptr };
};

//---

#include <QPushButton>

class CQColorEditMenuButton : public QPushButton {
  Q_OBJECT

 public:
  CQColorEditMenuButton(CQColorEdit *edit);

  void paintEvent(QPaintEvent *) override;

 private:
  CQColorEdit *edit_ { nullptr };
};

#endif
