#ifndef CQUTIL_GEOM_H
#define CQUTIL_GEOM_H

#include <CIBBox2D.h>
#include <CBBox2D.h>
#include <CPoint2D.h>
#include <CMatrix2D.h>

//---

namespace CQUtil {
  QPointF  toQPoint  (const CPoint2D &point);
  QPoint   toQPointI (const CPoint2D &point);
  CPoint2D fromQPoint(const QPointF &point);

  QPoint    toQPoint   (const CIPoint2D &point);
  CIPoint2D fromQPoint (const QPoint &point);
  CPoint2D  fromQPointF(const QPoint &point);

  QSizeF  toQSize  (const CSize2D &size);
  CSize2D fromQSize(const QSizeF &size);

  QSize    toQSize  (const CISize2D &size);
  CISize2D fromQSize(const QSize &size);

  QRectF  toQRect  (const CBBox2D &rect);
  QRect   toQRectI (const CBBox2D &rect);
  CBBox2D fromQRect(const QRectF &rect);

  QRect    toQRect  (const CIBBox2D &rect);
  CIBBox2D fromQRect(const QRect &rect);

  QMatrix   toQMatrix  (const CMatrix2D &m);
  CMatrix2D fromQMatrix(const QMatrix &m);

  QTransform toQTransform  (const CMatrix2D &m);
  CMatrix2D  fromQTransform(const QTransform &t);
}

#endif
