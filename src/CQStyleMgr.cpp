#include <CQStyleMgr.h>
#include <CQStyle.h>

#include <QApplication>

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

    style_->setTheme((CQStyle::Theme) theme_);
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
    style_->setTheme((CQStyle::Theme) theme_);

  emit themeChanged();
}

QColor
CQStyleMgr::
calcBaseColor() const
{
  return (theme() == Theme::DARK ? darkBaseColor() : lightBaseColor());
}
