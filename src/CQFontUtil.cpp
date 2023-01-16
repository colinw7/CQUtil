#include <CQFontUtil.h>

#ifdef CQUTIL_FONT
#include <CQFont.h>
#endif

#ifdef CQUTIL_FONT
QFont
CQFontUtil::
toQFont(CFontPtr font)
{
  auto *cqfont = dynamic_cast<CQFont *>(font.get());

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
