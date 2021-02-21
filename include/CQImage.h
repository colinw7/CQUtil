#ifndef CQ_IMAGE_H
#define CQ_IMAGE_H

#ifdef CQUTIL_IMAGE

#include <QImage>

#include <CImage.h>

/* Qt override for CImage to allow procesing either QImage or CImage data
   and sync the two when needed */

class CQImage : public CImage {
  // Create
 protected:
  CQImage();

  CQImage(int width, int height);

  CQImage(const CImage &image);

  explicit CQImage(const QImage &image);

 public:
  static void setPrototype();
  static void resetPrototype();

 public:
  CQImage(const CQImage &qimage);
 ~CQImage();

  CQImage &operator=(const QImage &image);

  void setImage(const QImage &image);

 protected:
  CQImage &operator=(const CImage &image);
  CQImage &operator=(const CQImage &image);

 public:
  CImagePtr dup() const;

  void updateCImage();

  QImage &getQImage();
  QImage *getQImageP();

  //------

  void setDataSizeV(int width, int height);

  bool setColorIndexPixel(int pos, uint pixel);
  bool setColorIndexPixel(int x, int y, uint pixel);

  bool setRGBAPixel(int pos, const CRGBA &rgba);
  bool setRGBAPixel(int x, int y, const CRGBA &rgba);

  //------

  void dataChanged();

 public:
  bool gaussianBlurExec(CImagePtr &dst, double bx, double by, int nx, int ny);

 private:
  void initQImage();

 private:
  QImage qimage_;
  bool   initialized_ { false };
};

#endif

#endif
