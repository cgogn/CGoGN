#ifndef _COMPUTECURVATURE_DIALOG_H_
#define _COMPUTECURVATURE_DIALOG_H_

#include "ui_computeCurvatureDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;

class ComputeCurvatureDialog : public QDialog, public Ui::ComputeCurvatureDialog
{
	Q_OBJECT

public:
	ComputeCurvatureDialog(Window* w);
	void init();

public slots:
	void cb_selectedMapChanged();

private:
	Window* m_window;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
