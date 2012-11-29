#ifndef _CAMERADIALOG_H_
#define _CAMERADIALOG_H_

#include "ui_cameraDialog.h"

class Window;

class CameraDialog : public QDialog, Ui::CameraDialog
{
	Q_OBJECT

public:
	CameraDialog(Window* window);
	~CameraDialog();

	void addCameraToList(const QString& name);
	void addViewToList(const QString& name);

private:
	Window* m_window;

public slots:
	void cb_addCamera();
	void cb_removeCamera();
};

#endif
