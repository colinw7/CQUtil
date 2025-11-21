#ifndef CQTreeView_H
#define CQTreeView_H

#include <QTreeView>

class CQHeaderView;

class CQTreeView : public QTreeView {
  Q_OBJECT

 public:
  CQTreeView(QWidget *parent=nullptr);

  virtual ~CQTreeView();

  virtual void addMenuActions(QMenu *menu);

 private Q_SLOTS:
  void customContextMenuSlot(const QPoint &pos);

  void expandAll  (const QModelIndex &ind=QModelIndex());
  void collapseAll(const QModelIndex &ind=QModelIndex());

  void setRootSlot();

 private:
  CQHeaderView *header_ { nullptr };
  QModelIndex   menuIndex_;
};

#endif
