#ifndef CQPixmapButton_H
#define CQPixmapButton_H

#include <QToolButton>

class CQPixmapButton : public QToolButton {
  Q_OBJECT

  Q_PROPERTY(int margin READ margin WRITE setMargin)

 public:
  CQPixmapButton(const QString &fileName);
  CQPixmapButton(uchar *data, uint len);
  CQPixmapButton(const QPixmap &pixmap);
  CQPixmapButton(const QIcon &icon);
  CQPixmapButton(const QIcon &icon, const QSize &size);

  virtual ~CQPixmapButton() { }

  int margin() const { return margin_; }
  void setMargin(int i);

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 private:
  void init(const QPixmap &pixmap);
  void init(const QIcon &icon);

 private:
  QSize size_;
  int   margin_ { 8 };
};

#endif
