#include <CQPixmapCache.h>
#include <cassert>
#include <iostream>

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

  if (p == idData_.end())
    std::cout << "Pixmap not found: " << id.toStdString() << std::endl;

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
