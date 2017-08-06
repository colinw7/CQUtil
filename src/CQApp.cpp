#include <CQApp.h>
#include <CQStyle.h>
#include <CQFont.h>
#include <CQImage.h>
#include <CQWindow.h>
#include <CConfig.h>
#include <CScreenUnits.h>

//#define USE_OBJEDIT 1

#ifdef USE_OBJEDIT
#include <CQObjEdit.h>
#endif

#include <QScreen>
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

    if (keyEvent->key() == Qt::Key_Home) {
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
  setStyle(new CQStyle);

  CQImage::setPrototype();

  CQFontMgrInst->setPrototype();

#ifdef USE_OBJEDIT
  setEventFilter(CQAppEventFilter);
#endif

  CQWindow::setFactory();

  //---

  // TODO: handle application font change
  QScreen *srn = QApplication::screens().at(0);
  double dotsPerInch = srn->logicalDotsPerInch();
  CScreenUnitsMgrInst->setDpi(dotsPerInch);

  QFont font("Helveltica", dotsPerInch/8);

  qApp->setFont(font);

  QFontMetrics fm(qApp->font());

  CScreenUnitsMgrInst->setEmSize(fm.height());
  CScreenUnitsMgrInst->setExSize(fm.width("x"));

  config_ = new CConfig("CQApp");

  double dpi;

  if (config_->getValue("dpi", &dpi))
    CScreenUnitsMgrInst->setDpi(dpi);
}

CQApp::
~CQApp()
{
  delete config_;
}

#ifdef USE_OBJEDIT
void
CQApp::
addObjEditFilter(QObject *o)
{
  o->installEventFilter(new CQAppObjEditFilter);
}
#endif
