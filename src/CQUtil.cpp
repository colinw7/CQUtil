#include <CQUtil.h>
#include <CQUtilRGBA.h>
#include <CQUtilLineDash.h>
#include <CQUtilAngle.h>
#include <CQUtilFont.h>
#include <CQUtilLineCap.h>
#include <CQUtilLineJoin.h>
#include <CQUtilAlignType.h>
#include <CQUtilGeom.h>
#include <CQUtilGradient.h>
#include <CQStyleMgr.h>

//#include <CQImageUtil.h>
//#include <CQUtilMeta.h>

#include <CRegExp.h>

#ifdef CQUTIL_LINE_DASH
#include <CLineDash.h>
#endif

#ifdef CQUTIL_ANGLE
#include <CAngle.h>
#endif

#ifdef CQUTIL_GRADIENT
#include <CLinearGradient.h>
#include <CRadialGradient.h>
#endif

#include <Qt>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QDockWidget>
#include <QAction>
#include <QMouseEvent>
#include <QMetaObject>
#include <QMetaProperty>
#include <QPalette>
#include <QLabel>
#include <QClipboard>
#include <QAbstractButton>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QBuffer>
#include <QGridLayout>

#ifdef CQUTIL_LINE_DASH
CQUTIL_DEF_META_TYPE_STD(CLineDash, toString, fromString)
#endif

#ifdef CQUTIL_ANGLE
CQUTIL_DEF_META_TYPE_STD(CAngle, toString, fromString)
#endif

//------

namespace {

QString metaMethodSignature(QMetaMethod &m) {
#if QT_VERSION < 0x050000
  return m.signature();
#else
  return m.methodSignature();
#endif
}

using ObjectAlias = std::map<QObject *, QString>;
using AliasObject = std::map<QString, QObject *>;

static ObjectAlias s_objectAlias;
static AliasObject s_aliasObject;

}

//------

void
CQUtil::
initProperties()
{
#ifdef CQUTIL_LINE_DASH
  CQUTIL_REGISTER_META(CLineDash);
#endif

#ifdef CQUTIL_ANGLE
  CQUTIL_REGISTER_META(CAngle);
#endif
}

//------

Qt::PenCapStyle
CQUtil::
toPenCapStyle(const CLineCapType &lineCap)
{
  switch (lineCap) {
    default:
    case LINE_CAP_TYPE_BUTT  : return Qt::FlatCap;
    case LINE_CAP_TYPE_ROUND : return Qt::RoundCap;
    case LINE_CAP_TYPE_SQUARE: return Qt::SquareCap;
  }
}

Qt::PenJoinStyle
CQUtil::
toPenJoinStyle(const CLineJoinType &lineJoin)
{
  switch (lineJoin) {
    default:
    case LINE_JOIN_TYPE_MITRE: return Qt::MiterJoin;
    case LINE_JOIN_TYPE_ROUND: return Qt::RoundJoin;
    case LINE_JOIN_TYPE_BEVEL: return Qt::BevelJoin;
  }
}

void
CQUtil::
decodeFont(const QFont &qfont, QString &family, CFontStyle &style, int &pixelSize)
{
  family = qfont.family();

  if      (qfont.bold() && qfont.italic())
    style = CFONT_STYLE_BOLD_ITALIC;
  else if (qfont.bold())
    style = CFONT_STYLE_BOLD;
  else if (qfont.italic())
    style = CFONT_STYLE_ITALIC;
  else
    style = CFONT_STYLE_NORMAL;

  pixelSize = qfont.pixelSize();

  if (pixelSize == -1) {
    //int dpi = QX11Info::appDpiY();
    //int dpi = QApplication::desktop()->logicalDpiY(); (Same ?)

    int pointSize = qfont.pointSize();

    //pixelSize = pointSize*dpi/72;

    pixelSize = pointSize;
  }
}

//---

QString
CQUtil::
fullName(const QObject *object)
{
  if (! object)
    return "";

  if (object == qApp)
    return QApplication::applicationName();

  QString name;

  QString sep("|");

  const auto *o = object;

  QString id;

  while (o) {
    if (name.length())
      name = o->objectName() + sep + name;
    else
      name = o->objectName();

    o = o->parent();
  }

  return name;
}

QObject *
CQUtil::
nameToObject(const QString &name)
{
  if (! name.length())
    return nullptr;

//if (name == QApplication::applicationName())
  if (name == "APPLICATION")
    return qApp;

  if (name == "STYLE_MGR")
    return CQStyleMgrInst;

  auto po = s_aliasObject.find(name);

  if (po != s_aliasObject.end())
    return (*po).second;

  auto names = name.split("|");

  int num_names = names.size();

  if (num_names == 0)
    return nullptr;

  auto baseName = names[0];

  QObject *current = nullptr;

  auto wlist = QApplication::topLevelWidgets();

  for (auto *w : wlist) {
    if (w->objectName() == baseName) {
      current = w;
      break;
    }
  }

  if (! current) {
    for (auto *w : wlist) {
      auto *l = w->layout();

      if (l && l->objectName() == baseName) {
        current = l;
        break;
      }
    }
  }

  if (! current)
    return nullptr;

  if (names.size() > 1)
    return hierChildObject(current, 1, names);
  else
    return current;
}

QObject *
CQUtil::
hierChildObject(QObject *object, int ind, const QStringList &names)
{
  auto children = childObjects(object, names[ind]);

  if (children.length() > 0) {
    auto *child = children[0];

    ++ind;

    if (ind < names.size())
      return hierChildObject(child, ind, names);
    else
      return child;
  }

  if (names[ind] == "*") {
    const auto &children = object->children();

    for (auto *child : children) {
      auto *child1 = hierChildObject(child, ind + 1, names);

      if (child1)
        return child1;
    }
  }

  return nullptr;
}

QObject *
CQUtil::
childObject(QObject *parent, const QString &name)
{
  return parent->findChild<QObject *>(name);
}

QList<QObject *>
CQUtil::
childObjects(QObject *parent, const QString &name)
{
  return parent->findChildren<QObject *>(name);
}

QString
CQUtil::
addObjectAlias(QObject *object)
{
  auto po = s_objectAlias.find(object);

  if (po != s_objectAlias.end())
    return (*po).second;

  auto name = object->objectName();

  if (name == "") {
    nameObject(object);

    name = object->objectName();
  }

  auto pn = s_aliasObject.find(name);

  if (pn != s_aliasObject.end()) {
    auto baseName = name;

    int num = 1;

    name = QString("%1_%2").arg(baseName).arg(num);

    pn = s_aliasObject.find(name);

    while (pn != s_aliasObject.end()) {
      ++num;

      name = QString("%1_%2").arg(baseName).arg(num);
    }
  }

  s_objectAlias[object] = name;
  s_aliasObject[name]   = object;

  return name;
}

//---

QWidget *
CQUtil::
getToplevelWidget(QWidget *widget)
{
  auto *parent = widget;

  QWidget *parent1;

  while ((parent1 = parent->parentWidget()) != nullptr)
    parent = parent1;

  return parent;
}

int
CQUtil::
getNumProperties(const QObject *object, bool inherited)
{
  if (! object)
    return 0;

  const auto *metaObject = object->metaObject();

  if (! metaObject)
    return 0;

  int firstProp = (inherited ? 0 : metaObject->propertyOffset());

  return metaObject->propertyCount() - firstProp;
}

QStringList
CQUtil::
getPropertyList(const QObject *object, bool inherited)
{
  const auto *metaObject = object->metaObject();

  return getPropertyList(object, inherited, metaObject);
}

QStringList
CQUtil::
getPropertyList(const QObject *object, bool inherited, const QMetaObject *metaObject)
{
  QStringList names;

  if (! object || ! metaObject)
    return names;

  int firstProp = (inherited ? 0 : metaObject->propertyOffset());

  for (int p = firstProp; p < metaObject->propertyCount(); ++p) {
    auto metaProperty = metaObject->property(p);

    names.push_back(metaProperty.name());
  }

  return names;
}

QString
CQUtil::
getPropertyName(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return QString();

  return metaProperty.name();
}

QVariant::Type
CQUtil::
getPropertyType(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return QVariant::Invalid;

  return metaProperty.type();
}

QString
CQUtil::
getPropertyTypeName(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return "";

  return metaProperty.typeName();
}

QVariant
CQUtil::
getPropertyValue(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return QVariant();

  return object->property(metaProperty.name());
}

bool
CQUtil::
getPropertyValueIsEnum(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return false;

  return metaProperty.isEnumType();
}

bool
CQUtil::
getPropertyValueIsFlag(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return false;

  return metaProperty.isFlagType();
}

QString
CQUtil::
getPropertyEnumName(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;
  QMetaEnum     metaEnum;
  bool          isFlag;

  if (! getMetaPropertyEnum(object, ind, inherited, metaProperty, metaEnum, isFlag))
    return QString();

  return metaEnum.name();
}

