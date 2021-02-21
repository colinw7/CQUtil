#include <CQEnumCombo.h>
#include <CQUtil.h>
#include <iostream>

CQEnumCombo::
CQEnumCombo(QWidget *parent, const QObject *obj, const QString &propName) :
 QComboBox(parent), obj_(obj), propName_(propName)
{
  init();

  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePropertySlot()));
}

CQEnumCombo::
CQEnumCombo(const QObject *obj, const QString &propName) :
 QComboBox(), obj_(obj), propName_(propName)
{
  init();

  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePropertySlot()));
}

void
CQEnumCombo::
init()
{
  setObjectName("enumCombo");

  updateItems();
}

void
CQEnumCombo::
setObj(const QObject *o)
{
  obj_ = o;

  updateItems();
}

void
CQEnumCombo::
setPropName(const QString &s)
{
  propName_ = s;

  updateItems();
}

void
CQEnumCombo::
updateItems()
{
  disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePropertySlot()));

  clear();

  if (! obj_)
    return;

  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (! CQUtil::getPropInfo(obj_, propName_, &propInfo))
    return;

  if (! propInfo.isEnumType())
    return;

  const auto &names = propInfo.enumNames();

  for (int i = 0; i < names.length(); ++i)
    addItem(names[i], QVariant(i));

  QVariant var;

  if (! CQUtil::getProperty(obj_, propName_, var))
    var = QVariant(0);

  int ind = var.toInt();

  if (ind >= 0)
    setCurrentIndex(ind);

  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePropertySlot()));
}

void
CQEnumCombo::
setItemIcon(int i, const QIcon &icon)
{
  QComboBox::setItemIcon(i, icon);
}

void
CQEnumCombo::
setItemText(int i, const QString &text)
{
  QComboBox::setItemText(i, text);
}

void
CQEnumCombo::
setItemData(int i, const QString &text, const QIcon &icon)
{
  QComboBox::setItemText(i, text);
  QComboBox::setItemIcon(i, icon);
}

void
CQEnumCombo::
updatePropertySlot()
{
  CQUtil::PropInfo propInfo;
  QString          typeName;

  if (! CQUtil::getPropInfo(obj_, propName_, &propInfo))
    return;

  if (! propInfo.isEnumType())
    return;

  QVariant v(currentIndex());

  if (! CQUtil::setProperty(obj_, propName_, v)) {
    std::cerr << "Failed to set property " << propName_.toStdString() << std::endl;
  }
}
