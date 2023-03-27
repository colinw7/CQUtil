#ifndef CQFlowLayout_H
#define CQFlowLayout_H

#include <QLayout>
#include <QStyle>

class CQFlowLayout : public QLayout {
 public:
  explicit CQFlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
  explicit CQFlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);

 ~CQFlowLayout();

  void addItem(QLayoutItem *item) override;

  int horizontalSpacing() const;
  int verticalSpacing  () const;

  Qt::Orientations expandingDirections() const override;

  bool hasHeightForWidth() const override;
  int heightForWidth(int) const override;

  int count() const override;

  QLayoutItem *itemAt(int index) const override;

  QSize minimumSize() const override;

  void setGeometry(const QRect &rect) override;

  QSize sizeHint() const override;

  QLayoutItem *takeAt(int index) override;

 private:
  int doLayout(const QRect &rect, bool testOnly) const;
  int smartSpacing(QStyle::PixelMetric pm) const;

 private:
  QList<QLayoutItem *> itemList_;

  int hSpace_ { -1 };
  int vSpace_ { -1 };
};

#endif
