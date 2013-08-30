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
	bool b_refreshingUI;

	void selectCurrentCamera();

public slots:
	void selectedCameraChanged();
	void selectCamera(View* view, Camera* camera);
	void deselectCamera(View* view, Camera* camera);
	void addCameraToList(Camera* c);
	void removeCameraFromList(Camera* c);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
