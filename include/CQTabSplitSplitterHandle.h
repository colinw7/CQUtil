#ifndef CQTabSplitSplitterHandle_H
#define CQTabSplitSplitterHandle_H

#include <QSplitter>

class CQTabSplitSplitter;

/*!
 * \brief custom splitter handler for CQTabSplitSplitter
 */
class CQTabSplitSplitterHandle : public QSplitterHandle {
  Q_OBJECT

  Q_PROPERTY(int barSize READ barSize WRITE setBarSize)

 public:
  CQTabSplitSplitterHandle(Qt::Orientation orient, CQTabSplitSplitter *splitter);

  CQTabSplitSplitter *splitter() const { return splitter_; }

  //! get/set bar size
  int barSize() const { return barSize_; }
  void setBarSize(int i) { barSize_ = i; }

  //! handle context menu event
  void contextMenuEvent(QContextMenuEvent *e) override;

  //! handle double click menu event (auto fit)
  void mouseDoubleClickEvent(QMouseEvent *) override;

  //! paint handle
  void paintEvent(QPaintEvent *) override;

  //! handle generic event (for hover)
  bool event(QEvent *event) override;

  //! return size hint
  QSize sizeHint() const override;

 private slots:
  void tabSlot();
  void splitSlot();
  void fitAllSlot();

 private:
  CQTabSplitSplitter *splitter_ { nullptr }; //!< parent splitter
  int                 barSize_  { 8 };       //!< bar size
  bool                hover_    { false };   //!< is hover
};

#endif
