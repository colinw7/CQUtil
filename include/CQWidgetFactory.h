#ifndef CQWidgetFactory_H
#define CQWidgetFactory_H

#include <QObject>

class QWidget;

class CQWidgetFactory {
 public:
  CQWidgetFactory() { }

  virtual ~CQWidgetFactory() { }

  virtual QWidget *createWidget(QWidget *parent=nullptr,
                                const QStringList &params=QStringList()) = 0;
};

//----

template<typename T>
class CQWidgetFactoryT : public CQWidgetFactory {
 public:
  CQWidgetFactoryT() { }

  QWidget *createWidget(QWidget *parent, const QStringList &) {
    auto *w = new T(parent);

    // TODO: set object name

    return w;
  }
};

//---

#define CQWidgetFactoryMgrInst CQWidgetFactoryMgr::instance()

class CQWidgetFactoryMgr : public QObject {
  Q_OBJECT

 public:
  static CQWidgetFactoryMgr *instance();

  CQWidgetFactoryMgr();
 ~CQWidgetFactoryMgr();

  //---

  bool isWidgetFactory(const QString &name) const;

  void addWidgetFactory(const QString &name, CQWidgetFactory *factory);

  void removeWidgetFactory(const QString &name);

  CQWidgetFactory *getWidgetFactory(const QString &name) const;

  QStringList widgetFactoryNames() const;

  //---

  QWidget *createWidget(const QString &type, QWidget *parent=nullptr,
                        const QStringList &options=QStringList());

 private:
  void init();

 private:
  using WidgetFactories = std::map<QString, CQWidgetFactory *>;

  WidgetFactories widgetFactories_;
};

#endif
