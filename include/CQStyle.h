#ifndef CQStyle_H
#define CQStyle_H

#include <CQStyleMgr.h>
#include <QProxyStyle>

class CQStyle : public QProxyStyle {
  Q_PROPERTY(Theme theme READ theme WRITE setTheme)

  Q_ENUMS(Theme)

 public:
  enum class Theme {
    LIGHT = int(CQStyleMgr::Theme::LIGHT),
    DARK  = int(CQStyleMgr::Theme::DARK)
  };

 public:
  CQStyle();

  //---

  const Theme &theme() const { return theme_; }
  void setTheme(const Theme &t);

  //---

  void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                           QPainter *painter, const QWidget *widget = nullptr) const override;

  void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter,
                   const QWidget *widget = nullptr) const override;

  void drawItemPixmap(QPainter *painter, const QRect &rect, int alignment,
                      const QPixmap &pixmap) const override;

  void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal,
                    bool enabled, const QString &text,
                    QPalette::ColorRole textRole = QPalette::NoRole) const override;

  void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                     const QWidget *widget = nullptr) const override;

  QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                              const QStyleOption *opt) const override;

  SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                   const QPoint &pos,
                                   const QWidget *widget = nullptr) const override;

  QRect itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const override;
  QRect itemTextRect(const QFontMetrics &fm, const QRect &r, int flags, bool enabled,
                     const QString &text) const override;

  int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr,
                  const QWidget *widget = nullptr) const override;

#if 0
  void polish(QWidget *widget) override;
  void polish(QPalette &pal) override;
  void polish(QApplication *app) override;
#endif

  QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size,
                         const QWidget *widget) const override;

  QPalette standardPalette() const override;

  QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                         const QWidget *widget = nullptr) const override;

  int styleHint(StyleHint hint, const QStyleOption *option = nullptr,
                const QWidget *widget = nullptr,
                QStyleHintReturn *returnData = nullptr) const override;

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

 private:
  Theme    theme_ { Theme::LIGHT };
  QPalette palette_;
};

#endif
