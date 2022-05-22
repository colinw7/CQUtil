#ifndef CQTabSplitSplitter_H
#define CQTabSplitSplitter_H

#include <QSplitter>

class CQTabSplit;
class CQTabSplitSplitterHandle;

/*!
 * \brief custom splitter widget for CQTabSplot
 */
class CQTabSplitSplitter : public QSplitter {
  Q_OBJECT

 public:
  using Sizes = QList<int>;

 public:
  CQTabSplitSplitter(CQTabSplit *split);

  //! get parent tab split
  CQTabSplit *split() const { return split_; }

  //! create custom handle
  QSplitterHandle *createHandle() override;

  //! get index for handle
  int handleIndex(CQTabSplitSplitterHandle *handle) const;

  //! fit specified split widget to size hint
  void autoFit(int ind);

  void setTabbed();

  void toggleSplit();

  //! fit all split widgets to size hint
  void fitAll();

  //! ensure sizes obey minimum size hint
  void fixSizes();

  //! handle resize
  void resizeEvent(QResizeEvent *) override;

  //! show/hide tool
  void showTool(const QPoint &pos);

 private:
  CQTabSplit *split_     { nullptr }; //!< parent tab split
  Sizes       lastSizes_;             //!< last splitter sizes
};

#endif
