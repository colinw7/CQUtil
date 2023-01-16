#ifndef CQUTIL_EVENT_H
#define CQUTIL_EVENT_H

#include <CEvent.h>

#include<QMouseEvent>

class QKeyEvent;

namespace CQUtil {

CMouseEvent *convertEvent(QMouseEvent *event);
CKeyEvent   *convertEvent(QKeyEvent *event);

CMouseButton   convertButton(Qt::MouseButton button);
CKeyType       convertKey(int key, Qt::KeyboardModifiers modifiers);
CEventModifier convertModifier(Qt::KeyboardModifiers modifiers);

}

#endif
