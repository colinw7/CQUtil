#ifndef CQAlignEdit_H
#define CQAlignEdit_H

#include <QFrame>

class QStyleOptionComboBox;

class CQAlignEdit : public QFrame {
  Q_OBJECT

 public:
  CQAlignEdit(QWidget *parent=nullptr);
 ~CQAlignEdit();

  const Qt::Alignment &align() const { return align_; }
  void setAlign(const Qt::Alignment &v);

  QString toString() const;

  QSize sizeHint() const override;

  static QString toString(Qt::Alignment align);
  static Qt::Alignment fromString(const QString &str);

 signals:
  void valueChanged(Qt::Alignment);

 private:
  void paintEvent(QPaintEvent *) override;

  void mousePressEvent(QMouseEvent *) override;

  void initStyleOption(QStyleOptionComboBox &opt) const;

 private:
  Qt::Alignment align_ { Qt::AlignCenter };
};

//------

class CQAlignEditMenuWidget : public QFrame {
  Q_OBJECT

 public:
  CQAlignEditMenuWidget(CQAlignEdit *edit);

  QSize sizeHint() const override;

 private:
  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void mouseMoveEvent(QMouseEvent *) override;

  void mousePressEvent(QMouseEvent *) override;

  void drawAlignRect(QPainter *p, Qt::Alignment rectAlign, bool on);

 private:
  struct Rect {
    QRect r;
    bool  inside { false };

    Rect(const QRect &r=QRect()) :
     r(r) {
    }
  };

  typedef std::map<Qt::Alignment, Rect> AlignRect;

  CQAlignEdit *edit_ { nullptr };
  AlignRect    alignRect_;
};

#endif
