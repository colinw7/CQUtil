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
  CQTabWidget(QWidget *parent=nullptr);
 ~CQTabWidget();

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

#endif
