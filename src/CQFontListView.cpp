#include <CQFontListView.h>

CQFontListView::
CQFontListView(QWidget *parent) :
QListView(parent)
{
  model_ = new QStringListModel(parent);

  setModel(model_);

  setEditTriggers(NoEditTriggers);
}

CQFontListView::
~CQFontListView()
{
  delete model_;
}
