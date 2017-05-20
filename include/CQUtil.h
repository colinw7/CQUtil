#ifndef CQUTIL_H
#define CQUTIL_H

#include <Qt>
#include <QColor>
#include <QEvent>
#include <QVariant>
#include <QTableWidget>
#include <QPen>

#include <CEvent.h>
#include <CRGBA.h>
#include <CLineDash.h>
#include <CAngle.h>
#include <CFontStyle.h>
#include <CFont.h>
#include <CImageLib.h>
#include <CPoint2D.h>
#include <CBBox2D.h>
#include <CMatrix2D.h>
#include <CBrush.h>
#include <CPen.h>

Q_DECLARE_METATYPE(CLineDash)
Q_DECLARE_METATYPE(CAngle)

class QMouseEvent;
class QKeyEvent;
class QMetaObject;
class QAbstractButton;
class QDockWidget;
class QLabel;

class CLinearGradient;
class CRadialGradient;

namespace CQUtil {
  class PropInfo {
   public:
    PropInfo() {
     type_         = QVariant::Invalid;
     is_writable_  = false;
     is_enum_type_ = false;
    }

    void init(const QMetaProperty &mp);

    QString        name    () const { return name_; }
    QVariant::Type type    () const { return type_; }
    QString        typeName() const { return typeName_; }

    bool isWritable() const { return is_writable_ ; }
    bool isEnumType() const { return is_enum_type_; }

    const QStringList &enumNames() { return enumNames_; }

   private:
    QString        name_;
    QVariant::Type type_;
    QString        typeName_;
    bool           is_writable_;
    bool           is_enum_type_;
    QStringList    enumNames_;
  };

  void initProperties();

  CMouseEvent *convertEvent(QMouseEvent *event);
  CKeyEvent   *convertEvent(QKeyEvent *event);

  CMouseButton   convertButton(Qt::MouseButton button);
  CKeyType       convertKey(int key, Qt::KeyboardModifiers modifiers);
  CEventModifier convertModifier(Qt::KeyboardModifiers modifiers);

  QColor rgbToColor(const CRGB &rgb);
  QColor rgbaToColor(const CRGBA &rgba);

  inline QColor toQColor(const CRGB  &rgb ) { return rgbToColor (rgb ); }
  inline QColor toQColor(const CRGBA &rgba) { return rgbaToColor(rgba); }

  uint rgbaToInt(const CRGBA &rgba);

  CRGB  colorToRGB(const QColor &color);
  CRGBA colorToRGBA(const QColor &color);

  QBrush toQBrush(const CBrush &brush);
  QPen   toQPen  (const CPen   &pen);

  Qt::PenCapStyle toPenCapStyle(const CLineCapType &lineCap);
  Qt::PenJoinStyle toPenJoinStyle(const CLineJoinType &lineJoin);

  inline CRGBA fromQColor(const QColor &color) { return colorToRGBA(color); }

  void decodeFont(const QFont &font, QString &family, CFontStyle &style, int &pixelSize);

  QString fullName(const QObject* object);

  QObject *nameToObject(const QString &name);
  QObject *nameToObject(QObject *parent, const QString &name);

  QObject *childObject(QObject *parent, const QString &name);

  QWidget *getToplevelWidget(QWidget *widget);

  QStringList getPropertyList(const QObject *object, bool inherited);
  QStringList getPropertyList(const QObject *object, bool inherited,
                              const QMetaObject* metaObject);

  bool getProperty(const QObject *object, const QString &name, QVariant &value);

  bool setProperty(const QObject *object, const QString &name, const QString &value);
  bool setProperty(const QObject *object, const QString &name, const QVariant &value);

  bool getPropInfo(const QObject *object, const QString &name, PropInfo *info);

  const QMetaObject *baseClass(QMetaObject *metaObject, const char *name);

  QStringList signalNames(const QObject* object, bool inherited);
  QStringList slotNames(const QObject* object, bool inherited);

  QString eventTypeToName(QEvent::Type type);
  QString eventToString(QEvent *event);

  QColor getForeground(QWidget *widget);
  void   setForeground(QWidget *widget, const QColor &color);

