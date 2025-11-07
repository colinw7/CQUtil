#ifndef CQMatrixStack2D_H
#define CQMatrixStack2D_H

#include <CMatrixStack2D.h>

#include <QFrame>

class CQRealSpin;
class CQIconButton;

class QStackedWidget;
class QListWidget;
class QListWidgetItem;
class QComboBox;
class QPushButton;
class QCheckBox;
class QLabel;

class CQMatrixStack2D : public QFrame {
  Q_OBJECT

 public:
  CQMatrixStack2D(QWidget *parent=nullptr);

  const CMatrixStack2D &matrixStack() const { return matrixStack_; }
  void setMatrixStack(const CMatrixStack2D &matrixStack);

 Q_SIGNALS:
  void matrixStackChanged();

 private:
  void updateWidgets();

  void connectSlots(bool b);

  void updateCurrentIndex();

 private Q_SLOTS:
  void modeSlot(int);
  void typeSlot(int);

  void autoUpdateSlot(int);
  void updateSlot();
  void addSlot();

  void updateEditSlot(double);

  void itemSelectedSlot(QListWidgetItem *item, QListWidgetItem *);

  void upSlot();
  void downSlot();
  void deleteSlot();
  void clearSlot();

 private:
  CMatrixStack2D matrixStack_;

  QComboBox* typeCombo_ { nullptr };
  QLabel*    typeLabel_ { nullptr };

  QStackedWidget* stack_ { nullptr };

  CQRealSpin* translateX_ { nullptr };
  CQRealSpin* translateY_ { nullptr };
  CQRealSpin* scaleX_     { nullptr };
  CQRealSpin* scaleY_     { nullptr };
  CQRealSpin* rotate_     { nullptr };

  QCheckBox   *autoUpdateCheck_ { nullptr };
  QPushButton *updateButton_    { nullptr };
  QPushButton *addButton_       { nullptr };

  QListWidget* list_ { nullptr };

  CQIconButton *upButton_     { nullptr };
  CQIconButton *downButton_   { nullptr };
  CQIconButton *deleteButton_ { nullptr };
  CQIconButton *clearButton_  { nullptr };

  bool autoUpdate_ { true };

  int ind_ { -1 };

  int addType_ { 0 };
};

#endif
