#ifndef CQTAB_WIDGET_H
#define CQTAB_WIDGET_H

#define CQTAB_WIDGET_MOVABLE 1

#include <QTabWidget>
#include <QTabBar>

class CQFloatEdit;
class CQTabBar;

class CQTabWidget : public QTabWidget {
  Q_OBJECT

  Q_PROPERTY(bool showMoveButtons READ getShowMoveButtons WRITE setShowMoveButtons)

 public:
  CQTabWidget(QWidget *parent = 0);

  void addCreateButton();

  bool getShowMoveButtons() const { return moveButtons_; }

  void setShowMoveButtons(bool show);

 public slots:
  void moveTabLeft ();
  void moveTabRight();

  void moveTab(int fromIndex, int toIndex);

 signals:
  void tabChanged(int ind);

  void createTab();

  void swapTabs(int ind1, int ind2);

 private:
  CQTabBar *tabBar_ { nullptr };
  bool      moveButtons_ { false };
  QWidget  *moveTabWidget_ { nullptr };
};

//------

class CQTabBar : public QTabBar {
  Q_OBJECT

 public:
  CQTabBar(QWidget *parent = 0);

  void stopEdit();

 protected:
  void mouseDoubleClickEvent(QMouseEvent *event);

#ifndef CQTAB_WIDGET_MOVABLE
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent (QMouseEvent *event);

  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent     (QDropEvent *event);
#endif

  void paintEvent(QPaintEvent *event);

 signals:
  void tabChanged(int ind);

  void tabMoveRequested(int fromIndex, int toIndex);

 private slots:
  void tabEditFinished(const QString &text);

 private:
  CQFloatEdit *edit_ { nullptr };
  int          ind_ { 0 };
  QPoint       press_pos_;
};

#endif
