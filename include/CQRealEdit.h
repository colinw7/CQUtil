#ifndef CQREAL_EDIT_H
#define CQREAL_EDIT_H

#include <QLineEdit>
#include <QAction>

class QDoubleValidator;
class QContextMenuEvent;
class QMenu;

class CQRealEdit : public QLineEdit {
  Q_OBJECT

 public:
  CQRealEdit(QWidget *parent, double value=0.0);
  CQRealEdit(double value=0.0);

  virtual ~CQRealEdit() { }

  double getValue() const;
  void setValue(double value);

  void setRange(double lower, double upper, int decimals=0);

  void addMenuValue(double value);

 private:
  void init(const QString &name, double value);

  void contextMenuEvent(QContextMenuEvent *) override;

 private Q_SLOTS:
  void editingFinishedI();
  void menuItemActivated(QAction *);

 Q_SIGNALS:
  void valueChanged(double);

 private:
  void valueToWidget();
  bool widgetToValue();

 private:
  double            value_     { 0.0 };
  QDoubleValidator *validator_ { nullptr };
  QMenu            *menu_      { nullptr };
};

//---

class CQRealEditAction : public QAction {
 public:
  CQRealEditAction(double value);

  double getValue() const { return value_; }

 private:
  double value_ { 0.0 };
};

#endif
