#ifndef CQWidgetUtil_H
#define CQWidgetUtil_H

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
      if (sizePolicy.horizontalPolicy() & QSizePolicy::ShrinkFlag)
        s.setWidth(minSizeHint.width());
      else
        s.setWidth(qMax(sizeHint.width(), minSizeHint.width()));
    }

    if (sizePolicy.verticalPolicy() != QSizePolicy::Ignored) {
      if (sizePolicy.verticalPolicy() & QSizePolicy::ShrinkFlag)
        s.setHeight(minSizeHint.height());
      else
        s.setHeight(qMax(sizeHint.height(), minSizeHint.height()));
    }

    s = s.boundedTo(maxSize);

    if (minSize.width() > 0)
      s.setWidth(minSize.width());

    if (minSize.height() > 0)
      s.setHeight(minSize.height());

    return s.expandedTo(QSize(0,0));
  }

  inline QSize SmartMinSize(const QWidget *w) {
    return SmartMinSize(w->sizeHint(), w->minimumSizeHint(),
                        w->minimumSize(), w->maximumSize(),
                        w->sizePolicy());
  }
}

#endif
