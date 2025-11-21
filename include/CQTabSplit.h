#ifndef CQTabSplit_H
#define CQTabSplit_H

#include <CQIconButton.h>
#include <CQGroupBox.h>

#include <QFrame>
#include <QSplitter>
#include <QTabWidget>
#include <QTabBar>
#include <QPointer>

class CQTabSplitSplitterTool;
class CQTabSplitTabBar;
class CQTabSplitGroup;

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
  Q_PROPERTY(bool            groupBold    READ isGroupBold    WRITE setGroupBold   )
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

  //! get/set group bolx
  bool isGroupBold() const { return groupBold_; }
  void setGroupBold(bool b);

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

  //! get/set current index
  int currentIndex() const { return currentIndex_; }
  void setCurrentIndex(int i);

  QWidget *currentWidget() const;

  //---

  //! get indexed widget
  QWidget *widget(int i) const;

  //! add new widget
  void addWidget(QWidget *w, const QString &name);

  //! remove widget
  void removeWidget(QWidget *w, bool deleteWidget=true);
  void removeWidget(int ind, bool deleteWidget=true);
  void removeAllWidgets();

  //! has widget
  bool hasWidget(QWidget *w) const;

  //! set widget name
  void setWidgetName(QWidget *w, const QString &name);

  //! number of widgets
  int count() const;

  //! set splitter sizes
  void setSizes(const Sizes &sizes);

  //---

  CQTabSplitSplitterTool *getSplitterTool();

  //---

  //! return size hint
  QSize sizeHint() const override;

 Q_SIGNALS:
  //! emitted when close of tab/splitter requested (if closable)
  void widgetCloseRequested(int i);

  //! emitted when current index changes
  void currentIndexChanged(int i);

 public Q_SLOTS:
  void currentIndexSlot(int);

  //! close specified tab (if closable)
  void tabCloseSlot(int i);

 private:
  //! init widget
  void init();

  void updateGroupColored();

 private:
  using WidgetP = QPointer<QWidget>;

  struct WidgetData {
    WidgetP          w;                  //!< widget
    QString          name;               //!< widget name (frou group)
    CQTabSplitGroup* group  { nullptr }; //!< group box (if grouped)
    QVBoxLayout*     layout { nullptr }; //!< layout

    WidgetData() { }

    WidgetData(QWidget *w, const QString &name) :
     w(w), name(name) {
    }
  };

  using Widgets = std::vector<WidgetData>;

  Qt::Orientation orient_       { Qt::Horizontal }; //!< current orientation
  State           state_        { State::HSPLIT };  //!< current state
  bool            grouped_      { false };          //!< is grouped (use group boxes)
  bool            groupBold_    { false };          //!< is group box bold
  bool            tabsClosable_ { false };          //!< are tabs closable
  bool            autoFit_      { false };          //!< is auto fit
  int             currentIndex_ { -1 };             //!< current index
  Widgets         widgets_;                         //!< widgets
  Sizes           hsizes_;                          //!< splitter sizes (horizontal)
  Sizes           vsizes_;                          //!< splitter sizes (vertical)
  QTabWidget*     tabWidget_    { nullptr };        //!< tab widget (if tabbed)
  QSplitter*      splitter_     { nullptr };        //!< splitter widget (if split)

  CQTabSplitSplitterTool *splitterTool_ { nullptr };
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
  void contextMenuEvent(QContextMenuEvent *e) override;

  bool event(QEvent *event) override;

 private:
  void showTool(const QPoint &pos);

 private Q_SLOTS:
  //! set to horizontal split mode
  void hsplitSlot();

  //! set to vertical split mode
  void vsplitSlot();

  //! set current tab from context menu
  void tabSlot();

 private:
  CQTabSplit*       split_  { nullptr }; //!< parent tab split
  CQTabSplitTabBar* tabBar_ { nullptr }; //!< tab bar
};

//---

/*!
 * \brief custom tab tab for CQTabSplit
 */
class CQTabSplitTabBar : public QTabBar {
  Q_OBJECT

 public:
  CQTabSplitTabBar(CQTabSplitTabWidget *tabWidget);

 private:
  CQTabSplitTabWidget* tabWidget_ { nullptr };
};

//---

class CQTabSplitTabMenu : public CQIconButton {
  Q_OBJECT

 public:
  CQTabSplitTabMenu(CQTabSplitTabWidget *tabWidget);

 private:
  CQTabSplitTabWidget* tabWidget_ { nullptr };
};

//---

class CQTabSplitGroup : public CQGroupBox {
  Q_OBJECT

 public:
  CQTabSplitGroup(CQTabSplit *tab, const QString &name, int ind);

 private Q_SLOTS:
  void pressSlot();

 private:
  CQTabSplit* tab_ { nullptr };
  int         ind_ { 0 };
};

#endif
