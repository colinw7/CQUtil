#ifndef CQAutoHide_H
#define CQAutoHide_H

#include <QWidget>
#include <QSet>

class CQAutoHide : public QObject {
  Q_OBJECT

 public:
  CQAutoHide(QWidget *w=0);

  void setWidget(QWidget *w);

  bool active() const { return active_; }

  void setActive(bool active);

  void addWidget(QWidget *w);

  void addRect(const QRect &r);

  void clear();

 signals:
  void autoHide();

 private:
  bool eventFilter(QObject *obj, QEvent *event);

  bool processEvent(QObject *obj, QEvent *event);

  bool isPreviewValid() const;

  bool isPreviewValid(QWidget *w, const QPoint &gp) const;

  bool checkPreviewRects(const QPoint &gp, int tol=0) const;

  bool isPreviewWidget(QWidget *w) const;

  bool isModalDialogWidget(QWidget *w) const;

 private:
  typedef QSet<QWidget *> Widgets;
  typedef QList<QRect>    Rects;

  QWidget *w_ { nullptr };
  bool     active_ { false };
  bool     hideOnRelease_ { false };
  bool     menuActive_ { false };
  QWidget *pressWidget_ { nullptr };
  Widgets  widgets_;
  Rects    rects_;
};

#endif