QString
CQUtil::
getPropertyEnumValue(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;
  QMetaEnum     metaEnum;
  bool          isFlag;

  if (! getMetaPropertyEnum(object, ind, inherited, metaProperty, metaEnum, isFlag))
    return QString();

  auto value = object->property(metaProperty.name());

  int eind = value.toInt();

  if (isFlag)
    return metaEnum.valueToKeys(eind);
  else
    return metaEnum.valueToKey(eind);
}

bool
CQUtil::
enumPropertyValueToString(const QObject *object, int ind, bool inherited, int value, QString &str)
{
  str = "";

  QMetaProperty metaProperty;
  QMetaEnum     metaEnum;
  bool          isFlag;

  if (! getMetaPropertyEnum(object, ind, inherited, metaProperty, metaEnum, isFlag))
    return false;

  if (isFlag)
    str = metaEnum.valueToKeys(value);
  else
    str = metaEnum.valueToKey(value);

  return true;
}

QStringList
CQUtil::
getMetaPropertyEnumNames(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;
  QMetaEnum     metaEnum;
  bool          isFlag;

  if (! getMetaPropertyEnum(object, ind, inherited, metaProperty, metaEnum, isFlag))
    return QStringList();

  QStringList names;

  for (int i = 0; i < metaEnum.keyCount(); ++i)
    names.push_back(metaEnum.key(i));

  return names;
}

QList<int>
CQUtil::
getMetaPropertyEnumValues(const QObject *object, int ind, bool inherited)
{
  QMetaProperty metaProperty;
  QMetaEnum     metaEnum;
  bool          isFlag;

  if (! getMetaPropertyEnum(object, ind, inherited, metaProperty, metaEnum, isFlag))
    return QList<int>();

  QList<int> values;

  for (int i = 0; i < metaEnum.keyCount(); ++i)
    values.push_back(metaEnum.value(i));

  return values;
}

bool
CQUtil::
getMetaPropertyEnumNameValues(const QObject *object, int ind, bool inherited,
                              QStringList &names, QList<int> &values)
{
  QMetaProperty metaProperty;
  QMetaEnum     metaEnum;
  bool          isFlag;

  if (! getMetaPropertyEnum(object, ind, inherited, metaProperty, metaEnum, isFlag))
    return false;

  for (int i = 0; i < metaEnum.keyCount(); ++i) {
    names .push_back(metaEnum.key  (i));
    values.push_back(metaEnum.value(i));
  }

  return true;
}

bool
CQUtil::
getMetaPropertyEnum(const QObject *object, int ind, bool inherited,
                    QMetaProperty &metaProperty, QMetaEnum &metaEnum, bool &isFlag)
{
  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return false;

  if (! metaProperty.isEnumType())
    return false;

  metaEnum = metaProperty.enumerator();
  isFlag   = metaProperty.isFlagType();

  return true;
}

bool
CQUtil::
getMetaProperty(const QObject *object, int ind, bool inherited, QMetaProperty &metaProperty)
{
  if (! object)
    return false;

  const auto *metaObject = object->metaObject();

  if (! metaObject)
    return false;

  int firstProp = (inherited ? 0 : metaObject->propertyOffset());

  if (ind < 0 || firstProp + ind >= metaObject->propertyCount())
    return false;

  metaProperty = metaObject->property(firstProp + ind);

  return true;
}

bool
CQUtil::
hasProperty(const QObject *object, const QString &propName)
{
  if (! object)
    return false;

  if (propName.isEmpty())
    return false;

  auto *obj = const_cast<QObject *>(object);

  const auto *meta = obj->metaObject();
  if (! meta) return false;

  int propIndex = meta->indexOfProperty(propName.toLatin1().data());
  if (propIndex < 0) return false;

  return true;
}

bool
CQUtil::
hasWritableProperty(const QObject *object, const QString &propName)
{
  if (! object)
    return false;

  if (propName.isEmpty())
    return false;

  auto *obj = const_cast<QObject *>(object);

  const auto *meta = obj->metaObject();
  if (! meta) return false;

  int propIndex = meta->indexOfProperty(propName.toLatin1().data());
  if (propIndex < 0) return false;

  auto metaProperty = meta->property(propIndex);

  if (! metaProperty.isWritable())
    return false;

  return true;
}

bool
CQUtil::
getProperty(const QObject *object, const QString &propName, QVariant &v)
{
  if (! object)
    return false;

  if (propName.isEmpty())
    return false;

  auto *obj = const_cast<QObject *>(object);

  const auto *meta = obj->metaObject();
  if (! meta) return false;

  int propIndex = meta->indexOfProperty(propName.toLatin1().data());
  if (propIndex < 0) return false;

//auto metaProperty = meta->property(propIndex);

  v = obj->property(propName.toLatin1().data());

  return true;
}

bool
CQUtil::
getTclProperty(const QObject *object, const QString &propName, QVariant &v)
{
  if (! object)
    return false;

  if (propName.isEmpty())
    return false;

  auto *obj = const_cast<QObject *>(object);

  const auto *meta = obj->metaObject();
  if (! meta) return false;

  int propIndex = meta->indexOfProperty(propName.toLatin1().data());
  if (propIndex < 0) return false;

  v = obj->property(propName.toLatin1().data());

  auto metaProperty = meta->property(propIndex);

  if (metaProperty.isEnumType()) {
    auto metaEnum = metaProperty.enumerator();
    bool isFlag   = metaProperty.isFlagType();

    int eind = v.toInt();

    QString str;

    if (isFlag)
      str = metaEnum.valueToKeys(eind);
    else
      str = metaEnum.valueToKey(eind);

    v = QVariant(str);
  }

  return true;
}

bool
CQUtil::
setPropertyValue(QObject *object, int ind, const QVariant &value, bool inherited)
{
  if (! object)
    return false;

  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return false;

  if (! metaProperty.isWritable())
    return false;

  return object->setProperty(metaProperty.name(), value);
}

bool
CQUtil::
setProperty(const QObject *object, const QString &propName, const QString &str)
{
  if (! object)
    return false;

  if (propName.isEmpty())
    return false;

  auto *obj = const_cast<QObject *>(object);

  const auto *meta = obj->metaObject();
  if (! meta) return false;

  int propIndex = meta->indexOfProperty(propName.toLatin1().data());
  if (propIndex < 0) return false;

  auto metaProperty = meta->property(propIndex);
  if (! metaProperty.isWritable()) return false;

  QVariant v;

  (void) getProperty(object, propName, v);

  if (! stringToVariant(str, metaProperty.type(), metaProperty.typeName(),
                        metaProperty.userType(), v, v))
    return false;

  return obj->setProperty(propName.toLatin1().data(), v);
}

bool
CQUtil::
setProperty(const QObject *object, const QString &propName, const QVariant &v)
{
  if (! object)
    return false;

  if (propName.isEmpty())
    return false;

  auto *obj = const_cast<QObject *>(object);

  const auto *meta = obj->metaObject();
  if (! meta) return false;

  int propIndex = meta->indexOfProperty(propName.toLatin1().data());
  if (propIndex < 0) return false;

  auto metaProperty = meta->property(propIndex);
  if (! metaProperty.isWritable()) return false;

  auto v1 = v;

  auto typeName = QString(v1.typeName());

  if (typeName == "QString" && metaProperty.isEnumType()) {
    auto v1Str = v1.toString().toLower();

    int value = 0;

    if (! stringToEnumValue(v1Str, metaProperty, value))
      return false;

    v1 = QVariant(value);

    typeName = v1.typeName();
  }

  if (typeName == "QString") {
    auto v1Str = v1.toString();

    QVariant v2;

    (void) getProperty(object, propName, v2);

    if (! stringToVariant(v1Str, metaProperty.type(), metaProperty.typeName(),
                          metaProperty.userType(), v1, v2))
      return false;

    typeName = metaProperty.typeName();
  }

  if (metaProperty.type() == QVariant::UserType) {
    if (typeName != metaProperty.typeName()) {
      auto str = v.toString();

      QVariant v2;

      (void) getProperty(object, propName, v2);

      if (userVariantFromString(v2, str))
        v1 = v2;
    }
  }

  return obj->setProperty(propName.toLatin1().data(), v1);
}

bool
CQUtil::
enumPropertyStringToValue(const QObject *object, int ind, bool inherited,
                          const QString &str, int &value)
{
  value = 0;

  if (! object)
    return false;

  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return false;

  if (! metaProperty.isEnumType())
    return false;

  return stringToEnumValue(str, metaProperty, value);
}

bool
CQUtil::
stringToEnumValue(const QString &str, const QMetaProperty &metaProperty, int &value)
{
  auto metaEnum = metaProperty.enumerator();

  if (metaProperty.isFlagType()) {
    bool found = false;

    auto strs = str.split("|", Qt::SkipEmptyParts);

    for (int i = 0; i < strs.length(); ++i) {
      int value1;

      if (stringToEnumSubValue(strs[i], metaEnum, value1)) {
        if (value1)
          value |= value1;

        found = true;
      }
    }

    if (! found)
      return false;
  }
  else {
    if (! stringToEnumSubValue(str, metaEnum, value))
      return false;
  }

  return true;
}

