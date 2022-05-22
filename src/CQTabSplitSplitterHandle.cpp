#include <CQTabSplitSplitterHandle.h>
#include <CQTabSplitSplitter.h>
#include <CQUtil.h>

#include <QContextMenuEvent>
#include <QPainter>

CQTabSplitSplitterHandle::
CQTabSplitSplitterHandle(Qt::Orientation orient, CQTabSplitSplitter *splitter) :
 QSplitterHandle(orient, splitter), splitter_(splitter)
{
  setObjectName("splitterHandle");

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQTabSplitSplitterHandle::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = CQUtil::makeWidget<QMenu>("menu");

  //---

  auto *tabAction = menu->addAction("Tabbed");

  connect(tabAction, SIGNAL(triggered()), this, SLOT(tabSlot()));

  if (splitter()->orientation() == Qt::Horizontal) {
    auto *splitAction = menu->addAction("Horizontal Split");

    connect(splitAction, SIGNAL(triggered()), this, SLOT(splitSlot()));
  }
  else {
    auto *splitAction = menu->addAction("Vertical Split");

    connect(splitAction, SIGNAL(triggered()), this, SLOT(splitSlot()));
  }

  auto *fitAllAction = menu->addAction("Fit All");

  connect(fitAllAction, SIGNAL(triggered()), this, SLOT(fitAllSlot()));

  //---

  (void) menu->exec(e->globalPos());

  delete menu;
}

void
CQTabSplitSplitterHandle::
mouseDoubleClickEvent(QMouseEvent *)
{
  int ind = splitter()->handleIndex(this);

  splitter()->autoFit(ind);
}

void
CQTabSplitSplitterHandle::
paintEvent(QPaintEvent *)
{
  auto blendColors = [](const QColor &c1, const QColor &c2, double f) {
    double f1 = 1.0 - f;

    double r = c1.redF  ()*f + c2.redF  ()*f1;
    double g = c1.greenF()*f + c2.greenF()*f1;
    double b = c1.blueF ()*f + c2.blueF ()*f1;

    return QColor(std::min(std::max(int(255*r), 0), 255),
                  std::min(std::max(int(255*g), 0), 255),
                  std::min(std::max(int(255*b), 0), 255));
  };

  //QSplitterHandle::paintEvent(e);

  QPainter painter(this);

  auto wc = palette().color(QPalette::Window);

  auto fc = (hover_ ? blendColors(palette().color(QPalette::Highlight), wc, 0.3) :
                      blendColors(palette().color(QPalette::Highlight), wc, 0.1));

  painter.fillRect(rect(), fc);

  //---

  auto fg = palette().color(QPalette::Text);
  auto bg = palette().color(QPalette::Base);

  int ss = 2*barSize();

  if (orientation() == Qt::Horizontal) {
    int ym = rect().center().y();
    int x1 = rect().left  () + 1;
    int x2 = rect().right () - 1;

    painter.fillRect(QRect(x1, ym - ss/2, x2 - x1 + 1, ss), bg);

    //---

    painter.setPen(fg);

    painter.drawLine(x1 + 1, ym - 4, x2 - 1, ym - 4);
    painter.drawLine(x1 + 1, ym    , x2 - 1, ym    );
    painter.drawLine(x1 + 1, ym + 4, x2 - 1, ym + 4);

    //---

    if (hover_) {
      painter.setRenderHints(QPainter::Antialiasing, true);

      int th = barSize()/2;

      QPainterPath path1;

      int yt1 = ym + ss;

      path1.moveTo(x1 - 1, yt1     );
      path1.lineTo(x2    , yt1 + th);
      path1.lineTo(x2    , yt1 - th);

      painter.fillPath(path1, bg);

      QPainterPath path2;

      int yt2 = ym - ss;

      path2.moveTo(x2 + 1, yt2     );
      path2.lineTo(x1    , yt2 + th);
      path2.lineTo(x1    , yt2 - th);

      painter.fillPath(path2, bg);
    }
  }
  else {
    int xm = rect().center().x();
    int y1 = rect().top   () + 1;
    int y2 = rect().bottom() - 1;

    painter.fillRect(QRect(xm - ss/2, y1, ss, y2 - y1 + 1), bg);

    //---

    painter.setPen(fg);

    painter.drawLine(xm - 4, y1 + 1, xm - 4, y2 - 1);
    painter.drawLine(xm    , y1 + 1, xm    , y2 - 1);
    painter.drawLine(xm + 4, y1 + 1, xm + 4, y2 - 1);

    //---

    if (hover_) {
      painter.setRenderHints(QPainter::Antialiasing, true);

      int tw = barSize()/2;

      QPainterPath path1;

      int xt1 = xm + ss;

      path1.moveTo(xt1     , y1 - 1);
      path1.lineTo(xt1 + tw, y2    );
      path1.lineTo(xt1 - tw, y2    );

      painter.fillPath(path1, bg);

      QPainterPath path2;

      int xt2 = xm - ss;

      path2.moveTo(xt2     , y2 + 1);
      path2.lineTo(xt2 + tw, y1    );
      path2.lineTo(xt2 - tw, y1    );

      painter.fillPath(path2, bg);
    }
  }
}

bool
CQTabSplitSplitterHandle::
event(QEvent *event)
{
  switch (event->type()) {
    case QEvent::HoverEnter: hover_ = true ; update(); break;
    case QEvent::HoverLeave: hover_ = false; update(); break;

    case QEvent::ToolTip: {
      auto *he = static_cast<QHelpEvent*>(event);

      splitter()->showTool(mapToGlobal(he->pos()));

      break;
    }

    default: break;
  }

  return QSplitterHandle::event(event);
}

void
CQTabSplitSplitterHandle::
tabSlot()
{
  splitter()->setTabbed();
}

void
CQTabSplitSplitterHandle::
splitSlot()
{
  splitter()->toggleSplit();
}

void
CQTabSplitSplitterHandle::
fitAllSlot()
{
  splitter()->fitAll();
}

QSize
CQTabSplitSplitterHandle::
sizeHint() const
{
  auto s = QSplitterHandle::sizeHint();

  if (splitter()->orientation() == Qt::Horizontal)
    return QSize(barSize(), s.height());
  else
    return QSize(s.width(), barSize());
}
