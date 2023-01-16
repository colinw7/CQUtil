#ifndef CQUTIL_GRADIENT_H
#define CQUTIL_GRADIENT_H

class CLinearGradient;
class CRadialGradient;

namespace CQUtil {

QLinearGradient toQGradient(const CLinearGradient *lgradient,
                            QGradient::CoordinateMode mode=QGradient::ObjectBoundingMode);
QRadialGradient toQGradient(const CRadialGradient *rgradient,
                            QGradient::CoordinateMode mode=QGradient::ObjectBoundingMode);

}

#endif
