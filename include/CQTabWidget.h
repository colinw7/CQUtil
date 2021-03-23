#ifndef CQTAB_WIDGET_H
#define CQTAB_WIDGET_H

#define CQTAB_WIDGET_MOVABLE 1

#include <QTabWidget>
#include <QTabBar>

class CQFloatEdit;
class CQTabWidgetTabBar;

class QMenu;

class CQTabWidget : public QTabWidget {
  Q_OBJECT

  Q_PROPERTY(bool showMoveButtons READ getShowMoveButtons WRITE setShowMoveButtons)

 public:
  CQTabWidget(QWidget *parent = 0);

  void addCreateButton();

  bool getShowMoveButtons() const { return moveButtons_; }

  void setShowMoveButtons(bool show);

  void contextMenuEvent(QContextMenuEvent *e) override;

  QMenu *createTabMenu() const;

 public slots:
  void moveTabLeft ();
  void moveTabRight();

  void moveTab(int fromIndex, int toIndex);

  void tabSlot();

 signals:
  void tabChanged(int ind);

  void createTab();

  void swapTabs(int ind1, int ind2);

 private:
  CQTabWidgetTabBar* tabBar_        { nullptr };
  bool               moveButtons_   { false };
  QWidget*           moveTabWidget_ { nullptr };
};

//------

class CQTabWidgetTabBar : public QTabBar {
  Q_OBJECT

 public:
  CQTabWidgetTabBar(CQTabWidget *tabWidget);

  void stopEdit();

 protected:
  void mouseDoubleClickEvent(QMouseEvent *event) override;

#ifndef CQTAB_WIDGET_MOVABLE
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent (QMouseEvent *event) override;

  void dragEnterEvent(QDragEnterEvent *event) override;
  void dropEvent     (QDropEvent *event) override;
#endif

  void paintEvent(QPaintEvent *event) override;

  void contextMenuEvent(QContextMenuEvent *e) override;

 signals:
  void tabChanged(int ind);

  void tabMoveRequested(int fromIndex, int toIndex);

 private slots:
  void tabEditFinished(const QString &text);

 private:
  CQTabWidget* tabWidget_ { nullptr };
  CQFloatEdit *edit_      { nullptr };
  int          ind_       { -1 };
  QPoint       pressPos_;
};

#endif
