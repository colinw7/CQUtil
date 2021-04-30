#ifndef CQWidgetFactory_H
#define CQWidgetFactory_H

#include <QObject>

class QWidget;
class QLayout;

class CQWidgetFactory {
 public:
  CQWidgetFactory() { }

  virtual ~CQWidgetFactory() { }

  virtual QWidget *createWidget(QWidget *parent=nullptr,
                                const QStringList &params=QStringList()) = 0;
};

class CQLayoutFactory {
 public:
  CQLayoutFactory() { }

  virtual ~CQLayoutFactory() { }

  virtual QLayout *createLayout(QWidget *parent=nullptr,
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

template<typename T>
class CQWidgetFactoryNoArgsT : public CQWidgetFactory {
 public:
  CQWidgetFactoryNoArgsT() { }

  QWidget *createWidget(QWidget *, const QStringList &) {
    auto *w = new T();

    // TODO: set object name

    return w;
  }
};

template<typename T>
class CQLayoutFactoryT : public CQLayoutFactory {
 public:
  CQLayoutFactoryT() { }

  QLayout *createLayout(QWidget *parent, const QStringList &) {
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

  template<typename T>
  void addWidgetFactoryT(const QString &name) {
    addWidgetFactory(name, new CQWidgetFactoryT<T>());
  }

  template<typename T>
  void addWidgetFactoryNoArgsT(const QString &name) {
    addWidgetFactory(name, new CQWidgetFactoryNoArgsT<T>());
  }

  void addWidgetFactory(const QString &name, CQWidgetFactory *factory);

  void removeWidgetFactory(const QString &name);

  CQWidgetFactory *getWidgetFactory(const QString &name) const;

  QStringList widgetFactoryNames() const;

  //---

  bool isLayoutFactory(const QString &name) const;

  void addLayoutFactory(const QString &name, CQLayoutFactory *factory);

  void removeLayoutFactory(const QString &name);

  CQLayoutFactory *getLayoutFactory(const QString &name) const;

  QStringList layoutFactoryNames() const;

  //---

  QWidget *createWidget(const QString &type, QWidget *parent=nullptr,
                        const QStringList &options=QStringList());

  QLayout *createLayout(const QString &type, QWidget *parent=nullptr,
                        const QStringList &options=QStringList());

 private:
  void init();

 private:
  using WidgetFactories = std::map<QString, CQWidgetFactory *>;
  using LayoutFactories = std::map<QString, CQLayoutFactory *>;

  WidgetFactories widgetFactories_;
  LayoutFactories layoutFactories_;
};

#endif
