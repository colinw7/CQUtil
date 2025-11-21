#include <CQStyleMgr.h>
#include <CQStyle.h>

#include <QApplication>
#include <QWidget>
#include <QAction>

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
  setObjectName("styleMgr");

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

  Q_EMIT themeChanged();
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

  Q_EMIT iconSizeChanged();
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

  Q_EMIT iconSizeChanged();
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

  Q_EMIT iconSizeChanged();
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

  updateFont();

  Q_EMIT fontChanged();
}

void
CQStyleMgr::
updateFont()
{
  qApp->setFont(font_);

  const auto &widgets = qApp->allWidgets();

  for (const auto &w : widgets) {
    w->setFont(font_);

    for (auto *a : w->actions())
      a->setFont(font_);
  }
}

void
CQStyleMgr::
setFixedFont(const QFont &font)
{
  fixedFont_ = font;

  Q_EMIT fixedFontChanged();
}
