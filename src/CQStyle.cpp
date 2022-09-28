#include <CQStyle.h>

#include <QApplication>
#include <QGroupBox>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

#include <qdrawutil.h>

//------

enum Direction {
  TopDown,
  FromLeft,
  BottomUp,
  FromRight
};

static void
drawGradient(QPainter *painter, const QRect &rect, const QColor &gradientStart,
             const QColor &gradientStop, Direction direction=TopDown, QBrush bgBrush=QBrush())
{
  int x = rect.center().x();
  int y = rect.center().y();

  QLinearGradient *gradient;

  switch (direction) {
    case FromLeft:
      gradient = new QLinearGradient(rect.left(), y, rect.right(), y);
      break;
    case FromRight:
      gradient = new QLinearGradient(rect.right(), y, rect.left(), y);
      break;
    case BottomUp:
      gradient = new QLinearGradient(x, rect.bottom(), x, rect.top());
      break;
    case TopDown:
    default:
      gradient = new QLinearGradient(x, rect.top(), x, rect.bottom());
      break;
  }

  if (bgBrush.gradient())
    gradient->setStops(bgBrush.gradient()->stops());
  else {
    gradient->setColorAt(0, gradientStart);
    gradient->setColorAt(1, gradientStop);
  }

  painter->fillRect(rect, *gradient);

  delete gradient;
}

static QColor
mergedColors(const QColor &colorA, const QColor &colorB, double factor = 50.0)
{
  const double mf = 100.0; // maxFactor

  QColor tmp = colorA;

  tmp.setRed  (int((tmp.red  ()*factor)/mf + (colorB.red  ()*(mf - factor))/mf));
  tmp.setGreen(int((tmp.green()*factor)/mf + (colorB.green()*(mf - factor))/mf));
  tmp.setBlue (int((tmp.blue ()*factor)/mf + (colorB.blue ()*(mf - factor))/mf));

  return tmp;
}

static QColor
grayColor(const QColor &c)
{
  int g = qGray(c.red(), c.green(), c.blue());

  return QColor(g, g, g);
}

static QColor
disabledColor(const QColor &c, const QColor &bg)
{
  return mergedColors(grayColor(c), bg, 40);
}

static QColor
inactiveColor(const QColor &c, const QColor &)
{
  return c;
}

#if 0
static const char * const
qt_scrollbar_button_left[] = {
  "16 16 6 1",
  "   c None",
  ".  c #BFBFBF",
  "+  c #979797",
  "#  c #FAFAFA",
  "<  c #FAFAFA",
  "*  c #FAFAFA",
  " .++++++++++++++",
  ".+#############+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  ".+<<<<<<<<<<<<<+",
  " .++++++++++++++"
};

static const char * const
qt_scrollbar_button_right[] =
{
  "16 16 6 1",
  "   c None",
  ".  c #BFBFBF",
  "+  c #979797",
  "#  c #FAFAFA",
  "<  c #FAFAFA",
  "*  c #FAFAFA",
  "++++++++++++++. ",
  "+#############+.",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+<<<<<<<<<<<<<+.",
  "++++++++++++++. "
};

static const char * const
qt_scrollbar_button_up[] = {
  "16 16 6 1",
  "   c None",
  ".  c #BFBFBF",
  "+  c #979797",
  "#  c #FAFAFA",
  "<  c #FAFAFA",
  "*  c #FAFAFA",
  " .++++++++++++. ",
  ".+############+.",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+<<<<<<<<<<<<<<+",
  "++++++++++++++++"
};

static const char * const
qt_scrollbar_button_down[] = {
  "16 16 6 1",
  "   c None",
  ".  c #BFBFBF",
  "+  c #979797",
  "#  c #FAFAFA",
  "<  c #FAFAFA",
  "*  c #FAFAFA",
  "++++++++++++++++",
  "+##############+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  "+#            <+",
  ".+<<<<<<<<<<<<+.",
  " .++++++++++++. "
};
#endif

//------

CQStyle::
CQStyle() :
 QProxyStyle()
{
  setTheme(Theme::LIGHT);
}

