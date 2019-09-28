#ifndef CQArrow_H
#define CQArrow_H

#include <QObject>
#include <QPointF>
#include <QPolygonF>
#include <QColor>

class QPainter;

#define ACCESSOR(T,V,G,S) \
const T &G() const { return V; } \
void S(const T &t) { V = t; }

#define ACCESSOR_IMPL_GET_DCL_SET(T,V,G,S) \
const T &G() const { return V; } \
void S(const T &t);

class CQArrowDevice;

class CQArrow : public QObject {
 public:
  Q_OBJECT

  Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth)

  Q_PROPERTY(bool     frontVisible   READ isFHead         WRITE setFHead)
  Q_PROPERTY(HeadType frontType      READ frontType       WRITE setFrontType)
  Q_PROPERTY(double   frontAngle     READ frontAngle      WRITE setFrontAngle)
  Q_PROPERTY(double   frontBackAngle READ frontBackAngle  WRITE setFrontBackAngle)
  Q_PROPERTY(double   frontLength    READ frontLength     WRITE setFrontLength)
  Q_PROPERTY(bool     frontLineEnds  READ isFrontLineEnds WRITE setFrontLineEnds)

  Q_PROPERTY(bool     tailVisible    READ isTHead        WRITE setTHead)
  Q_PROPERTY(HeadType tailType       READ tailType       WRITE setTailType)
  Q_PROPERTY(double   tailAngle      READ tailAngle      WRITE setTailAngle)
  Q_PROPERTY(double   tailBackAngle  READ tailBackAngle  WRITE setTailBackAngle)
  Q_PROPERTY(double   tailLength     READ tailLength     WRITE setTailLength)
  Q_PROPERTY(bool     tailLineEnds   READ isTailLineEnds WRITE setTailLineEnds)

  Q_PROPERTY(bool   filled      READ isFilled    WRITE setFilled)
  Q_PROPERTY(bool   stroked     READ isStroked   WRITE setStroked)
  Q_PROPERTY(double strokeWidth READ strokeWidth WRITE setStrokeWidth)

  Q_PROPERTY(bool debugPoints READ isDebugPoints WRITE setDebugPoints)
  Q_PROPERTY(bool debugText   READ isDebugText   WRITE setDebugText  )

  Q_ENUMS(HeadType)

 public:
  enum class HeadType {
    NONE,
    TRIANGLE,
    STEALTH,
    DIAMOND,
    LINE,
  };

 public:
  CQArrow(const QPointF &from=QPointF(0,0), const QPointF &to=QPointF(1,1));

  ACCESSOR(QPointF, from_     , from      , setFrom);
  ACCESSOR(QPointF, to_       , to        , setTo);
  ACCESSOR(bool   , relative_ , isRelative, setRelative);
  ACCESSOR(double , lineWidth_, lineWidth , setLineWidth);

  ACCESSOR_IMPL_GET_DCL_SET(HeadType, frontData_.type, frontType, setFrontType);

  ACCESSOR(bool    , frontData_.visible  , isFHead        , setFHead);
  ACCESSOR(double  , frontData_.angle    , frontAngle     , setFrontAngle);
  ACCESSOR(double  , frontData_.backAngle, frontBackAngle , setFrontBackAngle);
  ACCESSOR(double  , frontData_.length   , frontLength    , setFrontLength);
  ACCESSOR(bool    , frontData_.lineEnds , isFrontLineEnds, setFrontLineEnds);

  ACCESSOR_IMPL_GET_DCL_SET(HeadType, tailData_.type, tailType, setTailType);

  ACCESSOR(bool    , tailData_.visible  , isTHead       , setTHead);
  ACCESSOR(double  , tailData_.angle    , tailAngle     , setTailAngle);
  ACCESSOR(double  , tailData_.backAngle, tailBackAngle , setTailBackAngle);
  ACCESSOR(double  , tailData_.length   , tailLength    , setTailLength);
  ACCESSOR(bool    , tailData_.lineEnds , isTailLineEnds, setTailLineEnds);

  ACCESSOR(bool   , filled_     , isFilled     , setFilled);
  ACCESSOR(bool   , stroked_    , isStroked    , setStroked);
  ACCESSOR(double , strokeWidth_, strokeWidth  , setStrokeWidth);
  ACCESSOR(bool   , debugPoints_, isDebugPoints, setDebugPoints);
  ACCESSOR(bool   , debugText_  , isDebugText  , setDebugText);

  ACCESSOR(bool, inside_, isInside, setInside);

  bool isDebugLabels() const { return isDebugPoints() || isDebugText(); }

  void draw(QPainter *painter, CQArrowDevice *device=nullptr);

  bool contains(const QPointF &p) const;

 private:
  using Points = QVector<QPointF>;

 private:
  void drawPolygon(const Points &points, double w=1.0, bool filled=false, bool stroked=false);

  void drawLine(const QPointF &point1, const QPointF &point2, double width=1.0);

  struct PointLabel {
    PointLabel(const QPointF &point, const QString &text, bool above) :
     point(point), text(text), above(above) {
    }

    QPointF point;
    QString text;
    bool    above { true };
  };

  using PointLabels = std::vector<PointLabel>;

  void drawPointSymbolLabel(const QPointF &point, const QString &text, bool above=true);

  void drawPointSymbol(const QPointF &point);
  void drawPointLabel (const QPointF &point, const QString &text, bool above=true);

