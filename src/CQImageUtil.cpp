#include <CQImageUtil.h>
#include <CQImage.h>

#ifdef CQUTIL_IMAGE
QIcon
CQImageUtil::
imageToIcon(CImagePtr image)
{
  QImage qimage = toQImage(image);

  return QIcon(QPixmap::fromImage(qimage));
}

QImage
CQImageUtil::
toQImage(CImagePtr image)
{
  return dynamic_cast<CQImage *>(image.get())->getQImage();
}
#endif
