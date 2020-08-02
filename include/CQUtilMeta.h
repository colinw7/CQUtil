#ifndef CQUtilMeta_H
#define CQUtilMeta_H

#include <QMetaType>
#include <QDataStream>
#include <QDebug>

namespace CQUtilMetaData {
  inline bool *resultPtr() {
    static bool result;
    return &result;
  }

  inline bool getResult() { return *resultPtr(); }
  inline void setResult(bool b) { *resultPtr() = b; }
}

//---

#define CQUTIL_DCL_META_TYPE(TYPE) \
Q_DECLARE_METATYPE(TYPE) \
\
QDataStream &operator<<(QDataStream &out, const TYPE &t); \
\
QDataStream &operator>>(QDataStream &in, TYPE &t); \
\
QDebug operator<<(QDebug dbg, const TYPE &t);

//---

// convert meta type to/from data stream using GETTER and SETTER member functions that
// return and take QString
#define CQUTIL_DEF_META_TYPE_DATA_STREAM(TYPE, GETTER, SETTER) \
QDataStream &operator<<(QDataStream &out, const TYPE &t) { \
  QString str = t.GETTER(); \
  out << str; \
  return out; \
} \
\
QDataStream &operator>>(QDataStream &in, TYPE &t) { \
  QString str; \
  in >> str; \
  CQUtilMetaData::setResult(t.SETTER(str)); \
  return in; \
}

// convert meta type to/from data stream using GETTER and SETTER member functions that
// return and take std::string
#define CQUTIL_DEF_META_TYPE_STD_DATA_STREAM(TYPE, GETTER, SETTER) \
QDataStream &operator<<(QDataStream &out, const TYPE &t) { \
  QString str = t.GETTER().c_str(); \
  out << str; \
  return out; \
} \
\
QDataStream &operator>>(QDataStream &in, TYPE &t) { \
  QString str; \
  in >> str; \
  CQUtilMetaData::setResult(t.SETTER(str.toStdString())); \
  return in; \
} \

// convert meta type to/from debug using GETTER and SETTER member functions that
// return and take QString
#define CQUTIL_DEF_META_TYPE_DEBUG(TYPE, GETTER) \
QDebug operator<<(QDebug dbg, const TYPE &t) \
{ \
  dbg.nospace() << t.GETTER(); \
  return dbg.maybeSpace(); \
} \

// convert meta type to/from debug using GETTER and SETTER member functions that
// return and take std::string
#define CQUTIL_DEF_META_TYPE_STD_DEBUG(TYPE, GETTER) \
QDebug operator<<(QDebug dbg, const TYPE &t) \
{ \
  dbg.nospace() << t.GETTER().c_str(); \
  return dbg.maybeSpace(); \
}

// define routine to register meta type to Qt
#define CQUTIL_DEF_META_TYPE_REGISTER(TYPE, ID) \
namespace CQUtilMeta_##ID {\
int registerMetaType() { \
  static bool registered; \
  static int type = -1; \
  if (! registered) { \
    type = qRegisterMetaType<TYPE>(#TYPE); \
    qRegisterMetaTypeStreamOperators<TYPE>(#TYPE); \
    registered = true; \
  } \
  return type; \
} \
}

// define meta type using GETTER and SETTER member functions that
// return and take QString
#define CQUTIL_DEF_META_TYPE(TYPE, GETTER, SETTER) \
CQUTIL_DEF_META_TYPE_DATA_STREAM(TYPE, GETTER, SETTER) \
\
CQUTIL_DEF_META_TYPE_DEBUG(TYPE, GETTER) \
\
CQUTIL_DEF_META_TYPE_REGISTER(TYPE, TYPE)

#define CQUTIL_DEF_META_TYPE_ID(TYPE, ID, GETTER, SETTER) \
CQUTIL_DEF_META_TYPE_DATA_STREAM(TYPE, GETTER, SETTER) \
\
CQUTIL_DEF_META_TYPE_DEBUG(TYPE, GETTER) \
\
CQUTIL_DEF_META_TYPE_REGISTER(TYPE, ID)

//---

// define meta type using GETTER and SETTER member functions that
// return and take std::string
#define CQUTIL_DEF_META_TYPE_STD(TYPE, GETTER, SETTER) \
CQUTIL_DEF_META_TYPE_STD_DATA_STREAM(TYPE, GETTER, SETTER) \
\
CQUTIL_DEF_META_TYPE_STD_DEBUG(TYPE, GETTER) \
\
CQUTIL_DEF_META_TYPE_REGISTER(TYPE, TYPE)

//---

// call routine to register meta type
#define CQUTIL_REGISTER_META(TYPE) CQUtilMeta_##TYPE :: registerMetaType()

#define CQUTIL_REGISTER_META_ID(ID) CQUtilMeta_##ID :: registerMetaType()

#endif
