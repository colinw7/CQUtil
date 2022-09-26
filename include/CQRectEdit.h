#ifndef CQRectEdit_H
#define CQRectEdit_H

#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QHBoxLayout>

class CQRectEdit : public QFrame {
  Q_OBJECT

 public:
  CQRectEdit(QWidget *parent, const QRectF &value=QRectF(0, 0, 1, 1));
  CQRectEdit(const QRectF &value=QRectF(0, 0, 1, 1));

  virtual ~CQRectEdit() { }

  void setValue(const QRectF &rect);
  const QRectF &getValue() const;

 private:
  void init(const QRectF &value);

 private slots:
  void editingFinishedI();

 signals:
  void valueChanged();

 private:
  void updateRange();

  void rectToWidget();
  bool widgetToPoint();

 private:
  QRectF       rect_           { 0, 0, 1, 1 };
  QLineEdit*   edit_           { nullptr };
  QHBoxLayout* layout_         { nullptr };
  mutable bool disableSignals_ { false };
};

#endif
