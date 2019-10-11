#ifndef CQImageUtil_H
#define CQImageUtil_H

#ifdef CQUTIL_IMAGE
#include <CImagePtr.h>
#endif

#include <QIcon>

namespace CQImageUtil {
#ifdef CQUTIL_IMAGE
  QIcon imageToIcon(CImagePtr image);

  QImage toQImage(CImagePtr image);
#endif
}

#endif
