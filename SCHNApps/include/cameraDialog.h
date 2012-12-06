#ifndef _CAMERADIALOG_H_
#define _CAMERADIALOG_H_

#include "ui_cameraDialog.h"

class Window;
class Camera;

class CameraDialog : public QDialog, Ui::CameraDialog
{
	Q_OBJECT

public:
	CameraDialog(Window* window);
	~CameraDialog();

private:
	Window* m_window;

public slots:
	void cb_addCamera();
	void cb_removeCamera();
	void cb_addCameraToList(Camera* c);
	void cb_removeCameraFromList(Camera* c);
};

#endif
