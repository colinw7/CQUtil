#ifndef CQHeaderView_H
#define CQHeaderView_H

#include <QHeaderView>

class CQHeaderViewWidgetFactory;
class QTableView;
class QTreeView;
class QSortFilterProxyModel;

class CQHeaderView : public QHeaderView {
  Q_OBJECT

  Q_PROPERTY(bool initFit READ isInitFit WRITE setInitFit)

 public:
  CQHeaderView(QWidget *parent=nullptr);

  bool isInitFit() const { return initFit_; }
  void setInitFit(bool b) { initFit_ = b; }

  void setWidgetFactory(CQHeaderViewWidgetFactory *factory);

  void setSectionsMovable(bool movable);
  void setSectionsClickable(bool clickable);

  QWidget *widget(int i) const;

  void initWidgets();

  void fixWidgetPositions();

  void update();

 private:
  void doInitFit();

  void contextMenuEvent(QContextMenuEvent *event) override;

  void showEvent(QShowEvent *e) override;

  void resizeEvent(QResizeEvent *e) override;

 private slots:
  void handleSectionResized(int section, int oldSize, int newSize);

  void handleSectionMoved(int logical, int oldVisualIndex, int newVisualIndex);

  void handleSectionClicked(int logical);

  void fitColumnSlot();
  void fitAllSlot();

  void stretchLastSlot(bool b);
  void sortIndicatorSlot(bool b);

  void resetSortSlot();

 private:
  typedef std::map<int,int> ColumnWidths;

  void calcTableWidths(QTableView *table, ColumnWidths &columnWidths);

  void calcTreeWidths(QTreeView *tree, const QModelIndex &ind,
                      int depth, ColumnWidths &columnWidths);

  QSortFilterProxyModel *proxyModel();

 private:
  typedef QList<QWidget *> Widgets;

  CQHeaderViewWidgetFactory *factory_     { nullptr };
  Widgets                    widgets_;
  int                        menuSection_ { -1 };
  int                        sortRole_    { -1 };
  bool                       shown_       { false };
  bool                       resized_     { false };
  bool                       initFit_     { false };
};

//---

class CQHeaderViewWidgetFactory {
 public:
  CQHeaderViewWidgetFactory() { }

  virtual ~CQHeaderViewWidgetFactory() { }

  virtual QWidget *createWidget(int i) = 0;
};

#endif
