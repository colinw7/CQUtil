#include <CQIconButton.h>
#include <CQPixmapCache.h>
#include <CQStyleMgr.h>

CQIconButton::
CQIconButton(QWidget *parent) :
 QToolButton(parent)
{
  init();
}

CQIconButton::
CQIconButton(const QString &iconName, QWidget *parent) :
 QToolButton(parent), iconName_(iconName)
{
  init();
}

void
CQIconButton::
init()
{
  connect(CQStyleMgrInst, SIGNAL(themeChanged()), this, SLOT(updateIcon()));
  connect(CQStyleMgrInst, SIGNAL(iconSizeChanged()), this, SLOT(updateIcon()));

  updateIcon();
}

void
CQIconButton::
setIcon(const QString &iconName)
{
  iconName_ = iconName;

  updateIcon();
}

void
CQIconButton::
setSize(const Size &s)
{
  size_ = s;

  updateIcon();
}

void
CQIconButton::
updateIcon()
{
  if (iconName_.length())
    QToolButton::setIcon(CQPixmapCacheInst->getLightDarkIcon(iconName_));

  int is = 16;

  if      (size() == Size::SMALL)
    is = CQStyleMgrInst->calcSmallIconSize();
  else if (size() == Size::LARGE)
    is = CQStyleMgrInst->calcLargeIconSize();
  else
    is = CQStyleMgrInst->iconSize();

  setIconSize(QSize(is, is));
}