bool
CQUtil::
stringToEnumSubValue(const QString &str, const QMetaEnum &metaEnum, int &value)
{
  value = -1;

  auto lstr = str.toLower();

  int num_enums = metaEnum.keyCount();

  for (int i = 0; i < num_enums; ++i) {
    int value1 = metaEnum.value(i);

    auto lstr1 = QString(metaEnum.valueToKey(value1)).toLower();

    if (lstr == lstr1) {
      value = value1;
      return true;
    }
  }

  return false;
}

bool
CQUtil::
getPropertyInfo(const QObject *object, int ind, PropInfo *propInfo, bool inherited)
{
  if (! object)
    return false;

  QMetaProperty metaProperty;

  if (! getMetaProperty(object, ind, inherited, metaProperty))
    return false;

  auto *obj = const_cast<QObject *>(object);

  const auto *meta = obj->metaObject();

  if (! meta)
    return false;

  propInfo->init(metaProperty);

  return true;
}

bool
CQUtil::
getPropInfo(const QObject *object, const QString &propName, PropInfo *propInfo)
{
  if (! object)
    return false;

  if (propName.isEmpty())
    return false;

  auto *obj = const_cast<QObject *>(object);

  const auto *meta = obj->metaObject();
  if (! meta) return false;

  int propIndex = meta->indexOfProperty(propName.toLatin1().data());
  if (propIndex < 0) return false;

  auto metaProperty = meta->property(propIndex);

  propInfo->init(metaProperty);

  return true;
}

bool
CQUtil::
getPropInfoEnumNameValue(const CQUtil::PropInfo &propInfo, const QString &name, int &value)
{
  return propInfo.enumNameValue(name, value);
}

bool
CQUtil::
getPropInfoEnumValueName(const CQUtil::PropInfo &propInfo, int value, QString &name)
{
  return propInfo.enumValueName(value, name);
}

bool
CQUtil::
getPropInfoEnumFlagValueName(const CQUtil::PropInfo &propInfo, int value, QString &name)
{
  name = "";

  QString zeroName;
  for (const auto &ename : propInfo.enumNames()) {
    int evalue = 0; (void) propInfo.enumNameValue(ename, evalue);

    if (evalue) {
      if (value & evalue) {
        if (name != "")
          name += "|";

        name += ename;
      }
    }
    else {
      zeroName = ename;
    }
  }

  if (name == "")
    name = zeroName;

  return true;
}

//---

QString
CQUtil::
className(const QObject *object)
{
  const auto *mo = object->metaObject();

  if (! mo)
    return "";

  return mo->className();
}

QStringList
CQUtil::
hierClassNames(const QObject *object)
{
  QStringList names;

  const auto *mo = object->metaObject();

  while (mo) {
    names.push_back(mo->className());

    mo = mo->superClass();
  }

  return names;
}

const QMetaObject *
CQUtil::
baseClass(QMetaObject *metaObject, const char *name)
{
  const auto *baseMetaObject = metaObject;

  QString str(name);

  while (baseMetaObject) {
    if (str == baseMetaObject->className())
      break;

    baseMetaObject = baseMetaObject->superClass();
  }

  return baseMetaObject;
}

int
CQUtil::
numSignals(const QObject *object, bool inherited)
{
  if (! object)
    return 0;

  const auto *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  int num = 0;

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    auto metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Signal)
      continue;

    ++num;
  }

  return num;
}

QString
CQUtil::
signalName(const QObject *object, int ind, bool inherited)
{
  if (! object)
    return QString();

  const auto *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  int num = 0;

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    auto metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Signal)
      continue;

    if (num == ind)
      return metaMethodSignature(metaMethod);

    ++num;
  }

  return "";
}

QStringList
CQUtil::
signalNames(const QObject *object, bool inherited)
{
  QStringList names;

  if (! object)
    return names;

  const auto *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    auto metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Signal)
      continue;

    names.append(metaMethodSignature(metaMethod));
  }

  return names;
}

int
CQUtil::
numSlots(const QObject *object, bool inherited)
{
  if (! object)
    return 0;

  const auto *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  int num = 0;

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    auto metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Slot)
      continue;

    ++num;
  }

  return num;
}

QString
CQUtil::
slotName(const QObject *object, int ind, bool inherited)
{
  if (! object)
    return QString();

  const auto *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  int num = 0;

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    auto metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Slot)
      continue;

    if (ind == num)
      return metaMethodSignature(metaMethod);

    ++num;
  }

  return QString();
}

QStringList
CQUtil::
slotNames(const QObject *object, bool inherited)
{
  QStringList names;

  const auto *meta = object->metaObject();

  int start = (inherited ? 0 : meta->methodOffset());

  for (int pos = start; pos < meta->methodCount(); ++pos) {
    auto metaMethod = meta->method(pos);

    if (metaMethod.attributes() & QMetaMethod::Compatibility)
      continue;

    if (metaMethod.methodType() != QMetaMethod::Slot)
      continue;

    names.append(metaMethodSignature(metaMethod));
  }

  return names;
}

QString
CQUtil::
eventTypeToName(QEvent::Type type)
{
  switch (type) {
    case QEvent::KeyPress:
      return "KeyPress";
    case QEvent::KeyRelease:
      return "KeyRelease";
    case QEvent::MouseButtonDblClick:
      return "MouseButtonDblClick";
    case QEvent::MouseButtonPress:
      return "MouseButtonPress";
    case QEvent::MouseButtonRelease:
      return "MouseButtonRelease";
    case QEvent::MouseMove:
      return "MouseMove";
    case QEvent::MouseTrackingChange:
      return "MouseTrackingChange";
    default:
      return "Unknown";
  }
}

QString
CQUtil::
eventToString(QEvent *event)
{
  auto name = eventTypeToName(event->type());

  return name;
}

void
CQUtil::
setForeground(QWidget *widget, const QColor &color)
{
  auto palette = widget->palette();

  palette.setColor(widget->foregroundRole(), color);

  widget->setPalette(palette);
}

QColor
CQUtil::
getForeground(QWidget *widget)
{
  auto palette = widget->palette();

  return palette.color(widget->foregroundRole());
}

void
CQUtil::
setBackground(QWidget *widget, const QColor &color)
{
  auto palette = widget->palette();

  palette.setColor(widget->backgroundRole(), color);

  widget->setPalette(palette);

  widget->setAutoFillBackground(true);
}

QColor
CQUtil::
getBackground(QWidget *widget)
{
  auto palette = widget->palette();

  return palette.color(widget->backgroundRole());
}

QString
CQUtil::
variantToString(const QVariant &var)
{
  QString valueStr;

  variantToString(var, valueStr);

  return valueStr;
}

bool
CQUtil::
variantToString(const QVariant &var, QString &valueStr)
{
  valueStr = "";

  auto type = var.type();

  if      (type == QVariant::Palette) {
#ifdef CQUTIL_PALETTE
    auto palette = var.value<QPalette>();

    valueStr = CQUtil::paletteToString(palette);
#endif
  }
  else if (type == QVariant::Point) {
    auto point = var.value<QPoint>();

    valueStr = QString("%1 %2").arg(point.x()).arg(point.y());
  }
  else if (type == QVariant::PointF) {
    auto point = var.value<QPointF>();

    valueStr = QString("%1 %2").arg(point.x()).arg(point.y());
  }
  else if (type == QVariant::Rect) {
    auto rect = var.value<QRect>();

    valueStr = QString("{%1 %2} {%3 %4}").
               arg(rect.left ()).arg(rect.bottom()).
               arg(rect.right()).arg(rect.top   ());
  }
  else if (type == QVariant::RectF) {
    auto rect = var.value<QRectF>();

    valueStr = QString("{%1 %2} {%3 %4}").
               arg(rect.left ()).arg(rect.bottom()).
               arg(rect.right()).arg(rect.top   ());
  }
  else if (type == QVariant::Size) {
    auto size = var.value<QSize>();

    valueStr = QString("%1 %2").arg(size.width()).arg(size.height());
  }
  else if (type == QVariant::SizeF) {
    auto size = var.value<QSizeF>();

    valueStr = QString("%1 %2").arg(size.width()).arg(size.height());
  }
  else if (type == QVariant::Bool) {
    bool b = var.value<bool>();

    valueStr = (b ? "true" : "false");
  }
  else if (type == QVariant::Region) {
    auto region = var.value<QRegion>();

    auto r = region.boundingRect();

    valueStr = QString("{%1 %2} {%3 %4}").
               arg(r.left ()).arg(r.bottom()).
               arg(r.right()).arg(r.top   ());
  }
  else if (type == QVariant::Locale) {
    auto locale = var.value<QLocale>();

    valueStr = locale.name();
  }
  else if (type == QVariant::SizePolicy) {
    auto sp = var.value<QSizePolicy>();

    valueStr = QString("%1 %2 %3 %4").arg(policyToString(sp.horizontalPolicy())).
                                      arg(policyToString(sp.verticalPolicy  ())).
                                      arg(sp.horizontalStretch()).
                                      arg(sp.verticalStretch  ());
  }
  else if (type == QVariant::UserType) {
    if (! var.typeName()) {
      return false;
    }
#ifdef CQUTIL_LINE_DASH
    else if (strcmp(var.typeName(), "CLineDash") == 0) {
      auto lineDash = var.value<CLineDash>();

      valueStr = lineDash.toString().c_str();
    }
#endif
#ifdef CQUTIL_ANGLE
    else if (strcmp(var.typeName(), "CAngle") == 0) {
      auto angle = var.value<CAngle>();

      valueStr = angle.toString().c_str();
    }
#endif
    else
      return false;
  }
  else {
    if (! var.canConvert(QVariant::String))
      return false;

    valueStr = var.toString();
  }

  return true;
}

