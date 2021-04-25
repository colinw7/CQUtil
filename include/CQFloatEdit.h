#ifndef CQFLOAT_EDIT_H
#define CQFLOAT_EDIT_H

#include <QLineEdit>

class CQAutoHide;

class CQFloatEdit : public QLineEdit {
  Q_OBJECT

 public:
  CQFloatEdit(QWidget *parent=0);

 ~CQFloatEdit();

  void display(const QRect &rect, const QString &text);

 private:
  bool event(QEvent *e) override;

 private slots:
  void acceptSlot();

 signals:
  void valueChanged(const QString &text);

 private:
  bool        valid_ { false };
  QString     saveText_;
  CQAutoHide *hider_ { nullptr };
};

#endif
