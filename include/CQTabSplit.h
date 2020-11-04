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
  CQTabSplit(Qt::Orientation orient, QWidget *parent=nullptr);
  CQTabSplit(QWidget *parent=nullptr);

  //! get/set orientation
  Qt::Orientation orientation() const { return orient_; }
  void setOrientation(Qt::Orientation orient);

  //! get/set state
  State state() const { return state_; }
  void setState(State state);

  //! get/set grouped
  bool isGrouped() const { return grouped_; }
  void setGrouped(bool b);

  //! get/set tabs closable
  bool isTabsClosable() const { return tabsClosable_; }
  void setTabsClosable(bool b);

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
  void widgetCloseRequested(int i);

 public slots:
  void tabCloseSlot(int i);

 private:
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
  Widgets         widgets_;                         //!< widgets
  Sizes           hsizes_;                          //!< splitter sizes (horizontal)
  Sizes           vsizes_;                          //!< splitter sizes (vertical)
  QTabWidget*     tabWidget_    { nullptr };        //!< tab widget (if tabbed)
  QSplitter*      splitter_     { nullptr };        //!< splitter widget (if split)
};

//---

/*!
 * \brief custom splitter widget for CQTabSplot
 */
class CQTabSplitSplitter : public QSplitter {
  Q_OBJECT

 public:
  CQTabSplitSplitter(CQTabSplit *split);

  CQTabSplit *split() const { return split_; }

  QSplitterHandle *createHandle() override;

 private:
  CQTabSplit *split_ { nullptr };
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

  int barSize() const { return barSize_; }
  void setBarSize(int i) { barSize_ = i; }

  void contextMenuEvent(QContextMenuEvent *e) override;

  void paintEvent(QPaintEvent *) override;

  bool event(QEvent *event) override;

  QSize sizeHint() const override;

 private slots:
  void tabSlot();
  void splitSlot();

 private:
  CQTabSplitSplitter *splitter_ { nullptr };
  int                 barSize_  { 8 };
  bool                hover_    { false };
};

//---

/*!
 * \brief custom tab widget for CQTabSplit
 */
class CQTabSplitTabWidget : public QTabWidget {
  Q_OBJECT

 public:
  CQTabSplitTabWidget(CQTabSplit *split);

  CQTabSplit *split() const { return split_; }

  void contextMenuEvent(QContextMenuEvent *e);

 private slots:
  void hsplitSlot();
  void vsplitSlot();

 private:
  CQTabSplit *split_ { nullptr };
};

#endif
