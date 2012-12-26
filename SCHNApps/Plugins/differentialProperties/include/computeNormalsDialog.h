#ifndef _COMPUTENORMALS_DIALOG_H_
#define _COMPUTENORMALS_DIALOG_H_

#include "ui_computeNormalsDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;

class ComputeNormalsDialog : public QDialog, public Ui::ComputeNormalsDialog
{
	Q_OBJECT

public:
	ComputeNormalsDialog(Window* w);
	void init();

public slots:
	void cb_selectedMapChanged();

private:
	Window* m_window;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
