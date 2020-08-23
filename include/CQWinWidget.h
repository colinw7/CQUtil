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
  Q_PROPERTY(EditMode       editMode       READ editMode       WRITE setEditMode      )

  Q_PROPERTY(int    borderWidth   READ decorationBorder WRITE setDecorationBorder)
  Q_PROPERTY(QColor borderColor   READ decorationColor  WRITE setDecorationColor )
  Q_PROPERTY(bool   movable       READ isMovable        WRITE setMovable         )
  Q_PROPERTY(bool   resizable     READ isResizable      WRITE setResizable       )
  Q_PROPERTY(bool   closable      READ isClosable       WRITE setClosable        )
  Q_PROPERTY(bool   transparent   READ isTransparent    WRITE setTransparent     )
  Q_PROPERTY(bool   headerVisible READ isHeaderVisible  WRITE setHeaderVisible   )

  Q_ENUMS(DecorationType)
  Q_ENUMS(HeaderSide)
  Q_ENUMS(EditMode)

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
    DecorationType type         { HeaderBorderDecoration };
    HeaderSide     headerSide   { SideTop };
    int            headerHeight { 0 };
    QRect          headerRect;
    int            border       { 0 };
    QColor         borderColor  { Qt::white };

    Decoration() = default;

    Decoration(DecorationType type, HeaderSide headerSide, int headerHeight,
               int border, const QColor &borderColor) :
     type(type), headerSide(headerSide), headerHeight(headerHeight),
     border(border), borderColor(borderColor) {
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
    QPoint initPos;
    QSize  initSize;
    QPoint pressPos;
    bool   moving   { false };
    bool   resizing { false };
    bool   resizeL  { false };
    bool   resizeT  { false };
    bool   resizeR  { false };
    bool   resizeB  { false };

    State() { }
  };

  //---

 public:
  CQWinWidget(QWidget *parent=nullptr, const char *name=nullptr);

  virtual ~CQWinWidget() { }

  void setChild(QWidget *child);

  void fitChild();

  //---

  DecorationType decorationType() const { return decoration_.type; }
  void setDecorationType(DecorationType type) { decoration_.type = type; updateSize(); }

  HeaderSide headerSide() const { return decoration_.headerSide; }
  void setHeaderSide(HeaderSide side) { decoration_.headerSide = side; updateSize(); }

  void setDecorationHeaderHeight(int height) { decoration_.headerHeight = height; updateSize(); }

  int decorationBorder() const { return decoration_.border; }
  void setDecorationBorder(int border) { decoration_.border = border; updateSize(); }

  const QColor &decorationColor() const { return decoration_.borderColor; }
  void setDecorationColor(const QColor &c) { decoration_.borderColor = c; update(); }

  unsigned int ops() const { return ops_; }
  void setOps(unsigned int ops) { ops_ = ops; updateSize(); }

  bool isMovable() const { return ops_ & MoveOp; }
  void setMovable(bool movable) {
    ops_ = (movable ? ops_ | MoveOp : ops_ & ~MoveOp); updateSize(); }

  bool isResizable() const { return ops_ & ResizeOp; }
  void setResizable(bool resizable) {
    ops_ = (resizable ? ops_ | ResizeOp : ops_ & ~ResizeOp); updateSize(); }

  bool isClosable() const { return ops_ & CloseOp; }
  void setClosable(bool closable) {
    ops_ = (closable ? ops_ | CloseOp : ops_ & ~CloseOp); updateSize(); }

  bool isTransparent() const { return ! autoFillBackground(); }
  void setTransparent(bool flag=true) { setAutoFillBackground(! flag); update(); }

  EditMode editMode() const { return editMode_; }
  void setEditMode(EditMode editMode) { editMode_ = editMode; }

  bool isHeaderVisible() const { return (decoration_.type & HeaderDecoration); }
  void setHeaderVisible(bool visible) {
    decoration_.type = (DecorationType)
      (visible ? decoration_.type |  HeaderDecoration : decoration_.type & ~HeaderDecoration);
    updateSize(); }

  void setConstraint(Constraint constraint) { constraint_ = constraint; }

  void setChildSize(const QSize &size);

  //---

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

  //---

  void showContextMenu(const QPoint &p);

  void updateSize();

 private slots:
  void setHeaderSlot(bool);

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

 private:
  void paintEvent(QPaintEvent *event) override;

  void moveEvent  (QMoveEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  void mousePressEvent  (QMouseEvent *event) override;
  void mouseMoveEvent   (QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

  void contextMenuEvent(QContextMenuEvent *event) override;

  void enterEvent(QEvent *) override;
  void leaveEvent(QEvent *) override;

  void showEvent(QShowEvent *) override;
  void hideEvent(QHideEvent *) override;

  void setCursor(const uchar *bits, const uchar *mask, int xo, int yo);

 private:
  QWidget*       child_          { nullptr };
  bool           active_         { false };
  bool           pressed_        { false };
  Decoration     decoration_;
  CloseButton    closeButton_    { "CLOSE" };
  ExpandButton   expandButton_   { "MAXIMIZE" };
  CollapseButton collapseButton_ { "MINIMIZE" };
  State          state_;
  unsigned int   ops_            { MoveOp | ResizeOp | RaiseOp | LowerOp | CloseOp };
  EditMode       editMode_       { EDIT_MODE_DRAG };
  Constraint     constraint_     { NO_CONSTRAINT };
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

  void setEditMode(CQWinWidget::EditMode editMode);

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

 public:
  CQWinImage(QWidget *parent, const char *name=0);

  virtual ~CQWinImage() { }

  void show() override { QLabel::show(); widget_->show(); }
  void hide() override { QLabel::hide(); widget_->hide(); }

  QString getImageName() const override { return fileName_; }
  void setImageName(const QString &fileName) override;

  const QImage &getImage() const { return image_; }
  void setImage(const QImage &image);

 private:
  void contextMenuEvent(QContextMenuEvent *event) override;

 signals:
  void valueChanged();
  void geometryChanging();
  void geometryChanged();

 public slots:
  void loadImage();

 private:
  QString fileName_;
  QImage  image_;
  QMenu*  menu_ { nullptr };
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
