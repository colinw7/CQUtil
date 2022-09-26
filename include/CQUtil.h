#ifndef CQUTIL_H
#define CQUTIL_H

#define CQUTIL_EVENT     1
#define CQUTIL_LINE_DASH 1
#define CQUTIL_ANGLE     1
#define CQUTIL_GRADIENT  1

#include <Qt>
#include <QColor>
#include <QEvent>
#include <QVariant>
#include <QTableWidget>
#include <QButtonGroup>
#include <QMenu>
#include <QBoxLayout>
#include <QAction>
#include <QPen>

#ifdef CQUTIL_EVENT
#include <CEvent.h>
#endif

#include <CRGBA.h>

#ifdef CQUTIL_LINE_DASH
#include <CLineDash.h>
#endif

#ifdef CQUTIL_ANGLE
#include <CAngle.h>
#endif

#include <CFontStyle.h>
#include <CLineCapType.h>
#include <CLineJoinType.h>
#include <CIBBox2D.h>

#include <CPoint2D.h>
#include <CBBox2D.h>
#include <CMatrix2D.h>
#include <CAlignType.h>

#include <map>

//---

#include <CQUtilMeta.h>

#ifdef CQUTIL_LINE_DASH
CQUTIL_DCL_META_TYPE(CLineDash)
#endif

#ifdef CQUTIL_ANGLE
CQUTIL_DCL_META_TYPE(CAngle)
#endif

//---

class QMouseEvent;
class QKeyEvent;
class QAbstractButton;
class QDockWidget;
class QLabel;

struct QMetaObject;

#ifdef CQUTIL_GRADIENT
class CLinearGradient;
class CRadialGradient;
#endif

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

#ifdef CQUTIL_EVENT
  CMouseEvent *convertEvent(QMouseEvent *event);
  CKeyEvent   *convertEvent(QKeyEvent *event);

  CMouseButton   convertButton(Qt::MouseButton button);
  CKeyType       convertKey(int key, Qt::KeyboardModifiers modifiers);
  CEventModifier convertModifier(Qt::KeyboardModifiers modifiers);
#endif

  QColor rgbToColor(const CRGB &rgb);
  QColor rgbaToColor(const CRGBA &rgba);

  inline QColor toQColor(const CRGB  &rgb ) { return rgbToColor (rgb ); }
  inline QColor toQColor(const CRGBA &rgba) { return rgbaToColor(rgba); }

  uint rgbaToInt(const CRGBA &rgba);

  CRGB  colorToRGB(const QColor &color);
  CRGBA colorToRGBA(const QColor &color);

  Qt::PenCapStyle  toPenCapStyle (const CLineCapType  &lineCap);
  Qt::PenJoinStyle toPenJoinStyle(const CLineJoinType &lineJoin);

  inline CRGBA fromQColor(const QColor &color) { return colorToRGBA(color); }

  void decodeFont(const QFont &font, QString &family, CFontStyle &style, int &pixelSize);

  //---

  QString fullName(const QObject* object);

  QObject *nameToObject(const QString &name);
  QObject *nameToObject(QObject *parent, const QString &name);

  QObject *hierChildObject(QObject *object, int ind, const QStringList &names);

  QObject *childObject(QObject *parent, const QString &name);

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

  QPointF  toQPoint  (const CPoint2D &point);
  QPoint   toQPointI (const CPoint2D &point);
  CPoint2D fromQPoint(const QPointF &point);

  QPoint    toQPoint   (const CIPoint2D &point);
  CIPoint2D fromQPoint (const QPoint &point);
  CPoint2D  fromQPointF(const QPoint &point);

  QSizeF  toQSize  (const CSize2D &size);
  CSize2D fromQSize(const QSizeF &size);

  QSize    toQSize  (const CISize2D &size);
  CISize2D fromQSize(const QSize &size);

  QRectF  toQRect  (const CBBox2D &rect);
  QRect   toQRectI (const CBBox2D &rect);
  CBBox2D fromQRect(const QRectF &rect);

  QRect    toQRect  (const CIBBox2D &rect);
  CIBBox2D fromQRect(const QRect &rect);

  QMatrix   toQMatrix  (const CMatrix2D &m);
  CMatrix2D fromQMatrix(const QMatrix &m);

  QTransform toQTransform  (const CMatrix2D &m);
  CMatrix2D  fromQTransform(const QTransform &t);

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

#ifdef CQUTIL_GRADIENT
  QLinearGradient toQGradient(const CLinearGradient *lgradient,
                              QGradient::CoordinateMode mode=QGradient::ObjectBoundingMode);
  QRadialGradient toQGradient(const CRadialGradient *rgradient,
                              QGradient::CoordinateMode mode=QGradient::ObjectBoundingMode);
#endif

  void getScreenSize(uint *w, uint *h);

  void setSelectText(const std::string &text);

  void setDockVisible(QDockWidget *dock, bool visible);

#ifdef CQUTIL_LINE_DASH
  void penSetLineDash(QPen &pen, const CLineDash &dash);
#endif

  QString alignToString(Qt::Alignment align);
  bool stringToAlign(const QString &str, Qt::Alignment &align);

  Qt::Alignment toQAlign(CHAlignType halign);
  Qt::Alignment toQAlign(CVAlignType valign);

  CHAlignType toHAlign(Qt::Alignment align);
  CVAlignType toVAlign(Qt::Alignment align);

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
