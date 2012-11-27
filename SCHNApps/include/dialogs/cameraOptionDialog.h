#ifndef _CAMERAOPTIONDIALOG_H_
#define _CAMERAOPTIONDIALOG_H_


#include "ui_cameraOptionDialog.h"

class Camera;

class CameraOptionDialog: public QDialog, Ui::CODialog{
	Q_OBJECT
public:
	CameraOptionDialog(Camera* camera, QWidget* parent);
	~CameraOptionDialog(){}

public slots:
	void drawChecked(bool b);
	void drawPathChecked(bool b);

	void accept();

protected:
	Camera* m_camera;
};


#endif
