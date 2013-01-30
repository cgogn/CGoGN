#ifndef _CAMERASDIALOG_H_
#define _CAMERASDIALOG_H_

#include "ui_camerasDialog.h"
#include "view.h"

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
	bool b_refreshingUI;

public slots:
	void setCameraType(const QString& name, qglviewer::Camera::Type type);
	void setDrawCamera(const QString& name, bool b);
	void setDrawCameraPath(const QString& name, bool b);

	void cb_addCamera();
	void cb_removeCameras();
	void cb_addCameraToList(Camera* c);
	void cb_removeCameraFromList(Camera* c);
	void cb_itemChanged(QTreeWidgetItem* item, int column);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
