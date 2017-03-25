#ifndef CQFONT_H
#define CQFONT_H

#include <CFont.h>

#include <QFont>

class CConfig;

#define CQFontMgrInst CQFontMgr::getInstance()

class CQFontMgr {
 public:
  static CQFontMgr *getInstance();

  void setPrototype();

  CFontPtr lookupFont(const QFont &qfont);

  CFontPtr lookupFont(const std::string &family, CFontStyle style, double size,
                      double angle=0, double char_angle=0, uint x_res=100, uint y_res=100);

  std::string lookupFamily(const std::string &family);

  static CFontStyle fontStyle(const QFont &qfont);

  static int getPixelSize(const QFont &qfont);

 private:
  CQFontMgr();
 ~CQFontMgr();

 private:
  CConfig *config_ { 0 };
};

//------

class CQFont : public CFont {
 protected:
  CQFont(const std::string &family, CFontStyle style, double size, double angle=0,
         double char_angle=0, int x_res=100, int y_res=100);

  explicit CQFont(const std::string &full_name);

  virtual ~CQFont();

  CFontPtr dup(const std::string &family, CFontStyle style, double size, double angle=0,
               double char_angle=0, int x_res=100, int y_res=100) const;

 private:
  CQFont(const CFont &qfont);
  CQFont(const QFont &qfont);
  CQFont(const CQFont &qfont);

  const CQFont &operator=(const CFont &qfont);
  const CQFont &operator=(const CQFont &qfont);

 public:
  const QFont &getQFont() const { return *qfont_; }

  double getCharWidth () const;
  double getCharAscent() const;
  double getCharDescent() const;
  double getCharHeight() const;

  double getStringWidth(const std::string &str) const;

  bool isProportional() const;

  CImagePtr getStringImage(const std::string &str);

 private:
  friend class CQFontMgr;

  QFont *qfont_ { 0 };
};

#endif