bool
CQUtil::
stringToVariant(const QString &str, QVariant::Type type, const char *typeName,
                int userType, QVariant &var, const QVariant &oldVar)
{
  // Qt supports QString ->
  //   QVariant::StringList, QVariant::ByteArray, QVariant::Int      , QVariant::UInt,
  //   QVariant::Bool      , QVariant::Double   , QVariant::Date     , QVariant::Time,
  //   QVariant::DateTime  , QVariant::LongLong , QVariant::ULongLong, QVariant::Char,
  //   QVariant::Url       , QVariant::Uuid

  if      (type == QVariant::Bool) {
    auto lstr = str.toLower();

    if      (lstr == "0" || lstr == "false" || lstr == "no"  || lstr == "off")
      var = QVariant(false);
    else if (lstr == "1" || lstr == "true"  || lstr == "yes" || lstr == "on" )
      var = QVariant(true);
    else
      return false;

    return true;
  }
  else if (type == QVariant::Int) {
    bool ok;
    int i = str.toInt(&ok);
    if (! ok) return false;

    var = QVariant(i);

    return true;
  }
  else if (type == QVariant::Point) {
    QRegExp rx("\\s*(\\S+)\\s+(\\S+)\\s*");

    int pos = rx.indexIn(str);

    if (pos == -1)
      return false;

    bool b1; int x = rx.cap(1).toInt(&b1);
    bool b2; int y = rx.cap(2).toInt(&b2);

    if (! b1 && ! b2)
      return false;

    QPoint p(x, y);

    var = QVariant(p);

    return true;
  }
  else if (type == QVariant::PointF) {
    QRegExp rx("\\s*(\\S+)\\s+(\\S+)\\s*");

    int pos = rx.indexIn(str);

    if (pos == -1)
      return false;

    bool b1; double x = rx.cap(1).toDouble(&b1);
    bool b2; double y = rx.cap(2).toDouble(&b2);

    if (! b1 && ! b2)
      return false;

    QPointF p(x, y);

    var = QVariant(p);

    return true;
  }
  else if (type == QVariant::Rect) {
    int  x1, y1, x2, y2;
    bool b1, b2, b3, b4;

    QRegExp rx("\\{\\s*(\\S+)\\s+(\\S+)\\s*\\}\\s*\\{\\s*(\\S+)\\s+(\\S+)\\s*\\}");

    int pos = rx.indexIn(str);

    if (pos != -1) {
      x1 = rx.cap(1).toInt(&b1);
      y1 = rx.cap(2).toInt(&b2);
      x2 = rx.cap(3).toInt(&b3);
      y2 = rx.cap(4).toInt(&b4);
    }
    else {
      QRegExp rx1("\\{\\s*(\\S+)\\s+(\\S+)\\s*(\\S+)\\s+(\\S+)\\s*\\}");

      int pos1 = rx1.indexIn(str);

      if (pos1 != -1) {
        x1 = rx1.cap(1).toInt(&b1);
        y1 = rx1.cap(2).toInt(&b2);
        x2 = rx1.cap(3).toInt(&b3);
        y2 = rx1.cap(4).toInt(&b4);
      }
      else {
        QRegExp rx2("\\s*(\\S+)\\s+(\\S+)\\s*(\\S+)\\s+(\\S+)\\s*");

        int pos2 = rx2.indexIn(str);

        if (pos2 != -1) {
          x1 = rx2.cap(1).toInt(&b1);
          y1 = rx2.cap(2).toInt(&b2);
          x2 = rx2.cap(3).toInt(&b3);
          y2 = rx2.cap(4).toInt(&b4);
        }
        else
          return false;
      }
    }

    if (! b1 && ! b2 && ! b3 && ! b4)
      return false;

    QRect r;

    r.setCoords(x1, y1, x2, y2);

    var = QVariant(r);

    return true;
  }
  else if (type == QVariant::RectF) {
    double x1, y1, x2, y2;
    bool   b1, b2, b3, b4;

    QRegExp rx("\\{\\s*(\\S+)\\s+(\\S+)\\s*\\}\\s*\\{\\s*(\\S+)\\s+(\\S+)\\s*\\}");

    int pos = rx.indexIn(str);

    if (pos != -1) {
      x1 = rx.cap(1).toDouble(&b1);
      y1 = rx.cap(2).toDouble(&b2);
      x2 = rx.cap(3).toDouble(&b3);
      y2 = rx.cap(4).toDouble(&b4);
    }
    else {
      QRegExp rx1("\\{\\s*(\\S+)\\s+(\\S+)\\s*(\\S+)\\s+(\\S+)\\s*\\}");

      int pos1 = rx1.indexIn(str);

      if (pos1 != -1) {
        x1 = rx1.cap(1).toDouble(&b1);
        y1 = rx1.cap(2).toDouble(&b2);
        x2 = rx1.cap(3).toDouble(&b3);
        y2 = rx1.cap(4).toDouble(&b4);
      }
      else {
        QRegExp rx2("\\s*(\\S+)\\s+(\\S+)\\s*(\\S+)\\s+(\\S+)\\s*");

        int pos2 = rx2.indexIn(str);

        if (pos2 != -1) {
          x1 = rx2.cap(1).toDouble(&b1);
          y1 = rx2.cap(2).toDouble(&b2);
          x2 = rx2.cap(3).toDouble(&b3);
          y2 = rx2.cap(4).toDouble(&b4);
        }
        else
          return false;
      }
    }

    if (! b1 && ! b2 && ! b3 && ! b4)
      return false;

    QRectF r;

    r.setCoords(x1, y1, x2, y2);

    var = QVariant(r);

    return true;
  }
  else if (type == QVariant::Size) {
    QRegExp rx("\\s*(\\S+)\\s+(\\S+)\\s*");

    int pos = rx.indexIn(str);

    if (pos == -1)
      return false;

    bool b1; int w = rx.cap(1).toInt(&b1);
    bool b2; int h = rx.cap(2).toInt(&b2);

    if (! b1 && ! b2)
      return false;

    QSize s(w, h);

    var = QVariant(s);

    return true;
  }
  else if (type == QVariant::SizeF) {
    QRegExp rx("\\s*(\\S+)\\s+(\\S+)\\s*");

    int pos = rx.indexIn(str);

    if (pos == -1)
      return false;

    bool b1; double w = rx.cap(1).toDouble(&b1);
    bool b2; double h = rx.cap(2).toDouble(&b2);

    if (! b1 && ! b2)
      return false;

    QSizeF s(w, h);

    var = QVariant(s);

    return true;
  }
  else if (type == QVariant::Font) {
    if (str.length() && (str[0] == '+' || str[0] == '-')) {
      auto str1 = str.mid(1);

      if (oldVar.type() == QVariant::Font) {
        auto oldFont = oldVar.value<QFont>();

        QFontInfo oldFontInfo(oldFont);

        bool ok;

        int delta = str1.toInt(&ok);

        if (ok) {
          auto newFont = oldFont;

          double oldSize = oldFontInfo.pixelSize();

          if (str[0] == '+')
            newFont.setPixelSize(int(oldSize + delta));
          else
            newFont.setPixelSize(int(oldSize - delta));

          var = newFont;

          return true;
        }
      }
    }
  }
  else if (type == QVariant::SizePolicy) {
    auto stringToPolicy = [](const QString &str) {
      auto lstr = str.toLower();

      if (lstr == "fixed"    ) return QSizePolicy::Fixed;
      if (lstr == "minimum"  ) return QSizePolicy::Minimum;
      if (lstr == "maximum"  ) return QSizePolicy::Maximum;
      if (lstr == "preferred") return QSizePolicy::Preferred;
      if (lstr == "expanding") return QSizePolicy::Expanding;

      return QSizePolicy::Fixed;
    };

    auto strs = str.split(" ", Qt::SkipEmptyParts);

    QSizePolicy::Policy hpolicy  { QSizePolicy::Fixed }, vpolicy { QSizePolicy::Fixed };
    int                 hstretch { 0 }, vstretch { 0 };

    if (strs.size() > 0) hpolicy = stringToPolicy(strs[0]);
    if (strs.size() > 1) vpolicy = stringToPolicy(strs[1]);
    if (strs.size() > 2) hstretch = strs[2].toInt();
    if (strs.size() > 3) vstretch = strs[3].toInt();

    QSizePolicy sizePolicy(hpolicy, vpolicy);

    sizePolicy.setHorizontalStretch(hstretch);
    sizePolicy.setVerticalStretch  (vstretch);

    var = QVariant::fromValue(sizePolicy);

    return true;
  }
  else if (type == QVariant::UserType) {
    if (! typeName) {
      return false;
    }
#ifdef CQUTIL_LINE_DASH
    else if (strcmp(typeName, "CLineDash") == 0) {
      CLineDash lineDash(str.toStdString());

      var = QVariant::fromValue(lineDash);

      return true;
    }
#endif
#ifdef CQUTIL_ANGLE
    else if (strcmp(typeName, "CAngle") == 0) {
      CAngle angle;

      angle.fromString(str.toStdString());

      var = QVariant::fromValue(angle);

      return true;
    }
#endif
    else {
      assert(userType != QVariant::UserType);
      //assert(oldVar.userType() == userType);

      var = oldVar;

      if (userVariantFromString(var, str))
        return true;
    }
  }

  //---

  var = QVariant(str);

  if (! var.canConvert(int(type)))
    return false;

  if (! var.convert(int(type)))
    return false;

  return true;
}