void
CQStyle::
setTheme(const Theme &t)
{
  theme_ = t;

  palette_ = qApp->palette();

  auto windowTextColor    = QColor("#000000");
  auto buttonColor        = QColor("#efebe7");
  auto textColor          = QColor("#000000");
  auto buttonTextColor    = QColor("#000000");
  auto baseColor          = QColor("#ffffff");
  auto windowColor        = QColor("#efebe7");
  auto altBaseColor       = baseColor.darker(110);
  auto highlightColor     = QColor(Qt::darkBlue);
  auto highlightTextColor = QColor(Qt::white);
  auto linkColor          = QColor(Qt::blue);
  auto linkVisitedColor   = QColor(Qt::magenta);
  auto brightTextColor    = QColor(Qt::yellow);

  if      (theme_ == Theme::LIGHT) {
    windowTextColor    = QColor("#000000");
    buttonColor        = QColor("#efebe7");
    textColor          = QColor("#000000");
    buttonTextColor    = QColor("#000000");
    baseColor          = QColor("#f3f3f4");
    windowColor        = QColor("#efebe7");
    altBaseColor       = baseColor.darker(110);
    highlightColor     = QColor("#9ab87c");
    highlightTextColor = Qt::white;
    linkColor          = Qt::blue;
    linkVisitedColor   = Qt::magenta;
    brightTextColor    = Qt::yellow;
  }
  else if (theme_ == Theme::DARK) {
    windowTextColor    = QColor("#eaeaea");
    buttonColor        = QColor("#35353a");
    textColor          = QColor("#eaeaea");
    buttonTextColor    = QColor("#eaeaea");
    baseColor          = QColor("#373737");
    windowColor        = QColor("#373737");
    altBaseColor       = QColor("#424242");
    highlightColor     = QColor("#9ab87c");
    highlightTextColor = QColor("#d4d4d4");
    linkColor          = QColor("#3d81c6");
    linkVisitedColor   = QColor("#6b00b9");
    brightTextColor    = QColor("#c63333");
  }

  palette_.setColor(QPalette::Active, QPalette::WindowText     , windowTextColor   );
  palette_.setColor(QPalette::Active, QPalette::Button         , buttonColor       );
  palette_.setColor(QPalette::Active, QPalette::Text           , textColor         );
  palette_.setColor(QPalette::Active, QPalette::ButtonText     , buttonTextColor   );
  palette_.setColor(QPalette::Active, QPalette::Window         , windowColor       );
  palette_.setColor(QPalette::Active, QPalette::Base           , baseColor         );
  palette_.setColor(QPalette::Active, QPalette::AlternateBase  , altBaseColor      );
  palette_.setColor(QPalette::Active, QPalette::Highlight      , highlightColor    );
  palette_.setColor(QPalette::Active, QPalette::HighlightedText, highlightTextColor);
  palette_.setColor(QPalette::Active, QPalette::Link           , linkColor         );
  palette_.setColor(QPalette::Active, QPalette::LinkVisited    , linkVisitedColor  );

  //---

  auto lightColor    = mergedColors(buttonColor, Qt::white  );
  auto shadowColor   = mergedColors(buttonColor, Qt::black  );
  auto midlightColor = mergedColors(buttonColor, lightColor );
  auto darkColor     = mergedColors(buttonColor, shadowColor);
  auto midColor      = mergedColors(buttonColor, darkColor  );

  palette_.setColor(QPalette::Active, QPalette::Light     , lightColor     );
  palette_.setColor(QPalette::Active, QPalette::Midlight  , midlightColor  );
  palette_.setColor(QPalette::Active, QPalette::Dark      , darkColor      );
  palette_.setColor(QPalette::Active, QPalette::Mid       , midColor       );
  palette_.setColor(QPalette::Active, QPalette::Shadow    , shadowColor    );
  palette_.setColor(QPalette::Active, QPalette::BrightText, brightTextColor);

  //---

  QPalette::ColorRole roles[] = {
   QPalette::Window, QPalette::WindowText, QPalette::Base, QPalette::AlternateBase,
   QPalette::ToolTipBase, QPalette::ToolTipText, QPalette::Text, QPalette::Button,
   QPalette::ButtonText, QPalette::BrightText, QPalette::Light, QPalette::Midlight,
   QPalette::Dark, QPalette::Mid, QPalette::Shadow, QPalette::Highlight,
   QPalette::HighlightedText, QPalette::Link, QPalette::LinkVisited
  };

  int num_roles = sizeof(roles)/sizeof(roles[0]);

  for (int r = 0; r < num_roles; ++r) {
//  if (roles[r] == QPalette::Window || roles[r] == QPalette::Base || roles[r] == QPalette::Button)
//    continue;

    auto c = palette_.color(QPalette::Active, roles[r]);

    palette_.setColor(QPalette::Inactive, roles[r], inactiveColor(c, windowColor));
    palette_.setColor(QPalette::Disabled, roles[r], disabledColor(c, windowColor));
  }

  //---

  qApp->setPalette(palette_);
}

