#ifndef CQUTIL_H
#define CQUTIL_H

#include <Qt>
#include <QColor>
#include <QEvent>
#include <QVariant>
#include <QTableWidget>
#include <QButtonGroup>
#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QMenu>
#include <QBoxLayout>
#include <QTimer>
#include <QAction>
#include <QPen>

#include <map>

//---

#include <CQUtilMeta.h>

//---

class QMouseEvent;
class QKeyEvent;
class QAbstractButton;
class QDockWidget;
class QLabel;

struct QMetaObject;

namespace CQUtil {
  class PropInfo {
   public:
    PropInfo() { }

    void init(const QMetaProperty &mp);

    QString        name    () const { return name_; }
    QVariant::Type type    () const { return type_; }
    QString        typeName() const { return typeName_; }

    bool isWritable() const { return isWritable_ ; }
    bool isEnumType() const { return isEnumType_; }
    bool isFlagType() const { return isFlagType_; }

    const QStringList &enumNames() const { return enumNames_; }

    bool enumNameValue(const QString &name, int &value) const {
      auto p = enumNameValue_.find(name);
      if (p == enumNameValue_.end()) return false;

      value = (*p).second;

      return true;
    }

    bool enumValueName(int value, QString &name) const {
      auto p = enumValueName_.find(value);
      if (p == enumValueName_.end()) return false;

      name = (*p).second;

      return true;
    }

   private:
    using NameValueMap = std::map<QString, int>;
    using ValueNameMap = std::map<int, QString>;

    QString        name_;
    QVariant::Type type_       { QVariant::Invalid };
    QString        typeName_;
    bool           isWritable_ { false };
    bool           isEnumType_ { false };
    bool           isFlagType_ { false };
    QStringList    enumNames_;
    NameValueMap   enumNameValue_;
    ValueNameMap   enumValueName_;
  };

  void initProperties();

  //---

  QString fullName(const QObject* object);

  QObject *nameToObject(const QString &name);
  QObject *nameToObject(QObject *parent, const QString &name);

  QObject *hierChildObject(QObject *object, int ind, const QStringList &names);

  QObject *childObject(QObject *parent, const QString &name);

  QList<QObject *> childObjects(QObject *parent, const QString &name);

  QString addObjectAlias(QObject *object);

  //---

  QWidget *getToplevelWidget(QWidget *widget);

  //---

  int getNumProperties(const QObject *object, bool inherited=true);

  QStringList getPropertyList(const QObject *object, bool inherited=true);

  QStringList getPropertyList(const QObject *object, bool inherited,
                              const QMetaObject* metaObject);

  QString getPropertyName(const QObject *object, int ind, bool inherited=true);

  QVariant::Type getPropertyType(const QObject *object, int ind, bool inherited=true);

  QString getPropertyTypeName(const QObject *object, int ind, bool inherited=true);

  QVariant getPropertyValue(const QObject *object, int ind, bool inherited=true);

  bool getPropertyValueIsEnum(const QObject *object, int ind, bool inherited=true);
  bool getPropertyValueIsFlag(const QObject *object, int ind, bool inherited=true);

  QString getPropertyEnumName(const QObject *object, int ind, bool inherited=true);

  QString getPropertyEnumValue(const QObject *object, int ind, bool inherited=true);

  QStringList getMetaPropertyEnumNames(const QObject *object, int ind, bool inherited=true);
  QList<int> getMetaPropertyEnumValues(const QObject *object, int ind, bool inherited=true);

  bool getMetaPropertyEnumNameValues(const QObject *object, int ind, bool inherited,
                                     QStringList &names, QList<int> &values);

  bool enumPropertyValueToString(const QObject *object, int ind, bool inherited,
                                 int value, QString &str);
  bool enumPropertyStringToValue(const QObject *object, int ind, bool inherited,
                                 const QString &str, int &value);

  bool stringToEnumValue(const QString &str, const QMetaProperty &metaProperty, int &value);

  bool stringToEnumSubValue(const QString &str, const QMetaEnum &metaEnum, int &value);

  bool getMetaPropertyEnum(const QObject *object, int ind, bool inherited,
                           QMetaProperty &metaProperty, QMetaEnum &metaEnum, bool &isFlag);

  bool getMetaProperty(const QObject *object, int ind, bool inherited,
                       QMetaProperty &metaProperty);

  bool hasProperty(const QObject *object, const QString &name);
  bool hasWritableProperty(const QObject *object, const QString &name);

  bool getProperty(const QObject *object, const QString &name, QVariant &value);

  bool getTclProperty(const QObject *object, const QString &propName, QVariant &v);

  bool setPropertyValue(QObject *object, int ind, const QVariant &value, bool inherited=true);

  bool setProperty(const QObject *object, const QString &name, const QString &value);
  bool setProperty(const QObject *object, const QString &name, const QVariant &value);

  bool getPropertyInfo(const QObject *object, int ind, PropInfo *info, bool inherited=true);

  bool getPropInfo(const QObject *object, const QString &name, PropInfo *info);

