#include <CQMainWindow.h>

CQMainWindow::
CQMainWindow(const QString &title) :
 title_(title)
{
  setObjectName(title);
}

void
CQMainWindow::
init()
{
  auto *w = createCentralWidget();

  if (w)
    setCentralWidget(w);

  createWorkspace  ();
  createMenus      ();
  createToolBars   ();
  createStatusBar  ();
  createDockWindows();

  setWindowTitle(title_);

  initTerm();
}
