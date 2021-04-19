#ifndef CQAppOptions_H
#define CQAppOptions_H

#include <CQDialog.h>

class CQAppOptions : public CQDialog {
  Q_OBJECT

 public:
  static void show();

 private:
  CQAppOptions(QWidget *parent=nullptr);

 private slots:
  void darkSlot(bool);

  void vfontSlot(const QFont &);
  void ffontSlot(const QFont &);

  void iconSizeChanged(int i);
  void largeIconSizeChanged(int i);
  void smallIconSizeChanged(int i);
};

#endif
