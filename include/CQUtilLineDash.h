#ifndef CQUTIL_LINE_DASH_H
#define CQUTIL_LINE_DASH_H

#include <CLineDash.h>

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CLineDash)

namespace CQUtil {
  void penSetLineDash(QPen &pen, const CLineDash &dash);
}

#endif
