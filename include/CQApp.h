#ifndef CQApp_H
#define CQApp_H

//#define USE_OBJEDIT 1

#include <QApplication>

class CConfig;
class CQAppObjEditFilter;

#define CQAppInst CQApp::getApp()

class CQApp : public QApplication {
  Q_OBJECT

 public:
  static CQApp *getApp() { return app_; }

  CQApp(int &argc, char **argv);

 ~CQApp();

#ifdef USE_OBJEDIT
  void addObjEditFilter(QObject *o);
#endif

 private:
  static CQApp *app_;

  CConfig*            config_        { nullptr };
  CQAppObjEditFilter* objEditFilter_ { nullptr };
};

#ifdef USE_OBJEDIT
class CQAppObjEditFilter : public QObject {
  Q_OBJECT

 protected:
  bool eventFilter(QObject *obj, QEvent *event);
};
#endif

#endif
