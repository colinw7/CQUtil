#ifndef CQWidgetPtr_H
#define CQWidgetPtr_H

#include <QObject>

class CQWidgetPtr : public QObject {
  Q_OBJECT

 public:
  CQWidgetPtr(QWidget *w=nullptr) :
   w_(w) {
    watch();
  }

  bool isValid() const {
    return (w_ != nullptr);
  }

  void set(QWidget *w) {
    w_ = w;

    watch();
  }

  QWidget *get() const {
    return w_;
  }

  template<typename T>
  T *get() const {
    return dynamic_cast<T *>(w_);
  }

 private:
  void watch() {
    if (w_)
      connect(w_, SIGNAL(destroyed(QObject *)), this, SLOT(resetSlot()));
  }

 private slots:
  void resetSlot() {
    w_ = nullptr;
  }

 private:
  QWidget *w_;
};

#endif
