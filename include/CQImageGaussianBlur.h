#ifndef CQIMAGE_GAUSSIAN_BLUR_H
#define CQIMAGE_GAUSSIAN_BLUR_H

#include <QImage>
#include <CGaussianBlur.h>

class CQImageWrapper {
 public:
  explicit CQImageWrapper(QImage &image) :
   image_(image) {
    px1_ = 0; px2_ = image_.width () - 1;
    py1_ = 0; py2_ = image_.height() - 1;

    wx1_ = px1_; wy1_ = py1_;
    wx2_ = px2_; wy2_ = py2_;
  }

  void getPixelRange(int *x1, int *y1, int *x2, int *y2) const {
    *x1 = px1_; *y1 = py1_;
    *x2 = px2_; *y2 = py2_;
  }

  void getWindow(int *x1, int *y1, int *x2, int *y2) const {
    *x1 = wx1_; *y1 = wy1_;
    *x2 = wx2_; *y2 = wy2_;
  }

  void setWindow(int x1, int y1, int x2, int y2) {
    wx1_ = x1; wy1_ = y1;
    wx2_ = x2; wy2_ = y2;
  }

  void getRGBA(int x, int y, double *r, double *g, double *b, double *a) const {
    QRgb rgb = image_.pixel(x, y);

    *r = qRed  (rgb)/255.0;
    *g = qGreen(rgb)/255.0;
    *b = qBlue (rgb)/255.0;
    *a = qAlpha(rgb)/255.0;
  }

  void setRGBA(int x, int y, double r, double g, double b, double a) {
    QRgb rgb = qRgba(int(r*255), int(g*255), int(b*255), int(a*255));

    image_.setPixel(x, y, rgb);
  }

 private:
  friend class CQImageGaussianBlur;

  QImage &image_;
  int     px1_, py1_, px2_, py2_;
  int     wx1_, wy1_, wx2_, wy2_;
};

//---

class CQImageGaussianBlur {
 public:
  explicit CQImageGaussianBlur(const QImage &src) :
   src_(src), wsrc_(src_) {
  }

  void setWindow(int x1, int y1, int x2, int y2) {
    wsrc_.setWindow(x1, y1, x2, y2);
  }

  bool blur(QImage &dst, double bx=1, double by=1, int nx=0, int ny=0) {
    if (dst.isNull()) {
      int iw = src_.width ();
      int ih = src_.height();

      dst = QImage(iw, ih, QImage::Format_ARGB32);

      dst.fill(qRgba(0,0,0,0));
    }

    CGaussianBlur<CQImageWrapper> blur;

    CQImageWrapper wdst(dst);

    return blur.blur(wsrc_, wdst, bx, by, nx, ny);
  }

 private:
  QImage         src_;
  CQImageWrapper wsrc_;
};

#endif
