#ifndef CQStyleMgr_H
#define CQStyleMgr_H

#include <QObject>
#include <QColor>
#include <QFont>

#define CQStyleMgrInst CQStyleMgr::instance()

class CQStyle;

class CQStyleMgr : public QObject {
  Q_OBJECT

  Q_PROPERTY(Theme  theme          READ theme          WRITE setTheme         )
  Q_PROPERTY(QColor lightBaseColor READ lightBaseColor WRITE setLightBaseColor)
  Q_PROPERTY(QColor darkBaseColor  READ darkBaseColor  WRITE setDarkBaseColor )
  Q_PROPERTY(int    iconSize       READ iconSize       WRITE setIconSize      )
  Q_PROPERTY(int    smallIconSize  READ smallIconSize  WRITE setSmallIconSize )
  Q_PROPERTY(int    largeIconSize  READ largeIconSize  WRITE setLargeIconSize )
  Q_PROPERTY(QFont  font           READ font           WRITE setFont          )
  Q_PROPERTY(QFont  fixedFont      READ fixedFont      WRITE setFixedFont     )

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

  //---

  int iconSize() const { return iconSize_; }
  void setIconSize(int i);

  void setIconSizeFromFont();

  int smallIconSize() const { return smallIconSize_; }
  void setSmallIconSize(int i);

  int calcSmallIconSize() const;

  int largeIconSize() const { return largeIconSize_; }
  void setLargeIconSize(int i);

  int calcLargeIconSize() const;

  //---

  const QFont &font() const { return font_; }
  void setFont(const QFont &font);

  const QFont &fixedFont() const { return fixedFont_; }
  void setFixedFont(const QFont &font);

 protected:
  CQStyleMgr();

 signals:
  void themeChanged();
  void colorsChanged();

  void iconSizeChanged();

  void fontChanged();
  void fixedFontChanged();

 private:
  CQStyle* style_         { nullptr };
  QColor   lightBaseColor_;
  QColor   darkBaseColor_;
  Theme    theme_         { Theme::LIGHT };
  QFont    font_;
  QFont    fixedFont_;
  int      iconSize_      { 16 };
  int      largeIconSize_ { -1 };
  int      smallIconSize_ { -1 };
};

#endif
