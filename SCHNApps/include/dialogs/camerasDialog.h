#ifndef _CAMERASDIALOG_H_
#define _CAMERASDIALOG_H_

#include "ui_camerasDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class Camera;

class CamerasDialog : public QDialog, Ui::CamerasDialog
{
	Q_OBJECT

public:
	CamerasDialog(Window* window);
	~CamerasDialog();

private:
	Window* m_window;

public slots:
	void cb_addCamera();
	void cb_removeCameras();
	void cb_addCameraToList(Camera* c);
	void cb_removeCameraFromList(Camera* c);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
