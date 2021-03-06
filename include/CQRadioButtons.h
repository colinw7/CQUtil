#ifndef CQRadioButtons_H
#define CQRadioButtons_H

#include <vector>
#include <QWidget>

class QRadioButton;
class QLayout;

class CQRadioButtons : public QWidget {
  Q_OBJECT

 public:
  CQRadioButtons(QWidget *parent=0);

  virtual ~CQRadioButtons();

  void addButton(const QString &name);

  int getCurrentValue() const;

  QString getCurrentName () const;
  void setCurrentName(const QString &name);

 protected:
  void addButtons(const std::vector<QString> &names);

  virtual QLayout *createLayout(QWidget *parent);

  virtual void addWidget(QWidget *widget);

 protected slots:
  void valueChangedI(bool value);

 signals:
  void valueChanged(int id);
  void valueChanged(const QString &d);

 protected:
  typedef std::vector<QRadioButton *> Buttons;

  bool    hasLayout_ { false };
  Buttons buttons_;
};

//------

class CQHRadioButtons : public CQRadioButtons {
 public:
  CQHRadioButtons(QWidget *parent, const char *name, ...);
  CQHRadioButtons(QWidget *parent=0);

 protected:
  QLayout *createLayout(QWidget *parent);

  void addWidget(QWidget *widget);
};

//------

class CQVRadioButtons : public CQRadioButtons {
 public:
  CQVRadioButtons(QWidget *parent, const char *name, ...);
  CQVRadioButtons(QWidget *parent=0);

 protected:
  QLayout *createLayout(QWidget *parent);

  void addWidget(QWidget *widget);
};

//------

class CQToolbarRadioButtons : public CQRadioButtons {
 public:
  CQToolbarRadioButtons(QWidget *parent, const char *title, const char *name, ...);
  CQToolbarRadioButtons(QWidget *parent, const char *title);

 protected:
  QLayout *createLayout(QWidget *parent);

  void addWidget(QWidget *widget);

 protected:
  QString title_;
};

#endif
