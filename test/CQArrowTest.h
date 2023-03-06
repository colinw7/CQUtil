#include <QWidget>
#include <CQArrow.h>

class CQPropertyTree;

class QPainter;

class Canvas;

class Dialog : public QWidget {
  Q_OBJECT

 public:
  Dialog();
};

class Canvas : public QWidget, public CQArrowDevice {
  Q_OBJECT

 public:
  Canvas(QWidget *parent=0);

  void addPropeties(CQPropertyTree *tree);

  void paint(QPainter *p);

  QSize sizeHint() const override;

 private:
  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *e) override;

  void mousePressEvent  (QMouseEvent *e) override;
  void mouseMoveEvent   (QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

  void drawArrow(const CQArrow &arrow);

  QPointF windowToPixel(const QPointF &w) override;
  QPointF pixelToWindow(const QPointF &p) override;

 private:
  CQArrow*        arrow_   { nullptr };
  int             pw_      { 0 }, ph_ { 0 };
  QPainter*       painter_ { nullptr };
  double          xmin_    { 0 }, ymin_ { 0 };
  double          xmax_    { 1 }, ymax_ { 1 };
  Qt::MouseButton button_  { Qt::NoButton };
  bool            pressed_ { false };
};
