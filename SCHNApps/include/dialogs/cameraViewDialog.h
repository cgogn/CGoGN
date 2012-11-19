#ifndef _CAMERAVIEWDIALOG_H_
#define _CAMERAVIEWDIALOG_H_


#include "ui_cameraViewDialog.h"

class View;
class Camera;

class CameraViewDialog : public QDialog, Ui::CVDialog{
	Q_OBJECT
public:
	CameraViewDialog(View* view, QWidget* parent=0);
	~CameraViewDialog();

protected:
	View* view;

protected slots:
	void addCamera();
	void removeCamera();
	void changeCurrentCamera(QListWidgetItem * current, QListWidgetItem * previous);
	void cameraNameChange(QListWidgetItem * item);
	void settingsCamera();

private:
	bool autoText;
};


#endif