void
CQStyle::
drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                   QPainter *painter, const QWidget *widget) const
{
  switch (control) {
    case CC_GroupBox: {
      if (const auto *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
        // Draw frame
        auto textRect = proxy()->subControlRect(CC_GroupBox, option, SC_GroupBoxLabel, widget);

        QRect checkBoxRect =
          proxy()->subControlRect(CC_GroupBox, option, SC_GroupBoxCheckBox, widget);

        if (groupBox->subControls & QStyle::SC_GroupBoxFrame) {
#if QT_VERSION >= 0x050000
          QStyleOptionFrame frame;
#else
          QStyleOptionFrameV2 frame;
#endif

          frame.QStyleOption::operator=(*groupBox);

          frame.features     = groupBox->features;
          frame.lineWidth    = groupBox->lineWidth;
          frame.midLineWidth = groupBox->midLineWidth;
          frame.rect         = proxy()->subControlRect(CC_GroupBox, option,
                                                       SC_GroupBoxFrame, widget);

          painter->save();

          QRegion region(groupBox->rect);

          if (! groupBox->text.isEmpty()) {
            bool ltr = groupBox->direction == Qt::LeftToRight;

            QRect finalRect;

            if (groupBox->subControls & QStyle::SC_GroupBoxCheckBox) {
              finalRect = checkBoxRect.united(textRect);

              finalRect.adjust(ltr ? -4 : 0, 0, ltr ? 0 : 4, 0);
            }
            else {
              finalRect = textRect;
            }

            region -= finalRect;
          }

          painter->setClipRegion(region);

          proxy()->drawPrimitive(PE_FrameGroupBox, &frame, painter, widget);

          painter->restore();
        }

        // Draw title
        if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty()) {
          auto textColor = groupBox->textColor;

          if (textColor.isValid())
            painter->setPen(textColor);

          uint alignment = uint(groupBox->textAlignment);

          if (! proxy()->styleHint(QStyle::SH_UnderlineShortcut, option, widget))
            alignment |= Qt::TextHideMnemonic;

          proxy()->drawItemText(painter, textRect,
                                Qt::AlignmentFlag(uint(Qt::TextShowMnemonic) |
                                                  uint(Qt::AlignHCenter) | alignment),
                                groupBox->palette, groupBox->state & State_Enabled, groupBox->text,
                                textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);

          if (groupBox->state & State_HasFocus) {
            QStyleOptionFocusRect fropt;

            fropt.QStyleOption::operator=(*groupBox);

            fropt.rect = textRect;

            proxy()->drawPrimitive(PE_FrameFocusRect, &fropt, painter, widget);
          }
        }

        // Draw checkbox
        if (groupBox->subControls & SC_GroupBoxCheckBox) {
          QStyleOptionButton box;

          box.QStyleOption::operator=(*groupBox);

          box.rect = checkBoxRect;

          proxy()->drawPrimitive(PE_IndicatorCheckBox, &box, painter, widget);
        }
      }

      break;
    }
    case CC_ScrollBar: {
      auto palette = option->palette;

      auto button = palette.button().color();

      QColor dark;

      dark.setHsv(button.hue(),
                  qMin(255, static_cast<int>(button.saturation()*1.9)),
                  qMin(255, static_cast<int>(button.value()*0.7)));

      QColor grooveColor;

      grooveColor.setHsv(button.hue(),
                         qMin(255, static_cast<int>(button.saturation()*2.6)),
                         qMin(255, static_cast<int>(button.value()*0.9)));

      QColor darkOutline;

      darkOutline.setHsv(button.hue(),
                         qMin(255, static_cast<int>(button.saturation()*3.0)),
                         qMin(255, static_cast<int>(button.value()*0.6)));

      QColor alphaCornerColor;

      if (widget) {
        // ### backgroundrole/foregroundrole should be part of the style option
        alphaCornerColor = mergedColors(palette.color(widget->backgroundRole()), darkOutline);
      } else {
        alphaCornerColor = mergedColors(palette.window().color(), darkOutline);
      }

      auto gripShadow         = grooveColor.darker(110);
    //auto buttonShadow       = palette.button().color().darker(110);
      auto gradientStartColor = palette.button().color().lighter(108);
      auto gradientStopColor  = mergedColors(palette.button().color().darker(108),
                                               dark.lighter(150), 70);

      //---

      painter->save();

      painter->setRenderHint(QPainter::Antialiasing, true);

      if (const auto *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
        bool isEnabled  = scrollBar->state & State_Enabled;
//      bool reverse    = scrollBar->direction == Qt::RightToLeft;
        bool horizontal = scrollBar->orientation == Qt::Horizontal;
        bool sunken     = scrollBar->state & State_Sunken;

        painter->fillRect(option->rect, palette.window());

        QRect scrollBarSubLine =
          proxy()->subControlRect(control, scrollBar, SC_ScrollBarSubLine, widget);
        QRect scrollBarAddLine =
          proxy()->subControlRect(control, scrollBar, SC_ScrollBarAddLine, widget);
        QRect scrollBarSlider =
          proxy()->subControlRect(control, scrollBar, SC_ScrollBarSlider, widget);
        QRect grooveRect =
          proxy()->subControlRect(control, scrollBar, SC_ScrollBarGroove, widget);

        // paint groove
        if (scrollBar->subControls & SC_ScrollBarGroove) {
          painter->setBrush(grooveColor);
          painter->setPen  (Qt::NoPen);

          if (horizontal) {
            painter->drawRect(grooveRect);

            painter->setPen(darkOutline);
            painter->drawLine(grooveRect.topLeft(), grooveRect.topRight());
            painter->drawLine(grooveRect.bottomLeft(), grooveRect.bottomRight());
          } else {
            painter->drawRect(grooveRect);

            painter->setPen(darkOutline);
            painter->drawLine(grooveRect.topLeft(), grooveRect.bottomLeft());
            painter->drawLine(grooveRect.topRight(), grooveRect.bottomRight());
          }
        }

        //paint slider
        if (scrollBar->subControls & SC_ScrollBarSlider) {
          auto pixmapRect = scrollBarSlider;

          //if (horizontal)
          //  pixmapRect.adjust(-1, 0, 0, -1);
          //else
          //  pixmapRect.adjust(0, -1, -1, 0);

          if (isEnabled) {
            QLinearGradient gradient(pixmapRect.center().x(), pixmapRect.top(),
                                     pixmapRect.center().x(), pixmapRect.bottom());

            if (! horizontal)
              gradient = QLinearGradient(pixmapRect.left(), pixmapRect.center().y(),
                                         pixmapRect.right(), pixmapRect.center().y());

            if (palette.button().gradient()) {
              gradient.setStops(palette.button().gradient()->stops());
            }
            else {
              if (sunken || (option->state & State_MouseOver &&
                  (scrollBar->activeSubControls & SC_ScrollBarSlider))) {
                gradient.setColorAt(0, gradientStartColor.lighter(110));
                gradient.setColorAt(1, gradientStopColor.lighter(110));
              }
              else {
                gradient.setColorAt(0, gradientStartColor);
                gradient.setColorAt(1, gradientStopColor);
              }
            }

            painter->setPen  (QPen(darkOutline, 0));
            painter->setBrush(gradient);

            painter->drawRect(pixmapRect);

            //calculate offsets used by highlight and shadow
            int yoffset, xoffset;

            if (option->state & State_Horizontal) {
              xoffset = 0;
              yoffset = 1;
            }
            else {
              xoffset = 1;
              yoffset = 0;
            }

            //draw slider highlights
            painter->setPen(QPen(gradientStopColor, 0));

            painter->drawLine(scrollBarSlider.left  () + xoffset,
                              scrollBarSlider.bottom() - yoffset,
                              scrollBarSlider.right () - xoffset,
                              scrollBarSlider.bottom() - yoffset);
            painter->drawLine(scrollBarSlider.right () - xoffset,
                              scrollBarSlider.top   () + yoffset,
                              scrollBarSlider.right () - xoffset,
                              scrollBarSlider.bottom() - yoffset);

            //draw slider shadow
            painter->setPen(QPen(gradientStartColor, 0));

            painter->drawLine(scrollBarSlider.left  () + xoffset,
                              scrollBarSlider.top   () + yoffset,
                              scrollBarSlider.right () - xoffset,
                              scrollBarSlider.top   () + yoffset);
            painter->drawLine(scrollBarSlider.left  () + xoffset,
                              scrollBarSlider.top   () + yoffset,
                              scrollBarSlider.left  () + xoffset,
                              scrollBarSlider.bottom() - yoffset);
          }
          else {
            QLinearGradient gradient(pixmapRect.center().x(), pixmapRect.top(),
                                     pixmapRect.center().x(), pixmapRect.bottom());

            if (! horizontal) {
              gradient = QLinearGradient(pixmapRect.left (), pixmapRect.center().y(),
                                         pixmapRect.right(), pixmapRect.center().y());
            }

            if (sunken) {
              gradient.setColorAt(0, gradientStartColor.lighter(110));
              gradient.setColorAt(1, gradientStopColor .lighter(110));
            }
            else {
              gradient.setColorAt(0, gradientStartColor);
              gradient.setColorAt(1, gradientStopColor );
            }

            painter->setPen  (darkOutline);
            painter->setBrush(gradient);

            painter->drawRect(pixmapRect);
          }

          int gripMargin = 4;

          // draw grips
          if (horizontal) {
            for (int i = -3; i< 6 ; i += 3) {
              painter->setPen(QPen(gripShadow, 1));

              painter->drawLine(QPoint(scrollBarSlider.center().x() + i ,
                                       scrollBarSlider.top   () + gripMargin),
                                QPoint(scrollBarSlider.center().x() + i,
                                       scrollBarSlider.bottom() - gripMargin));

              painter->setPen(QPen(palette.light(), 1));

              painter->drawLine(QPoint(scrollBarSlider.center().x() + i + 1,
                                       scrollBarSlider.top   () + gripMargin  ),
                                QPoint(scrollBarSlider.center().x() + i + 1,
                                       scrollBarSlider.bottom() - gripMargin));
            }
          } else {
            for (int i = -3; i < 6 ; i += 3) {
              painter->setPen(QPen(gripShadow, 1));

              painter->drawLine(QPoint(scrollBarSlider.left  () + gripMargin ,
                                       scrollBarSlider.center().y()+ i),
                                QPoint(scrollBarSlider.right () - gripMargin,
                                       scrollBarSlider.center().y()+ i));

              painter->setPen(QPen(palette.light(), 1));

              painter->drawLine(QPoint(scrollBarSlider.left  () + gripMargin,
                                       scrollBarSlider.center().y() + 1 + i),
                                QPoint(scrollBarSlider.right () - gripMargin,
                                       scrollBarSlider.center().y() + 1 + i));
            }
          }
        }

        // The SubLine (up/left) buttons
        if (scrollBar->subControls & SC_ScrollBarSubLine) {
          //int scrollBarExtent = proxy()->pixelMetric(PM_ScrollBarExtent, option, widget);

          auto pixmapRect = scrollBarSubLine;

          if (isEnabled ) {
            auto fillRect = pixmapRect.adjusted(1, 1, -1, -1);

            // Gradients
            if ((scrollBar->activeSubControls & SC_ScrollBarSubLine) && sunken) {
              drawGradient(painter,
                           QRect(fillRect),
                           gradientStopColor.darker(120), gradientStopColor.darker(120),
                           horizontal ? TopDown : FromLeft,
                           palette.button());
            }
            else {
              drawGradient(painter,
                           QRect(fillRect),
                           gradientStartColor.lighter(105), gradientStopColor,
                           horizontal ? TopDown : FromLeft,
                           palette.button());
            }
          }

          // Details
#if 0
          QImage subButton;

          if (horizontal) {
            subButton = QImage(reverse ? qt_scrollbar_button_right : qt_scrollbar_button_left);
          }
          else {
            subButton = QImage(qt_scrollbar_button_up);
          }

          subButton.setColor(1, alphaCornerColor.rgba());
          subButton.setColor(2, darkOutline.rgba());

          if ((scrollBar->activeSubControls & SC_ScrollBarSubLine) && sunken) {
            subButton.setColor(3, gradientStopColor.darker(140).rgba());
            subButton.setColor(4, gradientStopColor.darker(120).rgba());
          }
          else {
            subButton.setColor(3, gradientStartColor.lighter(105).rgba());
            subButton.setColor(4, gradientStopColor.rgba());
          }

          subButton.setColor(5, scrollBar->palette.text().color().rgba());

          painter->drawImage(pixmapRect, subButton);
#else
          painter->setPen(darkOutline);

          painter->drawRoundedRect(pixmapRect, 3, 3);
#endif

          // Arrow
          PrimitiveElement arrow;

          if (option->state & State_Horizontal)
            arrow = (option->direction == Qt::LeftToRight ?
                       PE_IndicatorArrowLeft : PE_IndicatorArrowRight);
          else
            arrow = PE_IndicatorArrowUp;

          auto arrowOpt = *option;

          arrowOpt.rect = scrollBarSubLine.adjusted(3, 3, -2, -2);

          proxy()->drawPrimitive(arrow, &arrowOpt, painter, widget);
        }

        // The AddLine (down/right) button
        if (scrollBar->subControls & SC_ScrollBarAddLine) {
          //QString addLinePixmapName =
          //  QStyleHelper::uniqueName(QLatin1String("scrollbar_addline"), option, QSize(16, 16));

          auto addLinePixmapName = QString("scrollbar_addline_123");

          auto pixmapRect = scrollBarAddLine;

          if (isEnabled) {
            auto fillRect = pixmapRect.adjusted(1, 1, -1, -1);

            // Gradients
            if ((scrollBar->activeSubControls & SC_ScrollBarAddLine) && sunken) {
              drawGradient(painter,
                           fillRect,
                           gradientStopColor.darker(120), gradientStopColor.darker(120),
                           horizontal ? TopDown: FromLeft,
                           palette.button());
            } else {
              drawGradient(painter,
                           fillRect,
                           gradientStartColor.lighter(105), gradientStopColor,
                           horizontal ? TopDown : FromLeft,
                           palette.button());
            }
          }
          // Details
#if 0
          QImage addButton;

          if (horizontal) {
            addButton = QImage(reverse ? qt_scrollbar_button_left : qt_scrollbar_button_right);
          }
          else {
            addButton = QImage(qt_scrollbar_button_down);
          }

          addButton.setColor(1, alphaCornerColor.rgba());
          addButton.setColor(2, darkOutline.rgba());

          if ((scrollBar->activeSubControls & SC_ScrollBarAddLine) && sunken) {
            addButton.setColor(3, gradientStopColor.darker(140).rgba());
            addButton.setColor(4, gradientStopColor.darker(120).rgba());
          }
          else {
            addButton.setColor(3, gradientStartColor.lighter(105).rgba());
            addButton.setColor(4, gradientStopColor.rgba());
          }

          addButton.setColor(5, scrollBar->palette.text().color().rgba());

          painter->drawImage(pixmapRect, addButton);
#else
          painter->setPen(darkOutline);

          painter->drawRoundedRect(pixmapRect, 3, 3);
#endif

          // Arrow
          PrimitiveElement arrow;

          if (option->state & State_Horizontal)
            arrow = (option->direction == Qt::LeftToRight ?
              PE_IndicatorArrowRight : PE_IndicatorArrowLeft);
          else
            arrow = PE_IndicatorArrowDown;

          auto arrowOpt = *option;

          arrowOpt.rect = scrollBarAddLine.adjusted(3, 3, -2, -2);

          proxy()->drawPrimitive(arrow, &arrowOpt, painter, widget);
        }
      }

      painter->restore();

      break;
      //return QProxyStyle::drawComplexControl(control, option, painter, widget);
    }
    default: {
      return QProxyStyle::drawComplexControl(control, option, painter, widget);
    }
  }
}

