#ifndef CQPenUtil_H
#define CQPenUtil_H

#ifdef CQUTIL_PEN
#include <CPen.h>
#endif

#include <QPen>

namespace CQPenUtil {

#ifdef CQUTIL_PEN
QPen toQPen(const CPen &pen);
#endif

}

#endif
