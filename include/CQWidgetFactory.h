#ifndef CQWidgetFactory_H
#define CQWidgetFactory_H

#include <QObject>

class QWidget;
class QLayout;

// TODO: object factory ?

// abstract class to create widget
class CQWidgetFactory {
 public:
  CQWidgetFactory() { }

  virtual ~CQWidgetFactory() { }

  //! create widget for parent with arguments
  virtual QWidget *createWidget(QWidget *parent=nullptr,
                                const QStringList &params=QStringList()) const = 0;

  //! get/set is simple (single line) widget
  bool isSimple() const { return simple_; }

  void setSimple(bool b=true) { simple_ = b; }
  void setComplex(bool b=true) { simple_ = ! b; }

  //! get/set is toplevel (QDialog/QMainWindow) widget
  bool isTopLevel() const { return simple_; }
  void setTopLevel(bool b=true) { toplevel_ = b; }

  //! validate widget matched traits
  virtual bool validateWidget(QWidget *w) const;

 protected:
  bool simple_   { true };
  bool toplevel_ { false };
};

// abstract class to create layout
class CQLayoutFactory {
 public:
  CQLayoutFactory() { }

  virtual ~CQLayoutFactory() { }

  //! create layout for parent with arguments
  virtual QLayout *createLayout(QWidget *parent=nullptr,
                                const QStringList &params=QStringList()) const = 0;
};

//----

// implementation of CQWidgetFactory to create class of type T using constructor
// taking single parent argument
template<typename T>
class CQWidgetFactoryT : public CQWidgetFactory {
 public:
  CQWidgetFactoryT() { }

  QWidget *createWidget(QWidget *parent, const QStringList &) const override {
    auto *w = new T(parent);

    // TODO: set object name

    return w;
  }
};

// implementation of CQWidgetFactory to create class of type T using constructor
// taking no argumnents
template<typename T>
class CQWidgetFactoryNoArgsT : public CQWidgetFactory {
 public:
  CQWidgetFactoryNoArgsT() { }

  QWidget *createWidget(QWidget *, const QStringList &) const override {
    auto *w = new T();

    // TODO: set object name

    return w;
  }
};

// implementation of CQLayoutFactory to create class of type T using constructor
// taking single parent argument
template<typename T>
class CQLayoutFactoryT : public CQLayoutFactory {
 public:
  CQLayoutFactoryT() { }

  QLayout *createLayout(QWidget *parent, const QStringList &) const override {
    auto *w = new T(parent);

    // TODO: set object name

    return w;
  }
};

//---

// class to manage named set of widget factories
class CQWidgetFactorySet : public QObject {
 public:
  CQWidgetFactorySet(const QString &name) :
   name_(name) {
  }

 ~CQWidgetFactorySet() { }

  //---

  const QString &name() const { return name_; }

  //--

  //! is name an existing widget factory
  bool isWidgetFactory(const QString &name) const;

  //! add factory for type T with constructor taking single parent argument
  template<typename T>
  CQWidgetFactory *addWidgetFactoryT(const QString &name) {
    return addWidgetFactory(name, new CQWidgetFactoryT<T>());
  }

  //! add factory for type T with constructor taking no arguments
  template<typename T>
  CQWidgetFactory *addWidgetFactoryNoArgsT(const QString &name) {
    return addWidgetFactory(name, new CQWidgetFactoryNoArgsT<T>());
  }

  //! add widget factory implementation
  CQWidgetFactory *addWidgetFactory(const QString &name, CQWidgetFactory *factory);

  //! remove widget factory of name (asserts if name is no factory of name)
  void removeWidgetFactory(const QString &name);

  //! get widget factory (asserts if name is no factory of name)
  CQWidgetFactory *getWidgetFactory(const QString &name) const;

  //! get all widget factory names
  QStringList widgetFactoryNames(bool simple=false) const;

  //---

  //! is name an existing layout factory
  bool isLayoutFactory(const QString &name) const;

  //! add factory for type T with constructor taking no arguments
  template<typename T>
  CQLayoutFactory *addLayoutFactoryT(const QString &name) {
    return addLayoutFactory(name, new CQLayoutFactoryT<T>());
  }

  //! add layout factory implementation
  CQLayoutFactory *addLayoutFactory(const QString &name, CQLayoutFactory *factory);

