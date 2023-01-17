#ifndef CQScrollArea_H
#define CQScrollArea_H

#include <QFrame>

class QScrollBar;
class QPainter;

/*!
 * Area to contain a virtual scrollable widget.
 *
 * The actual widget is the same size as the scroll area
 * (unlike QScrollArea which uses a widget of any size).
 *
 * The widget needs to tell the scroll area the virtual widget size and the scroll bars
 * update the virtual widget offsets which are used in the widget's paint event to draw
 * the contents at the specified offset.
 *
 * This is more efficient for large widgets or widgets that can clip themselves.
 *
 * API:
 *   Use setXSize and setYSize to set virtual size
 *
 *   Use getXOffset and getYOffset when drawing into widget
 *
 *   Use setXSingleStep and setYSingleStep to set scroll step
 *
 *   Call handleWheelEvent in event override for QEvent::Wheel to scroll on wheel
 */
class CQScrollArea : public QFrame {
  Q_OBJECT

 public:
  CQScrollArea();
  CQScrollArea(QWidget *parent, QWidget *widget);
  CQScrollArea(QWidget *widget);

  virtual ~CQScrollArea() { }

  //! get/set child widget (can only be set once)
  QWidget *getWidget() const { return widget_; }
  void setWidget(QWidget *widget);

  //! show/hide horizontal/vertical scroll bar
  void showHBar(bool flag);
  void showVBar(bool flag);

  //! set corner widget
  void setCornerWidget(QWidget *w);

  //! get/set virtual widget size
  QSize getSize() const { return QSize(xSize_, ySize_); }
  void setSize(const QSize &size);

  //! get/set virtual widget size
  int getXSize() const { return xSize_; }
  void setXSize(int xSize);

  //! get/set virtual widget size
  int getYSize() const { return ySize_; }
  void setYSize(int ySize);

  //! get/set virtual widget offset
  QPoint getOffset() const { return QPoint(xOffset_, yOffset_); }
  void setOffset(const QPoint &offset);

  //! get/set virtual widget x offset
  int getXOffset() const { return xOffset_; }
  void setXOffset(int xOffset);

  //! get/set virtual widget y offset
  void setYOffset(int yOffset);
  int getYOffset() const { return yOffset_; }

  //! set x/y scrollbar single step
  void setXSingleStep(int x);
  void setYSingleStep(int y);

  //! handle scroll directions
  void scrollUp   (bool page=false);
  void scrollDown (bool page=false);
  void scrollLeft (bool page=false);
  void scrollRight(bool page=false);

  //! ensure pixel position widget
  void ensureVisible(int x, int y, int xmargin=50, int ymargin=50);

  //! update scrollbars (called when virtual widget changes)
  void updateScrollbars();

  //! update contents
  virtual void updateContents() { }

  //! handle wheel event
  void handleWheelEvent(QWheelEvent *e);

 private Q_SLOTS:
  void hscrollSlot(int value);
  void vscrollSlot(int value);

 signals:
  //! called when area scrolled
  void updateArea();

 private:
  void init();

 private:
  QWidget*    widget_  { nullptr };
  QScrollBar* hbar_    { nullptr };
  QScrollBar* vbar_    { nullptr };
  int         xSize_   { -1 };
  int         ySize_   { -1 };
  int         xOffset_ { 0 };
  int         yOffset_ { 0 };
};

#endif
