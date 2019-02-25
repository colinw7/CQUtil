#ifndef CQGroupBox_H
#define CQGroupBox_H

#include <QWidget>

class CQGroupBoxArea;

/*!
 * \brief Custom groupbox supporting custom title and line drawing and collapse
 */
class CQGroupBox : public QWidget {
  Q_OBJECT

  // title
  Q_PROPERTY(QString       title           READ title         WRITE setTitle         )
  Q_PROPERTY(bool          titleBold      READ isTitleBold    WRITE setTitleBold     )
  Q_PROPERTY(double        titleScale     READ titleScale     WRITE setTitleScale    )
  Q_PROPERTY(Qt::Alignment titleAlignment READ titleAlignment WRITE setTitleAlignment)

  // line top/bottom
  Q_PROPERTY(bool          lineTop
             READ hasLineTop          WRITE setHasLineTop         )
  Q_PROPERTY(bool          lineBottom
             READ hasLineBottom       WRITE setHasLineBottom      )
  Q_PROPERTY(Qt::Alignment lineTopAlignment
             READ lineTopAlignment    WRITE setLineTopAlignment   )
  Q_PROPERTY(Qt::Alignment lineBottomAlignment
             READ lineBottomAlignment WRITE setLineBottomAlignment)
  // margins
  Q_PROPERTY(int marginLeft   READ marginLeft   WRITE setMarginLeft  )
  Q_PROPERTY(int marginRight  READ marginRight  WRITE setMarginRight )
  Q_PROPERTY(int marginBottom READ marginBottom WRITE setMarginBottom)
  Q_PROPERTY(int marginTop    READ marginTop    WRITE setMarginTop   )

  // check
  Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable)
  Q_PROPERTY(bool checked   READ isChecked   WRITE setChecked  )

  // collapse
  Q_PROPERTY(bool collapsible READ isCollapsible WRITE setCollapsible)
  Q_PROPERTY(bool collapsed   READ isCollapsed   WRITE setCollapsed  )

 public:
  CQGroupBox(QWidget *parent=nullptr);
  CQGroupBox(const QString &title, QWidget *parent=nullptr);

  ~CQGroupBox();

  const QString &title() const { return title_; }
  void setTitle(const QString &title);

  bool isTitleBold() const { return titleBold_; }
  void setTitleBold(bool bold);

  double titleScale() const { return titleScale_; }
  void setTitleScale(double scale);

  Qt::Alignment titleAlignment() const { return titleAlignment_; }
  void setTitleAlignment(Qt::Alignment alignment);

  bool hasLineTop() const { return lineTop_; }
  void setHasLineTop(bool line);

  bool hasLineBottom() const { return lineBottom_; }
  void setHasLineBottom(bool line);

  Qt::Alignment lineTopAlignment() const { return lineTopAlignment_; }
  void setLineTopAlignment(Qt::Alignment alignment);

  Qt::Alignment lineBottomAlignment() const { return lineBottomAlignment_; }
  void setLineBottomAlignment(Qt::Alignment alignment);

  int marginLeft() const { return marginLeft_; }
  void setMarginLeft(int margin);

  int marginRight() const { return marginRight_; }
  void setMarginRight(int margin);

  int marginBottom() const { return marginBottom_; }
  void setMarginBottom(int margin);

  int marginTop() const { return marginTop_; }
  void setMarginTop(int margin);

  bool isCheckable() const { return checkable_; }
  void setCheckable(bool checkable);

  bool isChecked() const { return checked_; }

  bool isCollapsible() const { return collapsible_; }
  void setCollapsible(bool collapsible);

  bool isCollapsed() const { return collapsed_; }

 private:
  void init();

  void setTitleFont();

  void changeEvent(QEvent *e);

  void childEvent(QChildEvent *e);

  bool event(QEvent *e);

  void focusInEvent(QFocusEvent *e);

  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

  void paintEvent(QPaintEvent *e);

  void drawArcShape(QPainter *painter, double xc, double yc, double r,
                    double startAngle, int sides) const;

  void resizeEvent(QResizeEvent *e);

  QSize sizeHint() const;
  QSize minimumSizeHint() const;

  int spaceTop() const;
  int spaceBottom() const;

  void calculateFrame();

  void updateEnabled();
  void updateCollapsed();

 signals:
  void clicked(bool checked);
  void toggled(bool checked);

  void collapse(bool collapsed);

 public slots:
  void setChecked(bool checked);
  void setCollapsed(bool collapsed);

 private:
//CQGroupBoxArea *area_           { nullptr };

  QString         title_;
  bool            titleBold_      { true };
  double          titleScale_     { 1.0 };
  Qt::Alignment   titleAlignment_ { Qt::AlignLeft | Qt::AlignBottom };

  bool            lineTop_             { false };
  bool            lineBottom_          { true };
  Qt::Alignment   lineTopAlignment_    { Qt::AlignBottom };
  Qt::Alignment   lineBottomAlignment_ { Qt::AlignVCenter };

  int             marginLeft_   { 6 };
  int             marginRight_  { 2 };
  int             marginBottom_ { -1 };
  int             marginTop_    { -1 };

  bool            checkable_  { false };
  bool            checked_    { false };
  bool            checkPress_ { false };

  bool            collapsible_   { false };
  bool            collapsed_     { false };
  bool            collapsePress_ { false };

  QRect           checkRect_;
  QRect           collapseRect_;
  QRect           titleRect_;
  QFont           titleFont_;
  QColor          lineColor_;

  int             dx_ { 2 };
  int             dy_ { 2 };
};

//----

#if 0
class CQGroupBoxArea : public QWidget {
 public:
  CQGroupBoxArea(QWidget *parent=0);

  void paintEvent(QPaintEvent *e);
};
#endif

#endif
