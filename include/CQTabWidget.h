#ifndef CQTAB_WIDGET_H
#define CQTAB_WIDGET_H

#define CQTAB_WIDGET_MOVABLE 1

#include <QTabWidget>

class CQFloatEdit;
class CQTabWidgetTabBar;

class QMenu;

//! extension of tab widget to add move buttons
class CQTabWidget : public QTabWidget {
  Q_OBJECT

  Q_PROPERTY(bool showMoveButtons READ getShowMoveButtons WRITE setShowMoveButtons)

 public:
  CQTabWidget(QWidget *parent=nullptr);
 ~CQTabWidget();

  void addCreateButton();

  //! get/set show move buttons
  bool getShowMoveButtons() const { return moveButtons_; }
  void setShowMoveButtons(bool show);

  void contextMenuEvent(QContextMenuEvent *e) override;

  QMenu *createTabMenu() const;

 public Q_SLOTS:
  void moveTabLeft ();
  void moveTabRight();

  void moveTab(int fromIndex, int toIndex);

  void tabSlot();

 Q_SIGNALS:
  void tabChanged(int ind);

  void createTab();

  void swapTabs(int ind1, int ind2);

 private:
  CQTabWidgetTabBar* tabBar_        { nullptr };
  bool               moveButtons_   { false };
  QWidget*           moveTabWidget_ { nullptr };
};

#endif