void
CQStyle::
drawControl(ControlElement element, const QStyleOption *option, QPainter *painter,
            const QWidget *widget) const
{
  return QProxyStyle::drawControl(element, option, painter, widget);
}

void
CQStyle::
drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const
{
  return QProxyStyle::drawItemPixmap(painter, rect, alignment, pixmap);
}

void
CQStyle::
drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled,
             const QString &text, QPalette::ColorRole textRole) const
{
  return QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, text, textRole);
}

void
CQStyle::
drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
              const QWidget *widget) const
{
  // TODO: PE_PanelButtonCommand for button (with arrow pixmap)
  auto rect  = option->rect;
  uint state = option->state;

  auto base          = option->palette.base().color();
  auto button        = option->palette.button().color();
  auto text          = option->palette.text().color();
  auto buttonShadow  = (theme_ == Theme::DARK ? mergedColors(base, text) : button.darker(120));
  auto highlight     = option->palette.highlight().color();
  auto darkHighlight = highlight.darker(110);

  switch (element) {
    case PE_FrameGroupBox: {
#if QT_VERSION >= 0x050000
      if (const auto *frame = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
        if (frame->features & QStyleOptionFrame::Flat) {
          auto fr = frame->rect;

          QPoint p1(fr.x(), fr.y() + 1);
          QPoint p2(fr.x() + fr.width(), p1.y());

          qDrawShadeLine(painter, p1, p2, frame->palette, true,
                         frame->lineWidth, frame->midLineWidth);
        }
        else {
          qDrawShadeRect(painter, frame->rect.x(), frame->rect.y(), frame->rect.width(),
                         frame->rect.height(), frame->palette, true,
                         frame->lineWidth, frame->midLineWidth);
        }
      }
#else
      if (const auto *frame = qstyleoption_cast<const QStyleOptionFrameV2 *>(option)) {
        if (frame->features & QStyleOptionFrameV2::Flat) {
          auto fr = frame->rect;

          QPoint p1(fr.x(), fr.y() + 1);
          QPoint p2(fr.x() + fr.width(), p1.y());

          qDrawShadeLine(painter, p1, p2, frame->palette, true,
                         frame->lineWidth, frame->midLineWidth);
        }
        else {
          qDrawShadeRect(painter, frame->rect.x(), frame->rect.y(), frame->rect.width(),
                         frame->rect.height(), frame->palette, true,
                         frame->lineWidth, frame->midLineWidth);
        }
      }
#endif

      break;
    }
    case PE_IndicatorRadioButton: {
      int b = int(rect.width()*0.1);
      int w = int(rect.width()/3.0);

      //---

      auto oRect = rect.adjusted(b, b, -b, -b);

      QPainterPath opath;

      opath.addEllipse(oRect);

      //---

      auto c = rect.center();
      auto d = QPointF(w/2, w/2);

      auto iRect = QRectF(c - d, c + d);

      QPainterPath ipath;

      ipath.addEllipse(iRect);

      //---

      painter->save();
      painter->setRenderHint(QPainter::Antialiasing, true);

      if      (state & State_On) {
        painter->fillPath  (opath, QBrush(highlight    ));
        painter->strokePath(opath, QPen  (buttonShadow ));
        painter->fillPath  (ipath, QBrush(base         ));
        painter->strokePath(ipath, QPen  (darkHighlight));
      }
      else if (state & State_Sunken) {
        painter->fillPath  (opath, QBrush(button       ));
        painter->strokePath(opath, QPen  (buttonShadow ));
        painter->fillPath  (ipath, QBrush(base         ));
        painter->strokePath(ipath, QPen  (buttonShadow ));
      }
      else {
        painter->fillPath  (opath, QBrush(base        ));
        painter->strokePath(opath, QPen  (buttonShadow));
      }

      painter->restore();

      break;
    }
    case PE_IndicatorCheckBox: {
      int ob = int(rect.width()*0.10);
      int ib = int(rect.width()*0.25);

      //---

      auto oRect = rect.adjusted(ob, ob, -ob, -ob);
      QPainterPath opath;
      opath.addRect(oRect);

      //---

      auto iRect = rect.adjusted(ib, ib, -ib, -ib);

      //---

      painter->save();
      painter->setRenderHint(QPainter::Antialiasing, true);

      if      (state & State_On) {
        painter->fillPath  (opath, QBrush(highlight    ));
        painter->strokePath(opath, QPen  (buttonShadow ));

        drawCheck(painter, iRect, QBrush(base));
      }
      else if (state & State_Sunken) {
        painter->fillPath  (opath, QBrush(button       ));
        painter->strokePath(opath, QPen  (buttonShadow ));

        drawCheck(painter, iRect, QBrush(base));
      }
      else {
        painter->fillPath  (opath, QBrush(base        ));
        painter->strokePath(opath, QPen  (buttonShadow));
      }

      painter->restore();

      break;
    }
    case PE_IndicatorArrowUp:
    case PE_IndicatorArrowDown:
    case PE_IndicatorArrowRight:
    case PE_IndicatorArrowLeft: {
      auto r = option->rect;

      int size = qMin(r.height(), r.width());
      if (size <= 1) break;

      painter->save();

      painter->setRenderHint(QPainter::Antialiasing, true);

      double xc = r.center().x();
      double yc = r.center().y();
      double x1 = xc - size/2.0;
      double y1 = yc - size/2.0;
      double x2 = x1 + size;
      double y2 = y1 + size;

      QPainterPath path;

      if      (element == PE_IndicatorArrowUp) {
        path.moveTo(xc, y1);
        path.lineTo(x1, y2);
        path.lineTo(x2, y2);
      }
      else if (element == PE_IndicatorArrowDown) {
        path.moveTo(xc, y2);
        path.lineTo(x1, y1);
        path.lineTo(x2, y1);
      }
      else if (element == PE_IndicatorArrowRight) {
        path.moveTo(x2, yc);
        path.lineTo(x1, y1);
        path.lineTo(x1, y2);
      }
      else if (element == PE_IndicatorArrowLeft) {
        path.moveTo(x1, yc);
        path.lineTo(x2, y1);
        path.lineTo(x2, y2);
      }

      path.closeSubpath();

      auto button = option->palette.button().color();

      QColor dark;
      dark.setHsv(button.hue(),
                  qMin(255, static_cast<int>(button.saturation()*1.9)),
                  qMin(255, static_cast<int>(button.value     ()*0.4)));

      QColor darkOutline;
      darkOutline.setHsv(button.hue(),
                         qMin(255, static_cast<int>(button.saturation()*3.0)),
                         qMin(255, static_cast<int>(button.value     ()*0.6)));

      painter->fillPath(path, QBrush(dark));

      painter->restore();

      break;
    }
    default: {
      return QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
  }
}

QPixmap
CQStyle::
generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *opt) const
{
  return QProxyStyle::generatedIconPixmap(iconMode, pixmap, opt);
}

