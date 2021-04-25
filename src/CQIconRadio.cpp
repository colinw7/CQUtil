#include <CQIconRadio.h>
#include <CQPixmapCache.h>
#include <CQStyleMgr.h>

CQIconRadio::
CQIconRadio(QWidget *parent) :
 QRadioButton(parent)
{
  connect(CQStyleMgrInst, SIGNAL(themeChanged()), this, SLOT(updateIcon()));
  connect(CQStyleMgrInst, SIGNAL(iconSizeChanged()), this, SLOT(updateIcon()));

  updateIcon();
}

void
CQIconRadio::
setIcon(const QString &iconName)
{
  iconName_ = iconName;

  updateIcon();
}

void
CQIconRadio::
setSize(const Size &s)
{
  size_ = s;

  updateIcon();
}

void
CQIconRadio::
updateIcon()
{
  if (iconName_.length())
    QRadioButton::setIcon(CQPixmapCacheInst->getLightDarkIcon(iconName_));

  int is = 16;

  if      (size() == Size::SMALL)
    is = CQStyleMgrInst->calcSmallIconSize();
  else if (size() == Size::LARGE)
    is = CQStyleMgrInst->calcLargeIconSize();
  else
    is = CQStyleMgrInst->iconSize();

  setIconSize(QSize(is, is));
}
