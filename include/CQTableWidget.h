#ifndef CQTableWidget_H
#define CQTableWidget_H

#include <QTableWidget>
#include <QPointer>

#include <cassert>
#include <vector>
#include <map>

class CQTableWidget;
class CQHeaderView;

class QLineEdit;
class QCheckBox;

class CQTableWidgetItem : public QObject, public QTableWidgetItem {
  Q_OBJECT

 public:
  // run-time type ID that is unique for each item
  // e.g. QTableWidgetItem::UserType + N
  static void setType(uint type) { type_ = type; }
  static uint getType() { return type_; }

  static bool isType(uint type) { return type == type_; }

  //---

  CQTableWidgetItem(CQTableWidget *table);

 ~CQTableWidgetItem() { }

  //---

  //! get/set model index
  const QModelIndex &modelIndex() const { return ind_; }
  void setModelIndex(const QModelIndex &i) { ind_ = i; }

  //! get/set is optional
  bool isOptional() const { return optional_; }
  void setOptional(bool b) { optional_ = b; }

  //---

  // clone new item
  CQTableWidgetItem *clone() const override = 0;

  // create edit widget for item
  virtual QWidget *createEditor(QWidget *parent) const = 0;

  // update edit widget from current item's data
  virtual void setEditorData() = 0;

  // update current item's data from edit widget
  virtual void getEditorData(QString &str) = 0;

  // get hint to item size
  virtual bool sizeHint(const QStyleOptionViewItem &, QSize &) const { return false; }

  // drawing override for static (not editted) item
  virtual bool paint(QPainter *, const QStyleOptionViewItem &) const { return false; }

  // get string value
  virtual QString getString() const = 0;

  virtual bool validate() const { return true; }

  // draw background in correct color for item state when editing
  void paintBackground(QPainter *painter, const QStyleOptionViewItem &option) const;

  // set correct pen color to draw text
  void setTextPen(QPainter *painter, const QStyleOptionViewItem &option) const;

  //---

 Q_SIGNALS:
  void valueChanged();

 protected:
  using CheckBoxP = QPointer<QCheckBox>;

  static uint type_;

  CQTableWidget*    table_    { nullptr };
  QModelIndex       ind_;
  bool              optional_ { false };
  bool              set_      { false };
  mutable CheckBoxP check_;
};

//---

class CQTableWidgetDelegate;

class CQTableWidget : public QTableWidget {
  Q_OBJECT

 public:
  CQTableWidget(QWidget* parent=nullptr);

  virtual ~CQTableWidget();

  CQTableWidgetDelegate *delegate() const { return delegate_; }

  void registerType(uint type, CQTableWidgetItem *item);

  void setColumnLabel(int col, const QString &label);
  void setRowLabel(int row, const QString &label);

  void setColumnLabels(const QStringList &labels);
  void setRowLabels(const QStringList &labels);

  template<typename T>
  T *createItem(int row, int column) {
    uint type = T::getType();

    return dynamic_cast<T *>(createItem(row, column, type));
  }

  template<typename T>
  T *getItem(int row, int column) {
    auto *i = item(row, column);

    auto *i1 = dynamic_cast<CQTableWidgetItem *>(i);
    assert(i1 != NULL);

    assert(i1->getType() == T::getType());

    return dynamic_cast<T *>(i1);
  }

  CQTableWidgetItem *createItem(int row, int column, uint type);

  void setItem(int row, int column, CQTableWidgetItem *item);
  void setItem(int row, int column, QTableWidgetItem *item);
  void setItem(int row, int column, const QString &str);
  void setItem(int row, int column, const QVariant &var);
  void setItem(int row, int column, QWidget *w);

  //---

  void fitAll();

  void fixTableColumnWidths();

  void emitValueChanged(int row, int col);

 protected:
  void showEvent(QShowEvent *) override;

  void fixTableColumnWidths(QTableWidget *table, int max_len, bool init);

 protected Q_SLOTS:
  void itemClickedSlot(const QModelIndex &index);

 Q_SIGNALS:
  void valueChanged(int row, int col);

  void boolClicked(int row, int col, bool value);

 protected:
  using TypeList = std::map<uint, CQTableWidgetItem*>;

  CQHeaderView*          header_ { nullptr };
  TypeList               types_;
  CQTableWidgetDelegate* delegate_ { nullptr };
};

//---

class CQTableWidgetBoolItem : public CQTableWidgetItem {
  Q_OBJECT

 public:
  enum { TYPE = QTableWidgetItem::UserType + 1 };

  CQTableWidgetBoolItem(CQTableWidget *table, bool b) :
   CQTableWidgetItem(table), b_(b) {
  }

  CQTableWidgetBoolItem *clone() const override { return new CQTableWidgetBoolItem(table_, b_); }

  QString getString() const override { return (b_ ? "true" : "false"); }

  QWidget *createEditor(QWidget *parent) const override;

  void setEditorData() override;
  void getEditorData(QString &str) override;

  bool sizeHint(const QStyleOptionViewItem &option, QSize &size) const override {
    size = QSize(128, option.rect.height());

    return true;
  }

  bool paint(QPainter *painter, const QStyleOptionViewItem &option) const override;

  void click() { b_ = ! b_; }

  bool value() const { return b_; }
  void setValue(bool b) { b_ = b; }

 private:
  static uint type_;

  bool b_ { false };
};

