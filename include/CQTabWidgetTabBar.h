#ifndef CQTabWidgetTabBar_H
#define CQTabWidgetTabBar_H

#define CQTAB_WIDGET_MOVABLE 1

#include <QTabBar>

class CQTabWidget;
class CQFloatEdit;

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
