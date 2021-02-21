#include <CQApp.h>
#include <CQStyle.h>
#include <CQFont.h>
#include <CQImage.h>
#include <CQWindow.h>
//#include <CQPixmapCache.h>
//#include <CQToolTip.h>
//#include <CQPerfGraph.h>
#include <CConfig.h>
#ifdef CQUTIL_IMAGE
#include <CImageMgr.h>
#endif
#ifdef CQUTIL_FONT
#include <CFontMgr.h>
#endif
#include <CScreenUnits.h>

//#define USE_OBJEDIT 1

#ifdef USE_OBJEDIT
#include <CQObjEdit.h>
#else
//#include <CQMetaEdit.h>
#endif

#include <QScreen>
#include <QDesktopWidget>
#include <QKeyEvent>

CQApp *CQApp::app_;

#ifdef USE_OBJEDIT
extern bool CQAppXIsObjEditKey(void *);
#endif

#ifdef USE_OBJEDIT
bool
CQAppEventFilter(void *message, long *)
{
  bool show = CQAppXIsObjEditKey(message);

  if (show)
    CQApp::showMetaEdit();

  return show;
}
#endif

#ifdef USE_OBJEDIT
bool
CQAppObjEditFilter::
eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_Home) {
      CQApp::showMetaEdit();
      return true;
    }

#if 0
    if (keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key() == Qt::Key_End) {
      CQApp::showPerfDialog();
      return true;
    }
#endif
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
  setObjectName("app");

  //---

  if (! getenv("CQAPP_NO_STYLE")) {
    style_ = new CQStyle;

    setStyle(style_);
  }

  if (getenv("CQAPP_DARK_THEME"))
    setDarkTheme(true);

#ifdef CQUTIL_IMAGE
  CQImage::setPrototype();
#endif

#ifdef CQUTIL_FONT
  CQFontMgrInst->setPrototype();
#endif

#ifdef USE_OBJEDIT
  addObjEditFilter(this);
  //setEventFilter(CQAppEventFilter);
#endif

  CQWindow::setFactory();

  //---

  // TODO: handle application font change
  QScreen *srn = QApplication::screens().at(0);

  double dotsPerInch = srn->logicalDotsPerInch();

  CScreenUnitsMgrInst->setDpi(dotsPerInch);

  //---

  config_ = new CConfig("CQApp");

  std::string fontName;

  if (! config_->getValue("fontName", fontName))
    fontName = "Helvetica";

  double fontSize = 12;

  if (! config_->getValue("fontSize", &fontSize))
    fontSize = dotsPerInch/8.0;

  double dpi;

  if (config_->getValue("dpi", &dpi))
    CScreenUnitsMgrInst->setDpi(dpi);

  //---

  QFont font(fontName.c_str(), fontSize);

  qApp->setFont(font);

  QFontMetrics fm(qApp->font());

  CScreenUnitsMgrInst->setEmSize(fm.height());
  CScreenUnitsMgrInst->setExSize(fm.width("x"));

  //---

  QRect r = qApp->desktop()->availableGeometry();

  CScreenUnitsMgrInst->setScreenWidth (r.width ());
  CScreenUnitsMgrInst->setScreenHeight(r.height());
}

CQApp::
~CQApp()
{
#ifdef CQUTIL_IMAGE
  CQImage::resetPrototype();
#endif

#ifdef CQUTIL_FONT
  CQFontMgrInst->resetPrototype();
  CQFontMgrInst->clear();
#endif

  CQWindow::resetFactory();

  delete config_;

#ifdef USE_OBJEDIT
  delete objEditFilter_;
#endif

//CQToolTip::release();

#ifdef CQUTIL_FONT
  CQFontMgr::release();
#endif

//CQPixmapCache::release();

#ifdef CQUTIL_FONT
  CFontMgr::release();
#endif
#ifdef CQUTIL_IMAGE
  CImageMgr::release();
#endif

  CWindowMgr::release();

  CScreenUnitsMgr::release();
}

void
CQApp::
showMetaEdit(QObject *obj)
{
#ifdef USE_OBJEDIT_OBJEDIT
  CQObjEdit *objEdit = CQObjEdit::createInstance();

  if (obj)
    objEdit->setObject(obj);

  objEdit->show();

  objEdit->raise();
#else
  assert(obj);

/*
  CQMetaEdit *metaEdit = new CQMetaEdit;

  if (obj)
    metaEdit->setObject(obj);

  metaEdit->show();

  metaEdit->raise();
*/
#endif
}

void
CQApp::
showPerfDialog()
{
  //CQPerfDialog *dialog = CQPerfDialog::instance();

  //dialog->show();
}

bool
CQApp::
isDarkTheme() const
{
  return (style()->theme() == CQStyle::Theme::DARK);
}

void
CQApp::
setDarkTheme(bool b)
{
//CQPixmapCacheInst->setDark(b);

  if (b)
    style()->setTheme(CQStyle::Theme::DARK);
  else
    style()->setTheme(CQStyle::Theme::LIGHT);
}

#ifdef USE_OBJEDIT
void
CQApp::
addObjEditFilter(QObject *o)
{
  objEditFilter_ = new CQAppObjEditFilter;

  o->installEventFilter(objEditFilter_);
}
#endif
