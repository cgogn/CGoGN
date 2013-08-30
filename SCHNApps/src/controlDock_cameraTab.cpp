#include "controlDock_cameraTab.h"

#include <QMessageBox>

#include "schnapps.h"
#include "camera.h"
#include "view.h"

namespace CGoGN
{

namespace SCHNApps
{

ControlDock_CameraTab::ControlDock_CameraTab(SCHNApps* s) :
	m_schnapps(s),
	b_updatingUI(false)
{
	setupUi(this);

	connect(button_addCamera, SIGNAL(clicked()), this, SLOT(addCameraButtonClicked()));
	connect(list_cameras, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(cameraCheckStateChanged(QListWidgetItem*)));
	connect(group_projectionType, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(cameraProjectionChanged(QAbstractButton*)));
	connect(check_drawCamera, SIGNAL(clicked(bool)), this, SLOT(cameraDrawClicked(bool)));
	connect(check_drawCameraPath, SIGNAL(clicked(bool)), this, SLOT(cameraDrawPathClicked(bool)));

	connect(m_schnapps, SIGNAL(cameraAdded(Camera*)), this, SLOT(cameraAdded(Camera*)));
	connect(m_schnapps, SIGNAL(cameraRemoved(Camera*)), this, SLOT(cameraRemoved(Camera*)));
	connect(m_schnapps, SIGNAL(selectedViewChanged(View*,View*)), this, SLOT(selectedViewChanged(View*,View*)));
}





void ControlDock_CameraTab::addCameraButtonClicked()
{
	if(!b_updatingUI)
		m_schnapps->addCamera(QString("camera_") + QString::number(Camera::cameraCount));
}

void ControlDock_CameraTab::cameraCheckStateChanged(QListWidgetItem *item)
{
	if(!b_updatingUI)
	{
		if(item->checkState() == Qt::Checked)
			m_schnapps->getSelectedView()->setCurrentCamera(m_schnapps->getCamera(item->text()));
		else // forbid to uncheck current camera (current view cannot have no camera)
		{
			b_updatingUI = true;
			item->setCheckState(Qt::Checked);
			b_updatingUI = false;
		}
	}
}

void ControlDock_CameraTab::cameraProjectionChanged(QAbstractButton* b)
{
	if(!b_updatingUI)
	{
		Camera* c = m_schnapps->getSelectedView()->getCurrentCamera();
		if(radio_orthographicProjection->isChecked())
			c->setProjectionType(qglviewer::Camera::ORTHOGRAPHIC);
		else if(radio_perspectiveProjection->isChecked())
			c->setProjectionType(qglviewer::Camera::PERSPECTIVE);
	}
}

void ControlDock_CameraTab::cameraDrawClicked(bool b)
{
	if(!b_updatingUI)
	{
		Camera* c = m_schnapps->getSelectedView()->getCurrentCamera();
		c->setDraw(b);
	}
}

void ControlDock_CameraTab::cameraDrawPathClicked(bool b)
{
	if(!b_updatingUI)
	{
		Camera* c = m_schnapps->getSelectedView()->getCurrentCamera();
		c->setDrawPath(b);
	}
}





void ControlDock_CameraTab::cameraAdded(Camera* c)
{
	QListWidgetItem* item = new QListWidgetItem(c->getName(), list_cameras);
	b_updatingUI = true;
	item->setCheckState(Qt::Unchecked);
	b_updatingUI = false;
}

void ControlDock_CameraTab::cameraRemoved(Camera* c)
{
	QList<QListWidgetItem*> items = list_cameras->findItems(c->getName(), Qt::MatchExactly);
	if(!items.empty())
	{
		b_updatingUI = true;
		delete items[0];
		b_updatingUI = false;
	}
}

void ControlDock_CameraTab::selectedViewChanged(View* prev, View* cur)
{
	if(prev)
	{
		Camera* prevCam = prev->getCurrentCamera();
		QList<QListWidgetItem*> prevItems = list_cameras->findItems(prevCam->getName(), Qt::MatchExactly);
		if(!prevItems.empty())
		{
			b_updatingUI = true;
			prevItems[0]->setCheckState(Qt::Unchecked);
			b_updatingUI = false;
		}

		disconnect(prevCam, SIGNAL(projectionTypeChanged(int)), this, SLOT(currentCameraProjectionTypeChanged(int)));
		disconnect(prevCam, SIGNAL(drawChanged(bool)), this, SLOT(currentCameraDrawChanged(bool)));
		disconnect(prevCam, SIGNAL(drawPathChanged(bool)), this, SLOT(currentCameraDrawPathChanged(bool)));

		disconnect(prev, SIGNAL(currentCameraChanged(Camera*,Camera*)), this, SLOT(selectedViewCurrentCameraChanged(Camera*,Camera*)));
	}

	if(cur)
	{
		Camera* curCam = cur->getCurrentCamera();
		QList<QListWidgetItem*> curItems = list_cameras->findItems(curCam->getName(), Qt::MatchExactly);
		if(!curItems.empty())
		{
			b_updatingUI = true;
			curItems[0]->setCheckState(Qt::Checked);
			b_updatingUI = false;
		}

		connect(curCam, SIGNAL(projectionTypeChanged(int)), this, SLOT(currentCameraProjectionTypeChanged(int)));
		connect(curCam, SIGNAL(drawChanged(bool)), this, SLOT(currentCameraDrawChanged(bool)));
		connect(curCam, SIGNAL(drawPathChanged(bool)), this, SLOT(currentCameraDrawPathChanged(bool)));

		connect(cur, SIGNAL(currentCameraChanged(Camera*,Camera*)), this, SLOT(selectedViewCurrentCameraChanged(Camera*,Camera*)));
	}

	updateCurrentCameraInfo();
}





void ControlDock_CameraTab::selectedViewCurrentCameraChanged(Camera* prev, Camera* cur)
{
	if(prev)
	{
		QList<QListWidgetItem*> prevItems = list_cameras->findItems(prev->getName(), Qt::MatchExactly);
		if(!prevItems.empty())
		{
			b_updatingUI = true;
			prevItems[0]->setCheckState(Qt::Unchecked);
			b_updatingUI = false;
		}

		disconnect(prev, SIGNAL(projectionTypeChanged(int)), this, SLOT(currentCameraProjectionTypeChanged(int)));
		disconnect(prev, SIGNAL(drawChanged(bool)), this, SLOT(currentCameraDrawChanged(bool)));
		disconnect(prev, SIGNAL(drawPathChanged(bool)), this, SLOT(currentCameraDrawPathChanged(bool)));
	}

	if(cur)
	{
		QList<QListWidgetItem*> curItems = list_cameras->findItems(cur->getName(), Qt::MatchExactly);
		if(!curItems.empty())
		{
			b_updatingUI = true;
			curItems[0]->setCheckState(Qt::Checked);
			b_updatingUI = false;
		}

		connect(cur, SIGNAL(projectionTypeChanged(int)), this, SLOT(currentCameraProjectionTypeChanged(int)));
		connect(cur, SIGNAL(drawChanged(bool)), this, SLOT(currentCameraDrawChanged(bool)));
		connect(cur, SIGNAL(drawPathChanged(bool)), this, SLOT(currentCameraDrawPathChanged(bool)));
	}

	updateCurrentCameraInfo();
}





void ControlDock_CameraTab::currentCameraProjectionTypeChanged(int t)
{
	b_updatingUI = true;
	switch(t)
	{
		case qglviewer::Camera::ORTHOGRAPHIC : radio_orthographicProjection->setChecked(true); break;
		case qglviewer::Camera::PERSPECTIVE : radio_perspectiveProjection->setChecked(true); break;
	}
	b_updatingUI = false;
}

void ControlDock_CameraTab::currentCameraDrawChanged(bool b)
{
	b_updatingUI = true;
	if(b)
		check_drawCamera->setCheckState(Qt::Checked);
	else
		check_drawCamera->setCheckState(Qt::Unchecked);
	b_updatingUI = false;
}

void ControlDock_CameraTab::currentCameraDrawPathChanged(bool b)
{
	b_updatingUI = true;
	if(b)
		check_drawCameraPath->setCheckState(Qt::Checked);
	else
		check_drawCameraPath->setCheckState(Qt::Unchecked);
	b_updatingUI = false;
}




void ControlDock_CameraTab::updateCurrentCameraInfo()
{
	b_updatingUI = true;

	Camera* c = m_schnapps->getSelectedView()->getCurrentCamera();

	if(c->getProjectionType() == qglviewer::Camera::PERSPECTIVE)
		radio_perspectiveProjection->setChecked(true);
	else if(c->getProjectionType() == qglviewer::Camera::ORTHOGRAPHIC)
		radio_orthographicProjection->setChecked(true);

	check_drawCamera->setChecked(c->getDraw());

	check_drawCameraPath->setChecked(c->getDrawPath());

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
