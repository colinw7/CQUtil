#ifndef CQWINDOW_H
#define CQWINDOW_H

#include <QWidget>

#include <CWindow.h>

class CQWindowFactory : public CWindowFactory {
 public:
  CWindow *createWindow(int x, int y, uint width, uint height) override;

  CWindow *createWindow(CWindow *parent, int x, int y, uint width, uint height) override;
};

//------

class CQWindow : public QWidget, public CWindow {
  Q_OBJECT

 public:
  static void setFactory();
  static void resetFactory();

 public:
  CQWindow(CQWindow *parent=nullptr);

  CQWindow(QWidget *parent);

  virtual ~CQWindow();

  virtual bool isPressed() const { return pressed_; }
  virtual void setPressed(bool b);

  // get current position and size
  void getPosition(int *x, int *y) const override;
  void getSize(uint *w, uint *h) const override;

  void getScreenSize(uint *w, uint *h) const override;

  // destrow window
  void destroy() override;

  // move window
  void move(int x, int y) override;

  // resize window
  void resize(uint width, uint height) override;

  // map/unmap window
  void map  () override;
  void unmap() override;

  // get if window is mapped
  bool isMapped() override;

  // iconize/deiconize window
  void iconize  () override;
  void deiconize() override;

  // maximize/demaximize window
  void maximize  () override;
  void demaximize() override;

  // lower/raise window
  void lower() override;
  void raise() override;

  // set window/icon title
  void setWindowTitle(const std::string &title) override;
  void setIconTitle(const std::string &) override;

  // get window/icon title
  void getWindowTitle(std::string &title) const override;
  void getIconTitle(std::string &title) const override;

  // signal expose event
  void expose() override;

  // set event adapter
  void setEventAdapter(CEventAdapter *eventAdpater) override;

  // property
  bool setProperty(const std::string &name, const std::string &value) override;

  // selection
  bool setSelectText(const std::string &text) override;

  // draw window contents
  void redraw() override;

  void beep() override;

  //------

  // hints

  virtual void setMinSize(int width, int height);
  virtual void setBaseSize(int width, int height);
  virtual void setResizeInc(int width, int height);

  //------

  void mousePressEvent(QMouseEvent *) override;
  void mouseReleaseEvent(QMouseEvent *) override;
  void mouseDoubleClickEvent(QMouseEvent *) override;
  void mouseMoveEvent(QMouseEvent *) override;

  void keyPressEvent(QKeyEvent *) override;
  bool keyPressEvent(const CKeyEvent &) override;
  void keyReleaseEvent(QKeyEvent *) override;
  bool keyReleaseEvent(const CKeyEvent &) override;

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent *) override;
  bool resizeEvent() override;

  void hideEvent(QHideEvent *) override;
  bool hideEvent() override;
  void showEvent(QShowEvent *) override;
  bool showEvent() override;

  void enterEvent(QEvent *) override;
  bool enterEvent() override;
  void leaveEvent(QEvent *) override;
  bool leaveEvent() override;

 private:
  bool pressed_ { false };
};

#endif
