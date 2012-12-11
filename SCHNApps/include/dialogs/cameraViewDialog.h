#ifndef _CAMERAVIEWDIALOG_H_
#define _CAMERAVIEWDIALOG_H_

#include "ui_cameraViewDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class Camera;
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

	void selectCurrentCamera();

public slots:
	void cb_selectedCameraChanged();
	void cb_addCameraToList(Camera* c);
	void cb_removeCameraFromList(Camera* c);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
