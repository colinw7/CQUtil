#ifndef CQAlphaButton_H
#define CQAlphaButton_H

#include <QWidget>

class CQAlphaButton : public QWidget {
  Q_OBJECT

 public:
  CQAlphaButton(QWidget *parent=nullptr);

  double getAlpha() const;

  void setAlpha(double alpha);

  void setColor(QColor color);

 private:
  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

  void posToAlpha(int pos);
  int alphaToPos();

 signals:
  void valueChanged();

 private:
  double alpha_ { 1.0 };
  QColor fg_, bg_;
};

#endif
