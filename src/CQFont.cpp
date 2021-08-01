#include <CQFont.h>

#ifdef CQUTIL_FONT

#include <CConfig.h>
#include <CFontMgr.h>
#include <CQImage.h>
#include <CImageLib.h>

#include <QFontMetricsF>
#include <QPainter>

static CQFontMgr *s_instance;

CQFontMgr *
CQFontMgr::
instance()
{
  if (! s_instance)
    s_instance = new CQFontMgr;

  return s_instance;
}

void
CQFontMgr::
release()
{
  delete s_instance;

  s_instance = nullptr;
}

CQFontMgr::
CQFontMgr()
{
  config_ = new CConfig("CQFont");
}

CQFontMgr::
~CQFontMgr()
{
  clear();
}

void
CQFontMgr::
clear()
{
  delete config_;

  config_ = nullptr;
}

void
CQFontMgr::
setPrototype()
{
  CFontPtr font(new CQFont("courier", CFONT_STYLE_NORMAL, 12));

  CFontMgrInst->setPrototype(font);
}

void
CQFontMgr::
resetPrototype()
{
  CFontMgrInst->setPrototype(CFontPtr());
}

CFontPtr
CQFontMgr::
lookupFont(const QFont &qfont)
{
  CFontStyle style = CFONT_STYLE_NORMAL;

  if      (qfont.bold() && qfont.italic())
    style = CFONT_STYLE_BOLD_ITALIC;
  else if (qfont.bold())
    style = CFONT_STYLE_BOLD;
  else if (qfont.italic())
    style = CFONT_STYLE_ITALIC;

  return lookupFont(qfont.family().toStdString(), style, getPixelSize(qfont));
}

CFontPtr
CQFontMgr::
lookupFont(const std::string &family, CFontStyle style, double size, double angle,
           double char_angle, uint x_res, uint y_res)
{
  CFontPtr font = CFontMgrInst->lookupFont(family, style, size, angle, char_angle, x_res, y_res);

  assert(font.cast<CQFont>() != 0);

  return font;
}

std::string
CQFontMgr::
lookupFamily(const std::string &family)
{
  std::string value;

  if (config_->getValue(family, value) && value != "")
    return value;

  return family;
}

CFontStyle
CQFontMgr::
fontStyle(const QFont &qfont)
{
  CFontStyle fontStyle = CFONT_STYLE_NORMAL;

  if (qfont.bold()) {
    if (qfont.italic())
      fontStyle = CFONT_STYLE_BOLD_ITALIC;
    else
      fontStyle = CFONT_STYLE_BOLD;
  }
  else {
    if (qfont.italic())
      fontStyle = CFONT_STYLE_ITALIC;
  }

  return fontStyle;
}

int
CQFontMgr::
getPixelSize(const QFont &qfont)
{
  int size = qfont.pixelSize();

  if (size <= 0)
    size = qfont.pointSize();

  return size;
}

//----------

CQFont::
CQFont(const std::string &family, CFontStyle style, double size, double angle, double char_angle,
       int x_res, int y_res) :
 CFont(family, style, size, angle, char_angle, x_res, y_res)
{
  int weight = QFont::Normal;

  if (style & CFONT_STYLE_BOLD)
    weight = QFont::Bold;

  bool italic = (style & CFONT_STYLE_ITALIC);

  std::string qfamily = CQFontMgrInst->lookupFamily(family);

  //---

  double size1  = size;
  int    isize1 = qRound(size1);

  // ensure that actual font size matches size as closely as possible
  // if CFONT_STYLE_FULL_SIZE is set then size is full char height (ascent + descent)
  // if CFONT_STYLE_FULL_SIZE is NOT set then size is char ascent.
  for (int n = 0; n < 8; ++n) {
    delete qfont_;

    qfont_ = new QFont(qfamily.c_str(), isize1, weight, italic);

    qfont_->setPointSizeF(size1);

    QFontMetricsF fm(*qfont_);

    double th = (style & CFONT_STYLE_FULL_SIZE ? fm.ascent() + fm.descent() : fm.ascent());

    double scale = th/isize1;
//std::cerr << isize1 << "->" << th << ":" << scale << std::endl;

    double size2  = size/scale;
    int    isize2 = qRound(size2);

    if (isize2 == isize1)
      break;

    size1  = size2;
    isize1 = isize2;
  }

  //---

  assert(qfont_);

  if (style & CFONT_STYLE_UNDERLINE)
    qfont_->setUnderline(true);

  if (style & CFONT_STYLE_OVERLINE)
    qfont_->setOverline(true);

  if (style & CFONT_STYLE_STRIKEOUT)
    qfont_->setStrikeOut(true);
}

CQFont::
CQFont(const std::string &full_name) :
 CFont(full_name)
{
}

CQFont::
CQFont(const QFont &qfont) :
 CFont(qfont.family().toStdString(), CQFontMgr::fontStyle(qfont), CQFontMgr::getPixelSize(qfont))
{
  qfont_ = new QFont(qfont);
}

CQFont::
CQFont(const CQFont &qfont) :
 CFont(qfont)
{
  qfont_ = new QFont(*qfont.qfont_);
}

CQFont::
~CQFont()
{
  delete qfont_;
}

CFontPtr
CQFont::
dup(const std::string &family, CFontStyle style, double size, double angle, double char_angle,
    int x_res, int y_res) const
{
  CQFont *qfont = new CQFont(family, style, size, angle, char_angle, x_res, y_res);

  return CFontPtr(qfont);
}

CQFont &
CQFont::
operator=(const CQFont &qfont)
{
  qfont_ = new QFont(*qfont.qfont_);

  return *this;
}

double
CQFont::
getCharWidth() const
{
  QFontMetricsF fm(*qfont_);

  return fm.maxWidth();
}

double
CQFont::
getCharAscent() const
{
  QFontMetricsF fm(*qfont_);

  return fm.ascent();
}

double
CQFont::
getCharDescent() const
{
  QFontMetricsF fm(*qfont_);

  return fm.descent();
}

double
CQFont::
getCharHeight() const
{
  QFontMetricsF fm(*qfont_);

  return fm.height();
}

double
CQFont::
getStringWidth(const std::string &str) const
{
  QString qstr(str.c_str());

  QFontMetricsF fm(*qfont_);

  //return fm.size(0, qstr).width();
  return fm.width(qstr);
}

bool
CQFont::
isProportional() const
{
  return ! qfont_->fixedPitch();
}

CImagePtr
CQFont::
getStringImage(const std::string &str)
{
  uint w = getIStringWidth(str);
  uint h = getICharHeight();

  CImageNameSrc src("CQFont::getStringImage");

  CImagePtr image = CImageMgrInst->createImage(src);

  image->setDataSize(w, h);

  CQImage *qimage = image.cast<CQImage>();

  qimage->updateCImage();

  QPainter painter(&qimage->getQImage());

  qimage->updateCImage();

  QPen   pen;
  QBrush brush;

  brush.setColor(Qt::transparent);

  brush.setStyle(Qt::NoBrush);

  painter.fillRect(0, 0, w, h, brush);

  qimage->updateCImage();

  pen.setColor(QColor(0,0,0));

  painter.setPen  (pen);
  painter.setBrush(brush);

  painter.setFont(*qfont_);

  int ascent = getICharAscent();

  painter.drawText(0, ascent, QString(str.c_str()));

  qimage->updateCImage();

  return image;
}

#endif