QStyle::SubControl
CQStyle::
hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                      const QPoint &pos, const QWidget *widget) const
{
  SubControl sc = SC_None;

  switch (control) {
    case CC_GroupBox: {
      if (const auto *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
        QRect r;

        uint ctrl = SC_GroupBoxCheckBox;

        while (ctrl <= SC_GroupBoxFrame) {
          r = proxy()->subControlRect(control, groupBox, QStyle::SubControl(ctrl), widget);

          if (r.isValid() && r.contains(pos)) {
            sc = QStyle::SubControl(ctrl);
            break;
          }

          ctrl <<= 1;
        }
      }

      break;
    }
    default: {
      sc = QProxyStyle::hitTestComplexControl(control, option, pos, widget);
      break;
    }
  }

  return sc;
}

QRect
CQStyle::
itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const
{
  return QProxyStyle::itemPixmapRect(r, flags, pixmap);
}

QRect
CQStyle::
itemTextRect(const QFontMetrics &fm, const QRect &r, int flags, bool enabled,
             const QString &text) const
{
  return QProxyStyle::itemTextRect(fm, r, flags, enabled, text);
}

int
CQStyle::
pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
  int fh = (option ? option->fontMetrics.height() : 16);

  switch (metric) {
    case PM_SmallIconSize:
    case PM_ListViewIconSize:
    case PM_IconViewIconSize:
    case PM_TabBarIconSize:
    case PM_ButtonIconSize:
      return fh + 2;
    case PM_LargeIconSize:
    case PM_ToolBarIconSize:
    case PM_MessageBoxIconSize:
      return int(fh*1.5 + 2);
    case PM_IndicatorWidth:
    case PM_IndicatorHeight:
      return fh;
    default:
      return QProxyStyle::pixelMetric(metric, option, widget);
  }
}

