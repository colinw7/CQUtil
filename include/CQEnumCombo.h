#ifndef CQEnumCombo_H
#define CQEnumCombo_H

#include <QComboBox>

#if 0
#define Q_ENUM_CLASS(ENUM, NAME) \
class CQEnumComboEnum##NAME : public QObject { \
  Q_OBJECT \
\
  Q_ENUMS(ENUM) \
\
  Q_PROPERTY(ENUM value READ value WRITE setValue) \
\
 public: \
  CQEnumComboEnum##NAME() { } \
\
  ENUM value() const { return t_; } \
  void setValue(const ENUM &t) { t_ = t; }\
\
 private: \
  ENUM t_; \
};
#endif

class CQEnumCombo : public QComboBox {
  Q_OBJECT

  Q_PROPERTY(QString propName READ propName WRITE setPropName)

 public:
  CQEnumCombo(QWidget *parent, const QObject *obj=nullptr, const QString &name="");
  CQEnumCombo(const QObject *obj=nullptr, const QString &name="");

  const QObject *obj() const { return obj_; }
  void setObj(const QObject *o);

  const QString &propName() const { return propName_; }
  void setPropName(const QString &s);

  void setItemIcon(int i, const QIcon &icon);
  void setItemText(int i, const QString &text);

  void setItemData(int i, const QString &text, const QIcon &icon);

 private:
  void init();

  void updateItems();

 private slots:
  void updatePropertySlot();

 private:
  const QObject *obj_ { nullptr };
  QString        propName_;
};

#endif
