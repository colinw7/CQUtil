#include <CQApp.h>
#include <CQAppOptions.h>
#include <CQStyleMgr.h>
#include <CQUtil.h>
#include <CQFont.h>
#include <CQImage.h>
#include <CQMsgHandler.h>
#include <CQWidgetUtil.h>

#ifdef CQAPP_WINDOW
#include <CQWindow.h>
#endif

#include <CQPixmapCache.h>

#include <CQToolTip.h>

#ifdef CQPERF_GRAPH
#include <CQPerfGraph.h>
#endif

#include <CConfig.h>

#include <CImageMgr.h>
#include <CFontMgr.h>

#include <CScreenUnits.h>

#ifdef CQAPP_USE_OBJEDIT
#  ifdef CQAPP_USE_OBJEDIT_OBJEDIT
#include <CQObjEdit.h>
#  else
#include <CQMetaEdit.h>
#  endif
#endif

#include <QScreen>
#include <QDesktopWidget>
#include <QToolButton>
#include <QKeyEvent>

#include <svg/menu_svg.h>

CQApp *CQApp::app_;

#ifdef CQAPP_USE_OBJEDIT
extern bool CQAppXIsObjEditKey(void *);
#endif

#ifdef CQAPP_USE_OBJEDIT
bool
CQAppEventFilter(void *message, long *)
{
  bool show = CQAppXIsObjEditKey(message);

  if (show)
    CQApp::showMetaEdit();

  return show;
}
#endif

#ifdef CQAPP_USE_OBJEDIT
bool
CQAppObjEditFilter::
eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::KeyPress) {
    auto *keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_Home) {
      CQApp::showMetaEdit();
      return true;
    }

    if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_End) {
      CQApp::showPerfDialog();
      return true;
    }

    if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_PageDown) {
      CQAppOptions::show();
      return true;
    }
  }

  // standard event processing
  return QObject::eventFilter(obj, event);
}
#endif

/*! Constructor to create QApplication and correctly
    initialize CQImage and CQFont prototypes
*/
CQApp::
CQApp(int &argc, char **argv) :
 QApplication(argc, argv)
{
  assert(! app_);

  app_ = this;

  //---

  setObjectName("app");

  //---

  CQMsgHandler::install();

  //---

  if (! getenv("CQAPP_NO_STYLE"))
    CQStyleMgrInst->createStyle();

  if (getenv("CQAPP_DARK_THEME"))
    setDarkTheme(true);

  CQImage::setPrototype();

  CQFontMgrInst->setPrototype();

#ifdef CQAPP_USE_OBJEDIT
  addObjEditFilter(this);
  //setEventFilter(CQAppEventFilter);
#endif

#ifdef CQAPP_WINDOW
  CQWindow::setFactory();
#endif

  //---

  // TODO: handle application font change
  auto *srn = QApplication::screens().at(0);

  double dotsPerInch = srn->logicalDotsPerInch();

  CScreenUnitsMgrInst->setDpi(dotsPerInch);

  //---

  config_ = new CConfig("CQApp");

  std::string fontName;
  double      fontSize = 12;

  if (! getenv("CQAPP_NO_FONT")) {
    if (! config_->getValue("fontName", fontName))
      fontName = "Helvetica";

    if (! config_->getValue("fontSize", &fontSize))
      fontSize = dotsPerInch/8.0;
  }

  //---

  double dpi;

  if (config_->getValue("dpi", &dpi))
    CScreenUnitsMgrInst->setDpi(dpi);

  //---

  QFontMetrics fm(qApp->font());

  if (fontName != "") {
    QFont font(fontName.c_str(), int(fontSize));

    CQStyleMgrInst->setFont(font);

    fm = QFontMetrics(CQStyleMgrInst->font());
  }

  CScreenUnitsMgrInst->setEmSize(fm.height());
  CScreenUnitsMgrInst->setExSize(fm.horizontalAdvance("x"));

  //---

  auto fixedFont = CQUtil::getMonospaceFont();

  fixedFont.setPixelSize(int(fontSize));

  CQStyleMgrInst->setFixedFont(fixedFont);

  //---

  CQStyleMgrInst->setIconSizeFromFont();

  //---

  auto r = CQWidgetUtil::desktopAvailableGeometry();

  CScreenUnitsMgrInst->setScreenWidth (r.width ());
  CScreenUnitsMgrInst->setScreenHeight(r.height());
}

CQApp::
~CQApp()
{
  CQImage::resetPrototype();

  CQFontMgrInst->resetPrototype();
  CQFontMgrInst->clear();

#ifdef CQAPP_WINDOW
  CQWindow::resetFactory();
#endif

  delete config_;

#ifdef CQAPP_USE_OBJEDIT
  delete objEditFilter_;
#endif

  CQToolTip::release();

  CQFontMgr::release();

  CQPixmapCache::release();

  CFontMgr::release();

  CImageMgr::release();

#ifdef CQAPP_WINDOW
  CWindowMgr::release();
#endif

  CScreenUnitsMgr::release();
}

void
CQApp::
#ifdef CQAPP_USE_OBJEDIT
showMetaEdit(QObject *obj)
#else
showMetaEdit(QObject *)
#endif
{
#ifdef CQAPP_USE_OBJEDIT
#  ifdef CQAPP_USE_OBJEDIT_OBJEDIT
  auto *objEdit = CQObjEdit::createInstance();

  if (obj)
    objEdit->setObject(obj);

  objEdit->show();

  objEdit->raise();
#  else
  auto *metaEdit = new CQMetaEdit;

  if (obj)
    metaEdit->setObject(obj);

  metaEdit->show();

  metaEdit->raise();
#  endif
#endif
}

void
CQApp::
showPerfDialog()
{
#ifdef CQPERF_GRAPH
  auto *dialog = CQPerfDialog::instance();

  dialog->show();
#endif
}

bool
CQApp::
isDarkTheme() const
{
  return (CQStyleMgrInst->theme() == CQStyleMgr::Theme::DARK);
}

void
CQApp::
setDarkTheme(bool b)
{
  CQStyleMgrInst->setTheme(b ? CQStyleMgr::Theme::DARK : CQStyleMgr::Theme::LIGHT);
}

#ifdef CQAPP_USE_OBJEDIT
void
CQApp::
addObjEditFilter(QObject *o)
{
  objEditFilter_ = new CQAppObjEditFilter;

  o->installEventFilter(objEditFilter_);
}
#endif

QToolButton *
CQApp::
createDebugButton(QWidget *parent)
{
  int is = QFontMetrics(font()).height() + 6;

  auto *button = new QToolButton(parent);

  button->setIcon(CQPixmapCacheInst->getIcon("MENU"));
  button->setPopupMode(QToolButton::InstantPopup);

  button->setAutoRaise(true);
  button->setIconSize(QSize(is, is));

  auto *menu = new QMenu;

  auto *action1 = menu->addAction("Meta Edit");
  auto *action2 = menu->addAction("Performance");
  auto *action3 = menu->addAction("Options");

  connect(action1, SIGNAL(triggered()), this, SLOT(metaEditSlot()));
  connect(action2, SIGNAL(triggered()), this, SLOT(performanceSlot()));
  connect(action3, SIGNAL(triggered()), this, SLOT(optionsSlot()));

  button->setMenu(menu);

  return button;
}

void
CQApp::
metaEditSlot()
{
  CQApp::showMetaEdit();
}

void
CQApp::
performanceSlot()
{
  CQApp::showPerfDialog();
}

void
CQApp::
optionsSlot()
{
  CQAppOptions::show();
}
