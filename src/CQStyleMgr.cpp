#include <CQStyleMgr.h>
#include <CQStyle.h>

#include <QApplication>
#include <QWidget>

CQStyleMgr *
CQStyleMgr::
instance()
{
  static CQStyleMgr *instance;

  if (! instance)
    instance = new CQStyleMgr;

  return instance;
}

CQStyleMgr::
CQStyleMgr()
{
  lightBaseColor_ = QColor("#4444aa");
  darkBaseColor_  = QColor("#aaaa44");
};

CQStyle *
CQStyleMgr::
createStyle()
{
  if (! style_) {
    style_ = new CQStyle;

    style_->setTheme(static_cast<CQStyle::Theme>(theme_));
  }

  qApp->setStyle(style_);

  return style_;
}

void
CQStyleMgr::
setTheme(const Theme &theme)
{
  theme_ = theme;

  if (style_)
    style_->setTheme(static_cast<CQStyle::Theme>(theme_));

  emit themeChanged();
}

QColor
CQStyleMgr::
calcBaseColor() const
{
  return (theme() == Theme::DARK ? darkBaseColor() : lightBaseColor());
}

void
CQStyleMgr::
setIconSize(int i)
{
  iconSize_ = i;

  emit iconSizeChanged();
}

void
CQStyleMgr::
setIconSizeFromFont()
{
  QFontMetrics fm(font_);

  setIconSize(fm.height() + 4);
}

void
CQStyleMgr::
setLargeIconSize(int i)
{
  largeIconSize_ = i;

  emit iconSizeChanged();
}

int
CQStyleMgr::
calcLargeIconSize() const
{
  if (largeIconSize_ > 0)
    return largeIconSize_;

  return int(1.25*iconSize_);
}

void
CQStyleMgr::
setSmallIconSize(int i)
{
  smallIconSize_ = i;

  emit iconSizeChanged();
}

int
CQStyleMgr::
calcSmallIconSize() const
{
  if (smallIconSize_ > 0)
    return smallIconSize_;

  return int(0.85*iconSize_);
}

void
CQStyleMgr::
setFont(const QFont &font)
{
  font_ = font;

  qApp->setFont(font_);

  const auto &widgets = qApp->allWidgets();

  for (const auto &w : widgets)
    w->setFont(font_);

  emit fontChanged();
}

void
CQStyleMgr::
setFixedFont(const QFont &font)
{
  fixedFont_ = font;

  emit fixedFontChanged();
}
