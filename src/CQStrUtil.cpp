#include <CQStrUtil.h>

#ifdef ALLOW_NAN
#include <COSNaN.h>
#endif

namespace CQStrUtilI {

double
toReal(const char *c_str, bool &ok)
{
  ok = true;

  double r = 0.0;

  //---

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    ok = false;
    return r;
  }

  const char *p;

#ifdef ALLOW_NAN
  if (COS::has_nan() && strncmp(&c_str[i], "NaN", 3) == 0)
    p = &c_str[i + 3];
  else {
    errno = 0;

    r = strtod(&c_str[i], (char **) &p);

    if (errno == ERANGE) {
      ok = false;
      return r;
    }
  }
#else
  errno = 0;

  r = strtod(&c_str[i], const_cast<char **>(&p));

  if (errno == ERANGE) {
    ok = false;
    return r;
  }
#endif

  while (*p != 0 && ::isspace(*p))
    ++p;

  if (*p != '\0')
    ok = false;

  return r;
}

//------

long
toInt(const char *c_str, bool &ok, const char **p)
{
  ok = true;

  *p = 0;

  long integer = 0;

  int i = 0;

  while (c_str[i] != 0 && ::isspace(c_str[i]))
    ++i;

  if (c_str[i] == '\0') {
    ok = false;
    return integer;
  }

  errno = 0;

  const char *p1;

  integer = strtol(&c_str[i], const_cast<char **>(&p1), 10);

  if (errno == ERANGE) {
    ok = false;
    return integer;
  }

  while (*p1 != 0 && ::isspace(*p1))
    ++p1;

  if (*p1 == '.') {
    ++p1;

    while (*p1 == '0')
      ++p1;
  }

  if (*p1 != '\0') {
    ok = false;
    return integer;
  }

  *p = p1;

  return integer;
}

long
toInt(const char *c_str, bool &ok)
{
  const char *p;

  return toInt(c_str, ok, &p);
}

}

//------

namespace CQStrUtil {

double
toReal(const QString &str, bool &ok)
{
  std::string sstr = str.toStdString();

  return CQStrUtilI::toReal(sstr.c_str(), ok);
}

long
toInt(const QString &str, bool &ok)
{
  std::string sstr = str.toStdString();

  return CQStrUtilI::toInt(sstr.c_str(), ok);
}

long
toInt(const char *c_str, bool &ok, const char **p)
{
  return CQStrUtilI::toInt(c_str, ok, p);
}

}

//------

namespace CQStrUtil {

QStringList
matchStrs(const QString &str, const QStringList &strs, MatchType /*matchType*/)
{
  int n = strs.length();

  QStringList matches;

  for (int i = 0; i < n; ++i) {
    if (strs[i].startsWith(str))
      matches.push_back(strs[i]);
  }

  return matches;
}

// get longest prefix of all specified items
QString
longestMatch(const QStringList &items, bool &exact)
{
  exact = false;

  QString match = "";

  int num_items = items.size();

  if (num_items == 0)
    return "";

  if (num_items == 1) {
    exact = true;
    return items[0];
  }

  int len = items[0].length();

  for (int i = 0; i < len; ++i) {
    for (int j = 1; j < num_items; ++j) {
      int len1 = items[j].length();

      if (i >= len1 || items[0][i] != items[j][i])
        return items[0].mid(0, i);
    }
  }

  exact = true;

  return items[0];
}

}