#ifdef CQUTIL_PALETTE
bool
CQUtil::
paletteFromString(QPalette &palette, const QString &paletteDef)
{
  CRegExp regexp("fg=\"\\(.*\\)\" bg=\"\\(.*\\)\"");

  if (regexp.find(paletteDef.toStdString())) {
    std::vector<std::string> matches;

    regexp.getSubMatches(matches);

    palette.setColor(QPalette::WindowText, QString(matches[0].c_str()));
    palette.setColor(QPalette::Window    , QString(matches[1].c_str()));
  }

  return true;
}

QString
CQUtil::
paletteToString(const QPalette &palette)
{
  auto fg = palette.color(QPalette::WindowText);
  auto bg = palette.color(QPalette::Window    );

  return QString("fg=\"%1\" bg=\"%2\"").arg(fg.name()).arg(bg.name());
}
#endif

//------------

bool
CQUtil::
activateSlot(QObject *receiver, const char *slotName, const char *valuesStr)
{
#if 0
  QString procName(slotName);

  procName = procName.trimmed();

  auto sigCmd = QString("send_signal");

  sigCmd += procName.mid(procName.find("("));

  // attach a temporary slotmachine
  auto *pSlotMachine = new QtclSlotMachine(0, "temporary");

  bool bOk = QtclSlotMachine::connectWidgets(pSlotMachine, sigCmd, receiver, procName,
                                             0, false, false);

  if (! bOk)
    return false;

  //TODO: distinguish argument type

  QStringList valueList;

  int nValues = QtclUtils::convertToStringList(&valueList, valuesStr);

  if (nValues >= 0) {
    const int MAXPARAM = 4;

    const char *enumType = 0;

    int idx = procName.find("(") + 1;

    // skip blanks
    while (procName[idx].isSpace())
      ++idx;

    QVariant *v[MAXPARAM];

    for (int iValue=0; iValue < MAXPARAM; ++iValue) {
      if (iValue < nValues) {
        v[iValue] = new QVariant(valueList.at(iValue));

        if (procName.mid(idx, 3) == "int")
          v[iValue]->cast(QVariant::Int);

        if (procName.mid(idx, 11) == "Orientation") {
          auto strVal = v[iValue]->asString();
          v[iValue]->cast(QVariant::String);
          enumType = "Orientation";
        }
        else if (procName.mid(idx, 6) == "double")
          v[iValue]->cast(QVariant::Double);
        else if (procName.mid(idx, 4) == "bool") {
          QtclVariant tmp(*v[iValue]);
          tmp.castFromTclString(QVariant::Bool);
          *v[iValue] = tmp.asBool();
          v[iValue]->cast(QVariant::Bool);
        }
        else if (procName.mid(idx, 14) == "const QString&")
          v[iValue]->cast(QVariant::String);
        else if (procName.mid(idx, 13) == "const QColor&") {
          v[iValue]->cast(QVariant::Color);
        }

        if (iValue < nValues - 1) {
          int newIdx = procName.find(",", idx) + 1;

          while (procName[newIdx] == ' ')
            ++newIdx;

          if (newIdx <= idx)
            return false;

          idx = newIdx;
        }
      }
      else
        v[iValue] = 0;
    }

    //now call slot function
    pSlotMachine->emit_signal(MAXPARAM, v, enumType);

    for (int i = 0; i < MAXPARAM; ++i)
      delete v[i];
  }

  // remove temporary slotmachine
  delete pSlotMachine;

  return true;
#else
  QString slotNameStr(slotName);

  slotNameStr = slotNameStr.replace("const QString&", "QString");
  slotNameStr = slotNameStr.replace("const QColor&" , "QColor" );

  int iOpen  = slotNameStr.indexOf("(");
  int iClose = slotNameStr.indexOf(")");

  if (iOpen <= 0 || iClose <= iOpen)
    return false;

  auto plainSlotName = slotNameStr.left(iOpen);

  auto args = slotNameStr.mid(iOpen + 1, iClose - iOpen - 1);

  //------

  // check slot exists

  bool found = false;

  for (const auto *pM = receiver->metaObject(); pM && ! found; pM = pM->superClass()) {
    for (int i = 0; i < pM->methodCount(); ++i) {
      auto metaMethod = pM->method(i);

      if (metaMethod.methodType() != QMetaMethod::Slot)
        continue;

      if (slotNameStr == metaMethodSignature(metaMethod)) {
        found = true;
        break;
      }
    }
  }

  if (! found)
    return false;

  //------

  QGenericArgument qArgs[10];

  auto argTypeList = args.split(",", Qt::SkipEmptyParts);

  auto nArgs = argTypeList.count();

  auto valueList = QString(valuesStr).split(",", Qt::SkipEmptyParts);

  auto nValues = valueList.count();

  if (nArgs != nValues)
    return false;

  std::vector<QColor> colors;

  for (QStringList::size_type iArg = 0; iArg < nArgs; ++iArg) {
    QVariant v(valueList[iArg]);

    QString typeString(argTypeList[iArg]);

    auto type = QVariant::nameToType(typeString.toLatin1().data());

    if (! v.canConvert(int(type))) {
      qDebug("cannot convert slot argument '%s' to type '%s'",
             valueList[iArg].toLatin1().data(), typeString.toLatin1().data());
      return false;
    }

    switch (type) {
      case QVariant::Int:
        qArgs[iArg] = Q_ARG(int, v.toInt());
        break;

      case QVariant::Double:
        qArgs[iArg] = Q_ARG(double, v.toDouble());
        break;

      case QVariant::Bool:
        qArgs[iArg] = Q_ARG(bool, v.toBool());
        break;

      case QVariant::Color: {
        // need to save the color so it doesn't get deleted before we use it
        QColor color(valueList[iArg]);
        colors.push_back(color);
        qArgs[iArg] = Q_ARG(QColor, colors.back());
        break;
      }

      case QVariant::String:
        qArgs[iArg] = Q_ARG(QString, valueList[iArg]);
        break;

      case QVariant::Point:
        qArgs[iArg] = Q_ARG(QPoint, v.value<QPoint>());
        break;

      default:
        qDebug("slot argument of type '%s' not supported", typeString.toLatin1().data());
        break;
    }
  }

  bool bReturn =
    QMetaObject::invokeMethod(receiver, plainSlotName.toLatin1().data(), Qt::AutoConnection,
                              qArgs[0], qArgs[1], qArgs[2], qArgs[3], qArgs[4],
                              qArgs[5], qArgs[6], qArgs[7], qArgs[8], qArgs[9]);

  if (! bReturn) {
    qDebug("cannot activate slot '%s'", slotName);
    return false;
  }

  return true;
#endif
}

