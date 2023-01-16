#ifndef CQUTIL_ALIGN_TYPE_H
#define CQUTIL_ALIGN_TYPE_H

#include <CAlignType.h>

namespace CQUtil {

Qt::Alignment toQAlign(CHAlignType halign);
Qt::Alignment toQAlign(CVAlignType valign);

CHAlignType toHAlign(Qt::Alignment align);
CVAlignType toVAlign(Qt::Alignment align);

}

#endif
