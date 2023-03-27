#ifndef CQTabSplitSplitterTool_H
#define CQTabSplitSplitterTool_H

#include <QFrame>

class CQTabSplit;
class CQTabSplitSplitter;
class CQIconButton;
class QTimer;

class CQTabSplitSplitterTool : public QFrame {
  Q_OBJECT

 public:
  CQTabSplitSplitterTool(CQTabSplit *split);

  const CQTabSplitSplitter *splitter() const { return splitter_; }
  void setSplitter(CQTabSplitSplitter *p) { splitter_ = p; }

  bool event(QEvent *event) override;

 private:
  void startHideTimer();
  void resetHideTimer();

 public Q_SLOTS:
  void showSlot();
  void hideSlot();

 private Q_SLOTS:
  void hsplitSlot();
  void vsplitSlot();
  void tabbedSlot();

 private:
  CQTabSplit*         split_    { nullptr }; //!< parent tab split
  CQTabSplitSplitter* splitter_ { nullptr }; //!< parent tab splitter
  CQIconButton*       hbutton_  { nullptr }; //!< horizontal split button
  CQIconButton*       vbutton_  { nullptr }; //!< vertical split button
  CQIconButton*       tbutton_  { nullptr }; //!< tabbed button
  QTimer*             timer_    { nullptr }; //!< hide timer
};

#endif
