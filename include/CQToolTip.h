#ifndef CQToolTip_H
#define CQToolTip_H

#include <QWidget>
#include <QPointer>
#include <QMap>

class QVBoxLayout;

// tooltip interface
class CQToolTipIFace {
 public:
  CQToolTipIFace() { }

  virtual ~CQToolTipIFace() { }

  // can display tip at point
  virtual bool canTip(const QPoint &) const { return true; }

  // get tip widget for point (return widget to show)
  virtual QWidget *showWidget(const QPoint &pos) = 0;

  // hide tip widget
  virtual void hideWidget() { }

  // update tip widget for new point (return false if can't display at new point)
  virtual bool updateWidget(const QPoint &) { return true; }

  // update tip on mouse move (with new point)
  virtual bool trackMouse() const { return false; }

  // seconds before hide (-1 means default)
  virtual double hideSecs() const { return -1; }

  // tip widget margin (-1 means default)
  virtual int margin() const { return -1; }

  // tip widget opacity (-1 means default)
  virtual double opacity() const { return -1; }

  // does tip allow transparency
  virtual bool isTransparent() const { return false; }

  // alignment of tip widget
  virtual Qt::Alignment alignment() const { return Qt::AlignLeft | Qt::AlignTop; }

  // allow tip outside parent
  virtual bool outside() const { return false; }

  // grab key press events
  virtual bool grabKey() const { return false; }

  // does key press hide tip (return false for allowed keys)
  virtual bool isHideKey(int, Qt::KeyboardModifiers) const { return true; }

  // handle allowed key press in tip
  virtual bool keyPress(int, Qt::KeyboardModifiers) { return true; }

  // widget size hint
  virtual QSize sizeHint() const { return QSize(); }
};

//---

// interface for standard widget
class CQToolTipWidgetIFace : public CQToolTipIFace {
 public:
  CQToolTipWidgetIFace(QWidget *w) :
    w_(w) {
  }

  virtual QWidget *showWidget(const QPoint &) { return w_; }

 private:
  using QWidgetP = QPointer<QWidget>;

  QWidgetP w_;
};

//---

#define CQToolTipInst CQToolTip::instance()

// tooltip singleton
class CQToolTip : public QWidget {
  Q_OBJECT

 public:
  // get singleton
  static CQToolTipIFace *getToolTip(QWidget *w);

  // set tooltip widget for parent widget
  static void setToolTip(QWidget *parent, QWidget *tooltip);
  // set tooltip interface for parent widget
  static void setToolTip(QWidget *parent, CQToolTipIFace *tooltip);

  // unset tooltip for parent widget
  static void unsetToolTip(QWidget *parent);

  // free singleton
  static void release();

 protected:
  // get instance
  static CQToolTip *instance();

  // protected constructor/destructor
  CQToolTip();
 ~CQToolTip();

  // show tooltip for interface at specified position
  bool show(const QPoint &pos, CQToolTipIFace *tooltip, QWidget *parent);

 private:
  // show tip at position
  void showAtPos(const QPoint &pos);

  // handle enter event
  void enterEvent(QEvent *e) override;

  // handle paint event
  void paintEvent(QPaintEvent *) override;

  // handle generic event
  bool eventFilter(QObject *o, QEvent *e) override;

  // handle timer (hide) event
  void timerEvent(QTimerEvent *event) override;

  // hide after timeout
  void hideLater();

  // start/restart auto hide timer
  void startHideTimer();

  // stop hide timer
  void stopTimer();

  // show widget tip at position
  bool showTip  (QWidget *w, const QPoint &gpos);
  // update widget tip at position
  bool updateTip(QWidget *w, const QPoint &gpos);

  // update tip size
  void updateSize();

  // update tip opacity
  void updateOpacity(CQToolTipIFace *tooltip);

  // size hint
  QSize sizeHint() const override;

  // calc widget margin
  int calcMargin() const;

  // calc widget size
  QSize calcSize() const;

  // get desktop rect
  QRect desktopRect(const QPoint &pos) const;

 private slots:
  // handle hide timer signal
  void hideSlot();

 private:
  using QWidgetP  = QPointer<QWidget>;
  using WidgetMap = QMap<QWidgetP,CQToolTipIFace *>;

  WidgetMap tooltips_;          //!< widget tooltips
  QWidgetP  tooltip_;           //!< current tooltip widget
  QWidgetP  parent_;            //!< current parent widget
  double    hideSecs_  { 3.0 }; //!< default hide timeout
  int       hideTimer_ { 0 };   //!< hide timer
  int       margin_    { 1 };   //!< default margin
  double    opacity_   { 0.8 }; //!< default opacity
  QPoint    showPos_;           //!< default position
};

#endif
