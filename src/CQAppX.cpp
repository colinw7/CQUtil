#include <std_Xt.h>

bool
CQAppXIsObjEditKey(void *e)
{
  XEvent *event = reinterpret_cast<XEvent *>(e);

  if (event->xany.type == KeyPress) {
    static int last_keycode = 0;

    auto *kevent = reinterpret_cast<XKeyPressedEvent *>(event);

    // Control + Home
    if (last_keycode == 37 && kevent->keycode == 110)
      return true;

    last_keycode = int(kevent->keycode);
  }

  return false;
}
