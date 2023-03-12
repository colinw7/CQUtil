#ifndef CQSwitchLineEdit_H
#define CQSwitchLineEdit_H

#include <QFrame>
#include <QPointer>

class QLineEdit;

class CQSwitchLineEdit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool    showAltEdit READ isShowAltEdit WRITE setShowAltEdit)
  Q_PROPERTY(QString text        READ text          WRITE setText)

 public:
  static bool isSwitchKey(QKeyEvent *ke);

 public:
  CQSwitchLineEdit(QWidget *parent=nullptr);

  QLineEdit *edit() const { return edit_; }

  QString text() const;
  void setText(const QString &text);

  void setAltEdit(QWidget *edit);

  void setShowAltEdit(bool b);
  bool isShowAltEdit() const { return isAlt_; }

  QSize sizeHint() const override;

 Q_SIGNALS:
  void editSwitched(bool);
  void editingFinished();

 protected:
  virtual void updatePlacement();

 private:
  bool event(QEvent *e) override;

  bool eventFilter(QObject *o , QEvent *e) override;

 private:
  using WidgetP = QPointer<QWidget>;

  QLineEdit *edit_ { nullptr };
  WidgetP    altEdit_;
  bool       isAlt_ { false };
};

#endif
