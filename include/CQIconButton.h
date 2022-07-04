#ifndef CQIconButton_H
#define CQIconButton_H

#include <QToolButton>

class CQIconButton : public QToolButton {
  Q_OBJECT

  Q_PROPERTY(QString iconName READ icon WRITE setIcon)
  Q_PROPERTY(Size    size     READ size WRITE setSize)

  Q_ENUMS(Size)

 public:
  enum class Size {
    NORMAL,
    SMALL,
    LARGE
  };

 public:
  CQIconButton(QWidget *parent=nullptr);
  CQIconButton(const QString &iconName, QWidget *parent=nullptr);

  const Size &size() const { return size_; }
  void setSize(const Size &s);

  const QString &icon() const { return iconName_; }
  void setIcon(const QString &iconName);

  QIcon qicon() const;

 private slots:
  void init();

  void updateIcon();

 private:
  QString iconName_;
  Size    size_ { Size::NORMAL };
};

#endif