  bool getPropInfoEnumNameValue(const CQUtil::PropInfo &propInfo, const QString &name, int &value);
  bool getPropInfoEnumValueName(const CQUtil::PropInfo &propInfo, int value, QString &name);
  bool getPropInfoEnumFlagValueName(const CQUtil::PropInfo &propInfo, int value, QString &name);

  //---

  QString className(const QObject *object);

  QStringList hierClassNames(const QObject *object);

  const QMetaObject *baseClass(QMetaObject *metaObject, const char *name);

  //---

  int numSignals(const QObject *object, bool hierarchical=true);

  QString signalName(const QObject *object, int ind, bool hierarchical=true);

  QStringList signalNames(const QObject* object, bool inherited=true);

  //---

  int numSlots(const QObject *object, bool hierarchical=true);

  QString slotName(const QObject *object, int ind, bool hierarchical=true);

  QStringList slotNames(const QObject* object, bool inherited=true);

  //---

  QString eventTypeToName(QEvent::Type type);
  QString eventToString(QEvent *event);

  QColor getForeground(QWidget *widget);
  void   setForeground(QWidget *widget, const QColor &color);

  QColor getBackground(QWidget *widget);
  void   setBackground(QWidget *widget, const QColor &color);

  QString variantToString(const QVariant &var);
  bool variantToString(const QVariant &var, QString &str);

  bool stringToVariant(const QString &str, QVariant::Type type, const char *typeName,
                       int userType, QVariant &var, const QVariant &oldVar=QVariant());

  bool paletteFromString(QPalette &palette, const QString &paletteDef);
  QString paletteToString(const QPalette &palette);

  bool activateSlot(QObject *receiver, const char *slotName, const char *valuesStr);
  bool activateSignal(QObject* sender, const char *signalName, const char *valuesStr);

  //---

  bool userVariantToString(const QVariant &var, QString &str);
  bool userVariantFromString(QVariant &ivar, const QString &str);

  //---

  /*! template function to get table cell widget of type */
  template<typename T>
  T tableCellWidget(QTableWidget *table, int row, int col) {
    return qobject_cast<T>(table->cellWidget(row, col));
  }

  //---

  void getScreenSize(uint *w, uint *h);

  void setSelectText(const std::string &text);

  void setDockVisible(QDockWidget *dock, bool visible);

  QString alignToString(Qt::Alignment align);
  bool stringToAlign(const QString &str, Qt::Alignment &align);

  //-----

  QPixmap getPixmap(const QString &name);

  QIcon getIcon(const QString &name);

  //-----

  uint nameWidgetTree(QWidget *widget);

  void nameWidget(QWidget *widget);
  void nameObject(QObject *object);

  void nameWidgetButton(QAbstractButton *button);
  void nameWidgetLabel(QLabel *label);
  void nameObjectGen(QObject *object);

  //-----

  void recolorImage(QImage &image, const QColor &fg, const QColor &bg);

  //-----

  void drawHtmlText(QWidget *w, QPainter *painter, const QString &text,
                    const QPalette &palette, const QRect &rect, bool active=false);

  QString colorToHtml(const QColor &c);

  //----

  QString policyToString(QSizePolicy::Policy policy);

  //----

  QColor blendColors(const QColor &c1, const QColor &c2, double f);
}

//---

namespace CQUtil {
  template<class T>
  T *makeWidget(const QString &objName="") {
    T *t = new T;

    if (objName != "")
      t->setObjectName(objName);

    return t;
  }

  template<class T>
  T *makeWidget(QWidget *parent, const QString &objName="") {
    T *t = new T(parent);

    if (objName != "")
      t->setObjectName(objName);

    return t;
  }

  template<class T>
  T *makeLabelWidget(const QString &name, const QString &objName="") {
    T *t = new T(name);

    if (objName != "")
      t->setObjectName(objName);

    return t;
  }

  template<class T>
  T *makeLabelWidget(QWidget *parent, const QString &name, const QString &objName="") {
    T *t = new T(name, parent);

    if (objName != "")
      t->setObjectName(objName);

    return t;
  }

  template<class T>
  T *makeLayout(QWidget *parent, int margin=2, int spacing=2) {
    T *t = new T(parent);

    t->setMargin (margin );
    t->setSpacing(spacing);

    return t;
  }

  template<class T>
  T *makeLayout(QBoxLayout *parent, int margin=2, int spacing=2) {
    T *t = new T;

    t->setMargin (margin );
    t->setSpacing(spacing);

    parent->addLayout(t);

    return t;
  }

  template<class T>
  T *makeLayout(int margin=2, int spacing=2) {
    T *t = new T;

    t->setMargin (margin );
    t->setSpacing(spacing);

    return t;
  }

  inline QButtonGroup *makeButtonGroup(QWidget *parent,
   const std::initializer_list<QAbstractButton *> &buttons) {
    auto *buttonGroup = new QButtonGroup(parent);

    int ind = 0;

    for (auto button : buttons)
      buttonGroup->addButton(button, ind++);

    return buttonGroup;
  }

