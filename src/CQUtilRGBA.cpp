#include <CQUtilRGBA.h>

QColor
CQUtil::
rgbToColor(const CRGB &rgb)
{
  return QColor(int(rgb.getRedI()), int(rgb.getGreenI()), int(rgb.getBlueI()));
}

QColor
CQUtil::
rgbaToColor(const CRGBA &rgba)
{
  return QColor(int(rgba.getRedI()), int(rgba.getGreenI()), int(rgba.getBlueI()),
                int(rgba.getAlphaI()));
}

uint
CQUtil::
rgbaToInt(const CRGBA &rgba)
{
  // #AARRGGBB
  return (rgba.getAlphaI() << 24) |
         (rgba.getRedI  () << 16) |
         (rgba.getGreenI() << 8 ) |
         (rgba.getBlueI ()      );
}

CRGB
CQUtil::
colorToRGB(const QColor &color)
{
  return CRGB(color.redF(), color.greenF(), color.blueF());
}

CRGBA
CQUtil::
colorToRGBA(const QColor &color)
{
  return CRGBA(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}
