#ifndef CQApp_H
#define CQApp_H

//#define USE_OBJEDIT 1

#include <QApplication>

class CQStyle;
class CConfig;
class CQAppObjEditFilter;

#define CQAppInst CQApp::getApp()

class CQApp : public QApplication {
  Q_OBJECT

  Q_PROPERTY(bool darkTheme READ isDarkTheme WRITE setDarkTheme)

 public:
  static CQApp *getApp() { return app_; }

  CQApp(int &argc, char **argv);
 ~CQApp();

  CQStyle *style() const { return style_; }

  bool isDarkTheme() const;
  void setDarkTheme(bool b);

#ifdef USE_OBJEDIT
  void addObjEditFilter(QObject *o);
#endif

  static void showMetaEdit(QObject *obj=nullptr);
  static void showPerfDialog();

 private:
  static CQApp *app_;

  CQStyle*            style_         { nullptr };
  CConfig*            config_        { nullptr };
  CQAppObjEditFilter* objEditFilter_ { nullptr };
};

//---

#ifdef USE_OBJEDIT
class CQAppObjEditFilter : public QObject {
  Q_OBJECT

 protected:
  bool eventFilter(QObject *obj, QEvent *event) override;
};
#endif

#endif
