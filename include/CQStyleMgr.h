#ifndef CQStyleMgr_H
#define CQStyleMgr_H

#include <QObject>
#include <QColor>

#define CQStyleMgrInst CQStyleMgr::instance()

class CQStyle;

class CQStyleMgr : public QObject {
  Q_OBJECT

  Q_PROPERTY(Theme  theme          READ theme          WRITE setTheme         )
  Q_PROPERTY(QColor lightBaseColor READ lightBaseColor WRITE setLightBaseColor)
  Q_PROPERTY(QColor darkBaseColor  READ darkBaseColor  WRITE setDarkBaseColor )

  Q_ENUMS(Theme)

 public:
  enum class Theme {
    LIGHT,
    DARK
  };

 public:
  static CQStyleMgr *instance();

  CQStyle *createStyle();
  CQStyle *style() const { return style_; }

  //---

  const Theme &theme() const { return theme_; }
  void setTheme(const Theme &t);

  //---

  QColor calcBaseColor() const;

  //---

  const QColor &lightBaseColor() const { return lightBaseColor_; }
  void setLightBaseColor(const QColor &c) { lightBaseColor_ = c; emit colorsChanged(); }

  const QColor &darkBaseColor() const { return darkBaseColor_; }
  void setDarkBaseColor(const QColor &c) { darkBaseColor_ = c; emit colorsChanged(); }

 protected:
  CQStyleMgr();

 signals:
  void themeChanged();
  void colorsChanged();

 private:
  CQStyle* style_ { nullptr };
  QColor   lightBaseColor_;
  QColor   darkBaseColor_;
  Theme    theme_ { Theme::LIGHT };
};

#endif
