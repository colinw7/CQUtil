#include <CQFontUtil.h>

#ifdef CQUTIL_FONT
#include <CQFont.h>
#endif

#ifdef CQUTIL_FONT
QFont
CQFontUtil::
toQFont(CFontPtr font)
{
  CQFont *cqfont = font.cast<CQFont>();

  if (cqfont)
    return cqfont->getQFont();
  else
    return QFont();
}

CFontPtr
CQFontUtil::
fromQFont(QFont font)
{
  return CQFontMgrInst->lookupFont(font);
}
#endif
