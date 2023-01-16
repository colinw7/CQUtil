#include <CQUtilEvent.h>
#include <CEvent.h>

CMouseEvent *
CQUtil::
convertEvent(QMouseEvent *event)
{
  static CMouseEvent event1;

  event1 = CMouseEvent(CIPoint2D(event->x(), event->y()), convertButton(event->button()),
                       1, convertModifier(event->modifiers()));

  return &event1;
}

CKeyEvent *
CQUtil::
convertEvent(QKeyEvent *event)
{
  static CKeyEvent event1;

  CIPoint2D position;

  event1 = CKeyEvent(position, convertKey(event->key(), event->modifiers()),
                     event->text().toStdString(), convertModifier(event->modifiers()));

  return &event1;
}

CMouseButton
CQUtil::
convertButton(Qt::MouseButton button)
{
  switch (button) {
    case Qt::NoButton:     return CBUTTON_NONE;
    case Qt::LeftButton:   return CBUTTON_LEFT;
    case Qt::MiddleButton: return CBUTTON_MIDDLE;
    case Qt::RightButton:  return CBUTTON_RIGHT;
    case Qt::XButton1:     return CBUTTON_NONE;
    case Qt::XButton2:     return CBUTTON_NONE;
    default:               return CBUTTON_NONE;
  }
}

