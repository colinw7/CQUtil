#ifndef CQStyle_H
#define CQStyle_H

#include <QProxyStyle>

class CQStyle : public QProxyStyle {
 public:
  CQStyle();

  void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                           QPainter *painter, const QWidget *widget = 0) const override;
#if 0
  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter,
                   const QWidget *widget = 0) const override;
  void drawItemPixmap(QPainter *painter, const QRect &rect, int alignment,
                      const QPixmap &pixmap) const override;
  void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal,
                    bool enabled, const QString &text,
                    QPalette::ColorRole textRole = QPalette::NoRole) const override;
#endif

  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                     const QWidget *widget = 0) const override;

#if 0
  QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                              const QStyleOption *opt) const override;
#endif

  SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                   const QPoint &pos, const QWidget *widget = 0) const override;

#if 0
  QRect itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const override;
  QRect itemTextRect(const QFontMetrics &fm, const QRect &r, int flags, bool enabled,
                     const QString &text) const override;
#endif

  int pixelMetric(PixelMetric metric, const QStyleOption *option = 0,
                  const QWidget *widget = 0) const override;

#if 0
  void polish(QWidget *widget) override;
  void polish(QPalette &pal) override;
  void polish(QApplication *app) override;
#endif

  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size,
                         const QWidget *widget) const override;

#if 0
  QPalette standardPalette() const override;
  QPixmap  standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                          const QWidget *widget = 0) const override;
#endif

  int styleHint(StyleHint hint, const QStyleOption *option = 0, const QWidget *widget = 0,
                QStyleHintReturn *returnData = 0) const override;

  QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *option, SubControl sc,
                       const QWidget *widget) const override;
#if 0
  QRect subElementRect(SubElement element, const QStyleOption *option,
                       const QWidget *widget) const override;

  void unpolish(QWidget *widget) override;
  void unpolish(QApplication *app) override;
#endif

 private:
  void drawCheck(QPainter *p, const QRectF &r, const QBrush &b) const;
};

#endif
