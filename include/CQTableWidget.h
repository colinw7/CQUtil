#ifndef CQTableWidget_H
#define CQTableWidget_H

#include <QTableWidget>
#include <QPointer>

#include <cassert>
#include <vector>
#include <map>

class CQTableWidget;
class CQHeaderView;

class CQTableWidgetItem : public QTableWidgetItem {
 public:
  // run-time type ID that is unique for each item
  // e.g. QTableWidgetItem::UserType + N
  static void setType(uint type) { type_ = type; }
  static uint getType() { return type_; }

  static bool isType(uint type) { return type == type_; }

  //------

  CQTableWidgetItem(CQTableWidget *table);

 ~CQTableWidgetItem() { }

  // clone new item
  virtual CQTableWidgetItem *clone() const = 0;

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

 protected:
  static uint type_;

  CQTableWidget *table_ { nullptr };
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
  void setItem(int row, int column, QWidget *w);

  //---

  void fitAll();

  void fixTableColumnWidths();

  void emitValueChanged(int row, int col);

 protected:
  void showEvent(QShowEvent *) override;

  void fixTableColumnWidths(QTableWidget *table, int max_len, bool init);

 protected slots:
  void itemClickedSlot(const QModelIndex &index);

 signals:
  void valueChanged(int row, int col);

  void boolClicked(int row, int col, bool value);

 protected:
  using TypeList = std::map<uint,CQTableWidgetItem*>;

  CQHeaderView*          header_ { nullptr };
  TypeList               types_;
  CQTableWidgetDelegate* delegate_ { nullptr };
};

//---

class CQTableWidgetBoolItem : public CQTableWidgetItem {
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

class CQColorChooser;

class CQTableWidgetColorItem : public CQTableWidgetItem {
 public:
  enum { TYPE = QTableWidgetItem::UserType + 2 };

  CQTableWidgetColorItem(CQTableWidget *table, const QColor &c) :
   CQTableWidgetItem(table), c_(c) {
  }

  CQTableWidgetColorItem *clone() const override { return new CQTableWidgetColorItem(table_, c_); }

  QString getString() const override { return c_.name(); }

  QWidget *createEditor(QWidget *parent) const override;

  void setEditorData() override;
  void getEditorData(QString &str) override;

  bool sizeHint(const QStyleOptionViewItem &option, QSize &size) const override {
    size = QSize(128, option.rect.height());

    return true;
  }

  bool paint(QPainter *painter, const QStyleOptionViewItem &option) const override;

 private:
  using ColorChooserP = QPointer<CQColorChooser>;

  static uint type_;

  mutable ColorChooserP edit_;
  QColor                c_;
};

#endif
