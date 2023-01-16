#ifndef CQUTIL_FONT__H
#define CQUTIL_FONT__H

#include <CFontStyle.h>

namespace CQUtil {
  void decodeFont(const QFont &font, QString &family, CFontStyle &style, int &pixelSize);
}

#endif
