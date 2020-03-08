#include <CQPenUtil.h>
#include <CQUtil.h>

#ifdef CQUTIL_PEN
QPen
CQPenUtil::
toQPen(const CPen &pen)
{
  QPen qpen;

  CQUtil::penSetLineDash(qpen, pen.getLineDash());

  qpen.setColor    (CQUtil::rgbaToColor(pen.getColor()));
  qpen.setWidthF   (pen.getWidth());
  qpen.setCapStyle (CQUtil::toPenCapStyle(pen.getLineCap()));
  qpen.setJoinStyle(CQUtil::toPenJoinStyle(pen.getLineJoin()));

  return qpen;
}
#endif