bool
CQUtil::
activateSignal(QObject *sender, const char *signalName, const char *valuesStr)
{
  QString signalNameStr(signalName);

  signalNameStr = signalNameStr.replace("const QString&", "QString");
  signalNameStr = signalNameStr.replace("const QColor&" , "QColor" );

  int iOpen  = signalNameStr.indexOf("(");
  int iClose = signalNameStr.indexOf(")");

  if (iOpen <= 0 || iClose <= iOpen)
    return false;

  auto plainSignalName = signalNameStr.left(iOpen);

  auto args = signalNameStr.mid(iOpen + 1, iClose - iOpen - 1);

  //------

  // check signal exists

  bool found = false;

  for (const auto *pM = sender->metaObject(); pM && ! found; pM = pM->superClass()) {
    for (int i = 0; i < pM->methodCount(); ++i) {
      auto metaMethod = pM->method(i);

      if (metaMethod.methodType() != QMetaMethod::Signal)
        continue;

      if (signalNameStr == metaMethodSignature(metaMethod)) {
        found = true;
        break;
      }
    }
  }

  if (! found)
    return false;

  //------

  QGenericArgument qArgs[10];

  auto argTypeList = args.split(",", Qt::SkipEmptyParts);

  auto nArgs = argTypeList.count();

  auto valueList = QString(valuesStr).split(",", Qt::SkipEmptyParts);

  auto nValues = valueList.count();

  if (nArgs != nValues)
    return false;

  std::vector<QColor> colors;

  for (QStringList::size_type iArg = 0; iArg < nArgs; ++iArg) {
    QVariant v(valueList[iArg]);

    QString typeString(argTypeList[iArg]);

    auto type = QVariant::nameToType(typeString.toLatin1().data());

    if (! v.canConvert(int(type))) {
      qDebug("cannot convert signal argument '%s' to type '%s'",
             valueList[iArg].toLatin1().data(), typeString.toLatin1().data());
      return false;
    }

    switch (type) {
      case QVariant::Int:
        qArgs[iArg] = Q_ARG(int, v.toInt());
        break;

      case QVariant::Double:
        qArgs[iArg] = Q_ARG(double, v.toDouble());
        break;

      case QVariant::Bool:
        qArgs[iArg] = Q_ARG(bool, v.toBool());
        break;

      case QVariant::Color: {
        // need to save the color so it doesn't get deleted before we use it
        QColor color(valueList[iArg]);
        colors.push_back(color);
        qArgs[iArg] = Q_ARG(QColor, colors.back());
        break;
      }

      case QVariant::String:
        qArgs[iArg] = Q_ARG(QString, valueList[iArg]);
        break;

      case QVariant::Point:
        qArgs[iArg] = Q_ARG(QPoint, v.value<QPoint>());
        break;

      default:
        qDebug("signal argument of type '%s' not supported", typeString.toLatin1().data());
        break;
    }
  }

  bool bReturn =
    QMetaObject::invokeMethod(sender, plainSignalName.toLatin1().data(),
                              Qt::AutoConnection,
                              qArgs[0], qArgs[1], qArgs[2], qArgs[3], qArgs[4],
                              qArgs[5], qArgs[6], qArgs[7], qArgs[8], qArgs[9]);

  if (! bReturn) {
    qDebug("cannot activate signal '%s'", signalName);
    return false;
  }

  return true;
}

//----------

namespace {

QDataStream::Version dataStreamVersion() {
#if QT_VERSION >= 0x050000
  return QDataStream::Qt_5_0;
#else
  return QDataStream::Qt_4_0;
#endif
}

}

bool
CQUtil::
userVariantToString(const QVariant &var, QString &str)
{
  QByteArray ba;

  // write user type data to data stream using registered DataStream methods
  QBuffer obuffer(&ba);
  obuffer.open(QIODevice::WriteOnly);

  QDataStream out(&obuffer);
  out.setVersion(dataStreamVersion());

  if (! QMetaType::save(out, var.userType(), var.constData()))
    return false;

  //---

  // read back from data stream as string (assumes registered DataStream method
  // serializes as string
  QBuffer ibuffer(&ba);
  ibuffer.open(QIODevice::ReadOnly);

  QDataStream in(&ibuffer);
  in.setVersion(dataStreamVersion());

  in >> str;

  return true;
}

bool
CQUtil::
variantFromString(QVariant &var, const QString &str)
{
#if 0
  QByteArray ba;

  // write current string to buffer
  QBuffer obuffer(&ba);
  obuffer.open(QIODevice::WriteOnly);

  QDataStream out(&obuffer);
  out.setVersion(dataStreamVersion());

  out << str;

  // create type data from data stream using registered DataStream methods
  QBuffer ibuffer(&ba);
  ibuffer.open(QIODevice::ReadOnly);

  QDataStream in(&ibuffer);
  in.setVersion(dataStreamVersion());

#if 0
  QVariant var1(var.type(), 0);

  // const cast is safe since we operate on a newly constructed variant
  if (! QMetaType::load(in, var.type(), const_cast<void *>(var1.constData())))
    return false;

  if (! var1.isValid())
    return false;

  var = var1;
#else
  //QVariant var1(var.type(), 0);

  // const cast is safe since we operate on a newly constructed variant
  CQUtilMetaData::setResult(true);

  if (! QMetaType::load(in, var.type(), const_cast<void *>(var.constData())))
    return false;

  if (! CQUtilMetaData::getResult())
    return false;

  if (! var.isValid())
    return false;
#endif

  return true;
#else
  return stringToVariant(str, var.type(), var.typeName(), var.userType(), var, var);
#endif
}

bool
CQUtil::
userVariantFromString(QVariant &var, const QString &str)
{
  QByteArray ba;

  // write current string to buffer
  QBuffer obuffer(&ba);
  obuffer.open(QIODevice::WriteOnly);

  QDataStream out(&obuffer);
  out.setVersion(dataStreamVersion());

  out << str;

  // create user type data from data stream using registered DataStream methods
  QBuffer ibuffer(&ba);
  ibuffer.open(QIODevice::ReadOnly);

  QDataStream in(&ibuffer);
  in.setVersion(dataStreamVersion());

#if 0
  QVariant var1(var.userType(), 0);

  // const cast is safe since we operate on a newly constructed variant
  if (! QMetaType::load(in, var.userType(), const_cast<void *>(var1.constData())))
    return false;

  if (! var1.isValid())
    return false;

  var = var1;
#else
  //QVariant var1(var.userType(), 0);

  // const cast is safe since we operate on a newly constructed variant
  CQUtilMetaData::setResult(true);

  if (! QMetaType::load(in, var.userType(), const_cast<void *>(var.constData())))
    return false;

  if (! CQUtilMetaData::getResult())
    return false;

  if (! var.isValid())
    return false;
#endif

  return true;
}

//----------

#ifdef CQUTIL_GRADIENT
QLinearGradient
CQUtil::
toQGradient(const CLinearGradient *lgradient, QGradient::CoordinateMode mode)
{
  QLinearGradient qgradient(lgradient->getX1(), lgradient->getY1(),
                            lgradient->getX2(), lgradient->getY2());

  if      (lgradient->getSpread() == CGRADIENT_SPREAD_PAD)
    qgradient.setSpread(QGradient::PadSpread);
  else if (lgradient->getSpread() == CGRADIENT_SPREAD_REPEAT)
    qgradient.setSpread(QGradient::RepeatSpread);
  else if (lgradient->getSpread() == CGRADIENT_SPREAD_REFLECT)
    qgradient.setSpread(QGradient::ReflectSpread);

  qgradient.setCoordinateMode(mode);

  QGradientStops stops;

  for (CLinearGradient::StopList::const_iterator ps = lgradient->beginStops();
         ps != lgradient->endStops(); ++ps) {
    const CGradientStop &stop = *ps;

    stops.push_back(QGradientStop(stop.getOffset(), CQUtil::rgbaToColor(stop.getColor())));
  }

  qgradient.setStops(stops);

  return qgradient;
}

QRadialGradient
CQUtil::
toQGradient(const CRadialGradient *rgradient, QGradient::CoordinateMode mode)
{
  QRadialGradient qgradient(rgradient->getCenterX(), rgradient->getCenterY(),
                            rgradient->getRadius(),
                            rgradient->getFocusX(), rgradient->getFocusY());

  if      (rgradient->getSpread() == CGRADIENT_SPREAD_PAD)
    qgradient.setSpread(QGradient::PadSpread);
  else if (rgradient->getSpread() == CGRADIENT_SPREAD_REPEAT)
    qgradient.setSpread(QGradient::RepeatSpread);
  else if (rgradient->getSpread() == CGRADIENT_SPREAD_REFLECT)
    qgradient.setSpread(QGradient::ReflectSpread);

  qgradient.setCoordinateMode(mode);

  QGradientStops stops;

  for (CRadialGradient::StopList::const_iterator ps = rgradient->beginStops();
         ps != rgradient->endStops(); ++ps) {
    const CGradientStop &stop = *ps;

    stops.push_back(QGradientStop(stop.getOffset(), CQUtil::rgbaToColor(stop.getColor())));
  }

  qgradient.setStops(stops);

  return qgradient;
}
#endif

void
CQUtil::
getScreenSize(uint *w, uint *h)
{
  //auto r = QApplication::desktop()->screenGeometry();
  auto r = qApp->primaryScreen()->geometry();

  *w = uint(r.width ());
  *h = uint(r.height());
}

void
CQUtil::
setSelectText(const std::string &text)
{
  auto *clipboard = QApplication::clipboard();

  clipboard->setText(text.c_str(), QClipboard::Selection);
}

QPoint
CQUtil::
toQPoint(const CIPoint2D &point)
{
  return QPoint(point.x, point.y);
}

CIPoint2D
CQUtil::
fromQPoint(const QPoint &point)
{
  return CIPoint2D(point.x(), point.y());
}

CPoint2D
CQUtil::
fromQPointF(const QPoint &point)
{
  return CPoint2D(point.x(), point.y());
}

QPointF
CQUtil::
toQPoint(const CPoint2D &point)
{
  return QPointF(point.x, point.y);
}

QPoint
CQUtil::
toQPointI(const CPoint2D &point)
{
  return QPoint(int(point.x), int(point.y));
}

