#ifndef CQProgressLabel_H
#define CQProgressLabel_H

#include <QFrame>

class CQProgressLabel : public QFrame {
  Q_OBJECT

  Q_PROPERTY(int progress READ progress WRITE setProgress)

 public:
  CQProgressLabel(QWidget *parent=nullptr);

  int progress() const { return progress_; }
  void setProgress(int i) { progress_ = std::min(std::max(i, 0), 100); update(); }

  const QColor &progressColor() const { return progressColor_; }
  void setProgressColor(const QColor &c) { progressColor_ = c; }

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  int    progress_      { 0 };
  QColor progressColor_ { 40, 180, 40 };
};

#endif