#if 0
void
CQStyle::
polish(QWidget *widget)
{
  return QProxyStyle::polish(widget);
}

void
CQStyle::
polish(QPalette &pal)
{
  return QProxyStyle::polish(pal);
}

void
CQStyle::
polish(QApplication *app)
{
  return QProxyStyle::polish(app);
}
#endif

QSize
CQStyle::
sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size,
                 const QWidget *widget) const
{
  QSize sz;

  switch (type) {
    case CT_GroupBox: {
      if (const auto *grb = qobject_cast<const QGroupBox *>(widget))
        sz += QSize(grb && !grb->isFlat() ? 16 : 0, 0);
      break;
    }
    default: {
      sz = QProxyStyle::sizeFromContents(type, option, size, widget);
      break;
    }
  }

  return sz;
}

QPixmap
CQStyle::
standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt, const QWidget *widget) const
{
  return QProxyStyle::standardPixmap(standardPixmap, opt, widget);
}

int
CQStyle::
styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget,
          QStyleHintReturn *returnData) const
{
  int ret = 0;

  switch (hint) {
    case SH_GroupBox_TextLabelVerticalAlignment:
      ret = Qt::AlignVCenter;
      break;
    case SH_GroupBox_TextLabelColor:
      ret = option ? int(option->palette.color(QPalette::Text).rgba()) : 0;
      break;
    default:
      ret = QProxyStyle::styleHint(hint, option, widget, returnData);
      break;
  }

  return ret;
}