//---

class CQTableWidgetStringItem : public CQTableWidgetItem {
  Q_OBJECT

 public:
  enum { TYPE = QTableWidgetItem::UserType + 2 };

  CQTableWidgetStringItem(CQTableWidget *table, const QString &s) :
   CQTableWidgetItem(table), s_(s) {
  }

  CQTableWidgetStringItem *clone() const override {
    return new CQTableWidgetStringItem(table_, s_); }

  const QString &value() const { return s_; }

  QString getString() const override { return s_; }

  QWidget *createEditor(QWidget *parent) const override;

  void setEditorData() override;
  void getEditorData(QString &str) override;

  bool sizeHint(const QStyleOptionViewItem &option, QSize &size) const override {
    size = QSize(128, option.rect.height());

    return true;
  }

  bool paint(QPainter *painter, const QStyleOptionViewItem &option) const override;

 private:
  using LineEditP = QPointer<QLineEdit>;

  static uint type_;

  mutable LineEditP edit_;
  QString           s_;
};

//---

class CQColorChooser;

class CQTableWidgetColorItem : public CQTableWidgetItem {
  Q_OBJECT

 public:
  enum { TYPE = QTableWidgetItem::UserType + 3 };

  CQTableWidgetColorItem(CQTableWidget *table) :
   CQTableWidgetItem(table) {
  }

  CQTableWidgetColorItem(CQTableWidget *table, const QColor &c) :
   CQTableWidgetItem(table) {
    setValue(c);
  }

  CQTableWidgetColorItem *clone() const override {
    auto *item = new CQTableWidgetColorItem(table_);
    if (set_) item->setValue(c_);
    return item;
  }

  //! get/set value
  bool hasValue() const { return set_; }
  const QColor &value() const { assert(set_); return c_; }
  void setValue(const QColor &c) { set_ = true; c_ = c; }

  QString getString() const override { return (set_ ? value().name() : ""); }

  QWidget *createEditor(QWidget *parent) const override;

  void setEditorData() override;
  void getEditorData(QString &str) override;

  bool sizeHint(const QStyleOptionViewItem &option, QSize &size) const override {
    size = QSize(128, option.rect.height());

    return true;
  }

  bool paint(QPainter *painter, const QStyleOptionViewItem &option) const override;

 private Q_SLOTS:
  void checkSlot(int);

 private:
  using ColorChooserP = QPointer<CQColorChooser>;

  static uint type_;

  mutable ColorChooserP edit_;
  QColor                c_;
};

//---

class CQFontChooser;

class CQTableWidgetFontItem : public CQTableWidgetItem {
  Q_OBJECT

 public:
  enum { TYPE = QTableWidgetItem::UserType + 4 };

  CQTableWidgetFontItem(CQTableWidget *table) :
   CQTableWidgetItem(table) {
  }

  CQTableWidgetFontItem(CQTableWidget *table, const QFont &font) :
   CQTableWidgetItem(table) {
    setValue(font);
  }

  CQTableWidgetFontItem *clone() const override {
    auto *item = new CQTableWidgetFontItem(table_);
    if (set_) item->setValue(font_);
    return item;
  }

  //! get/set value
  bool hasValue() const { return set_; }
  const QFont &value() const { assert(set_); return font_; }
  void setValue(const QFont &font) { set_ = true; font_ = font; }

  QString getString() const override { return font_.toString(); }

  QWidget *createEditor(QWidget *parent) const override;

  void setEditorData() override;
  void getEditorData(QString &str) override;

  bool sizeHint(const QStyleOptionViewItem &option, QSize &size) const override {
    size = QSize(128, option.rect.height());

    return true;
  }

  bool paint(QPainter *painter, const QStyleOptionViewItem &option) const override;

 private:
  using FontChooserP = QPointer<CQFontChooser>;

  static uint type_;

  mutable FontChooserP edit_;
  QFont                font_;
};

//---

class CQAlignEdit;

class CQTableWidgetAlignItem : public CQTableWidgetItem {
  Q_OBJECT

 public:
  enum { TYPE = QTableWidgetItem::UserType + 5 };

  CQTableWidgetAlignItem(CQTableWidget *table) :
   CQTableWidgetItem(table) {
  }

  CQTableWidgetAlignItem(CQTableWidget *table, const Qt::Alignment &align) :
   CQTableWidgetItem(table) {
    setValue(align);
  }

  CQTableWidgetAlignItem *clone() const override {
    auto *item = new CQTableWidgetAlignItem(table_);
    if (set_) item->setValue(align_);
    return item;
  }

  //! get/set value
  bool hasValue() const { return set_; }
  const Qt::Alignment &value() const { assert(set_); return align_; }
  void setValue(const Qt::Alignment &align) { set_ = true; align_ = align; }

  QString getString() const override;

  QWidget *createEditor(QWidget *parent) const override;

  void setEditorData() override;
  void getEditorData(QString &str) override;

  bool sizeHint(const QStyleOptionViewItem &option, QSize &size) const override {
    size = QSize(128, option.rect.height());

    return true;
  }

  bool paint(QPainter *painter, const QStyleOptionViewItem &option) const override;

 private:
  using AlignChooserP = QPointer<CQAlignEdit>;

  static uint type_;

  mutable AlignChooserP edit_;
  Qt::Alignment         align_ { Qt::AlignCenter };
};

#endif