CKeyType
CQUtil::
convertKey(int key, Qt::KeyboardModifiers modifiers)
{
  if (key >= Qt::Key_A && key <= Qt::Key_Z) {
    if (modifiers & Qt::ShiftModifier)
      return CKeyType(CKEY_TYPE_A + (key - Qt::Key_A));
    else
      return CKeyType(CKEY_TYPE_a + (key - Qt::Key_A));
  }

  switch (key) {
    case Qt::Key_Escape:                return CKEY_TYPE_Escape;
    case Qt::Key_Tab:                   return CKEY_TYPE_Tab;
    case Qt::Key_Backtab:               return CKEY_TYPE_Backtab;
    case Qt::Key_Backspace:             return CKEY_TYPE_BackSpace;
    case Qt::Key_Return:                return CKEY_TYPE_Return;
    case Qt::Key_Enter:                 return CKEY_TYPE_KP_Enter;
    case Qt::Key_Insert:                return CKEY_TYPE_Insert;
    case Qt::Key_Delete:                return CKEY_TYPE_DEL;
    case Qt::Key_Pause:                 return CKEY_TYPE_Pause;
    case Qt::Key_Print:                 return CKEY_TYPE_Print;
    case Qt::Key_SysReq:                return CKEY_TYPE_Sys_Req;
    case Qt::Key_Clear:                 return CKEY_TYPE_Clear;
    case Qt::Key_Home:                  return CKEY_TYPE_Home;
    case Qt::Key_End:                   return CKEY_TYPE_End;
    case Qt::Key_Left:                  return CKEY_TYPE_Left;
    case Qt::Key_Up:                    return CKEY_TYPE_Up;
    case Qt::Key_Right:                 return CKEY_TYPE_Right;
    case Qt::Key_Down:                  return CKEY_TYPE_Down;
    case Qt::Key_PageUp:                return CKEY_TYPE_Page_Up;
    case Qt::Key_PageDown:              return CKEY_TYPE_Page_Down;
    case Qt::Key_Shift:                 return CKEY_TYPE_Shift_L;
    case Qt::Key_Control:               return CKEY_TYPE_Control_L;
    case Qt::Key_Meta:                  return CKEY_TYPE_Meta_L;
    case Qt::Key_Alt:                   return CKEY_TYPE_Alt_L;
    case Qt::Key_AltGr:                 return CKEY_TYPE_Super_L;
    case Qt::Key_CapsLock:              return CKEY_TYPE_Caps_Lock;
    case Qt::Key_NumLock:               return CKEY_TYPE_Num_Lock;
    case Qt::Key_ScrollLock:            return CKEY_TYPE_Scroll_Lock;
    case Qt::Key_F1:                    return CKEY_TYPE_F1;
    case Qt::Key_F2:                    return CKEY_TYPE_F2;
    case Qt::Key_F3:                    return CKEY_TYPE_F3;
    case Qt::Key_F4:                    return CKEY_TYPE_F4;
    case Qt::Key_F5:                    return CKEY_TYPE_F5;
    case Qt::Key_F6:                    return CKEY_TYPE_F6;
    case Qt::Key_F7:                    return CKEY_TYPE_F7;
    case Qt::Key_F8:                    return CKEY_TYPE_F8;
    case Qt::Key_F9:                    return CKEY_TYPE_F9;
    case Qt::Key_F10:                   return CKEY_TYPE_F10;
    case Qt::Key_F11:                   return CKEY_TYPE_F11;
    case Qt::Key_F12:                   return CKEY_TYPE_F12;
    case Qt::Key_F13:                   return CKEY_TYPE_F13;
    case Qt::Key_F14:                   return CKEY_TYPE_F14;
    case Qt::Key_F15:                   return CKEY_TYPE_F15;
    case Qt::Key_F16:                   return CKEY_TYPE_F16;
    case Qt::Key_F17:                   return CKEY_TYPE_F17;
    case Qt::Key_F18:                   return CKEY_TYPE_F18;
    case Qt::Key_F19:                   return CKEY_TYPE_F19;
    case Qt::Key_F20:                   return CKEY_TYPE_F20;
    case Qt::Key_F21:                   return CKEY_TYPE_F21;
    case Qt::Key_F22:                   return CKEY_TYPE_F22;
    case Qt::Key_F23:                   return CKEY_TYPE_F23;
    case Qt::Key_F24:                   return CKEY_TYPE_F24;
    case Qt::Key_F25:                   return CKEY_TYPE_F25;
    case Qt::Key_F26:                   return CKEY_TYPE_F26;
    case Qt::Key_F27:                   return CKEY_TYPE_F27;
    case Qt::Key_F28:                   return CKEY_TYPE_F28;
    case Qt::Key_F29:                   return CKEY_TYPE_F29;
    case Qt::Key_F30:                   return CKEY_TYPE_F30;
    case Qt::Key_F31:                   return CKEY_TYPE_F31;
    case Qt::Key_F32:                   return CKEY_TYPE_F32;
    case Qt::Key_F33:                   return CKEY_TYPE_F33;
    case Qt::Key_F34:                   return CKEY_TYPE_F34;
    case Qt::Key_F35:                   return CKEY_TYPE_F35;
    case Qt::Key_Super_L:               return CKEY_TYPE_Super_L;
    case Qt::Key_Super_R:               return CKEY_TYPE_Super_R;
    case Qt::Key_Menu:                  return CKEY_TYPE_Menu;
    case Qt::Key_Hyper_L:               return CKEY_TYPE_Hyper_L;
    case Qt::Key_Hyper_R:               return CKEY_TYPE_Hyper_R;
    case Qt::Key_Help:                  return CKEY_TYPE_Help;
    case Qt::Key_Direction_L:           return CKEY_TYPE_NUL;
    case Qt::Key_Direction_R:           return CKEY_TYPE_NUL;
    case Qt::Key_Space:                 return CKEY_TYPE_Space;
    case Qt::Key_Exclam:                return CKEY_TYPE_Exclam;
    case Qt::Key_QuoteDbl:              return CKEY_TYPE_QuoteDbl;
    case Qt::Key_NumberSign:            return CKEY_TYPE_NumberSign;
    case Qt::Key_Dollar:                return CKEY_TYPE_Dollar;
    case Qt::Key_Percent:               return CKEY_TYPE_Percent;
    case Qt::Key_Ampersand:             return CKEY_TYPE_Ampersand;
    case Qt::Key_Apostrophe:            return CKEY_TYPE_Apostrophe;
    case Qt::Key_ParenLeft:             return CKEY_TYPE_ParenLeft;
    case Qt::Key_ParenRight:            return CKEY_TYPE_ParenRight;
    case Qt::Key_Asterisk:              return CKEY_TYPE_Asterisk;
    case Qt::Key_Plus:                  return CKEY_TYPE_Plus;
    case Qt::Key_Comma:                 return CKEY_TYPE_Comma;
    case Qt::Key_Minus:                 return CKEY_TYPE_Minus;
    case Qt::Key_Period:                return CKEY_TYPE_Period;
    case Qt::Key_Slash:                 return CKEY_TYPE_Slash;
    case Qt::Key_0:                     return CKEY_TYPE_0;
    case Qt::Key_1:                     return CKEY_TYPE_1;
    case Qt::Key_2:                     return CKEY_TYPE_2;
    case Qt::Key_3:                     return CKEY_TYPE_3;
    case Qt::Key_4:                     return CKEY_TYPE_4;
    case Qt::Key_5:                     return CKEY_TYPE_5;
    case Qt::Key_6:                     return CKEY_TYPE_6;
    case Qt::Key_7:                     return CKEY_TYPE_7;
    case Qt::Key_8:                     return CKEY_TYPE_8;
    case Qt::Key_9:                     return CKEY_TYPE_9;
    case Qt::Key_Colon:                 return CKEY_TYPE_Colon;
    case Qt::Key_Semicolon:             return CKEY_TYPE_Semicolon;
    case Qt::Key_Less:                  return CKEY_TYPE_Less;
    case Qt::Key_Equal:                 return CKEY_TYPE_Equal;
    case Qt::Key_Greater:               return CKEY_TYPE_Greater;
    case Qt::Key_Question:              return CKEY_TYPE_Question;
    case Qt::Key_At:                    return CKEY_TYPE_At;
    case Qt::Key_BracketLeft:           return CKEY_TYPE_BracketLeft;
    case Qt::Key_Backslash:             return CKEY_TYPE_Backslash;
    case Qt::Key_BracketRight:          return CKEY_TYPE_BracketRight;
    case Qt::Key_AsciiCircum:           return CKEY_TYPE_AsciiCircum;
    case Qt::Key_Underscore:            return CKEY_TYPE_Underscore;
    case Qt::Key_QuoteLeft:             return CKEY_TYPE_QuoteLeft;
    case Qt::Key_BraceLeft:             return CKEY_TYPE_BraceLeft;
    case Qt::Key_Bar:                   return CKEY_TYPE_Bar;
    case Qt::Key_BraceRight:            return CKEY_TYPE_BraceRight;
    case Qt::Key_AsciiTilde:            return CKEY_TYPE_AsciiTilde;
    case Qt::Key_nobreakspace:          return CKEY_TYPE_nobreakspace;
    case Qt::Key_exclamdown:            return CKEY_TYPE_exclamdown;
    case Qt::Key_cent:                  return CKEY_TYPE_cent;
    case Qt::Key_sterling:              return CKEY_TYPE_sterling;
    case Qt::Key_currency:              return CKEY_TYPE_currency;
    case Qt::Key_yen:                   return CKEY_TYPE_yen;
    case Qt::Key_brokenbar:             return CKEY_TYPE_brokenbar;
    case Qt::Key_section:               return CKEY_TYPE_section;
    case Qt::Key_diaeresis:             return CKEY_TYPE_diaeresis;
    case Qt::Key_copyright:             return CKEY_TYPE_copyright;
    case Qt::Key_ordfeminine:           return CKEY_TYPE_ordfeminine;
    case Qt::Key_guillemotleft:         return CKEY_TYPE_guillemotleft;
    case Qt::Key_notsign:               return CKEY_TYPE_notsign;
    case Qt::Key_hyphen:                return CKEY_TYPE_hyphen;
    case Qt::Key_registered:            return CKEY_TYPE_registered;
    case Qt::Key_macron:                return CKEY_TYPE_macron;
    case Qt::Key_degree:                return CKEY_TYPE_degree;
    case Qt::Key_plusminus:             return CKEY_TYPE_plusminus;
    case Qt::Key_twosuperior:           return CKEY_TYPE_twosuperior;
    case Qt::Key_threesuperior:         return CKEY_TYPE_threesuperior;
    case Qt::Key_acute:                 return CKEY_TYPE_acute;
    case Qt::Key_mu:                    return CKEY_TYPE_mu;
    case Qt::Key_paragraph:             return CKEY_TYPE_paragraph;
    case Qt::Key_periodcentered:        return CKEY_TYPE_periodcentered;
    case Qt::Key_cedilla:               return CKEY_TYPE_cedilla;
    case Qt::Key_onesuperior:           return CKEY_TYPE_onesuperior;
    case Qt::Key_masculine:             return CKEY_TYPE_masculine;
    case Qt::Key_guillemotright:        return CKEY_TYPE_guillemotright;
    case Qt::Key_onequarter:            return CKEY_TYPE_onequarter;
    case Qt::Key_onehalf:               return CKEY_TYPE_onehalf;
    case Qt::Key_threequarters:         return CKEY_TYPE_threequarters;
    case Qt::Key_questiondown:          return CKEY_TYPE_questiondown;
    case Qt::Key_Agrave:                return CKEY_TYPE_Agrave;
    case Qt::Key_Aacute:                return CKEY_TYPE_Aacute;
    case Qt::Key_Acircumflex:           return CKEY_TYPE_Acircumflex;
    case Qt::Key_Atilde:                return CKEY_TYPE_Atilde;
    case Qt::Key_Adiaeresis:            return CKEY_TYPE_Adiaeresis;
    case Qt::Key_Aring:                 return CKEY_TYPE_Aring;
    case Qt::Key_AE:                    return CKEY_TYPE_AE;
    case Qt::Key_Ccedilla:              return CKEY_TYPE_Ccedilla;
    case Qt::Key_Egrave:                return CKEY_TYPE_Egrave;
    case Qt::Key_Eacute:                return CKEY_TYPE_Eacute;
    case Qt::Key_Ecircumflex:           return CKEY_TYPE_Ecircumflex;
    case Qt::Key_Ediaeresis:            return CKEY_TYPE_Ediaeresis;
    case Qt::Key_Igrave:                return CKEY_TYPE_Igrave;
    case Qt::Key_Iacute:                return CKEY_TYPE_Iacute;
    case Qt::Key_Icircumflex:           return CKEY_TYPE_Icircumflex;
    case Qt::Key_Idiaeresis:            return CKEY_TYPE_Idiaeresis;
    case Qt::Key_ETH:                   return CKEY_TYPE_ETH;
    case Qt::Key_Ntilde:                return CKEY_TYPE_Ntilde;
    case Qt::Key_Ograve:                return CKEY_TYPE_Ograve;
    case Qt::Key_Oacute:                return CKEY_TYPE_Oacute;
    case Qt::Key_Ocircumflex:           return CKEY_TYPE_Ocircumflex;
    case Qt::Key_Otilde:                return CKEY_TYPE_Otilde;
    case Qt::Key_Odiaeresis:            return CKEY_TYPE_Odiaeresis;
    case Qt::Key_multiply:              return CKEY_TYPE_multiply;
    case Qt::Key_Ooblique:              return CKEY_TYPE_Ooblique;
    case Qt::Key_Ugrave:                return CKEY_TYPE_Ugrave;
    case Qt::Key_Uacute:                return CKEY_TYPE_Uacute;
    case Qt::Key_Ucircumflex:           return CKEY_TYPE_Ucircumflex;
    case Qt::Key_Udiaeresis:            return CKEY_TYPE_Udiaeresis;
    case Qt::Key_Yacute:                return CKEY_TYPE_Yacute;
    case Qt::Key_THORN:                 return CKEY_TYPE_THORN;
    case Qt::Key_ssharp:                return CKEY_TYPE_ssharp;
    case Qt::Key_division:              return CKEY_TYPE_division;
    case Qt::Key_ydiaeresis:            return CKEY_TYPE_ydiaeresis;
    case Qt::Key_Multi_key:             return CKEY_TYPE_NUL;
    case Qt::Key_Codeinput:             return CKEY_TYPE_NUL;
    case Qt::Key_SingleCandidate:       return CKEY_TYPE_NUL;
    case Qt::Key_MultipleCandidate:     return CKEY_TYPE_NUL;
    case Qt::Key_PreviousCandidate:     return CKEY_TYPE_NUL;
    case Qt::Key_Mode_switch:           return CKEY_TYPE_Mode_switch;
    case Qt::Key_Kanji:                 return CKEY_TYPE_NUL;
    case Qt::Key_Muhenkan:              return CKEY_TYPE_NUL;
    case Qt::Key_Henkan:                return CKEY_TYPE_NUL;
    case Qt::Key_Romaji:                return CKEY_TYPE_NUL;
    case Qt::Key_Hiragana:              return CKEY_TYPE_NUL;
    case Qt::Key_Katakana:              return CKEY_TYPE_NUL;
    case Qt::Key_Hiragana_Katakana:     return CKEY_TYPE_NUL;
    case Qt::Key_Zenkaku:               return CKEY_TYPE_NUL;
    case Qt::Key_Hankaku:               return CKEY_TYPE_NUL;
    case Qt::Key_Zenkaku_Hankaku:       return CKEY_TYPE_NUL;
    case Qt::Key_Touroku:               return CKEY_TYPE_NUL;
    case Qt::Key_Massyo:                return CKEY_TYPE_NUL;
    case Qt::Key_Kana_Lock:             return CKEY_TYPE_NUL;
    case Qt::Key_Kana_Shift:            return CKEY_TYPE_NUL;
    case Qt::Key_Eisu_Shift:            return CKEY_TYPE_NUL;
    case Qt::Key_Eisu_toggle:           return CKEY_TYPE_NUL;
    case Qt::Key_Hangul:                return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Start:          return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_End:            return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Hanja:          return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Jamo:           return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Romaja:         return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Jeonja:         return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Banja:          return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_PreHanja:       return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_PostHanja:      return CKEY_TYPE_NUL;
    case Qt::Key_Hangul_Special:        return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Grave:            return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Acute:            return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Circumflex:       return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Tilde:            return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Macron:           return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Breve:            return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Abovedot:         return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Diaeresis:        return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Abovering:        return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Doubleacute:      return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Caron:            return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Cedilla:          return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Ogonek:           return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Iota:             return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Voiced_Sound:     return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Semivoiced_Sound: return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Belowdot:         return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Hook:             return CKEY_TYPE_NUL;
    case Qt::Key_Dead_Horn:             return CKEY_TYPE_NUL;
    case Qt::Key_Back:                  return CKEY_TYPE_NUL;
    case Qt::Key_Forward:               return CKEY_TYPE_NUL;
    case Qt::Key_Stop:                  return CKEY_TYPE_NUL;
    case Qt::Key_Refresh:               return CKEY_TYPE_NUL;
    case Qt::Key_VolumeDown:            return CKEY_TYPE_NUL;
    case Qt::Key_VolumeMute:            return CKEY_TYPE_NUL;
    case Qt::Key_VolumeUp:              return CKEY_TYPE_NUL;
    case Qt::Key_BassBoost:             return CKEY_TYPE_NUL;
    case Qt::Key_BassUp:                return CKEY_TYPE_NUL;
    case Qt::Key_BassDown:              return CKEY_TYPE_NUL;
    case Qt::Key_TrebleUp:              return CKEY_TYPE_NUL;
    case Qt::Key_TrebleDown:            return CKEY_TYPE_NUL;
    case Qt::Key_MediaPlay:             return CKEY_TYPE_NUL;
    case Qt::Key_MediaStop:             return CKEY_TYPE_NUL;
    case Qt::Key_MediaPrevious:         return CKEY_TYPE_NUL;
    case Qt::Key_MediaNext:             return CKEY_TYPE_NUL;
    case Qt::Key_MediaRecord:           return CKEY_TYPE_NUL;
    case Qt::Key_HomePage:              return CKEY_TYPE_NUL;
    case Qt::Key_Favorites:             return CKEY_TYPE_NUL;
    case Qt::Key_Search:                return CKEY_TYPE_NUL;
    case Qt::Key_Standby:               return CKEY_TYPE_NUL;
    case Qt::Key_OpenUrl:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchMail:            return CKEY_TYPE_NUL;
    case Qt::Key_LaunchMedia:           return CKEY_TYPE_NUL;
    case Qt::Key_Launch0:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch1:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch2:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch3:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch4:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch5:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch6:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch7:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch8:               return CKEY_TYPE_NUL;
    case Qt::Key_Launch9:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchA:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchB:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchC:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchD:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchE:               return CKEY_TYPE_NUL;
    case Qt::Key_LaunchF:               return CKEY_TYPE_NUL;
    case Qt::Key_MediaLast:             return CKEY_TYPE_NUL;
    case Qt::Key_unknown:               return CKEY_TYPE_NUL;
    case Qt::Key_Call:                  return CKEY_TYPE_NUL;
    case Qt::Key_Context1:              return CKEY_TYPE_NUL;
    case Qt::Key_Context2:              return CKEY_TYPE_NUL;
    case Qt::Key_Context3:              return CKEY_TYPE_NUL;
    case Qt::Key_Context4:              return CKEY_TYPE_NUL;
    case Qt::Key_Flip:                  return CKEY_TYPE_NUL;
    case Qt::Key_Hangup:                return CKEY_TYPE_NUL;
    case Qt::Key_No:                    return CKEY_TYPE_NUL;
    case Qt::Key_Select:                return CKEY_TYPE_Select;
    case Qt::Key_Yes:                   return CKEY_TYPE_NUL;
    default:                            return CKEY_TYPE_NUL;
  }
}

CEventModifier
CQUtil::
convertModifier(Qt::KeyboardModifiers modifiers)
{
  uint modifiers1 = CMODIFIER_NONE;

  if (modifiers & Qt::ShiftModifier  ) modifiers1 |= CMODIFIER_SHIFT;
  if (modifiers & Qt::ControlModifier) modifiers1 |= CMODIFIER_CONTROL;
  if (modifiers & Qt::AltModifier    ) modifiers1 |= CMODIFIER_ALT;
  if (modifiers & Qt::MetaModifier   ) modifiers1 |= CMODIFIER_META;
  if (modifiers & Qt::KeypadModifier ) modifiers1 |= CMODIFIER_KEYPAD;

  return static_cast<CEventModifier>(modifiers1);
}
