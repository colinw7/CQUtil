#include <CQPixmapCache.h>
#include <cassert>

CQPixmapCache *
CQPixmapCache::
instance()
{
  static CQPixmapCache *inst;

  if (! inst)
    inst = new CQPixmapCache;

  return inst;
}

CQPixmapCache::
CQPixmapCache()
{
}

void
CQPixmapCache::
addData(const QString &id, const uchar *data, int len)
{
  idData_[id] = Data(data, len);
}

const QPixmap &
CQPixmapCache::
getPixmap(const QString &id)
{
  auto p = idData_.find(id);

  assert(p != idData_.end());

  if (! (*p).second.pixmap) {
    (*p).second.pixmap = new QPixmap;

    (*p).second.pixmap->loadFromData((*p).second.data, (*p).second.len);
  }

  return *(*p).second.pixmap;
}

QIcon
CQPixmapCache::
getIcon(const QString &id)
{
  return QIcon(getPixmap(id));
}
