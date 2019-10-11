#ifndef CQFontUtil_H
#define CQFontUtil_H

#ifdef CQUTIL_FONT
#include <CFontPtr.h>
#endif

#include <QFont>

namespace CQFontUtil {

#ifdef CQUTIL_FONT
QFont    toQFont  (CFontPtr font);
CFontPtr fromQFont(QFont font);
#endif

}

#endif