QRect
CQStyle::
subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl sc,
               const QWidget *widget) const
{
  QRect ret;

  switch (control) {
    case CC_GroupBox: {
      if (const auto *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
        switch (sc) {
          case SC_GroupBoxFrame:
            // FALL THROUGH
          case SC_GroupBoxContents: {
            int topMargin         = 0;
            int topHeight         = 0;
            int verticalAlignment =
              proxy()->styleHint(SH_GroupBox_TextLabelVerticalAlignment, groupBox, widget);

            if (groupBox->text.size() || (groupBox->subControls & QStyle::SC_GroupBoxCheckBox)) {
              topHeight = groupBox->fontMetrics.height();

              if      (verticalAlignment & Qt::AlignVCenter)
                topMargin = topHeight / 2;
              else if (verticalAlignment & Qt::AlignTop)
                topMargin = topHeight;
            }

            auto frameRect = groupBox->rect;
            frameRect.setTop(topMargin);

            if (sc == SC_GroupBoxFrame) {
              ret = frameRect;
              break;
            }

            int frameWidth = 0;

#if 0
            if ((groupBox->features & QStyleOptionFrameV2::Flat) == 0)
              frameWidth = proxy()->pixelMetric(PM_DefaultFrameWidth, groupBox, widget);
#else
            if ((groupBox->features & QStyleOptionFrame::Flat) == 0)
              frameWidth = proxy()->pixelMetric(PM_DefaultFrameWidth, groupBox, widget);
#endif

            ret = frameRect.adjusted(frameWidth, frameWidth + topHeight - topMargin,
                                     -frameWidth, -frameWidth);

            break;
          }
          case SC_GroupBoxCheckBox:
            // FALL THROUGH
          case SC_GroupBoxLabel: {
            auto fontMetrics = groupBox->fontMetrics;

            int h  = fontMetrics.height();
            int tw = fontMetrics.size(Qt::TextShowMnemonic,
                                      groupBox->text + QLatin1Char(' ')).width();
#if 0
            int marg = (groupBox->features & QStyleOptionFrameV2::Flat ? 0 : 8);
#else
            int marg = (groupBox->features & QStyleOptionFrame::Flat ? 0 : 8);
#endif

            ret = groupBox->rect.adjusted(marg, 0, -marg, 0);
            ret.setHeight(h);

            int indicatorWidth = proxy()->pixelMetric(PM_IndicatorWidth, option, widget);
            int indicatorSpace = proxy()->pixelMetric(PM_CheckBoxLabelSpacing, option, widget) - 1;

            bool hasCheckBox  = groupBox->subControls & QStyle::SC_GroupBoxCheckBox;
            int  checkBoxSize = hasCheckBox ? (indicatorWidth + indicatorSpace) : 0;

            // Adjusted rect for label + indicatorWidth + indicatorSpace
            auto totalRect = alignedRect(groupBox->direction, groupBox->textAlignment,
                                         QSize(tw + checkBoxSize, h), ret);

            // Adjust totalRect if checkbox is set
            if (hasCheckBox) {
              bool ltr  = (groupBox->direction == Qt::LeftToRight);
              int  left = 0;

              // Adjust for check box
              if (sc == SC_GroupBoxCheckBox) {
                int indicatorHeight = proxy()->pixelMetric(PM_IndicatorHeight, option, widget);

                left = ltr ? totalRect.left() : (totalRect.right() - indicatorWidth);

                int top = totalRect.top() + qMax(0, fontMetrics.height() - indicatorHeight) / 2;

                totalRect.setRect(left, top, indicatorWidth, indicatorHeight);
                // Adjust for label
              }
              else {
                left = ltr ? (totalRect.left() + checkBoxSize - 2) : totalRect.left();

                totalRect.setRect(left, totalRect.top(),
                                  totalRect.width() - checkBoxSize, totalRect.height());
              }
            }

            ret = totalRect;

            break;
          }
          default:
            break;
        }
      }
      break;
    }
    default: {
      ret = QProxyStyle::subControlRect(control, option, sc, widget);
      break;
    }
  }

  return ret;
}

#if 0
QRect
CQStyle::
subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
  return QProxyStyle::subElementRect(element, option, widget);
}

void
CQStyle::
unpolish(QWidget *widget)
{
  return QProxyStyle::unpolish(widget);
}

void
CQStyle::
unpolish(QApplication *app)
{
  return QProxyStyle::unpolish(app);
}
#endif

void
CQStyle::
drawCheck(QPainter *painter, const QRectF &r, const QBrush &b) const
{
  double xo = r.left();
  double yo = r.top();
  double xs = r.width ()/100.0;
  double ys = r.height()/100.0;

  auto mapf = [&](double x, double y) -> QPointF {
    return QPointF(xo + x*xs, yo + (100 - y)*ys);
  };

  QPainterPath path;

  path.moveTo(mapf(  0,  32));
  path.lineTo(mapf( 36,   0));
  path.lineTo(mapf( 49,   0));
  path.lineTo(mapf(100,  92));
  path.lineTo(mapf( 92, 100));
  path.lineTo(mapf( 42,  37));
  path.lineTo(mapf( 26,  61));

  path.closeSubpath();

  painter->fillPath(path, b);
}

QPalette
CQStyle::
standardPalette() const
{
  return palette_;
}
