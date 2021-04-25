#ifndef CQIMAGE_BUTTON_H
#define CQIMAGE_BUTTON_H

#include <QToolButton>
#include <CImageLib.h>

class CQImageButton : public QToolButton {
  Q_OBJECT

 public:
#ifdef CQUTIL_IMAGE
  CQImageButton(CImagePtr image);
#endif
  CQImageButton(const QString &fileName);
  CQImageButton(uchar *data, uint len);
  CQImageButton(const QPixmap &pixmap);
  CQImageButton(const QIcon &icon);

  virtual ~CQImageButton() { }

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 private:
#ifdef CQUTIL_IMAGE
  void init(CImagePtr image);
#endif
  void init(const QPixmap &pixmap);
  void init(const QIcon &icon);

 private:
  QSize size_;
};

#endif
