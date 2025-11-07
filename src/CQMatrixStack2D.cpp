#include <CQMatrixStack2D.h>
#include <CQRealSpin.h>
#include <CQIconButton.h>
#include <CQUtil.h>

#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QHBoxLayout>

#include <svg/up_light_svg.h>
#include <svg/down_light_svg.h>
#include <svg/delete_svg.h>
#include <svg/delete_all_svg.h>

#include <iostream>

//---

CQMatrixStack2D::
CQMatrixStack2D(QWidget *parent) :
 QFrame(parent)
{
  auto *layout = new QVBoxLayout(this);

  auto addLabelWidget = [&](QBoxLayout *l, const QString &label, auto *w) {
    auto *frame   = new QFrame;
    auto *flayout = new QHBoxLayout(frame);

    flayout->addWidget(new QLabel(label));
    flayout->addWidget(w);

    l->addWidget(frame);

    return w;
  };

  //---

  auto *modeFrame  = new QFrame;
  auto *modeLayout = new QHBoxLayout(modeFrame);

  auto *modifyRadio = new QRadioButton("Modify");
  auto *addRadio    = new QRadioButton("Add");

  auto *modeGroup = new QButtonGroup;

  modeGroup->addButton(modifyRadio);
  modeGroup->addButton(addRadio);

  modeGroup->setId(modifyRadio, 0);
  modeGroup->setId(addRadio   , 1);

  modeLayout->addWidget(modifyRadio);
  modeLayout->addWidget(addRadio);
  modeLayout->addStretch(1);

  layout->addWidget(modeFrame);

  modifyRadio->setChecked(true);

  connect(modeGroup, SIGNAL(buttonClicked(int)), this, SLOT(modeSlot(int)));

  //---

  typeCombo_ = new QComboBox;

  typeCombo_->addItems(QStringList() << "Translate" << "Scale" << "Rotate");

  layout->addWidget(typeCombo_);

  typeCombo_->setVisible(false);

  //---

  typeLabel_ = new QLabel;

  layout->addWidget(typeLabel_);

  //---

  stack_ = new QStackedWidget;

  layout->addWidget(stack_);

  //---

  auto *nullFrame = new QFrame;

  stack_->addWidget(nullFrame);

  //---

  auto *translateFrame  = new QFrame;
  auto *translateLayout = new QVBoxLayout(translateFrame);

  translateX_ = addLabelWidget(translateLayout, "X", new CQRealSpin);
  translateY_ = addLabelWidget(translateLayout, "Y", new CQRealSpin);

  translateLayout->addStretch(1);

  stack_->addWidget(translateFrame);

  //---

  auto *scaleFrame  = new QFrame;
  auto *scaleLayout = new QVBoxLayout(scaleFrame);

  scaleX_ = addLabelWidget(scaleLayout, "X", new CQRealSpin);
  scaleY_ = addLabelWidget(scaleLayout, "Y", new CQRealSpin);

  scaleLayout->addStretch(1);

  stack_->addWidget(scaleFrame);

  //---

  auto *rotateFrame  = new QFrame;
  auto *rotateLayout = new QVBoxLayout(rotateFrame);

  rotate_ = addLabelWidget(rotateLayout, "Angle", new CQRealSpin);

  rotateLayout->addStretch(1);

  stack_->addWidget(rotateFrame);

  //---

  auto *bframe  = new QFrame;
  auto *blayout = new QHBoxLayout(bframe);

  autoUpdateCheck_ = new QCheckBox("Auto Update");
  updateButton_    = new QPushButton("Update");
  addButton_       = new QPushButton("Add");

  blayout->addWidget(addButton_);
  blayout->addWidget(autoUpdateCheck_);
  blayout->addWidget(updateButton_);
  blayout->addStretch(1);

  connect(updateButton_, SIGNAL(clicked()), this, SLOT(updateSlot()));
  connect(addButton_, SIGNAL(clicked()), this, SLOT(addSlot()));

  layout->addWidget(bframe);

  addButton_->setVisible(false);

  //---

  auto *lframe  = new QFrame;
  auto *llayout = new QHBoxLayout(lframe);

  lframe->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(lframe);

  list_ = new QListWidget;

  list_->setSelectionMode(QListWidget::SingleSelection);
  list_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  llayout->addWidget(list_);

  auto *lbframe  = new QFrame;
  auto *lblayout = new QVBoxLayout(lbframe);

  lbframe->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  lblayout->setMargin(0); lblayout->setSpacing(2);

  llayout->addWidget(lbframe);

  auto addToolButton = [&](const QString &name, const QString &iconName,
                           const QString &tip, const char *slotName) {
    auto *button = new CQIconButton;

    button->setObjectName(name);
    button->setIcon(iconName);
    button->setIconSize(QSize(32, 32));
    button->setAutoRaise(true);
    button->setToolTip(tip);

    connect(button, SIGNAL(clicked()), this, slotName);

    return button;
  };

  upButton_     = addToolButton("up"    , "UP_LIGHT"  , "Up"    , SLOT(upSlot()));
  downButton_   = addToolButton("down"  , "DOWN_LIGHT", "Down"  , SLOT(downSlot()));
  deleteButton_ = addToolButton("delete", "DELETE"    , "Delete", SLOT(deleteSlot()));
  clearButton_  = addToolButton("clear" , "DELETE_ALL", "Clear" , SLOT(clearSlot()));

  lblayout->addWidget(upButton_);
  lblayout->addWidget(downButton_);
  lblayout->addWidget(deleteButton_);
  lblayout->addWidget(clearButton_);
  lblayout->addStretch(1);

  //---

  connectSlots(true);
}

