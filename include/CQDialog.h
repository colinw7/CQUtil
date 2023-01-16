#ifndef CQDIALOG_H
#define CQDIALOG_H

#include <QDialog>

class QGridLayout;
class QCheckBox;
class QGroupBox;
class QHBoxLayout;

class CQDialogForm : public QWidget {
  Q_OBJECT

 public:
  CQDialogForm(QWidget *parent=nullptr);

  QCheckBox *addCheckBox(const QString &name, const QObject *receiver=nullptr,
                         const char *member=nullptr);

  QGroupBox *addRadioButtons(const QString &name, const std::vector<QString> &names,
                             const QObject *receiver=nullptr, const char *member=nullptr);

  QWidget *addField(const QString &name, QWidget *w, bool stretch=false);

  void addStretch();

 private:
  QGridLayout *grid_ { nullptr };
  mutable int  row_  { 0 };
};

//---

class CQDialog : public QDialog {
  Q_OBJECT

 public:
  enum ButtonType {
    BUTTON_NONE   = 0,
    BUTTON_OK     = (1<<0),
    BUTTON_APPLY  = (1<<1),
    BUTTON_CANCEL = (1<<2),
    BUTTON_ALL    = (BUTTON_OK | BUTTON_APPLY | BUTTON_CANCEL)
  };

 public:
  CQDialog(QWidget *parent=nullptr, uint buttons=BUTTON_ALL);

  virtual ~CQDialog() { }

  void init();

  int exec() override;

  QCheckBox *addCheckBox(const QString &name, const QObject *receiver=nullptr,
                         const char *member=nullptr);

  QGroupBox *addRadioButtons(const QString &name, const std::vector<QString> &names,
                             const QObject *receiver=nullptr, const char *member=nullptr);

  QWidget *addField(const QString &name, QWidget *w, bool stretch=false);

  template<typename WIDGET>
  WIDGET *addFieldT(const QString &name, WIDGET *w, bool stretch=false) {
    return dynamic_cast<WIDGET *>(addField(name, w, stretch));
  }

  QPushButton *addButton(const QString &name);

  bool isAccepted() const { return accepted_; }

  void addStretch();

 signals:
  void accepted();

 protected:
  virtual void createWidgets(QWidget *) { }

  virtual void accept() override { }
  virtual void reject() override { }

 protected slots:
  void acceptSlot();
  void applySlot ();
  void rejectSlot();

 private:
  CQDialogForm *frame_             { nullptr };
  bool          initialized_       { false };
  bool          accepted_          { false };
  QHBoxLayout  *buttonFrameLayout_ { nullptr };
};

#endif
