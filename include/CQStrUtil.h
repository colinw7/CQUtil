#ifndef CQStrUtil_H
#define CQStrUtil_H

#ifdef ALLOW_NAN
#include <COSNaN.h>
#endif

namespace CQStrUtil {

inline double toReal(const char *c_str, bool &ok) {
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

  r = strtod(&c_str[i], (char **) &p);

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

inline double toReal(const QString &str, bool &ok) {
  std::string sstr = str.toStdString();

  return toReal(sstr.c_str(), ok);
}

//---

inline long toInt(const char *c_str, bool &ok, const char **p) {
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

  integer = strtol(&c_str[i], (char **) &p1, 10);

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

inline long toInt(const char *c_str, bool &ok) {
  const char *p;

  return toInt(c_str, ok, &p);
}

inline long toInt(const QString &str, bool &ok) {
  std::string sstr = str.toStdString();

  return toInt(sstr.c_str(), ok);
}

}

#endif
