#ifndef CQWINDOW_H
#define CQWINDOW_H

#include <QWidget>

#include <CWindow.h>

class CQWindowFactory : public CWindowFactory {
 public:
  CWindow *createWindow(int x, int y, uint width, uint height);

  CWindow *createWindow(CWindow *parent, int x, int y, uint width, uint height);
};

//------

class CQWindow : public QWidget, public CWindow {
  Q_OBJECT

 public:
  static void setFactory();

 public:
  CQWindow(CQWindow *parent=nullptr);

  CQWindow(QWidget *parent);

  virtual ~CQWindow();

  virtual bool isPressed() const { return pressed_; }
  virtual void setPressed(bool b);

  // get current position and size
  virtual void getPosition(int *x, int *y) const;
  virtual void getSize(uint *w, uint *h) const;

  virtual void getScreenSize(uint *w, uint *h) const;

  // destrow window
  virtual void destroy();

  // move window
  virtual void move(int x, int y);

  // resize window
  virtual void resize(uint width, uint height);

  // map/unmap window
  virtual void map  ();
  virtual void unmap();

  // get if window is mapped
  virtual bool isMapped();

  // iconize/deiconize window
  virtual void iconize  ();
  virtual void deiconize();

  // maximize/demaximize window
  virtual void maximize  ();
  virtual void demaximize();

  // lower/raise window
  virtual void lower();
  virtual void raise();

  // set window/icon title
  virtual void setWindowTitle(const std::string &title);
  virtual void setIconTitle(const std::string &);

  // get window/icon title
  virtual void getWindowTitle(std::string &title) const;
  virtual void getIconTitle(std::string &title) const;

  // signal expose event
  virtual void expose();

  // set event adapter
  virtual void setEventAdapter(CEventAdapter *eventAdpater);

  // property
  virtual bool setProperty(const std::string &name, const std::string &value);

  // selection
  virtual bool setSelectText(const std::string &text);

  // draw window contents
  virtual void redraw();

  virtual void beep();

  //------

  // hints

  virtual void setMinSize(int width, int height);
  virtual void setBaseSize(int width, int height);
  virtual void setResizeInc(int width, int height);

  //------

  virtual void mousePressEvent(QMouseEvent *);
  virtual void mouseReleaseEvent(QMouseEvent *);
  virtual void mouseDoubleClickEvent(QMouseEvent *);
  virtual void mouseMoveEvent(QMouseEvent *);

  virtual void keyPressEvent(QKeyEvent *);
  virtual bool keyPressEvent(const CKeyEvent &);
  virtual void keyReleaseEvent(QKeyEvent *);
  virtual bool keyReleaseEvent(const CKeyEvent &);

  virtual void paintEvent(QPaintEvent *);

  virtual void resizeEvent(QResizeEvent *);
  virtual bool resizeEvent();

  virtual void hideEvent(QHideEvent *);
  virtual bool hideEvent();
  virtual void showEvent(QShowEvent *);
  virtual bool showEvent();

  virtual void enterEvent(QEvent *);
  virtual bool enterEvent();
  virtual void leaveEvent(QEvent *);
  virtual bool leaveEvent();

 private:
  bool pressed_ { false };
};

#endif
