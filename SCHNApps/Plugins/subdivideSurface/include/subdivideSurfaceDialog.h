#ifndef _SUBDIVIDESURFACE_DIALOG_H_
#define _SUBDIVIDESURFACE_DIALOG_H_

#include "ui_subdivideSurfaceDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;

class SubdivideSurfaceDialog : public QDialog, public Ui::SubdivideSurfaceDialog
{
	Q_OBJECT

public:
	SubdivideSurfaceDialog(Window* w);
	void init();

public slots:
	void cb_selectedMapChanged();

private:
	Window* m_window;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