#if 0
  double pixelWidthToWindowWidth  (double p);
  double pixelHeightToWindowHeight(double p);
#endif

  double windowWidthToPixelWidth  (double w);
  double windowHeightToPixelHeight(double w);

  QPointF windowToPixel(const QPointF &w);
//QPointF pixelToWindow(const QPointF &w);

  static double pointLineDistance(const QPointF &p, const QPointF &p1, const QPointF &p2);

 private:
  struct HeadData {
    bool     visible   { false };
    HeadType type      { HeadType::NONE };
    double   angle     { -1 };
    double   backAngle { -1 };
    double   length    { -1 };
    bool     lineEnds  { false };
  };

  struct Line {
    QPointF p1;
    QPointF p2;
    bool    valid { false };

    Line() { }

    Line(const QPointF &p1, const QPointF &p2) :
     p1(p1), p2(p2), valid(true) {
    }

    void reset() { valid = false; }

    double distance(const QPointF &p) const {
      return CQArrow::pointLineDistance(p, p1, p2);
    }
  };

  struct Polygon {
    QPolygonF points;
    bool      valid { false };

    Polygon() { }

    Polygon(const Points &points) :
     points(points), valid(true) {
    }

    void reset() { valid = false; }
  };

  //---

  int            ind_         { -1 };
  QPointF        from_        { 0, 0 };
  QPointF        to_          { 1, 1 };
  bool           relative_    { false };
  double         lineWidth_   { -1 };
  HeadData       frontData_;
  HeadData       tailData_;
  bool           filled_      { true };
  bool           stroked_     { false };
  double         strokeWidth_ { -1 };
  bool           debugPoints_ { false };
  bool           debugText_   { false };
  QPainter*      painter_     { nullptr };
  CQArrowDevice* device_      { nullptr };
  QColor         fillColor_   { 100, 100, 200 };
  QColor         strokeColor_ { 100, 200, 100 };
  QColor         textColor_   { 255, 0, 0 };
  bool           inside_      { false };

  mutable Line    frontLine1_;
  mutable Line    frontLine2_;
  mutable Line    endLine1_;
  mutable Line    endLine2_;
  mutable Line    midLine_;
  mutable Polygon frontPoly_;
  mutable Polygon tailPoly_;
  mutable Polygon arrowPoly_;
};

class CQArrowDevice {
 public:
  CQArrowDevice() { }

  virtual ~CQArrowDevice() { }

  virtual QPointF windowToPixel(const QPointF &w) = 0;
  virtual QPointF pixelToWindow(const QPointF &p) = 0;
};

#endif
