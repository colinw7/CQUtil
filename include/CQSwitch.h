#ifndef CQSwitch_H
#define CQSwitch_H

#include <QAbstractButton>

class CQSwitch : public QAbstractButton {
  Q_OBJECT

  Q_PROPERTY(QString onLabel     READ onLabel       WRITE setOnLabel    )
  Q_PROPERTY(QString offLabel    READ offLabel      WRITE setOffLabel   )
  Q_PROPERTY(int     margin      READ margin        WRITE setMargin     )
  Q_PROPERTY(bool    highlightOn READ isHighlightOn WRITE setHighlightOn)

 public:
  CQSwitch(QWidget *parent=nullptr);
  CQSwitch(QWidget *parent, const QString &onLabel, const QString &offLabel);
  CQSwitch(const QString &onLabel, const QString &offLabel, QWidget *parent=nullptr);

  const QString &onLabel() const { return onLabel_; }
  void setOnLabel(const QString &v) { onLabel_ = v; }

  const QString &offLabel() const { return offLabel_; }
  void setOffLabel(const QString &v) { offLabel_ = v; }

  int margin() const { return margin_; }
  void setMargin(int i) { margin_ = i; }

  bool isHighlightOn() const { return highlightOn_; }
  void setHighlightOn(bool b) { highlightOn_ = b; }

  QSize sizeHint() const;

 public slots:
  void click();
  void toggle();

 signals:
  void released();

  void toggled(bool b);

 private:
  void init();

  void mousePressEvent  (QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);

  void paintEvent(QPaintEvent *);

 private:
  QString onLabel_     { "On"  };
  QString offLabel_    { "Off" };
  int     margin_      { 2 };
  bool    highlightOn_ { true };
};

#endif