void
CQMatrixStack2D::
setMatrixStack(const CMatrixStack2D &matrixStack)
{
  matrixStack_ = matrixStack;

  updateWidgets();
}

void
CQMatrixStack2D::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b, typeCombo_, SIGNAL(currentIndexChanged(int)),
                            this, SLOT(typeSlot(int)));

  CQUtil::connectDisconnect(b, translateX_, SIGNAL(realValueChanged(double)),
                            this, SLOT(updateEditSlot(double)));
  CQUtil::connectDisconnect(b, translateY_, SIGNAL(realValueChanged(double)),
                            this, SLOT(updateEditSlot(double)));
  CQUtil::connectDisconnect(b, scaleX_, SIGNAL(realValueChanged(double)),
                            this, SLOT(updateEditSlot(double)));
  CQUtil::connectDisconnect(b, scaleY_, SIGNAL(realValueChanged(double)),
                            this, SLOT(updateEditSlot(double)));
  CQUtil::connectDisconnect(b, rotate_, SIGNAL(realValueChanged(double)),
                            this, SLOT(updateEditSlot(double)));

  CQUtil::connectDisconnect(b, autoUpdateCheck_, SIGNAL(stateChanged(int)),
                            this, SLOT(autoUpdateSlot(int)));

  CQUtil::connectDisconnect(b, list_,
    SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
    this, SLOT(itemSelectedSlot(QListWidgetItem *, QListWidgetItem *)));
}

void
CQMatrixStack2D::
updateWidgets()
{
  connectSlots(false);

  //---

  autoUpdateCheck_->setChecked(autoUpdate_);

  //---

  list_->clear();

  auto n = matrixStack_.length();

  QListWidgetItem *selectedItem = nullptr;

  for (size_t i = 0; i < n; ++i) {
    const auto &t = matrixStack_.transform(i);

    auto label = QString::fromStdString(t.name());

    if      (t.type() == CMatrixTransformType::TRANSLATE) {
      label += QString(" %1 %2").arg(t.dx()).arg(t.dy());
    }
    else if (t.type() == CMatrixTransformType::SCALE2) {
      label += QString(" %1 %2").arg(t.xscale()).arg(t.yscale());
    }
    else if (t.type() == CMatrixTransformType::ROTATE) {
      auto a = CMathGen::RadToDeg(t.angle());

      label += QString(" %1").arg(a);
    }

    auto *item = new QListWidgetItem(label);

    item->setData(Qt::UserRole, int(i));

    list_->addItem(item);

    if (int(i) == ind_)
      selectedItem = item;
  }

  if (selectedItem)
    list_->setCurrentItem(selectedItem);

  connectSlots(true);
}

void
CQMatrixStack2D::
modeSlot(int id)
{
  typeLabel_->setVisible(id == 0);
  typeCombo_->setVisible(id == 1);

  autoUpdateCheck_->setVisible(id == 0);
  updateButton_   ->setVisible(id == 0);
  addButton_      ->setVisible(id == 1);

  if (id == 1) {
    stack_->setCurrentIndex(addType_ + 1);
  }
  else {
    stack_->setCurrentIndex(0);

    typeLabel_->setText(" ");

    updateCurrentIndex();
  }
}

