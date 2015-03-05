#ifndef _CONTROL_DOCK_CAMERA_TAB_H_
#define _CONTROL_DOCK_CAMERA_TAB_H_

#include "ui_controlDock_CameraTabWidget.h"

#include "dll.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class Camera;
class View;

class SCHNAPPS_API ControlDock_CameraTab : public QWidget, public Ui::ControlDock_CameraTabWidget
{
	Q_OBJECT

public:
	ControlDock_CameraTab(SCHNApps* s);
	QString title() { return QString("Cameras"); }

private slots:
	// slots called from UI actions
	void addCameraButtonClicked();
	void selectedCameraChanged();
	void cameraProjectionChanged(QAbstractButton* b);
	void cameraDrawClicked(bool b);
	void cameraDrawPathClicked(bool b);

	// slots called from SCHNApps signals
	void cameraAdded(Camera* c);
	void cameraRemoved(Camera* c);

	// slots called from selected Camera signals
	void selectedCameraProjectionTypeChanged(int t);
	void selectedCameraDrawChanged(bool b);
	void selectedCameraDrawPathChanged(bool b);

private:
	void updateSelectedCameraInfo();

	SCHNApps* m_schnapps;
	Camera* m_selectedCamera;
	bool b_updatingUI;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
