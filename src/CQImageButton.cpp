#include <CQImageButton.h>

#ifdef CQUTIL_IMAGE
#include <CImageMgr.h>
#include <CQImage.h>
#endif

CQImageButton::
CQImageButton(const QString &fileName)
{
  CImageFileSrc src(fileName.toStdString());

#ifdef CQUTIL_IMAGE
  auto image = CImageMgrInst->lookupImage(src);

  init(image);
#else
  QPixmap pixmap(fileName);

  init(pixmap);
#endif
}

#ifdef CQUTIL_IMAGE
CQImageButton::
CQImageButton(CImagePtr image)
{
  init(image);
}
#endif

CQImageButton::
CQImageButton(uchar *data, uint len)
{
  QPixmap pixmap;

  pixmap.loadFromData(data, len);

  init(pixmap);
}

CQImageButton::
CQImageButton(const QPixmap &pixmap)
{
  init(pixmap);
}

CQImageButton::
CQImageButton(const QIcon &icon)
{
  init(icon);
}

#ifdef CQUTIL_IMAGE
void
CQImageButton::
init(CImagePtr image)
{
  setObjectName("ibutton");

  auto *qimage = dynamic_cast<CQImage *>(image.get());

  if (qimage) {
    auto pixmap = QPixmap::fromImage(qimage->getQImage());

    init(pixmap);
  }
}
#endif

void
CQImageButton::
init(const QPixmap &pixmap)
{
  size_ = pixmap.size();

  QIcon icon(pixmap);

  setIcon(icon);

#ifndef OS_OSX
  setFixedSize(size_ + QSize(8, 8));

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
#endif
}

void
CQImageButton::
init(const QIcon &icon)
{
  size_ = iconSize();

  setIcon(icon);

#ifndef OS_OSX
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
#endif
}

QSize
CQImageButton::
sizeHint() const
{
  return size_ + QSize(8, 8);
}

QSize
CQImageButton::
minimumSizeHint() const
{
  return size_ + QSize(8, 8);
}
