#ifndef CQBrushUtil_H
#define CQBrushUtil_H

#ifdef CQUTIL_BRUSH
#include <CBrush.h>
#endif

#include <QBrush>

namespace CQBrushUtil {

#ifdef CQUTIL_BRUSH
QBrush toQBrush(const CBrush &brush);
#endif

}

#endif
