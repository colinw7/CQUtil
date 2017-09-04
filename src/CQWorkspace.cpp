#include <CQWorkspace.h>

CQWorkspaceUpdateWindowsMenuObject::
CQWorkspaceUpdateWindowsMenuObject(CQMenu *menu, CQWorkspaceUpdateWindowsMenuIFace *iface) :
 iface_(iface)
{
  connect(menu->getMenu(), SIGNAL(aboutToShow()), this, SLOT(updateWindowsMenu()));
}

void
CQWorkspaceUpdateWindowsMenuObject::
updateWindowsMenu()
{
  iface_->exec();
}
