#ifndef _CAMERAVIEWDIALOG_H_
#define _CAMERAVIEWDIALOG_H_

#include "ui_cameraViewDialog.h"

class Window;

class CameraViewDialog : public QDialog, Ui::CameraViewDialog
{
	Q_OBJECT

public:
	CameraViewDialog(Window* window);
	~CameraViewDialog();

private:
	void updateCameraList();
	void updateViewList();

	Window* m_window;

public slots:
	void cb_addCamera();
	void cb_removeCamera();
	void cb_selectedViewChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void cb_selectedCameraChanged(QListWidgetItem* current, QListWidgetItem* previous);
};

#endif
