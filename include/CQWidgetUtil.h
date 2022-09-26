#ifndef CQWidgetUtil_H
#define CQWidgetUtil_H

#include <QWheelEvent>
#include <QApplication>
#include <QScreen>

namespace CQWidgetUtil {
  inline void resetWidgetMinMaxWidth(QWidget *w) {
    w->setMinimumWidth(0); w->setMaximumWidth(QWIDGETSIZE_MAX);
  }

  inline void resetWidgetMinMaxHeight(QWidget *w) {
    w->setMinimumHeight(0); w->setMaximumHeight(QWIDGETSIZE_MAX);
  }

  inline void resetWidgetMinMaxSize(QWidget *w) {
    resetWidgetMinMaxWidth (w);
    resetWidgetMinMaxHeight(w);
  }

  inline void setWidgetFixedWidth(QWidget *w, int width) {
    w->setFixedWidth(width);
  }

  inline void setWidgetFixedHeight(QWidget *w, int height) {
    w->setFixedHeight(height);
  }

  inline void setWidgetMinMaxWidth(QWidget *w, int min_w, int max_w) {
    w->setMinimumWidth(min_w); w->setMaximumWidth(max_w);
  }

  inline void setWidgetMinMaxHeight(QWidget *w, int min_h, int max_h) {
    w->setMinimumHeight(min_h); w->setMaximumHeight(max_h);
  }

  inline QSize SmartMinSize(const QSize &sizeHint, const QSize &minSizeHint,
                            const QSize &minSize, const QSize &maxSize,
                            const QSizePolicy &sizePolicy) {
    QSize s(0, 0);

    if (sizePolicy.horizontalPolicy() != QSizePolicy::Ignored) {
      if (uint(sizePolicy.horizontalPolicy()) & QSizePolicy::ShrinkFlag)
        s.setWidth(minSizeHint.width());
      else
        s.setWidth(qMax(sizeHint.width(), minSizeHint.width()));
    }

    if (sizePolicy.verticalPolicy() != QSizePolicy::Ignored) {
      if (uint(sizePolicy.verticalPolicy()) & QSizePolicy::ShrinkFlag)
        s.setHeight(minSizeHint.height());
      else
        s.setHeight(qMax(sizeHint.height(), minSizeHint.height()));
    }

    s = s.boundedTo(maxSize);

    if (minSize.width() > 0)
      s.setWidth(minSize.width());

    if (minSize.height() > 0)
      s.setHeight(minSize.height());

    return s.expandedTo(QSize(0, 0));
  }

  inline QSize SmartMinSize(const QWidget *w) {
    return SmartMinSize(w->sizeHint(), w->minimumSizeHint(),
                        w->minimumSize(), w->maximumSize(),
                        w->sizePolicy());
  }

  inline void getContentsMargins(const QWidget *w, int *l, int *t, int *r, int *b) {
    auto m = w->contentsMargins();

    if (l) *l = m.left  ();
    if (t) *t = m.top   ();
    if (r) *r = m.right ();
    if (b) *b = m.bottom();
  }

  inline int wheelDelta(QWheelEvent *e) {
    return e->angleDelta().y();
  }

  inline QRect desktopAvailableGeometry(QWidget * /*w*/ =nullptr) {
#if 0
    if (w)
      return qApp->desktop()->availableGeometry(w);
    else
      return qApp->desktop()->availableGeometry();
#else
    auto *screen = QApplication::screens().at(0);

    return screen->availableGeometry();
#endif
  }

  inline QRect desktopAvailableGeometry(const QWidget * /*w*/, const QPoint &pos) {
#if 0
    auto *desktop = QApplication::desktop();

    int snum;

    if (desktop->isVirtualDesktop())
      snum = desktop->screenNumber(pos);
    else
      snum = desktop->screenNumber(this);

    return desktop->availableGeometry(snum);
#else
    auto *screen = qApp->screenAt(pos);

    return screen->availableGeometry();
#endif
  }
}

#endif
