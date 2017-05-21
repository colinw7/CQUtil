#ifndef CQWinWidget_H
#define CQWinWidget_H

#include <QWidget>
#include <QPoint>
#include <QTextEdit>
#include <QLabel>
#include <QTableWidget>
#include <QTreeWidget>

class QMenu;

class CQWinWidget : public QWidget {
  Q_OBJECT

  Q_PROPERTY(DecorationType decorationType READ decorationType WRITE setDecorationType)
  Q_PROPERTY(HeaderSide     headerSide     READ headerSide     WRITE setHeaderSide    )

  Q_ENUMS(DecorationType)
  Q_ENUMS(HeaderSide)

 public:
  enum GeometryOps {
    MoveOp     = (1<<0),
    ResizeOp   = (1<<1),
    RaiseOp    = (1<<2),
    LowerOp    = (1<<3),
    CloseOp    = (1<<4),
    ExpandOp   = (1<<5),
    CollapseOp = (1<<6)
  };

  enum DecorationType {
    NoDecoration           = 0,
    BorderDecoration       = (1<<0),
    HeaderDecoration       = (1<<1),
    HeaderBorderDecoration = (BorderDecoration | HeaderDecoration)
  };

  enum EditMode {
    EDIT_MODE_DRAG,
    EDIT_MODE_CLICK
  };

  enum HeaderSide {
    SideLeft,
    SideRight,
    SideTop,
    SideBottom
  };

  enum Constraint {
    NO_CONSTRAINT,
    VISIBLE_CONSTRAINT,
    UNCLIPPED_CONSTRAINT
  };

 private:
  struct Decoration {
    DecorationType type;
    HeaderSide     header_side;
    int            header_height { 0 };
    QRect          header_rect;
    int            border { 0 };
    QColor         border_color;

    Decoration(DecorationType type1, HeaderSide header_side1, int header_height1,
               int border1, const QColor &border_color1) :
     type(type1), header_side(header_side1), header_height(header_height1),
     border(border1), border_color(border_color1) {
    }
  };

  //---

  struct HeaderButton {
    QString iconName;
    QIcon   icon;
    QRect   rect;
    bool    displayed { false };
    bool    active    { false };
    bool    pressed   { false };

    HeaderButton(const QString &iconName);

    void updateActive(const QPoint &p, bool pressed1);

    void draw(QPainter *painter, CQWinWidget *preview);
  };

  //---

  struct CloseButton : public HeaderButton {
    CloseButton(const QString &iconName) : HeaderButton(iconName) { }
  };

  struct ExpandButton : public HeaderButton {
    ExpandButton(const QString &iconName) : HeaderButton(iconName) { }
  };

  struct CollapseButton : public HeaderButton {
    CollapseButton(const QString &iconName) : HeaderButton(iconName) { }
  };

  //---

  struct State {
    QPoint init_pos;
    QSize  init_size;
    QPoint press_pos;
    bool   moving   { false };
    bool   resizing { false };
    bool   resize_l { false };
    bool   resize_t { false };
    bool   resize_r { false };
    bool   resize_b { false };

    State() { }
  };

  //---

 public:
  CQWinWidget(QWidget *parent=0, const char *name=0);

  virtual ~CQWinWidget() { }

  void setChild(QWidget *child);

  void fitChild();

  DecorationType decorationType() const { return decoration_.type; }
  void setDecorationType(DecorationType type) { decoration_.type = type; }

  HeaderSide headerSide() const { return decoration_.header_side; }
  void setHeaderSide(HeaderSide side) { decoration_.header_side = side; }

  void setDecorationHeaderHeight(int height) {
    decoration_.header_height = height;
  }

  void setDecorationBorder(int border) {
    decoration_.border = border;
  }

  void setOps(unsigned int ops) {
    ops_ = ops;
  }

  void setMovable(bool movable) {
    ops_ = (movable ? ops_ | MoveOp : ops_ & ~MoveOp);
  }

  void setResizable(bool resizable) {
    ops_ = (resizable ? ops_ | ResizeOp : ops_ & ~ResizeOp);
  }

  void setClosable(bool closable) {
    ops_ = (closable ? ops_ | CloseOp : ops_ & ~CloseOp);
  }

  void setTransparent(bool flag=true) {
    setAutoFillBackground(! flag);
  }

  void setEditMode(EditMode edit_mode) {
    edit_mode_ = edit_mode;
  }

  void setConstraint(Constraint constraint) {
    constraint_ = constraint;
  }

  void setChildSize(const QSize &size);

  int getX() const;
  int getY() const;

  int getWidth () const;
  int getHeight() const;

  int getHeaderHeight() const;

  void setPos (int x, int y);
  void setSize(int w, int h);

  virtual bool checkMove(QPoint &p) const;

  virtual bool checkGeometry(const QRect &r) const;

  int getBorder() const;

