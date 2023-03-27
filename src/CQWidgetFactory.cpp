#include <CQWidgetFactory.h>
#include <CQFlowLayout.h>

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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>

#include <iostream>

#define addWidgetFactoryT(N) \
  CQWidgetFactoryMgrInst->addWidgetFactory(#N, new CQWidgetFactoryT<N>())

#define addToplevelWidgetFactoryT(N) \
  CQWidgetFactoryMgrInst->addWidgetFactory(#N, new CQWidgetFactoryT<N>())->setTopLevel()

#define addComplexWidgetFactoryT(N) \
  CQWidgetFactoryMgrInst->addWidgetFactory(#N, new CQWidgetFactoryT<N>())->setComplex()

#define addLayoutFactoryT(N) \
  CQWidgetFactoryMgrInst->addLayoutFactory(#N, new CQLayoutFactoryT<N>())

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
CQWidgetFactoryMgr() :
 baseSet_("")
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
  addToplevelWidgetFactoryT(QColorDialog);
  addToplevelWidgetFactoryT(QDialog);
  addToplevelWidgetFactoryT(QFileDialog);
  addToplevelWidgetFactoryT(QFontDialog);
  addToplevelWidgetFactoryT(QMessageBox);
//addToplevelWidgetFactoryT(QPrintDialog);
  addToplevelWidgetFactoryT(QProgressDialog);

  addComplexWidgetFactoryT(QCalendarWidget);
//addComplexWidgetFactoryT(QWebView);
  addComplexWidgetFactoryT(QWizard);

  addWidgetFactoryT(QCheckBox);
  addWidgetFactoryT(QComboBox);
  addWidgetFactoryT(QDial);
  addWidgetFactoryT(QDateEdit);
  addWidgetFactoryT(QDateTimeEdit);
  addWidgetFactoryT(QDockWidget);
  addWidgetFactoryT(QDoubleSpinBox);
  addWidgetFactoryT(QFontComboBox);
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
  addWidgetFactoryT(QPlainTextEdit);
  addWidgetFactoryT(QProgressBar);
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
  addWidgetFactoryT(QWizardPage);

  //---

  addLayoutFactoryT(QHBoxLayout);
  addLayoutFactoryT(QVBoxLayout);
//addLayoutFactoryT(QBoxLayout );
  addLayoutFactoryT(QGridLayout);
  addLayoutFactoryT(QFormLayout);
  addLayoutFactoryT(CQFlowLayout);
}

//---

bool
CQWidgetFactoryMgr::
isWidgetFactory(const QString &name) const
{
  const CQWidgetFactorySet *set;
  QString                   widgetName;

  if (! decodeSet(name, set, widgetName))
    return false;

  return set->isWidgetFactory(widgetName);
}

CQWidgetFactory *
CQWidgetFactoryMgr::
addWidgetFactory(const QString &name, CQWidgetFactory *factory)
{
  QString setName, widgetName;

  decodeName(name, setName, widgetName);

  if (setName == "")
    baseSet_.addWidgetFactory(widgetName, factory);
  else {
    auto ps = factorySets_.find(setName);

    if (ps == factorySets_.end())
      ps = factorySets_.insert(ps,
             FactorySets::value_type(setName, new CQWidgetFactorySet(setName)));

    (*ps).second->addWidgetFactory(widgetName, factory);
  }

  return factory;
}

void
CQWidgetFactoryMgr::
removeWidgetFactory(const QString &name)
{
  CQWidgetFactorySet *set;
  QString             widgetName;

  if (! decodeSet(name, set, widgetName))
    return;

  set->removeWidgetFactory(widgetName);
}

CQWidgetFactory *
CQWidgetFactoryMgr::
getWidgetFactory(const QString &name) const
{
  const CQWidgetFactorySet *set;
  QString                   widgetName;

  if (! decodeSet(name, set, widgetName))
    return nullptr;

  return set->getWidgetFactory(widgetName);
}

QStringList
CQWidgetFactoryMgr::
widgetFactorySetNames() const
{
  QStringList names;

  for (const auto &ps : factorySets_)
    names << ps.first;

  return names;
}

QStringList
CQWidgetFactoryMgr::
setWidgetFactoryNames(const QString &setName, bool simple) const
{
  QStringList names;

  const auto *set = getSet(setName);

  if (set)
    names = set->widgetFactoryNames(simple);

  return names;
}

QStringList
CQWidgetFactoryMgr::
widgetFactoryNames(bool simple) const
{
  QStringList names;

  names << baseSet_.widgetFactoryNames(simple);

  for (const auto &ps : factorySets_)
    names << ps.second->widgetFactoryNames(simple);

  return names;
}

//---

bool
CQWidgetFactoryMgr::
isLayoutFactory(const QString &name) const
{
  const CQWidgetFactorySet *set;
  QString                   widgetName;

  if (! decodeSet(name, set, widgetName))
    return false;

  return set->isLayoutFactory(widgetName);
}

CQLayoutFactory *
CQWidgetFactoryMgr::
addLayoutFactory(const QString &name, CQLayoutFactory *factory)
{
  QString setName, widgetName;

  decodeName(name, setName, widgetName);

  if (setName == "")
    baseSet_.addLayoutFactory(widgetName, factory);
  else {
    auto ps = factorySets_.find(setName);

    if (ps == factorySets_.end())
      ps = factorySets_.insert(ps,
             FactorySets::value_type(setName, new CQWidgetFactorySet(setName)));

    (*ps).second->addLayoutFactory(widgetName, factory);
  }

  return factory;
}

void
CQWidgetFactoryMgr::
removeLayoutFactory(const QString &name)
{
  CQWidgetFactorySet *set;
  QString             widgetName;

  if (! decodeSet(name, set, widgetName))
    return;

  set->removeLayoutFactory(widgetName);
}

CQLayoutFactory *
CQWidgetFactoryMgr::
getLayoutFactory(const QString &name) const
{
  const CQWidgetFactorySet *set;
  QString                   widgetName;

  if (! decodeSet(name, set, widgetName))
    return nullptr;

  return set->getLayoutFactory(widgetName);
}

QStringList
CQWidgetFactoryMgr::
layoutFactoryNames() const
{
  QStringList names;

  names << baseSet_.layoutFactoryNames();

  for (const auto &ps : factorySets_)
    names << ps.second->layoutFactoryNames();

  return names;
}

//---

QWidget *
CQWidgetFactoryMgr::
createWidget(const QString &type, QWidget *parent, const QStringList &options) const
{
  const CQWidgetFactorySet *set;
  QString                   typeName;

  if (! decodeSet(type, set, typeName))
    return nullptr;

  return set->createWidget(typeName, parent, options);
}

QLayout *
CQWidgetFactoryMgr::
createLayout(const QString &type, QWidget *parent, const QStringList &options) const
{
  auto *factory = getLayoutFactory(type);

  auto *l = factory->createLayout(parent, options);

  return l;
}

//---

bool
CQWidgetFactoryMgr::
decodeSet(const QString &name, CQWidgetFactorySet* &set, QString &widgetName)
{
  QString setName;

  decodeName(name, setName, widgetName);

  set = const_cast<CQWidgetFactorySet *>(getSet(setName));
  if (! set) return false;

  return true;
}

bool
CQWidgetFactoryMgr::
decodeSet(const QString &name, const CQWidgetFactorySet* &set, QString &widgetName) const
{
  QString setName;

  decodeName(name, setName, widgetName);

  set = getSet(setName);
  if (! set) return false;

  return true;
}

const CQWidgetFactorySet *
CQWidgetFactoryMgr::
getSet(const QString &setName) const
{
  if (setName == "")
    return &baseSet_;

  auto ps = factorySets_.find(setName);
  if (ps == factorySets_.end()) return nullptr;

  return (*ps).second;
}

void
CQWidgetFactoryMgr::
decodeName(const QString &name, QString &setName, QString &widgetName) const
{
  auto ind = name.indexOf(':');

  if (ind >= 0) {
    setName    = name.mid(0, ind);
    widgetName = name.mid(ind + 1);
  }
  else {
    setName    = "";
    widgetName = name;
  }
}

//------

bool
CQWidgetFactorySet::
isWidgetFactory(const QString &name) const
{
  return (widgetFactories_.find(name) != widgetFactories_.end());
}

CQWidgetFactory *
CQWidgetFactorySet::
addWidgetFactory(const QString &name, CQWidgetFactory *factory)
{
  if (isWidgetFactory(name))
    removeWidgetFactory(name);

  widgetFactories_[name] = factory;

  return factory;
}

void
CQWidgetFactorySet::
removeWidgetFactory(const QString &name)
{
  auto p = widgetFactories_.find(name);
  assert(p != widgetFactories_.end());

  widgetFactories_.erase(p);
}

CQWidgetFactory *
CQWidgetFactorySet::
getWidgetFactory(const QString &name) const
{
  auto p = widgetFactories_.find(name);
  assert(p != widgetFactories_.end());

  return (*p).second;
}

QStringList
CQWidgetFactorySet::
widgetFactoryNames(bool simple) const
{
  QStringList names;

  for (const auto &pf : widgetFactories_) {
    if (simple && ! pf.second->isSimple())
      continue;

    names << pf.first;
  }

  return names;
}

//---

bool
CQWidgetFactorySet::
isLayoutFactory(const QString &name) const
{
  return (layoutFactories_.find(name) != layoutFactories_.end());
}

CQLayoutFactory *
CQWidgetFactorySet::
addLayoutFactory(const QString &name, CQLayoutFactory *factory)
{
  if (isLayoutFactory(name))
    removeLayoutFactory(name);

  layoutFactories_[name] = factory;

  return factory;
}

void
CQWidgetFactorySet::
removeLayoutFactory(const QString &name)
{
  auto p = layoutFactories_.find(name);
  assert(p != layoutFactories_.end());

  layoutFactories_.erase(p);
}

CQLayoutFactory *
CQWidgetFactorySet::
getLayoutFactory(const QString &name) const
{
  auto p = layoutFactories_.find(name);
  assert(p != layoutFactories_.end());

  return (*p).second;
}

QStringList
CQWidgetFactorySet::
layoutFactoryNames() const
{
  QStringList names;

  for (const auto &pf : layoutFactories_)
    names << pf.first;

  return names;
}

//---

QWidget *
CQWidgetFactorySet::
createWidget(const QString &type, QWidget *parent, const QStringList &options) const
{
  auto *factory = getWidgetFactory(type);

  auto *w = factory->createWidget(parent, options);

  if (! factory->validateWidget(w))
    std::cerr << "Invalid factory traits for " << type.toStdString() << "\n";

  return w;
}

QLayout *
CQWidgetFactorySet::
createLayout(const QString &type, QWidget *parent, const QStringList &options) const
{
  auto *factory = getLayoutFactory(type);

  auto *l = factory->createLayout(parent, options);

  return l;
}

//---

bool
CQWidgetFactory::
validateWidget(QWidget *w) const
{
  auto *qdialog = qobject_cast<QDialog *>(w);
  auto *qmw     = qobject_cast<QMainWindow *>(w);

  if (qdialog || qmw)
    return isTopLevel();

  return true;
}
