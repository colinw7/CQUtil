#include <CQApp.h>
#include <CQStyle.h>
#include <CQFont.h>
#include <CQImage.h>
#include <CQWindow.h>
//#include <CQPixmapCache.h>
//#include <CQToolTip.h>
#include <CConfig.h>
#include <CFontMgr.h>
#include <CScreenUnits.h>

//#define USE_OBJEDIT 1

#ifdef USE_OBJEDIT
#include <CQObjEdit.h>
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

  if (show) {
    CQObjEdit *objEdit = CQObjEdit::createInstance();

    objEdit->show();

    objEdit->raise();
  }

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
      CQObjEdit *objEdit = CQObjEdit::createInstance();

      objEdit->show();

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
  setObjectName("app");

  //---

  style_ = new CQStyle;

  setStyle(style_);

  if (getenv("CQAPP_DARK_THEME"))
    setDarkTheme(true);

  CQImage::setPrototype();

  CQFontMgrInst->setPrototype();

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
  CQImage::resetPrototype();

  CQFontMgrInst->resetPrototype();
  CQFontMgrInst->clear();

  CQWindow::resetFactory();

  delete config_;

#ifdef USE_OBJEDIT
  delete objEditFilter_;
#endif

//CQToolTip::release();

  CQFontMgr::release();

//CQPixmapCache::release();

  CFontMgr ::release();
  CImageMgr::release();

  CWindowMgr::release();

  CScreenUnitsMgr::release();
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