  void drawHeaderButtonH(QPainter *painter, HeaderButton &button, int &x1, int y1, int b1);
  void drawHeaderButtonV(QPainter *painter, HeaderButton &button, int x1, int &y1, int b1);

 private slots:
  void closeSlot();
  void expandSlot();
  void collapseSlot();

 signals:
  void geometryChanging();
  void geometryChanged();

  void closing();
  void closed();
  void expand();
  void collapse();

  void showContextMenu(const QPoint &p);

 private:
  void paintEvent(QPaintEvent *event);

  void moveEvent  (QMoveEvent *event);
  void resizeEvent(QResizeEvent *event);

  void mousePressEvent  (QMouseEvent *event);
  void mouseMoveEvent   (QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

  void contextMenuEvent(QContextMenuEvent *event);

  void enterEvent(QEvent *);
  void leaveEvent(QEvent *);

  void showEvent(QShowEvent *);
  void hideEvent(QHideEvent *);

  void setCursor(const uchar *bits, const uchar *mask, int xo, int yo);

 private:
  QWidget *      child_ { 0 };
  bool           active_ { false };
  bool           pressed_ { false };
  Decoration     decoration_;
  CloseButton    closeButton_ { "CLOSE" };
  ExpandButton   expandButton_ { "MAXIMIZE" };
  CollapseButton collapseButton_ { "MINIMIZE" };
  State          state_;
  unsigned int   ops_ { MoveOp | ResizeOp | RaiseOp | LowerOp | CloseOp };
  EditMode       edit_mode_ { EDIT_MODE_DRAG };
  Constraint     constraint_ { NO_CONSTRAINT };
};

//------

class CQWinWidgetBase {
 public:
  CQWinWidgetBase(QWidget *parent);

  virtual ~CQWinWidgetBase();

  CQWinWidget *widget() const { return widget_; }

  int getX() const { return widget_->x() + widget_->getX(); }
  int getY() const { return widget_->y() + widget_->getY(); }

  int getWidth () const { return widget_->getWidth (); }
  int getHeight() const { return widget_->getHeight(); }

  void setPos (int x, int y) { widget_->setPos (x, y); }
  void setSize(int w, int h) { widget_->setSize(w, h); }

  virtual void show() = 0;
  virtual void hide() = 0;

  virtual void setText(const QString &) { }

  virtual QString getText() const { return ""; }

  virtual void setImageName(const QString &) { }

  virtual QString getImageName() const { return ""; }

  void setEditMode(CQWinWidget::EditMode edit_mode);

 protected:
  CQWinWidget *widget_ { 0 };
};

//------

class CQWinTextEdit : public QTextEdit, public CQWinWidgetBase {
  Q_OBJECT

 public:
  CQWinTextEdit(QWidget *parent, const char *name=0);

  virtual ~CQWinTextEdit() { }

  void show() { QTextEdit::show(); widget_->show(); }
  void hide() { QTextEdit::hide(); widget_->hide(); }

  void setText(const QString &text) { QTextEdit::setText(text); }

  QString getText() const { return QTextEdit::toPlainText(); }

 signals:
  void valueChanged();
  void geometryChanging();
  void geometryChanged();
};

//------

class CQWinImage : public QLabel, public CQWinWidgetBase {
  Q_OBJECT

 private:
  QString  fileName_;
  QImage   image_;
  QMenu   *menu_;

 public:
  CQWinImage(QWidget *parent, const char *name=0);

  virtual ~CQWinImage() { }

  void show() { QLabel::show(); widget_->show(); }
  void hide() { QLabel::hide(); widget_->hide(); }

  void setImageName(const QString &fileName);

  void setImage(const QImage &image);

  QString getImageName() const { return fileName_; }

  const QImage &getImage() const { return image_; }

 private:
  void contextMenuEvent(QContextMenuEvent *event);

 signals:
  void valueChanged();
  void geometryChanging();
  void geometryChanged();

 public slots:
  void loadImage();
};

//------

class CQWinTable : public QTableWidget, public CQWinWidgetBase {
  Q_OBJECT

 public:
  CQWinTable(QWidget *parent, const char *name=0);

  virtual ~CQWinTable() { }

  void show() { QTableWidget::show(); widget_->show(); }
  void hide() { QTableWidget::hide(); widget_->hide(); }

 signals:
  void valueChanged();
  void geometryChanging();
  void geometryChanged();
};

//------

class CQWinTree : public QTreeWidget, public CQWinWidgetBase {
  Q_OBJECT

 public:
  CQWinTree(QWidget *parent, const char *name=0);

  virtual ~CQWinTree() { }

  void show() { QTreeWidget::show(); widget_->show(); }
  void hide() { QTreeWidget::hide(); widget_->hide(); }

 signals:
  void valueChanged();
  void geometryChanging();
  void geometryChanged();
};

#endif
