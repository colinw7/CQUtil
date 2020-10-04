#ifndef CQStrUtil_H
#define CQStrUtil_H

#include <QStringList>

namespace CQStrUtil {

double toReal(const QString &str, bool &ok);

long toInt(const QString &str, bool &ok);

long toInt(const char *c_str, bool &ok, const char **p);

}

//---

namespace CQStrUtil {

enum class MatchType {
  START
};

QStringList matchStrs(const QString &str, const QStringList &strs,
                      MatchType matchType=MatchType::START);

QString longestMatch(const QStringList &items, bool &exact);

}

#endif
