#ifndef _COMPUTENORMAL_DIALOG_H_
#define _COMPUTENORMAL_DIALOG_H_

#include "ui_computeNormalDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;

class ComputeNormalDialog : public QDialog, public Ui::ComputeNormalDialog
{
	Q_OBJECT

public:
	ComputeNormalDialog(Window* w);
	void init();

public slots:
	void cb_selectedMapChanged();

private:
	Window* m_window;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
