#include <CQIconLabel.h>
#include <CQPixmapCache.h>
#include <CQStyleMgr.h>

CQIconLabel::
CQIconLabel(QWidget *parent) :
 QLabel(parent)
{
  connect(CQStyleMgrInst, SIGNAL(themeChanged()), this, SLOT(updateIcon()));
  connect(CQStyleMgrInst, SIGNAL(iconSizeChanged()), this, SLOT(updateIcon()));

  updateIcon();
}

void
CQIconLabel::
setIcon(const QString &iconName)
{
  iconName_ = iconName;

  updateIcon();
}

void
CQIconLabel::
setSize(const Size &s)
{
  size_ = s;

  updateIcon();
}

void
CQIconLabel::
updateIcon()
{
  int is = 16;

  if      (size() == Size::SMALL)
    is = CQStyleMgrInst->calcSmallIconSize();
  else if (size() == Size::LARGE)
    is = CQStyleMgrInst->calcLargeIconSize();
  else
    is = CQStyleMgrInst->iconSize();

  if (iconName_.length()) {
    auto icon   = CQPixmapCacheInst->getLightDarkIcon(iconName_);
    auto pixmap = icon.pixmap(QSize(is, is));

    QLabel::setPixmap(pixmap);
  }
}
