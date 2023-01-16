#ifndef CQUTIL_RGBA_H
#define CQUTIL_RGBA_H

#include <CRGBA.h>

#include <QColor>

namespace CQUtil {

QColor rgbToColor(const CRGB &rgb);
QColor rgbaToColor(const CRGBA &rgba);

inline QColor toQColor(const CRGB  &rgb ) { return rgbToColor (rgb ); }
inline QColor toQColor(const CRGBA &rgba) { return rgbaToColor(rgba); }

uint rgbaToInt(const CRGBA &rgba);

CRGB  colorToRGB(const QColor &color);
CRGBA colorToRGBA(const QColor &color);

inline CRGBA fromQColor(const QColor &color) { return colorToRGBA(color); }
}

#endif
