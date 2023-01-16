#include <CQImage.h>

#include <CQImageGaussianBlur.h>
#include <CQUtil.h>
#include <CQUtilRGBA.h>
#include <CImageLib.h>
#include <QImage>

void
CQImage::
setPrototype()
{
  CImagePtr image(new CQImage());

  CImageMgrInst->setPrototype(image);
}

void
CQImage::
resetPrototype()
{
  CImageMgrInst->setPrototype(CImagePtr());
}

CQImage::
CQImage()
{
}

CQImage::
CQImage(int width, int height) :
 CImage(width, height)
{
}

CQImage::
CQImage(const CImage &image) :
 CImage(image)
{
}

void
CQImage::
initQImage()
{
  // set qimage from cimage
  int x1, y1, x2, y2;

  getWindow(&x1, &y1, &x2, &y2);

  uint width  = uint(x2 - x1 + 1);
  uint height = uint(y2 - y1 + 1);

  if (qimage_.isNull() || qimage_.width () != int(width) || qimage_.height() != int(height))
    qimage_ = QImage(int(width), int(height), QImage::Format_ARGB32);

  CRGBA rgba;

  for (int y = y1; y <= y2; ++y) {
    for (int x = x1; x <= x2; ++x) {
      getRGBAPixel(x, y, rgba);

      qimage_.setPixel(x - x1, y - y1, CQUtil::rgbaToColor(rgba).rgba());
    }
  }

  initialized_ = true;
}

void
CQImage::
updateCImage()
{
  // set cimage from qimage
  if (! initialized_)
    return;

  uint width  = uint(qimage_.width ());
  uint height = uint(qimage_.height());

  if (getWidth() != width || getHeight() != height)
    CImage::setDataSizeV(int(width), int(height));

  CRGBA rgba;

  for (uint y = 0; y < height; ++y) {
    for (uint x = 0; x < width; ++x) {
      rgba = CQUtil::colorToRGBA(QColor::fromRgba(qimage_.pixel(int(x), int(y))));

      setRGBAPixel(int(x), int(y), rgba);
    }
  }
}

CQImage::
CQImage(const QImage &qimage)
{
  qimage_      = qimage;
  initialized_ = true;

  updateCImage();
}

CQImage::
CQImage(const CQImage &qimage) :
 CImage(qimage)
{
  qimage_      = qimage.qimage_;
  initialized_ = qimage.initialized_;
}

CQImage::
~CQImage()
{
}

CQImage &
CQImage::
operator=(const CImage &image)
{
  const CQImage &qimage = dynamic_cast<const CQImage &>(image);

  qimage_      = qimage.qimage_;
  initialized_ = qimage.initialized_;

  updateCImage();

  return *this;
}

CQImage &
CQImage::
operator=(const CQImage &qimage)
{
  qimage_      = qimage.qimage_;
  initialized_ = qimage.initialized_;

  updateCImage();

  return *this;
}

CQImage &
CQImage::
operator=(const QImage &qimage)
{
  qimage_      = qimage;
  initialized_ = true;

  updateCImage();

  return *this;
}

CImagePtr
CQImage::
dup() const
{
  return CImagePtr(new CQImage(*this));
}

void
CQImage::
setImage(const QImage &qimage)
{
  qimage_      = qimage;
  initialized_ = true;

  updateCImage();
}

QImage &
CQImage::
getQImage()
{
  if (! initialized_)
    initQImage();

  return qimage_;
}

QImage *
CQImage::
getQImageP()
{
  if (! initialized_)
    initQImage();

  return &qimage_;
}

void
CQImage::
setDataSizeV(int width, int height)
{
  qimage_ = QImage(width, height, QImage::Format_ARGB32);

  CImage::setDataSizeV(width, height);
}

bool
CQImage::
setColorIndexPixel(int pos, uint pixel)
{
  int w = int(getWidth());

  setColorIndexPixel(pos % w, pos / w, pixel);

  return true;
}

bool
CQImage::
setColorIndexPixel(int x, int y, uint pixel)
{
  int w = int(getWidth());

  if (initialized_)
    qimage_.setPixel(QPoint(x, y), pixel);

  CImage::setColorIndexPixel(y*w + x, pixel);

  return true;
}

bool
CQImage::
setRGBAPixel(int pos, const CRGBA &rgba)
{
  int w = int(getWidth());

  setRGBAPixel(pos % w, pos / w, rgba);

  return true;
}

bool
CQImage::
setRGBAPixel(int x, int y, const CRGBA &rgba)
{
  int w = int(getWidth());

  if (initialized_)
    qimage_.setPixel(QPoint(x, y), CQUtil::rgbaToColor(rgba).rgba());

  CImage::setRGBAPixel(y*w + x, rgba);

  return true;
}

void
CQImage::
dataChanged()
{
  initialized_ = false;
}

//------

bool
CQImage::
gaussianBlurExec(CImagePtr &dst, double bx, double by, int nx, int ny)
{
  QImage *image1 = getQImageP();
  QImage *image2 = dynamic_cast<CQImage *>(dst.getPtr())->getQImageP();

  CQImageGaussianBlur blur(*image1);

  int x1, y1, x2, y2;

  if (getWindow(&x1, &y1, &x2, &y2))
    blur.setWindow(x1, y1, x2, y2);

  return blur.blur(*image2, bx, by, nx, ny);
}
