#include <CQAppOptions.h>
#include <CQApp.h>
#include <CQStyleMgr.h>
#include <CQFontChooser.h>
#include <CQIntegerSpin.h>

void
CQAppOptions::
show()
{
  static CQAppOptions *options;

  if (! options)
    options = new CQAppOptions;

  options->CQDialog::show();
  options->raise();
}

CQAppOptions::
CQAppOptions(QWidget *parent) :
 CQDialog(parent)
{
  auto *vfontChooser = addFieldT<CQFontChooser>("Variable Font", new CQFontChooser);
  auto *ffontChooser = addFieldT<CQFontChooser>("Fixed Font"   , new CQFontChooser);

  vfontChooser->setFont(qApp->font());
  ffontChooser->setFont(CQStyleMgrInst->fixedFont());

  connect(vfontChooser, SIGNAL(fontChanged(const QFont &)), this, SLOT(vfontSlot(const QFont &)));
  connect(ffontChooser, SIGNAL(fontChanged(const QFont &)), this, SLOT(ffontSlot(const QFont &)));

  auto *iconSizeSpin      = addFieldT("Icon Size"      , new CQIntegerSpin);
  auto *largeIconSizeSpin = addFieldT("Large Icon Size", new CQIntegerSpin);
  auto *smallIconSizeSpin = addFieldT("Small Icon Size", new CQIntegerSpin);

  iconSizeSpin->setValue(CQStyleMgrInst->iconSize());
  iconSizeSpin->setRange(4, 99);

  largeIconSizeSpin->setValue(CQStyleMgrInst->largeIconSize());
  largeIconSizeSpin->setRange(-1, 99);

  smallIconSizeSpin->setValue(CQStyleMgrInst->smallIconSize());
  smallIconSizeSpin->setRange(-1, 99);

  connect(iconSizeSpin, SIGNAL(valueChanged(int)), this, SLOT(iconSizeChanged(int)));
  connect(largeIconSizeSpin, SIGNAL(valueChanged(int)), this, SLOT(largeIconSizeChanged(int)));
  connect(smallIconSizeSpin, SIGNAL(valueChanged(int)), this, SLOT(smallIconSizeChanged(int)));

  addCheckBox("Dark", this, SLOT(darkSlot(bool)));

  addStretch();
}

void
CQAppOptions::
darkSlot(bool b)
{
  CQStyleMgrInst->setTheme(b ? CQStyleMgr::Theme::DARK : CQStyleMgr::Theme::LIGHT);
}

void
CQAppOptions::
vfontSlot(const QFont &font)
{
  CQStyleMgrInst->setFont(font);
}

void
CQAppOptions::
ffontSlot(const QFont &font)
{
  CQStyleMgrInst->setFixedFont(font);
}

void
CQAppOptions::
iconSizeChanged(int i)
{
  CQStyleMgrInst->setIconSize(i);
}

void
CQAppOptions::
largeIconSizeChanged(int i)
{
  CQStyleMgrInst->setLargeIconSize(i);
}

void
CQAppOptions::
smallIconSizeChanged(int i)
{
  CQStyleMgrInst->setSmallIconSize(i);
}
