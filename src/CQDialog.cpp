#include <CQDialog.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>

CQDialog::
CQDialog(QWidget *parent, uint buttons) :
 QDialog(parent)
{
  setObjectName("dialog");

  auto *layout = new QVBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  //---

  frame_ = new CQDialogForm;

  layout->addWidget(frame_);

  //---

  auto *buttonLayout = new QHBoxLayout;

  auto *buttonFrame = new QFrame;
  buttonFrame->setObjectName("buttonFrame");

  buttonFrameLayout_ = new QHBoxLayout(buttonFrame);
  buttonFrameLayout_->setMargin(0); buttonFrameLayout_->setSpacing(2);

  buttonLayout->addWidget(buttonFrame);
  buttonLayout->addStretch();

  if (buttons & BUTTON_OK) {
    auto *ok = new QPushButton("OK");
    ok->setObjectName("OK");

    connect(ok, SIGNAL(clicked()), this, SLOT(acceptSlot()));

    buttonLayout->addWidget(ok);
  }

  if (buttons & BUTTON_APPLY) {
    auto *apply = new QPushButton("Apply");
    apply->setObjectName("Apply");

    connect(apply, SIGNAL(clicked()), this, SLOT(applySlot()));

    buttonLayout->addWidget(apply);
  }

  if (buttons & BUTTON_CANCEL) {
    auto *cancel = new QPushButton("Cancel");
    cancel->setObjectName("Cancel");

    connect(cancel, SIGNAL(clicked()), this, SLOT(rejectSlot()));

    buttonLayout->addWidget(cancel);
  }

  layout->addLayout(buttonLayout);
}

void
CQDialog::
init()
{
  createWidgets(frame_);

  initialized_ = true;
}

int
CQDialog::
exec()
{
  if (! initialized_)
    init();

  return QDialog::exec();
}

QCheckBox *
CQDialog::
addCheckBox(const QString &name, const QObject *receiver, const char *member)
{
  return frame_->addCheckBox(name, receiver, member);
}

QGroupBox *
CQDialog::
addRadioButtons(const QString &name, const std::vector<QString> &names,
                const QObject *receiver, const char *member)
{
  return frame_->addRadioButtons(name, names, receiver, member);
}

QWidget *
CQDialog::
addField(const QString &name, QWidget *w, bool stretch)
{
  return frame_->addField(name, w, stretch);
}

QPushButton *
CQDialog::
addButton(const QString &name)
{
  auto *pb = new QPushButton(name);
  pb->setObjectName(name);

  buttonFrameLayout_->addWidget(pb);

  return pb;
}

void
CQDialog::
addStretch()
{
#if 1
  frame_->addStretch();
#else
  auto *l = qobject_cast<QVBoxLayout *>(layout());

  l->addStretch(1);
#endif
}

void
CQDialog::
acceptSlot()
{
  accepted_ = true;

  accept();

  emit accepted();

  QDialog::accept();
}

void
CQDialog::
applySlot()
{
  accepted_ = true;

  accept();

  emit accepted();

  accepted_ = false;
}

void
CQDialog::
rejectSlot()
{
  accepted_ = false;

  reject();

  QDialog::reject();
}

//------

CQDialogForm::
CQDialogForm(QWidget *parent) :
 QWidget(parent)
{
  setObjectName("form");
}

QCheckBox *
CQDialogForm::
addCheckBox(const QString &name, const QObject *receiver, const char *member)
{
  auto *check = new QCheckBox;
  check->setObjectName(name);

  addField(name, check);

  check->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  if (receiver)
    connect(check, SIGNAL(clicked(bool)), receiver, member);

  return check;
}

QGroupBox *
CQDialogForm::
addRadioButtons(const QString &name, const std::vector<QString> &names,
                const QObject *receiver, const char *member)
{
  auto *box = new QGroupBox;
  box->setObjectName(name);

  auto *layout = new QHBoxLayout(box);
  layout->setMargin(0); layout->setSpacing(2);

  int i = 0;

  for (const auto &n : names) {
    auto *button = new QRadioButton(n);
    button->setObjectName(n);

    layout->addWidget(button);

    button->setChecked(! (i++));
  }

  layout->addStretch();

  addField(name, box);

  if (receiver)
    connect(box, SIGNAL(clicked(bool)), receiver, member);

  return box;
}

QWidget *
CQDialogForm::
addField(const QString &name, QWidget *w, bool stretch)
{
  if (! grid_) {
    grid_ = new QGridLayout(this);

    grid_->setColumnStretch(1, 1);
  }

  if (stretch)
    grid_->setColumnStretch(2, 1);

  if (name.length()) {
    grid_->addWidget(new QLabel(name), row_, 0);
    grid_->addWidget(w               , row_, 1);
  }
  else
    grid_->addWidget(w, row_, 0, 1, 2);

  ++row_;

  return w;
}

void
CQDialogForm::
addStretch()
{
  if (grid_)
    grid_->setRowStretch(row_, 1);
}