CPoint2D
CQUtil::
fromQPoint(const QPointF &point)
{
  return CPoint2D(point.x(), point.y());
}

QSize
CQUtil::
toQSize(const CISize2D &size)
{
  return QSize(size.width, size.height);
}

CISize2D
CQUtil::
fromQSize(const QSize &size)
{
  return CISize2D(size.width(), size.height());
}

QSizeF
CQUtil::
toQSize(const CSize2D &size)
{
  return QSizeF(size.getWidth(), size.getHeight());
}

CSize2D
CQUtil::
fromQSize(const QSizeF &size)
{
  return CSize2D(size.width(), size.height());
}

QRectF
CQUtil::
toQRect(const CBBox2D &rect)
{
  return QRectF(toQPoint(rect.getLL()), toQPoint(rect.getUR())).normalized();
}

QRect
CQUtil::
toQRectI(const CBBox2D &rect)
{
  return QRect(toQPointI(rect.getLL()), toQPointI(rect.getUR())).normalized();
}

CBBox2D
CQUtil::
fromQRect(const QRectF &rect)
{
  return CBBox2D(fromQPoint(rect.bottomLeft()), fromQPoint(rect.topRight()));
}

QRect
CQUtil::
toQRect(const CIBBox2D &rect)
{
  return QRect(toQPoint(rect.getLL()), toQPoint(rect.getUR())).normalized();
}

CIBBox2D
CQUtil::
fromQRect(const QRect &rect)
{
  return CIBBox2D(fromQPoint(rect.bottomLeft()), fromQPoint(rect.topRight()));
}

QMatrix
CQUtil::
toQMatrix(const CMatrix2D &m)
{
  double a, b, c, d, tx, ty;

  m.getValues(&a, &b, &c, &d, &tx, &ty);

  //return QMatrix(a, b, c, d, tx, ty);
  return QMatrix(a, c, b, d, tx, ty);
}

CMatrix2D
CQUtil::
fromQMatrix(const QMatrix &m)
{
  return CMatrix2D(m.m11(), m.m12(), m.m21(), m.m22(), m.dx(), m.dy());
}

QTransform
CQUtil::
toQTransform(const CMatrix2D &m)
{
  double a, b, c, d, tx, ty;

  m.getValues(&a, &b, &c, &d, &tx, &ty);

  //return QTransform(a, b, c, d, tx, ty);
  return QTransform(a, c, b, d, tx, ty);
}

CMatrix2D
CQUtil::
fromQTransform(const QTransform &t)
{
  return CMatrix2D(t.m11(), t.m12(), t.m21(), t.m22(), t.dx(), t.dy());
}

void
CQUtil::
setDockVisible(QDockWidget *dock, bool visible)
{
  auto *action = dock->toggleViewAction();

  if (visible != ! dock->isHidden())
    action->trigger();
}

#ifdef CQUTIL_LINE_DASH
void
CQUtil::
penSetLineDash(QPen &pen, const CLineDash &dash)
{
  auto num = dash.getNumLengths();

  if (num > 0) {
    pen.setStyle(Qt::CustomDashLine);

    pen.setDashOffset(dash.getOffset());

    QVector<qreal> dashes;

    double w = pen.widthF();

    if (w <= 0.0) w = 1.0;

    for (int i = 0; i < int(num); ++i)
      dashes << dash.getLength(i)*w;

    if (num & 1)
      dashes << dash.getLength(0)*w;

    pen.setDashPattern(dashes);
  }
  else
    pen.setStyle(Qt::SolidLine);
}
#endif

//------

QString
CQUtil::
alignToString(Qt::Alignment align)
{
  QString str;

  if      (align & Qt::AlignLeft   ) str += "AlignLeft";
  else if (align & Qt::AlignRight  ) str += "AlignRight";
  else if (align & Qt::AlignHCenter) str += "AlignHCenter";
  else if (align & Qt::AlignJustify) str += "AlignJustify";

  str += "|";

  if      (align & Qt::AlignTop    ) str += "AlignTop";
  else if (align & Qt::AlignBottom ) str += "AlignBottom";
  else if (align & Qt::AlignVCenter) str += "AlignVCenter";

  return str;
}

bool
CQUtil::
stringToAlign(const QString &str, Qt::Alignment &align)
{
  auto stringAddAlign = [](const QString &str, Qt::Alignment &align) {
    if      (str == "alignleft"   ) align |= Qt::AlignLeft;
    else if (str == "alignright"  ) align |= Qt::AlignRight;
    else if (str == "alignhcenter") align |= Qt::AlignHCenter;
    else if (str == "alignjustify") align |= Qt::AlignJustify;

    else if (str == "aligntop"    ) align |= Qt::AlignTop;
    else if (str == "alignbottom" ) align |= Qt::AlignBottom;
    else if (str == "alignvcenter") align |= Qt::AlignVCenter;

    else if (str == "aligncenter" ) align |= Qt::AlignCenter;

    else                            return false;

    return true;
  };

  //---

  bool rc = true;

  align = Qt::Alignment();

  auto strs = str.split("|");

  for (int i = 0; i < strs.length(); ++i) {
    auto str = strs[i].toLower();

    if (! stringAddAlign(str, align))
      rc = false;
  }

  return rc;
}

Qt::Alignment
CQUtil::
toQAlign(CHAlignType halign)
{
  switch (halign) {
    case CHALIGN_TYPE_LEFT   : return Qt::AlignLeft;
    case CHALIGN_TYPE_CENTER : return Qt::AlignHCenter;
    case CHALIGN_TYPE_RIGHT  : return Qt::AlignRight;
    case CHALIGN_TYPE_JUSTIFY: return Qt::AlignJustify;
    default                  : return Qt::AlignHCenter;
  }
}

Qt::Alignment
CQUtil::
toQAlign(CVAlignType valign)
{
  switch (valign) {
    case CVALIGN_TYPE_TOP     : return Qt::AlignTop;
    case CVALIGN_TYPE_CENTER  : return Qt::AlignVCenter;
    case CVALIGN_TYPE_BOTTOM  : return Qt::AlignBottom;
    case CVALIGN_TYPE_BASELINE: return Qt::AlignBottom;
    default                   : return Qt::AlignVCenter;
  }
}

CHAlignType
CQUtil::
toHAlign(Qt::Alignment align)
{
  switch (align) {
    case Qt::AlignLeft:    return CHALIGN_TYPE_LEFT;
    case Qt::AlignHCenter: return CHALIGN_TYPE_CENTER;
    case Qt::AlignRight:   return CHALIGN_TYPE_RIGHT;
    case Qt::AlignJustify: return CHALIGN_TYPE_JUSTIFY;
    default:               return CHALIGN_TYPE_CENTER;
  }
}

CVAlignType
CQUtil::
toVAlign(Qt::Alignment align)
{
  switch (align) {
    case Qt::AlignTop:     return CVALIGN_TYPE_TOP;
    case Qt::AlignVCenter: return CVALIGN_TYPE_CENTER;
    case Qt::AlignBottom:  return CVALIGN_TYPE_BOTTOM;
    default:               return CVALIGN_TYPE_CENTER;
  }
}

//------------

QPixmap
CQUtil::
getPixmap(const QString &name)
{
  // TODO: search path
  QPixmap pm;

  pm.load(name);

  return pm;
}

QIcon
CQUtil::
getIcon(const QString &name)
{
  return QIcon(getPixmap(name));
}

//------------

uint
CQUtil::
nameWidgetTree(QWidget *widget)
{
  assert(widget != nullptr);

  uint num = 0;

  auto name = widget->objectName();

  if (name.isNull() || name.isEmpty()) {
    nameWidget(widget);

    ++num;
  }

  const auto &children = widget->children();

  for (auto *child : children) {
    auto *widget1 = qobject_cast<QWidget *>(child);

    if (widget1)
      num += nameWidgetTree(widget1);
  }

  return num;
}

void
CQUtil::
nameWidget(QWidget *widget)
{
  nameObject(widget);
}

void
CQUtil::
nameObject(QObject *object)
{
  auto *button = qobject_cast<QAbstractButton *>(object);

  if (button)
    nameWidgetButton(button);
  else {
    auto *label = qobject_cast<QLabel *>(object);

    if (label)
      nameWidgetLabel(label);
    else
      nameObjectGen(object);
  }
}

void
CQUtil::
nameWidgetButton(QAbstractButton *button)
{
  auto text = button->text();

  if (text.isNull() || text.isEmpty())
    return nameObjectGen(static_cast<QWidget*>(button));

  text.replace(QChar(' '), QChar('_'));
  text.replace(QChar('&'), QString(""));
  text.replace(QChar('$'), QString(""));

  button->setObjectName(text);
}

void
CQUtil::
nameWidgetLabel(QLabel *label)
{
  auto text = label->text();

  if (text.isNull() || text.isEmpty())
    return nameObjectGen(static_cast<QWidget*>(label));

  text.replace(QChar(' '), QChar('_'));
  text.replace(QChar('&'), QString(""));
  text.replace(QChar('$'), QString(""));

  label->setObjectName(text);
}

