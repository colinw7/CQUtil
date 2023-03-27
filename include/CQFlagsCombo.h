#ifndef CQFlagsCombo_H
#define CQFlagsCombo_H

#include <QFrame>
#include <QMenu>
#include <QListWidget>
#include <QPushButton>

class CQFlagsComboMenuButton;
class CQFlagsComboMenu;
class CQFlagsComboList;

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QStyleOptionComboBox;

class CQFlagsCombo : public QFrame {
  Q_OBJECT

  Q_PROPERTY(uint value READ value WRITE setValue)

 public:
  CQFlagsCombo(QWidget *parent = nullptr);
 ~CQFlagsCombo();

  uint value() const { return value_; }
  void setValue(uint value);

  void addItem(const QString &item, uint id);

  QLineEdit *edit() const { return edit_; }

  void paintEvent(QPaintEvent *) override;

  void initStyleOption(QStyleOptionComboBox &opt);

 private:
  void updateEdit();

  CQFlagsComboMenu *getMenu() const;

 Q_SIGNALS:
  void valueChanged(uint);

 public Q_SLOTS:
  void showMenu();
  void hideMenu();

 private Q_SLOTS:
  void returnPressedSlot();

 private:
  using ItemId = std::map<QString, uint>;
  using IdItem = std::map<uint, QString>;

  uint value_ { 0 };

  QLineEdit*              edit_   { nullptr };
  CQFlagsComboMenuButton* button_ { nullptr };
  CQFlagsComboMenu*       menu_   { nullptr }; //! menu widget

  ItemId itemId_;
  ItemId litemId_;
  IdItem idItem_;
};

//---

class CQFlagsComboMenuButton : public QPushButton {
  Q_OBJECT

 public:
  CQFlagsComboMenuButton(CQFlagsCombo *combo);

  void paintEvent(QPaintEvent *) override;

 private:
  CQFlagsCombo *combo_ { nullptr };
};

//---

class CQFlagsComboMenu : public QMenu {
  Q_OBJECT

 public:
  CQFlagsComboMenu(CQFlagsCombo *combo);

  void addItem(const QString &item, uint id);

  bool isCancelled() const { return cancelled_; }
  void setCancelled(bool b) { cancelled_ = b; }

  CQFlagsCombo *combo() const { return combo_; }

  CQFlagsComboList *menuList() const { return menuList_; }

  void popup(const QRect &rect);
  void popup(const QPoint &pos);

 public Q_SLOTS:
  void hideMenu();

  void itemSelected(QListWidgetItem *);

  void itemActivated(QListWidgetItem *);

 private:
  void calcSize();

  void updateValue();

 public Q_SLOTS:
  void showSlot();
  void hideSlot();

 private:
  CQFlagsCombo*     combo_     { nullptr };
  CQFlagsComboList* menuList_  { nullptr };
  int               height_    { 100 };
  bool              cancelled_ { false };
  uint              origValue_ { 0 };
};

//---

class CQFlagsComboList : public QListWidget {
  Q_OBJECT

 public:
  //! create widget as child of menu
  CQFlagsComboList(CQFlagsComboMenu *menu);

 ~CQFlagsComboList();

  void setValue(uint value);
  uint calcValue() const;

 public Q_SLOTS:
  void checkAll();
  void checkNone();

 private:
  //! handle raw event to forward key press/release to edit widget
  bool event(QEvent *event) override;

 private:
  CQFlagsComboMenu *menu_ { nullptr }; //! parent menu list
};

#endif
