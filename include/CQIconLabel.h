#ifndef CQIconLabel_H
#define CQIconLabel_H

#include <QLabel>

class CQIconLabel : public QLabel {
  Q_OBJECT

  Q_PROPERTY(Size size READ size WRITE setSize)

  Q_ENUMS(Size)

 public:
  enum class Size {
    NORMAL,
    SMALL,
    LARGE
  };

 public:
  CQIconLabel(QWidget *parent=nullptr);

  const Size &size() const { return size_; }
  void setSize(const Size &s);

  void setIcon(const QString &iconName);

 private slots:
  void updateIcon();

 private:
  QString iconName_;
  Size    size_ { Size::NORMAL };
};

#endif
