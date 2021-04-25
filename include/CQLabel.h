#ifndef CQLABEL_H
#define CQLABEL_H

#include <QLabel>
#include <QVariant>

/*!
 * \brief label widget supporting horizontal or vertical text
 */
class CQLabel : public QLabel {
 public:
  Q_PROPERTY(QVariant        value       READ value       WRITE setValue      )
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

 public:
  CQLabel(const QVariant &value=QVariant(), QWidget *parent=nullptr);
  CQLabel(const QVariant &value, Qt::Orientation orient, QWidget *parent=nullptr);

  virtual ~CQLabel() { }

  const QVariant &value() const { return value_; }
  void setValue(const QVariant &v);

  Qt::Orientation orientation() const { return orientation_; }
  void setOrientation(Qt::Orientation orient);

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 protected:
  void paintEvent(QPaintEvent *e) override;

 private:
  void updateState();

 private:
  QVariant        value_;
  Qt::Orientation orientation_ { Qt::Horizontal };
};

#endif