void
CQUtil::
nameObjectGen(QObject *object)
{
  const auto *mo = object->metaObject();

  const char *cname = mo->className();

  int num = 1;

  auto name = QString("%1_%2").arg(cname).arg(num);

  while (object->parent() && object->parent()->findChild<QObject *>(name)) {
    ++num;

    name = QString("%1_%2").arg(cname).arg(num);
  }

  object->setObjectName(name);
}

//------------

void
CQUtil::
recolorImage(QImage &image, const QColor &fg, const QColor &bg)
{
  double fg_r = fg.red  ()/255.0;
  double fg_g = fg.green()/255.0;
  double fg_b = fg.blue ()/255.0;
//double fg_a = fg.alpha()/255.0; needed ?

  double bg_r = bg.red  ()/255.0;
  double bg_g = bg.green()/255.0;
  double bg_b = bg.blue ()/255.0;
//double bg_a = bg.alpha()/255.0; needed ?

  if      (image.format() == QImage::Format_ARGB32) {
    for (int y = 0; y < image.height(); ++y) {
      for (int x = 0; x < image.width(); ++x) {
        auto rgb = image.pixel(x, y);

        int fg_gray = 255 - qGray(rgb);
        int bg_gray = 255 - fg_gray;

        rgb = qRgba(int(fg_gray*fg_r + bg_gray*bg_r),
                    int(fg_gray*fg_g + bg_gray*bg_g),
                    int(fg_gray*fg_b + bg_gray*bg_b), qAlpha(rgb));

        image.setPixel(x, y, rgb);
      }
    }
  }
  else if (image.format() == QImage::Format_Indexed8) {
    int ncolors = image.colorCount();

    for (int i = 0; i < ncolors; ++i) {
      auto rgb = image.color(i);

      int fg_gray = 255 - qGray(rgb);
      int bg_gray = 255 - fg_gray;

      rgb = qRgba(int(fg_gray*fg_r + bg_gray*bg_r),
                  int(fg_gray*fg_g + bg_gray*bg_g),
                  int(fg_gray*fg_b + bg_gray*bg_b), qAlpha(rgb));

      image.setColor(i, rgb);
    }
  }
  else
    assert(false);
}

//------------

void
CQUtil::
drawHtmlText(QWidget *w, QPainter *painter, const QString &text,
             const QPalette &palette, const QRect &rect, bool active)
{
  painter->setRenderHints(QPainter::Antialiasing);

  QTextDocument td;

  td.setHtml(text);

  auto trect = rect.translated(-rect.x(), -rect.y());

  painter->translate(rect.x(), rect.y());

  painter->setClipRect(trect);

  QAbstractTextDocumentLayout::PaintContext ctx;

  if (active)
    ctx.palette.setColor(QPalette::Text, palette.highlightedText().color());
  else
    ctx.palette.setColor(QPalette::Text, palette.text().color());

  auto *layout = td.documentLayout();

  layout->setPaintDevice(w);

  layout->draw(painter, ctx);

  painter->translate(-rect.x(), -rect.y());
}

QString
CQUtil::
colorToHtml(const QColor &c)
{
  // TODO: transparency
  QChar pad('0');

  return QString("#%1%2%3").
    arg(c.red(), 2, 16, pad).arg(c.green(), 2, 16, pad).arg(c.blue(), 2, 16, pad);
}

QString
CQUtil::
policyToString(QSizePolicy::Policy policy)
{
  switch (policy) {
    case QSizePolicy::Fixed           : return "Fixed";
    case QSizePolicy::Minimum         : return "Minimum";
    case QSizePolicy::Maximum         : return "Maximum";
    case QSizePolicy::Preferred       : return "Preferred";
    case QSizePolicy::Expanding       : return "Expanding";
    case QSizePolicy::MinimumExpanding: return "MinimumExpanding";
    case QSizePolicy::Ignored         : return "Ignored";
    default                           : return QString("%1").arg(policy);
  }
}

//------------

QColor
CQUtil::
blendColors(const QColor &c1, const QColor &c2, double f)
{
  CRGBA rgba1 = colorToRGBA(c1);
  CRGBA rgba2 = colorToRGBA(c2);

  CRGBA rgba = rgba1.blended(rgba2, f);

  return rgbaToColor(rgba);
}

//------------

void
CQUtil::PropInfo::
init(const QMetaProperty &mp)
{
  name_       = mp.name();
  type_       = mp.type();
  typeName_   = mp.typeName();
  isWritable_ = mp.isWritable();
  isEnumType_ = mp.isEnumType();
  isFlagType_ = mp.isFlagType();

  enumNames_    .clear();
  enumNameValue_.clear();
  enumValueName_.clear();

  if (isEnumType_) {
    auto metaEnum = mp.enumerator();

    int num_enums = metaEnum.keyCount();

    for (int i = 0; i < num_enums; ++i) {
      int value = metaEnum.value(i);

      auto name = metaEnum.valueToKey(value);

      enumNames_.push_back(name);

      enumNameValue_[name ] = value;
      enumValueName_[value] = name;
    }
  }
}

//------------

double
CQUtil::
polygonArea(const QPolygonF &poly)
{
  return fabs(0.5*polygonArea2(poly));
}

double
CQUtil::
polygonArea2(const QPolygonF &poly)
{
  int n = poly.length();

  double area = 0.0;

  int i1 = n - 1;
  int i2 = 0;

  for ( ; i2 < n; i1 = i2++)
    area += poly[i1].x()*poly[i2].y() - poly[i1].y()*poly[i2].x();

  return area;
}

//------

/**
 * Helper function. Deletes all child widgets of the given layout @a item.
 */
void
CQUtil::
deleteLayoutChildWidgets(QLayoutItem *item)
{
  if (item->layout()) {
    // Process all child items recursively.
    for (int i = 0; i < item->layout()->count(); i++) {
      deleteLayoutChildWidgets(item->layout()->itemAt(i));
    }
  }
  delete item->widget();
}

/**
 * Helper function. Removes all layout items within the given @a layout
 * which either span the given @a row or @a column. If @a deleteWidgets
 * is true, all concerned child widgets become not only removed from the
 * layout, but also deleted.
 */
void
CQUtil::
removeGridItem(QGridLayout *layout, int row, int column, bool deleteWidgets)
{
  // We avoid usage of QGridLayout::itemAtPosition() here to improve performance.
  for (int i = layout->count() - 1; i >= 0; i--) {
    int r, c, rs, cs;

    layout->getItemPosition(i, &r, &c, &rs, &cs);

    if ((r <= row && r + rs - 1 >= row) || (c <= column && c + cs - 1 >= column)) {
      // This layout item is subject to deletion.
      auto *item = layout->takeAt(i);

      if (deleteWidgets)
        deleteLayoutChildWidgets(item);

      delete item;
    }
  }
}

/**
 * Removes all layout items on the given @a row from the given grid
 * @a layout. If @a deleteWidgets is true, all concerned child widgets
 * become not only removed from the layout, but also deleted. Note that
 * this function doesn't actually remove the row itself from the grid
 * layout, as this isn't possible (i.e. the rowCount() and row indices
 * will stay the same after this function has been called).
 */
void
CQUtil::
removeGridRow(QGridLayout *layout, int row, bool deleteWidgets)
{
  removeGridItem(layout, row, -1, deleteWidgets);

  layout->setRowMinimumHeight(row, 0);
  layout->setRowStretch(row, 0);
}

/**
 * Removes all layout items on the given @a column from the given grid
 * @a layout. If @a deleteWidgets is true, all concerned child widgets
 * become not only removed from the layout, but also deleted. Note that
 * this function doesn't actually remove the column itself from the grid
 * layout, as this isn't possible (i.e. the columnCount() and column
 * indices will stay the same after this function has been called).
 */
void
CQUtil::
removeGridColumn(QGridLayout *layout, int column, bool deleteWidgets)
{
  removeGridItem(layout, -1, column, deleteWidgets);

  layout->setColumnMinimumWidth(column, 0);
  layout->setColumnStretch(column, 0);
}

void
CQUtil::
removeGridItems(QGridLayout *layout, bool deleteWidgets)
{
  // We avoid usage of QGridLayout::itemAtPosition() here to improve performance.
  for (int i = layout->count() - 1; i >= 0; i--) {
    // This layout item is subject to deletion.
    auto *item = layout->takeAt(i);

    if (deleteWidgets)
      deleteLayoutChildWidgets(item);

    delete item;
  }
}

//------

bool
CQUtil::
isFixedPitch(const QFont &font)
{
  const QFontInfo fi(font);
  //qDebug() << fi.family() << fi.fixedPitch();

  return fi.fixedPitch();
}

QFont
CQUtil::
getMonospaceFont()
{
  QFont font("monospace");
  if (isFixedPitch(font)) return font;

  font.setStyleHint(QFont::Monospace);
  if (isFixedPitch(font)) return font;

  font.setStyleHint(QFont::TypeWriter);
  if (isFixedPitch(font)) return font;

  font.setFamily("courier");
  if (isFixedPitch(font)) return font;

  return font;
}