  //---

  inline QAction *addAction(QMenu *menu, const QString &name,
                            QObject *receiver=nullptr, const char *slotName=nullptr) {
    auto *action = new QAction(name, menu);

    if (slotName)
      QObject::connect(action, SIGNAL(triggered()), receiver, slotName);

    menu->addAction(action);

    return action;
  }

  inline QAction *addCheckedAction(QMenu *menu, const QString &name, bool checked,
                                   QObject *receiver=nullptr, const char *slotName=nullptr) {
    auto *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(checked);

    if (slotName)
      QObject::connect(action, SIGNAL(triggered(bool)), receiver, slotName);

    menu->addAction(action);

    return action;
  }

  inline QActionGroup *createActionGroup(QMenu *menu, QObject *receiver=nullptr,
                                         const char *slotName=nullptr) {
    auto *actionGroup = new QActionGroup(menu);

    if (slotName)
       QObject::connect(actionGroup, SIGNAL(triggered(QAction *)), receiver, slotName);

    return actionGroup;
  }

  inline QAction *addGroupCheckAction(QActionGroup *group, const QString &name, bool checked,
                                      QObject *receiver=nullptr, const char *slotName=nullptr) {
    auto *menu = qobject_cast<QMenu *>(group->parent());
    assert(menu);

    auto *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(checked);

    if (slotName)
      QObject::connect(action, SIGNAL(triggered()), receiver, slotName);

    group->addAction(action);

    return action;
  }

  inline void addActionGroupToMenu(QActionGroup *group) {
    auto *menu = qobject_cast<QMenu *>(group->parent());
    assert(menu);

    menu->addActions(group->actions());
  }

  //---

  // default widget connection
  template<typename T> struct Connector { };

  template<>
  struct Connector<QPushButton> {
    void doConnect(QPushButton *w, QObject *obj, const char *slotName) {
      QObject::connect(w, SIGNAL(clicked()), obj, slotName);
    }
  };

  template<>
  struct Connector<QToolButton> {
    void doConnect(QToolButton *w, QObject *obj, const char *slotName) {
      QObject::connect(w, SIGNAL(clicked()), obj, slotName);
    }
  };

  template<>
  struct Connector<QRadioButton> {
    void doConnect(QRadioButton *w, QObject *obj, const char *slotName) {
      QObject::connect(w, SIGNAL(toggled(bool)), obj, slotName);
    }
  };

  template<>
  struct Connector<QCheckBox> {
    void doConnect(QCheckBox *w, QObject *obj, const char *slotName) {
      QObject::connect(w, SIGNAL(stateChanged(int)), obj, slotName);
    }
  };

  template<>
  struct Connector<QTimer> {
    void doConnect(QTimer *w, QObject *obj, const char *slotName) {
      QObject::connect(w, SIGNAL(timeout()), obj, slotName);
    }
  };

  template<typename T>
  inline void defConnect(T *w, QObject *obj, const char *slotName) {
    Connector<T> connector; connector.doConnect(w, obj, slotName);
  }

  //---

  inline void connectDisconnect(bool b, QObject *from, const char *fromName,
                                QObject *to, const char *toName) {
    if (b)
      QObject::connect(from, fromName, to, toName);
    else
      QObject::disconnect(from, fromName, to, toName);
  }

  inline void optConnectDisconnect(bool b, QObject *from, const char *fromName,
                                   QObject *to, const char *toName) {
    if (! from || ! to) return;

    connectDisconnect(b, from, fromName, to, toName);
  }
}

//---

namespace CQUtil {

/*!
 * \brief RAII Class to Auto Connect/Disconnect to signal/slot
 * \ingroup Charts
 */
class AutoDisconnect {
 public:
  AutoDisconnect(QObject *from, const char *fromName, QObject *to, const char *toName) :
   from_(from), fromName_(fromName), to_(to), toName_(toName) {
    assert(from_ && to_);

    connectDisconnect(false, from_, fromName_, to_, toName_);
  }

 ~AutoDisconnect() {
    connectDisconnect(true, from_, fromName_, to_, toName_);
  }

 private:
  QObject    *from_     { nullptr };
  const char *fromName_ { nullptr };
  QObject    *to_       { nullptr };
  const char *toName_   { nullptr };
};

}

//---

namespace CQUtil {
  double polygonArea(const QPolygonF &poly);

  double polygonArea2(const QPolygonF &poly);
}

//---

class QGridLayout;
class QLayoutItem;

namespace CQUtil {
  void deleteLayoutChildWidgets(QLayoutItem *item);

  void removeGridItem(QGridLayout *layout, int row, int column, bool deleteWidgets);

  void removeGridRow   (QGridLayout *layout, int row, bool deleteWidgets);
  void removeGridColumn(QGridLayout *layout, int column, bool deleteWidgets);

  void removeGridItems(QGridLayout *layout, bool deleteWidgets);
}

//---

namespace CQUtil {
  bool isFixedPitch(const QFont &font);

  QFont getMonospaceFont();
}

#endif
