#ifndef _CAMERAVIEWDIALOG_H_
#define _CAMERAVIEWDIALOG_H_

#include "ui_cameraViewDialog.h"

class Window;
class View;

class CameraViewDialog : public QDialog, Ui::CameraViewDialog
{
	Q_OBJECT

public:
	CameraViewDialog(Window* window, View* view);
	~CameraViewDialog();

private:
	Window* m_window;
	View* m_view;

public slots:
	void cb_selectedCameraChanged(QListWidgetItem* current, QListWidgetItem* previous);
};

#endif
