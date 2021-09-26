#ifndef CQTabSplit_H
#define CQTabSplit_H

#include <QFrame>
#include <QSplitter>
#include <QTabWidget>
#include <QPointer>

class CQGroupBox;
class QVBoxLayout;

/*!
 * Widget which can switch between tabs and splitters
 *
 * TODO: auto switch depending on min size hint
 */
class CQTabSplit : public QFrame {
  Q_OBJECT

  Q_PROPERTY(Qt::Orientation orientation  READ orientation    WRITE setOrientation )
  Q_PROPERTY(State           state        READ state          WRITE setState       )
  Q_PROPERTY(bool            grouped      READ isGrouped      WRITE setGrouped     )
  Q_PROPERTY(bool            tabsClosable READ isTabsClosable WRITE setTabsClosable)
  Q_PROPERTY(bool            autoFit      READ isAutoFit      WRITE setAutoFit     )
  Q_PROPERTY(int             currentIndex READ currentIndex   WRITE setCurrentIndex)

  Q_ENUMS(State)

 public:
  enum class State {
    HSPLIT,
    VSPLIT,
    TAB,
    AUTO
  };

  using Sizes = QList<int>;

 public:
  explicit CQTabSplit(Qt::Orientation orient, QWidget *parent=nullptr);
  explicit CQTabSplit(QWidget *parent=nullptr);

  //---

  //! get/set orientation
  Qt::Orientation orientation() const { return orient_; }
  void setOrientation(Qt::Orientation orient);

  //! get/set state
  State state() const { return state_; }
  void setState(State state);

  //! get/set grouped
  bool isGrouped() const { return grouped_; }
  void setGrouped(bool b);

  //! get/set grouped (per widget)
  bool isGrouped(QWidget *w) const;
  void setGrouped(QWidget *w, bool b);

  //! get/set tabs closable
  bool isTabsClosable() const { return tabsClosable_; }
  void setTabsClosable(bool b);

  //! get/set auto fit
  bool isAutoFit() const { return autoFit_; }
  void setAutoFit(bool b) { autoFit_ = b; }

  //---

  int currentIndex() const { return currentIndex_; }
  void setCurrentIndex(int i);

  //---

  //! get indexed widget
  QWidget *widget(int i) const;

  //! add new widget
  void addWidget(QWidget *w, const QString &name);

  //! remove widget
  void removeWidget(QWidget *w, bool deleteWidget=true);
  void removeAllWidgets();

  //! has widget
  bool hasWidget(QWidget *w) const;

  //! set widget name
  void setWidgetName(QWidget *w, const QString &name);

  //! number of widgets
  int count() const;

  //! set splitter sizes
  void setSizes(const Sizes &sizes);

  //! return size hint
  QSize sizeHint() const override;

 signals:
  //! emitted when close of tab/splitter requested (if closable)
  void widgetCloseRequested(int i);

 public slots:
  void currentIndexSlot(int);

  //! close specified tab (if closable)
  void tabCloseSlot(int i);

 private:
  //! init widget
  void init();

 private:
  using WidgetP = QPointer<QWidget>;

  struct WidgetData {
    WidgetP      w;                 //!< widget
    QString      name;              //!< widget name (frou group)
    CQGroupBox*  group { nullptr }; //!< group box (if grouped)
    QVBoxLayout* layout;            //!< layout

    WidgetData(QWidget *w, const QString &name) :
     w(w), name(name) {
    }
  };

  using Widgets = std::vector<WidgetData>;

  Qt::Orientation orient_       { Qt::Horizontal }; //!< current orientation
  State           state_        { State::HSPLIT };  //!< current state
  bool            grouped_      { false };          //!< is grouped (use group boxes)
  bool            tabsClosable_ { false };          //!< are tabs closable
  bool            autoFit_      { false };          //!< is auto fit
  int             currentIndex_ { -1 };             //!< current index
  Widgets         widgets_;                         //!< widgets
  Sizes           hsizes_;                          //!< splitter sizes (horizontal)
  Sizes           vsizes_;                          //!< splitter sizes (vertical)
  QTabWidget*     tabWidget_    { nullptr };        //!< tab widget (if tabbed)
  QSplitter*      splitter_     { nullptr };        //!< splitter widget (if split)
};

//---

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

  //! fit all split widgets to size hint
  void fitAll();

  //! ensure sizes obey minimum size hint
  void fixSizes();

  //! handle resize
  void resizeEvent(QResizeEvent *) override;

 private:
  CQTabSplit *split_     { nullptr }; //!< parent tab split
  Sizes       lastSizes_;             //!< last splitter sizes
};

//---

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

//---

/*!
 * \brief custom tab widget for CQTabSplit
 */
class CQTabSplitTabWidget : public QTabWidget {
  Q_OBJECT

 public:
  CQTabSplitTabWidget(CQTabSplit *split);

  //! get parent tab split
  CQTabSplit *split() const { return split_; }

  //! handle context menu event
  void contextMenuEvent(QContextMenuEvent *e);

 private slots:
  //! set to horizontal split mode
  void hsplitSlot();

  //! set to vertical split mode
  void vsplitSlot();

  //! set current tab from context menu
  void tabSlot();

 private:
  CQTabSplit *split_ { nullptr }; //!< parent tab split
};

#endif
