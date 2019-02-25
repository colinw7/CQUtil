#ifndef CQFontEdit_H
#define CQFontEdit_H

#include <QFrame>

class CQWidgetMenu;
class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QStyleOptionComboBox;
class QFontDialog;

class CQFontEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QFont   font     READ font       WRITE setFont    )
  Q_PROPERTY(QString fontName READ fontName   WRITE setFontName)
  Q_PROPERTY(bool    editable READ isEditable WRITE setEditable)

 public:
  CQFontEdit(QWidget *parent=nullptr);

  //---

  const QFont &font() const { return font_; }
  void setFont(const QFont &font);

  QString fontName() const { return fontName_; }
  void setFontName(const QString &fontName);

  bool isEditable() const { return editable_; }
  void setEditable(bool editable=true);

  void setNoFocus();

  //---

  QLineEdit *lineEdit() const { return edit_; }

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;

 private:
  friend class CQFontEditMenuButton;

  void updateEdit();

  void setNoFocusHier(QWidget *w);

  void emitFontChanged();

  void initStyle(QStyleOptionComboBox &opt);

 private slots:
  void textChangedSlot();

  void buttonSlot();

  void showMenuSlot();
  void hideMenuSlot();

  void fontSlot(const QFont &font);

  void closeMenuSlot();

 signals:
  void fontChanged(const QFont &font);
  void fontChanged(const QString &fontName);

  void menuHidden();

 private:
  bool          editable_  { true };
  QFont         font_;
  QString       fontName_;
  QLineEdit*    edit_      { nullptr }; // text edit
  QPushButton*  button_    { nullptr }; // menu button
  CQWidgetMenu* menu_      { nullptr };
  QFontDialog*  fontDlg_   { nullptr };
};

//---

#include <QLineEdit>

class CQFontEditEdit : public QLineEdit {
  Q_OBJECT

 public:
  CQFontEditEdit(CQFontEdit *edit);

  void paintEvent(QPaintEvent *) override;

 private:
  CQFontEdit *edit_ { nullptr };
};

//---

#include <QPushButton>

class CQFontEditMenuButton : public QPushButton {
  Q_OBJECT

 public:
  CQFontEditMenuButton(CQFontEdit *edit);

  void paintEvent(QPaintEvent *) override;

 private:
  CQFontEdit *edit_ { nullptr };
};

#endif
