#include <CQPixmapCache.h>
#include <QIconEngine>
#include <QPainter>
#include <cassert>
#include <iostream>

class CQPixmapIcon : public QIconEngine {
 public:
  CQPixmapIcon(const QString &lightId, const QString &darkId) :
   lightId_(lightId), darkId_(darkId), dark_(CQPixmapCacheInst->isDark()) {
    updateIcon();
  }

  void updateIcon() {
    if (dark_ && darkId_.length())
      icon_ = CQPixmapCacheInst->getPixmapIcon(darkId_);
    else
      icon_ = CQPixmapCacheInst->getPixmapIcon(lightId_);
  }

  QSize actualSize(const QSize &size, QIcon::Mode, QIcon::State) {
    return size;
  }

  QString iconName() const {
    if (dark_ && darkId_.length())
      return darkId_;

    return lightId_;
  }

  void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) {
    painter->drawPixmap(0, 0, pixmap(rect.size(), mode, state));
  }

  QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) {
    bool dark = CQPixmapCacheInst->isDark();

    if (dark_ != dark) {
      dark_ = dark;

      if (darkId_.length())
        updateIcon();
    }

    if (size != state_.size || mode != state_.mode || state != state_.state) {
      pixmap_ = icon_.pixmap(size, mode, state);

      state_.size  = size;
      state_.mode  = mode;
      state_.state = state;
    }

    return pixmap_;
  }

  QIconEngine *clone() const {
    return new CQPixmapIcon(lightId_, darkId_);
  }

 private:
  struct State {
    QSize        size;
    QIcon::Mode  mode;
    QIcon::State state;
  };

  QString lightId_;
  QString darkId_;
  QIcon   icon_;
  QPixmap pixmap_;
  State   state_;
  bool    dark_ { false };
};

//------

static CQPixmapCache *s_inst;

CQPixmapCache *
CQPixmapCache::
instance()
{
  if (! s_inst)
    s_inst = new CQPixmapCache;

  return s_inst;
}

void
CQPixmapCache::
release()
{
  delete s_inst;

  s_inst = nullptr;
}

CQPixmapCache::
CQPixmapCache()
{
}

CQPixmapCache::
~CQPixmapCache()
{
}

void
CQPixmapCache::
clear()
{
  idData_.clear();
}

void
CQPixmapCache::
addData(const QString &id, const uchar *data, int len)
{
  idData_[id] = Data(data, len);
}

bool
CQPixmapCache::
hasPixmap(const QString &id) const
{
  auto p = idData_.find(id);

  return (p != idData_.end());
}

const QPixmap &
CQPixmapCache::
getPixmap(const QString &id)
{
  static QPixmap dummy_pixmap;

  auto p = idData_.find(id);

  if (p == idData_.end()) {
    std::cerr << "Pixmap '" << id.toStdString() << "' not found\n";

    if (getenv("CQPIXMAP_CACHE_ASSERT"))
      assert(false);
    else
      return dummy_pixmap;
  }

  Data &data = (*p).second;

  if (! data.pixmap) {
    data.pixmap = new QPixmap;

    bool rc = data.pixmap->loadFromData(data.data, data.len);

    if (! rc) {
      std::cerr << "Pixmap '" << id.toStdString() << "' failed to load\n";

      if (getenv("CQPIXMAP_CACHE_ASSERT"))
        assert(false);
      else
        return dummy_pixmap;
    }
  }

  return *data.pixmap;
}

QPixmap
CQPixmapCache::
getSizedPixmap(const QString &id, const QSize &s)
{
  return getPixmap(id).scaled(s, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QIcon
CQPixmapCache::
getIcon(const QString &lightId, const QString &darkId)
{
  return QIcon(new CQPixmapIcon(lightId, darkId));
}

QIcon
CQPixmapCache::
getPixmapIcon(const QString &id)
{
  return QIcon(getPixmap(id));
}
