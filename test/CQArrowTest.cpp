#include <CQArrowTest.h>
#include <CQPropertyTree.h>
#include <CQPropertyItem.h>
#include <CQPropertyEditor.h>
#include <CMathGeom2D.h>

#include <QApplication>
#include <QSplitter>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

Dialog::
Dialog()
{
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  QSplitter *splitter = new QSplitter;

  Canvas *canvas = new Canvas;

  canvas->setMinimumSize(QSize(800, 800));

  splitter->addWidget(canvas);

  QFrame *rframe = new QFrame;

  rframe->setMinimumWidth(275);

  QVBoxLayout *rlayout = new QVBoxLayout(rframe);
  rlayout->setMargin(2); rlayout->setSpacing(2);

  CQPropertyTree *tree = new CQPropertyTree;

  canvas->addPropeties(tree);

  rlayout->addWidget(tree);

  QFrame *buttonFrame = new QFrame;

  QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);
  buttonLayout->setMargin(2); buttonLayout->setSpacing(2);

  buttonLayout->addStretch(1);

  rlayout->addWidget(buttonFrame);

  splitter->addWidget(rframe);

  layout->addWidget(splitter);
}

//------

Canvas::
Canvas(QWidget *parent) :
 QWidget(parent), xmin_(-1), ymin_(-1), xmax_(1), ymax_(1)
{
  arrow_ = new CQArrow();

  arrow_->setFrom(QPointF(-0.7,-0.7));
  arrow_->setTo  (QPointF( 0.7, 0.7));

  arrow_->setFHead         (true);
  arrow_->setFrontAngle    (30);
  arrow_->setFrontBackAngle(45);
  arrow_->setFrontLength   (0.4);

  arrow_->setTHead         (true);
  arrow_->setTailAngle     (45);
  arrow_->setTailBackAngle (60);
  arrow_->setTailLength    (0.2);

  arrow_->setFilled (false);
  arrow_->setStroked(true);

  arrow_->setLineWidth(0.1);

  setMouseTracking(true);
}

void
Canvas::
addPropeties(CQPropertyTree *tree)
{
  CQPropertyRealEditor *redit1 = new CQPropertyRealEditor(-1, 9999, 0.01);
  CQPropertyRealEditor *redit2 = new CQPropertyRealEditor(-1, 180, 1);

  tree->addProperty("Arrow", arrow_, "lineWidth")->setEditorFactory(redit2);

  tree->addProperty("Arrow", arrow_, "frontVisible"  );
  tree->addProperty("Arrow", arrow_, "frontType"     );
  tree->addProperty("Arrow", arrow_, "frontAngle"    )->setEditorFactory(redit2);
  tree->addProperty("Arrow", arrow_, "frontBackAngle")->setEditorFactory(redit2);
  tree->addProperty("Arrow", arrow_, "frontLength"   )->setEditorFactory(redit1);
  tree->addProperty("Arrow", arrow_, "frontLineEnds" );

  tree->addProperty("Arrow", arrow_, "tailVisible"  );
  tree->addProperty("Arrow", arrow_, "tailType"     );
  tree->addProperty("Arrow", arrow_, "tailAngle"    )->setEditorFactory(redit2);
  tree->addProperty("Arrow", arrow_, "tailBackAngle")->setEditorFactory(redit2);
  tree->addProperty("Arrow", arrow_, "tailLength"   )->setEditorFactory(redit1);
  tree->addProperty("Arrow", arrow_, "tailLineEnds" );

  tree->addProperty("Arrow", arrow_, "filled"     );
  tree->addProperty("Arrow", arrow_, "stroked"    );
  tree->addProperty("Arrow", arrow_, "strokeWidth")->setEditorFactory(redit2);
  tree->addProperty("Arrow", arrow_, "debugLabels");

  connect(tree, SIGNAL(valueChanged(QObject *, const QString &)), this, SLOT(update()));
}

void
Canvas::
resizeEvent(QResizeEvent *)
{
  pw_ = width ();
  ph_ = height();
}

void
Canvas::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  QFont font("Sans", 24);

  p.setFont(font);

  painter_ = &p;

  pw_ = painter_->device()->width ();
  ph_ = painter_->device()->height();

  painter_->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  painter_->fillRect(QRect(0, 0, pw_, ph_), QBrush(QColor(255,255,255)));

  arrow_->draw(painter_, const_cast<Canvas *>(this));
}

void
Canvas::
mousePressEvent(QMouseEvent *e)
{
  button_  = e->button();
  pressed_ = true;

  mouseMoveEvent(e);
}

void
Canvas::
mouseMoveEvent(QMouseEvent *e)
{
  QPointF p = QPointF(e->pos().x(), e->pos().y());
  QPointF w = pixelToWindow(p);

  if (pressed_) {
    if      (button_ == Qt::LeftButton)
      arrow_->setFrom(w);
    else if (button_ == Qt::MidButton)
      arrow_->setTo  (w);
  }
  else {
    bool inside = arrow_->contains(p);

    if (inside != arrow_->isInside()) {
      arrow_->setInside(inside);

      update();
    }
  }

  update();
}

void
Canvas::
mouseReleaseEvent(QMouseEvent *)
{
  pressed_ = false;
}

QPointF
Canvas::
windowToPixel(const QPointF &w)
{
  int margin = pw_/10;

  int pxmin = margin, pxmax = pw_ - margin;
  int pymin = margin, pymax = ph_ - margin;

  double px = ((w.x() - xmin_)/(xmax_ - xmin_))*(pxmax - pxmin) + pxmin;
  double py = ((w.y() - ymax_)/(ymin_ - ymax_))*(pymax - pymin) + pymin;

  return QPointF(px, py);
}

QPointF
Canvas::
pixelToWindow(const QPointF &p)
{
  int margin = pw_/10;

  int pxmin = margin, pxmax = pw_ - margin;
  int pymin = margin, pymax = ph_ - margin;

  double wx = (p.x() - pxmin)*(xmax_ - xmin_)/(pxmax - pxmin) + xmin_;
  double wy = (p.y() - pymin)*(ymin_ - ymax_)/(pymax - pymin) + ymax_;

  return QPointF(wx, wy);
}

//------

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QFont font("Sans", 16);

  qApp->setFont(font);

  Dialog *dialog = new Dialog;

  dialog->show();

  return app.exec();
}