void
CQMatrixStack2D::
typeSlot(int ind)
{
  addType_ = ind;

  stack_->setCurrentIndex(ind + 1);
}

void
CQMatrixStack2D::
updateEditSlot(double)
{
  if (autoUpdate_)
    updateSlot();
}

void
CQMatrixStack2D::
autoUpdateSlot(int state)
{
  autoUpdate_ = state;
}

void
CQMatrixStack2D::
updateSlot()
{
  auto n = matrixStack_.length();
  if (ind_ < 0 || size_t(ind_) >= n) return;

  auto t = matrixStack_.transform(ind_);

  if      (stack_->currentIndex() == 1) { // translate
    auto x = translateX_->value();
    auto y = translateY_->value();

    t.setDx(x);
    t.setDy(y);
  }
  else if (stack_->currentIndex() == 2) { // scale
    auto x = scaleX_->value();
    auto y = scaleY_->value();

    t.setXScale(x);
    t.setYScale(y);
  }
  else if (stack_->currentIndex() == 3) { // rotate
    auto a = CMathGen::DegToRad(rotate_->value());

    t.setAngle(a);
  }

  matrixStack_.setTransform(ind_, t);

  //---

  Q_EMIT matrixStackChanged();
}

void
CQMatrixStack2D::
addSlot()
{
  if      (addType_ == 0) { // translate
    auto x = translateX_->value();
    auto y = translateY_->value();

    matrixStack_.addTranslation(x, y);
  }
  else if (addType_ == 1) { // scale
    auto x = scaleX_->value();
    auto y = scaleY_->value();

    matrixStack_.addScale(x, y);
  }
  else if (addType_ == 2) { // rotate
    auto a = CMathGen::DegToRad(rotate_->value());

    matrixStack_.addRotation(a);
  }

  //---

  ind_ = matrixStack_.length() - 1;

  auto *item = list_->item(ind_);

  if (item)
    list_->setCurrentItem(item);

  //---

  Q_EMIT matrixStackChanged();
}

void
CQMatrixStack2D::
itemSelectedSlot(QListWidgetItem *item, QListWidgetItem *)
{
  ind_ = item->data(Qt::UserRole).toInt();

  updateCurrentIndex();
}

void
CQMatrixStack2D::
updateCurrentIndex()
{
  auto n = matrixStack_.length();
  if (ind_ < 0 || size_t(ind_) >= n) return;

  connectSlots(false);

  const auto &t = matrixStack_.transform(ind_);

  if      (t.type() == CMatrixTransformType::TRANSLATE) {
    typeLabel_->setText("Translate");

    stack_->setCurrentIndex(1);

    translateX_->setValue(t.dx());
    translateY_->setValue(t.dy());
  }
  else if (t.type() == CMatrixTransformType::SCALE2) {
    typeLabel_->setText("Scale");

    stack_->setCurrentIndex(2);

    scaleX_->setValue(t.xscale());
    scaleY_->setValue(t.yscale());
  }
  else if (t.type() == CMatrixTransformType::ROTATE) {
    typeLabel_->setText("Rotate");

    stack_->setCurrentIndex(3);

    auto a = CMathGen::RadToDeg(t.angle());

    rotate_->setValue(a);
  }

  connectSlots(true);
}

void
CQMatrixStack2D::
upSlot()
{
  auto n = matrixStack_.length();
  if (ind_ < 0 || size_t(ind_) >= n) return;

  if (matrixStack_.moveUp(ind_))
    --ind_;

  Q_EMIT matrixStackChanged();
}

void
CQMatrixStack2D::
downSlot()
{
  auto n = matrixStack_.length();
  if (ind_ < 0 || size_t(ind_) >= n) return;

  if (matrixStack_.moveDown(ind_))
    ++ind_;

  Q_EMIT matrixStackChanged();
}

void
CQMatrixStack2D::
deleteSlot()
{
  auto n = matrixStack_.length();
  if (ind_ < 0 || size_t(ind_) >= n) return;

  matrixStack_.remove(ind_);

  --ind_;

  Q_EMIT matrixStackChanged();
}

void
CQMatrixStack2D::
clearSlot()
{
  matrixStack_.reset();

  Q_EMIT matrixStackChanged();
}
