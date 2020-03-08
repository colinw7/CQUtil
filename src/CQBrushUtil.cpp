#include <CQBrushUtil.h>
#include <CQImageUtil.h>
#include <CQUtil.h>
#include <CImageLib.h>
#include <CAlignType.h>
#include <CLinearGradient.h>
#include <CRadialGradient.h>

#ifdef CQUTIL_BRUSH
QBrush
CQBrushUtil::
toQBrush(const CBrush &brush)
{
  QBrush qbrush;

  if      (brush.getStyle() == CBRUSH_STYLE_SOLID) {
    qbrush = QBrush(CQUtil::rgbaToColor(brush.getColor()));
  }
  else if (brush.getStyle() == CBRUSH_STYLE_PATTERN) {
    //qbrush = QBrush(CQUtil::toQPattern(brush.getPattern()));
    std::cerr << "Invalid pattern brush" << std::endl;
  }
#ifdef CQUTIL_GRADIENT
  else if (brush.getStyle() == CBRUSH_STYLE_GRADIENT) {
    CLinearGradient *lgradient = dynamic_cast<CLinearGradient *>(brush.getGradient().get());
    CRadialGradient *rgradient = dynamic_cast<CRadialGradient *>(brush.getGradient().get());

    if      (lgradient)
      qbrush = QBrush(CQUtil::toQGradient(lgradient));
    else if (rgradient)
      qbrush = QBrush(CQUtil::toQGradient(rgradient));
    else
      std::cerr << "Invalid gradient type" << std::endl;
  }
#endif
#ifdef CQUTIL_IMAGE
  else if (brush.getStyle() == CBRUSH_STYLE_TEXTURE) {
    qbrush = QBrush(CQImageUtil::toQImage(brush.getTexture()));
  }
#endif
  else {
    std::cerr << "Invalid brush" << std::endl;
  }

  return qbrush;
}
#endif
