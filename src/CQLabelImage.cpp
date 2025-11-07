#include <CQLabelImage.h>

#include <QPainter>

CQLabelImage::
CQLabelImage(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("qlabel");
}

void
CQLabelImage::
updateLines()
{
  lines_.clear();

  auto label = this->label();

  auto pos = label.indexOf('\n');

  while (pos != -1) {
    auto lhs = label.mid(0, pos);

    lines_.push_back(lhs);

    label = label.mid(pos + 1);

    pos = label.indexOf('\n');
  }

  if (label != "")
    lines_.push_back(label);
}

void
CQLabelImage::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  //---

  QFontMetrics fm(font());

  bool hasLabel = false;;
  int tw = 0, ta = 0, td = 0;

  if (label().length()) {
    hasLabel = true;

    tw = fm.horizontalAdvance(label());
    ta = fm.ascent();
    td = fm.descent();
  }

  bool hasImage = false;;
  int iw = 0, ih = 0;

  if (! image_.isNull()) {
    hasImage = true;

    iw = image_.width();
    ih = image_.height();
  }

  int b = 2;

  //---

  painter.setPen(palette().windowText().color());

  auto rect = contentsRect();

  int x = rect.left() + b;
  int y = rect.top () + b;
  int w = rect.width () - 2*b;
  int h = rect.height() - 2*b;

  int xc = x + w/2;
  int yc = y + h/2;

  if      (! hasImage) {
    int yc = (rect.height() - lines_.size()*(ta + td))/2 + (ta + td)/2;

    for (const auto &line : lines_) {
      auto tw = fm.horizontalAdvance(line);

      if      (justify() & Qt::AlignLeft)
        painter.drawText(x, yc + (ta - td)/2, line);
      else if (justify() & Qt::AlignHCenter)
        painter.drawText(xc - tw/2, yc + (ta - td)/2, line);
      else if (justify() & Qt::AlignRight)
        painter.drawText(x + w - tw, yc + (ta - td)/2, line);
      else
        painter.drawText(x, yc + (ta - td)/2, line);

      yc += ta + td;
    }
  }
  else if (! hasLabel) {
    painter.drawImage(xc - iw/2, yc - ih/2, image());
  }
  else {
    if      (type_ == Type::LEFT) {
      painter.drawImage(x, yc - ih/2, image()); x += iw + b;

      painter.drawText(x, yc + (ta - td)/2, label());
    }
    else if (type_ == Type::RIGHT) {
      painter.drawText(x, yc + (ta - td)/2, label()); x += tw + b;

      painter.drawImage(x, yc - ih/2, image());
    }
    else if (type_ == Type::BOTTOM) {
      painter.drawText(xc - tw/2, y + ta, label()); y += ta + td;

      painter.drawImage(xc - iw/2, y, image());
    }
    else if (type_ == Type::TOP) {
      painter.drawImage(xc - iw/2, y, image()); y += ih + b;

      painter.drawText(xc - tw/2, y + ta, label());
    }
    else if (type_ == Type::CENTER) {
      painter.drawImage(xc - iw/2, yc - ih/2, image());
      painter.drawText (xc - tw/2, yc + (ta - td)/2, label());
    }
    else {
      painter.drawImage(xc - iw/2, yc - ih/2, image());
      painter.drawText (xc - tw/2, yc + (ta - td)/2, label());
    }
  }
}

QSize
CQLabelImage::
sizeHint() const
{
  QFontMetrics fm(font());

  bool hasLabel = false;
  int labelWidth = 0;

  if (label().length()) {
    hasLabel = true;

    labelWidth = fm.horizontalAdvance(label());
  }

  int fontHeight = fm.height();

  bool hasImage = false;
  int iw = 0, ih = 0;

  if (! image_.isNull()) {
    hasImage = true;

    iw = image_.width();
    ih = image_.height();
  }

  int b = 2;

  //---

  if      (! hasImage) {
    int tw = 0;

    if (userWidth() > 0)
      tw = userWidth()*fm.horizontalAdvance("0");
    else {
      if (! lines_.empty()) {
        for (auto &line : lines_)
          tw = std::max(tw, fm.horizontalAdvance(line));
      }
      else
        tw = fm.horizontalAdvance(" ");
    }

    int th = 0;

    if (userHeight() > 0)
      th = userHeight()*fontHeight;
    else {
      if (! lines_.empty())
        th = lines_.size()*fontHeight;
      else
        th = fontHeight;
    }

    return QSize(tw + 2*b, th + 2*b);
  }
  else if (! hasLabel) {
    if (userWidth() > 0)
      iw = userWidth();

    if (userHeight() > 0)
      ih = userHeight();

    return QSize(iw + 2*b, ih + 2*b);
  }
  else {
    int w = 0, h = 0;

    if      (type_ == Type::LEFT || type_ == Type::RIGHT) {
      w = labelWidth + iw + b;
      h = std::max(fontHeight, ih);
    }
    else if (type_ == Type::TOP || type_ == Type::BOTTOM) {
      w = std::max(labelWidth, iw);
      h = ih + fontHeight + b;
    }
    else if (type_ == Type::CENTER) {
      w = std::max(labelWidth, iw);
      h = std::max(fontHeight, ih);
    }
    else {
      w = std::max(labelWidth, iw);
      h = std::max(fontHeight, ih);
    }

    if (userWidth() > 0)
      w = userWidth();

    if (userHeight() > 0)
      h = userHeight();

    return QSize(w + 2*b, h + 2*b);
  }
}