  QColor getBackground(QWidget *widget);
  void   setBackground(QWidget *widget, const QColor &color);

  QPointF  toQPoint  (const CPoint2D &point);
  CPoint2D fromQPoint(const QPointF &point);

  QPoint    toQPoint  (const CIPoint2D &point);
  CIPoint2D fromQPoint(const QPoint &point);

  QSizeF  toQSize  (const CSize2D &size);
  CSize2D fromQSize(const QSizeF &size);

  QSize    toQSize  (const CISize2D &size);
  CISize2D fromQSize(const QSize &size);

  QRectF  toQRect  (const CBBox2D &rect);
  CBBox2D fromQRect(const QRectF &rect);

  QRect    toQRect  (const CIBBox2D &rect);
  CIBBox2D fromQRect(const QRect &rect);

  QMatrix   toQMatrix  (const CMatrix2D &m);
  CMatrix2D fromQMatrix(const QMatrix &m);

  QTransform toQTransform  (const CMatrix2D &m);
  CMatrix2D  fromQTransform(const QTransform &t);

  QFont    toQFont  (CFontPtr font);
  CFontPtr fromQFont(QFont font);

  QString variantToString(const QVariant &var);
  bool variantToString(const QVariant &var, QString &str);

  bool stringToVariant(const QString &str, QVariant::Type type,
                       const char *typeName, QVariant &var);

  bool paletteFromString(QPalette &palette, const QString &paletteDef);
  QString paletteToString(const QPalette &palette);

  bool activateSlot(QObject *receiver, const char *slotName, const char *valuesStr);
  bool activateSignal(QObject* sender, const char *signalName, const char *valuesStr);

  /*! template function to get table cell widget of type */
  template<typename T>
  T tableCellWidget(QTableWidget *table, int row, int col) {
    return qobject_cast<T>(table->cellWidget(row, col));
  }

  QIcon imageToIcon(CImagePtr image);

  QImage toQImage(CImagePtr image);

  QLinearGradient toQGradient(const CLinearGradient *lgradient,
                              QGradient::CoordinateMode mode=QGradient::ObjectBoundingMode);
  QRadialGradient toQGradient(const CRadialGradient *rgradient,
                              QGradient::CoordinateMode mode=QGradient::ObjectBoundingMode);

  void getScreenSize(uint *w, uint *h);

  void setSelectText(const std::string &text);

  void setDockVisible(QDockWidget *dock, bool visible);

  void penSetLineDash(QPen &pen, const CLineDash &dash);

  Qt::Alignment toQAlign(CHAlignType halign);
  Qt::Alignment toQAlign(CVAlignType valign);

  CHAlignType toHAlign(Qt::Alignment align);
  CVAlignType toVAlign(Qt::Alignment align);

  //-----

  QPixmap getPixmap(const QString &name);

  QIcon getIcon(const QString &name);

  //-----

  uint nameWidgetTree(QWidget *widget);

  void nameWidget(QWidget *widget);

  void nameWidgetButton(QAbstractButton *button);
  void nameWidgetLabel(QLabel *label);
  void nameWidgetGen(QWidget *widget);

  //-----

  void recolorImage(QImage &image, const QColor &fg, const QColor &bg);

  //-----

  void drawHtmlText(QWidget *w, QPainter *painter, const QString &text,
                    const QPalette &palette, const QRect &rect, bool active=false);

  QString colorToHtml(const QColor &c);
};

class CQWidgetPtr : public QObject {
  Q_OBJECT

 public:
  CQWidgetPtr(QWidget *w=nullptr) :
   w_(w) {
    watch();
  }

  bool isValid() const {
    return (w_ != nullptr);
  }

  void set(QWidget *w) {
    w_ = w;

    watch();
  }

  QWidget *get() const {
    return w_;
  }

  template<typename T>
  T *get() const {
    return dynamic_cast<T *>(w_);
  }

 private:
  void watch() {
    if (w_)
      connect(w_, SIGNAL(destroyed(QObject *)), this, SLOT(resetSlot()));
  }

 private slots:
  void resetSlot() {
    w_ = nullptr;
  }

 private:
  QWidget *w_;
};

#endif
