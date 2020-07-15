#ifndef CQDragLabel_H
#define CQDragLabel_H

#include <QLabel>

class CQDragLabel : public QLabel {
  Q_OBJECT

  Q_PROPERTY(bool   dragable READ isDragable WRITE setDragable)
  Q_PROPERTY(int    dragSize READ dragSize   WRITE setDragSize)
  Q_PROPERTY(double dragMin  READ dragMin    WRITE setDragMin )
  Q_PROPERTY(double dragMax  READ dragMax    WRITE setDragMax )

 public:
  CQDragLabel(QWidget *parent=nullptr);
  CQDragLabel(const QString &label, QWidget *parent=nullptr);

  bool isDragable() const { return dragable_; }
  void setDragable(bool b) { dragable_ = b; }

  int dragSize() const { return dragSize_; }
  void setDragSize(int i) { dragSize_ = i; }

  double dragMin() const { return dragMin_; }
  void setDragMin(double r) { dragMin_ = r; }

  double dragMax() const { return dragMax_; }
  void setDragMax(double r) { dragMax_ = r; }

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

 private:
  void init();

 signals:
  void dragValueChanged(double v);

 private:
  bool   pressed_  { false };
  QPoint pos_;
  bool   dragable_ { true };
  int    dragSize_ { 256 };
  double dragMin_  { -1.0 };
  double dragMax_  { 1.0 };
  int    lastDx_   { 0 };
};

#endif
