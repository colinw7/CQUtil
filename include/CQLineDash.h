#ifndef CQLINE_DASH_H
#define CQLINE_DASH_H

#include <QFrame>
#include <QAction>
#include <CLineDash.h>
#include <map>

class QMenu;
class QLineEdit;
class QToolButton;
class CQIconCombo;
class CQLineDashAction;

class CQLineDash : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool editable READ isEditable WRITE setEditable)

 public:
  CQLineDash(QWidget *parent=0);

  bool isEditable() const { return editable_; }
  void setEditable(bool b);

  void setLineDash(const CLineDash &dash);
  const CLineDash &getLineDash() const { return dash_; }

  void addDashOption(const std::string &id, const CLineDash &dash);

  static QIcon dashIcon(const CLineDash &dash);

 private:
  void updateState();

  bool event(QEvent *e) override;

  bool eventFilter(QObject *o , QEvent *e) override;

 private slots:
  void dashChangedSlot();
  void menuItemActivated(QAction *);
  void comboItemChanged();

 signals:
  void valueChanged(const CLineDash &dash);

 private:
  using Actions = std::map<std::string, CQLineDashAction *>;

  bool         editable_ { false };
  CLineDash    dash_;
  QLineEdit   *edit_     { nullptr };
  QToolButton *button_   { nullptr };
  QMenu       *menu_     { nullptr };
  CQIconCombo *combo_    { nullptr };
  Actions      actions_;
};

//---

class CQLineDashAction : public QAction {
 public:
  CQLineDashAction(CQLineDash *parent, const std::string &id,
                   const CLineDash &dash, const QIcon &icon);

 private:
  void init();

 private:
  CQLineDash  *parent_ { nullptr };
  std::string  id_;
  CLineDash    dash_;
};

#endif
