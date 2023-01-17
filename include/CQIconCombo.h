#ifndef CQIconCombo_H
#define CQIconCombo_H

#include <QComboBox>

class CQIconComboModel;

class CQIconCombo : public QComboBox {
  Q_OBJECT

  Q_PROPERTY(int iconWidth     READ iconWidth     WRITE setIconWidth)
  Q_PROPERTY(int menuTextWidth READ menuTextWidth WRITE setMenuTextWidth)

 public:
  CQIconCombo(QWidget *parent=nullptr);

  virtual ~CQIconCombo();

  int iconWidth() const { return iconWidth_; }
  void setIconWidth(int i) { iconWidth_ = i; }

  int menuTextWidth() const { return textWidth_; }
  void setMenuTextWidth(int w);

  void addItem(const QIcon &icon, const QString &str, const QVariant &var=QVariant());

  QVariant itemData(int ind) const;

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

  void paintEvent(QPaintEvent *e) override;

 private slots:
  void updateTip();

 protected:
  void showPopup() override;

  void calcMenuTextWidth();

 private:
  CQIconComboModel *model_     { nullptr };
  int               iconWidth_ { -1 };
  int               textWidth_ { -1 };
};

#endif
