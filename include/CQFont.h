#ifndef CQFONT_H
#define CQFONT_H

#include <CFont.h>

#include <QFont>

class CConfig;

#define CQFontMgrInst CQFontMgr::instance()

class CQFontMgr {
 public:
  static CQFontMgr *instance();

  static void release();

  void setPrototype();
  void resetPrototype();

  void clear();

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
  CConfig *config_ { nullptr };
};

//------

class CQFont : public CFont {
 protected:
  CQFont(const std::string &family, CFontStyle style, double size, double angle=0,
         double char_angle=0, int x_res=100, int y_res=100);

  explicit CQFont(const std::string &full_name);

  CFontPtr dup(const std::string &family, CFontStyle style, double size, double angle=0,
               double char_angle=0, int x_res=100, int y_res=100) const override;

 public:
  virtual ~CQFont();

 private:
  CQFont(const CFont &qfont);
  CQFont(const QFont &qfont);
  CQFont(const CQFont &qfont);

  CQFont &operator=(const CFont &qfont);
  CQFont &operator=(const CQFont &qfont);

 public:
  const QFont &getQFont() const { return *qfont_; }

  double getCharWidth () const override;
  double getCharAscent() const override;
  double getCharDescent() const override;
  double getCharHeight() const override;

  double getStringWidth(const std::string &str) const override;

  bool isProportional() const override;

  CImagePtr getStringImage(const std::string &str) override;

 private:
  friend class CQFontMgr;

  QFont *qfont_ { nullptr };
};

#endif
