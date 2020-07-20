#include <CQWidgetFactory.h>

#include <QCalendarWidget>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDial>
#include <QDialog>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QFontComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLCDNumber>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
//#include <QPrintDialog>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSlider>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QToolBox>
#include <QToolButton>
#include <QTreeWidget>
#include <QUndoView>
//#include <QWebView>
#include <QWizard>

#define addWidgetFactoryT(N) \
CQWidgetFactoryMgrInst->addWidgetFactory(#N, new CQWidgetFactoryT<N>())

//---

CQWidgetFactoryMgr *
CQWidgetFactoryMgr::
instance()
{
  static CQWidgetFactoryMgr *instance;

  if (! instance) {
    instance = new CQWidgetFactoryMgr;

    instance->init();
  }

  return instance;
}

CQWidgetFactoryMgr::
CQWidgetFactoryMgr()
{
}

CQWidgetFactoryMgr::
~CQWidgetFactoryMgr()
{
}

//---

void
CQWidgetFactoryMgr::
init()
{
  addWidgetFactoryT(QCalendarWidget);
  addWidgetFactoryT(QCheckBox);
  addWidgetFactoryT(QColorDialog);
  addWidgetFactoryT(QComboBox);
  addWidgetFactoryT(QDial);
  addWidgetFactoryT(QDialog);
  addWidgetFactoryT(QDateEdit);
  addWidgetFactoryT(QDateTimeEdit);
  addWidgetFactoryT(QDockWidget);
  addWidgetFactoryT(QDoubleSpinBox);
  addWidgetFactoryT(QFileDialog);
  addWidgetFactoryT(QFontComboBox);
  addWidgetFactoryT(QFontDialog);
  addWidgetFactoryT(QFrame);
  addWidgetFactoryT(QGroupBox);
  addWidgetFactoryT(QLabel);
  addWidgetFactoryT(QLCDNumber);
  addWidgetFactoryT(QLineEdit);
  addWidgetFactoryT(QListView);
  addWidgetFactoryT(QListWidget);
  addWidgetFactoryT(QMainWindow);
  addWidgetFactoryT(QMdiArea);
  addWidgetFactoryT(QMdiSubWindow);
  addWidgetFactoryT(QMenu);
  addWidgetFactoryT(QMenuBar);
  addWidgetFactoryT(QMessageBox);
  addWidgetFactoryT(QPlainTextEdit);
//addWidgetFactoryT(QPrintDialog);
  addWidgetFactoryT(QProgressBar);
  addWidgetFactoryT(QProgressDialog);
  addWidgetFactoryT(QPushButton);
  addWidgetFactoryT(QRadioButton);
  addWidgetFactoryT(QScrollArea);
  addWidgetFactoryT(QScrollBar);
  addWidgetFactoryT(QSlider);
  addWidgetFactoryT(QSpinBox);
  addWidgetFactoryT(QSplitter);
  addWidgetFactoryT(QStackedWidget);
  addWidgetFactoryT(QStatusBar);
  addWidgetFactoryT(QTabBar);
  addWidgetFactoryT(QTabWidget);
  addWidgetFactoryT(QTableView);
  addWidgetFactoryT(QTableWidget);
  addWidgetFactoryT(QTextEdit);
  addWidgetFactoryT(QToolBar);
  addWidgetFactoryT(QToolBox);
  addWidgetFactoryT(QToolButton);
  addWidgetFactoryT(QTreeView);
  addWidgetFactoryT(QTreeWidget);
  addWidgetFactoryT(QUndoView);
  addWidgetFactoryT(QWidget);
//addWidgetFactoryT(QWebView);
  addWidgetFactoryT(QWizard);
  addWidgetFactoryT(QWizardPage);
}

//---

bool
CQWidgetFactoryMgr::
isWidgetFactory(const QString &name) const
{
  return (widgetFactories_.find(name) != widgetFactories_.end());
}

void
CQWidgetFactoryMgr::
addWidgetFactory(const QString &name, CQWidgetFactory *factory)
{
  if (isWidgetFactory(name))
    removeWidgetFactory(name);

  widgetFactories_[name] = factory;
}

void
CQWidgetFactoryMgr::
removeWidgetFactory(const QString &name)
{
  auto p = widgetFactories_.find(name);
  assert(p != widgetFactories_.end());

  widgetFactories_.erase(p);
}

CQWidgetFactory *
CQWidgetFactoryMgr::
getWidgetFactory(const QString &name) const
{
  auto p = widgetFactories_.find(name);
  assert(p != widgetFactories_.end());

  return (*p).second;
}

QStringList
CQWidgetFactoryMgr::
widgetFactoryNames() const
{
  QStringList names;

  for (const auto &pf : widgetFactories_)
    names << pf.first;

  return names;
}

//---

QWidget *
CQWidgetFactoryMgr::
createWidget(const QString &type, QWidget *parent, const QStringList &options)
{
  auto *factory = getWidgetFactory(type);

  auto *w = factory->createWidget(parent, options);

  return w;
}