  //! remove layout factory of name (asserts if name is no factory of name)
  void removeLayoutFactory(const QString &name);

  //! get layout factory of name (asserts if name is no factory of name)
  CQLayoutFactory *getLayoutFactory(const QString &name) const;

  //! get all layout factory names
  QStringList layoutFactoryNames() const;

  //---

  // create widget of specified type using widget factory for type name
  QWidget *createWidget(const QString &type, QWidget *parent=nullptr,
                        const QStringList &options=QStringList()) const;

  // create layout of specified type using widget factory for type name
  QLayout *createLayout(const QString &type, QWidget *parent=nullptr,
                        const QStringList &options=QStringList()) const;

 private:
  void init();

 private:
  using WidgetFactories = std::map<QString, CQWidgetFactory *>;
  using LayoutFactories = std::map<QString, CQLayoutFactory *>;

  QString         name_;
  WidgetFactories widgetFactories_;
  LayoutFactories layoutFactories_;
};

//---

#define CQWidgetFactoryMgrInst CQWidgetFactoryMgr::instance()

// class to manage all widget factories and factory sets
class CQWidgetFactoryMgr : public QObject {
  Q_OBJECT

 public:
  static CQWidgetFactoryMgr *instance();

  CQWidgetFactoryMgr();
 ~CQWidgetFactoryMgr();

  //---

  //! is name an existing widget factory
  bool isWidgetFactory(const QString &name) const;

  //! add factory for type T with constructor taking single parent argument
  template<typename T>
  CQWidgetFactory *addWidgetFactoryT(const QString &name) {
    return addWidgetFactory(name, new CQWidgetFactoryT<T>());
  }

  //! add factory for type T with constructor taking no arguments
  template<typename T>
  CQWidgetFactory *addWidgetFactoryNoArgsT(const QString &name) {
    return addWidgetFactory(name, new CQWidgetFactoryNoArgsT<T>());
  }

  //! add widget factory implementation
  CQWidgetFactory *addWidgetFactory(const QString &name, CQWidgetFactory *factory);

  //! remove widget factory of name (asserts if name is no factory of name)
  void removeWidgetFactory(const QString &name);

  //! get widget factory (asserts if name is no factory of name)
  CQWidgetFactory *getWidgetFactory(const QString &name) const;

  //! get all widget factory set names
  QStringList widgetFactorySetNames() const;
  QStringList setWidgetFactoryNames(const QString &set, bool simple=false) const;

  //! get all widget factory names
  QStringList widgetFactoryNames(bool simple=false) const;

  //---

  //! is name an existing layout factory
  bool isLayoutFactory(const QString &name) const;

  //! add factory for type T with constructor taking no arguments
  template<typename T>
  CQLayoutFactory *addLayoutFactoryT(const QString &name) {
    return addLayoutFactory(name, new CQLayoutFactoryT<T>());
  }

  //! add layout factory implementation
  CQLayoutFactory *addLayoutFactory(const QString &name, CQLayoutFactory *factory);

  //! remove layout factory of name (asserts if name is no factory of name)
  void removeLayoutFactory(const QString &name);

  //! get layout factory of name (asserts if name is no factory of name)
  CQLayoutFactory *getLayoutFactory(const QString &name) const;

  //! get all layout factory names
  QStringList layoutFactoryNames() const;

  //---

  // create widget of specified type using widget factory for type name
  QWidget *createWidget(const QString &type, QWidget *parent=nullptr,
                        const QStringList &options=QStringList()) const;

  // create layout of specified type using widget factory for type name
  QLayout *createLayout(const QString &type, QWidget *parent=nullptr,
                        const QStringList &options=QStringList()) const;

 private:
  void init();

  bool decodeSet(const QString &name, CQWidgetFactorySet* &set, QString &widgetName);
  bool decodeSet(const QString &name, const CQWidgetFactorySet* &set, QString &widgetName) const;

  const CQWidgetFactorySet *getSet(const QString &setName) const;

  void decodeName(const QString &name, QString &setName, QString &widgetName) const;

 private:
  using FactorySets     = std::map<QString, CQWidgetFactorySet *>;
  using WidgetFactories = std::map<QString, CQWidgetFactory *>;
  using LayoutFactories = std::map<QString, CQLayoutFactory *>;

  FactorySets        factorySets_;
  CQWidgetFactorySet baseSet_;
};

#endif
