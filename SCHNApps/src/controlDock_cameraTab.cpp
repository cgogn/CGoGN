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
	m_selectedCamera(NULL),
	b_updatingUI(false)
{
	setupUi(this);

	connect(button_addCamera, SIGNAL(clicked()), this, SLOT(addCameraButtonClicked()));
	connect(list_cameras, SIGNAL(itemSelectionChanged()), this, SLOT(selectedCameraChanged()));
	connect(group_projectionType, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(cameraProjectionChanged(QAbstractButton*)));
	connect(check_drawCamera, SIGNAL(clicked(bool)), this, SLOT(cameraDrawClicked(bool)));
	connect(check_drawCameraPath, SIGNAL(clicked(bool)), this, SLOT(cameraDrawPathClicked(bool)));

	connect(m_schnapps, SIGNAL(cameraAdded(Camera*)), this, SLOT(cameraAdded(Camera*)));
	connect(m_schnapps, SIGNAL(cameraRemoved(Camera*)), this, SLOT(cameraRemoved(Camera*)));
}

void ControlDock_CameraTab::addCameraButtonClicked()
{
	if (!b_updatingUI)
		m_schnapps->addCamera();
}

void ControlDock_CameraTab::selectedCameraChanged()
{
	if (!b_updatingUI)
	{
		Camera* old = m_selectedCamera;

		if (old)
		{
			disconnect(m_selectedCamera, SIGNAL(projectionTypeChanged(int)), this, SLOT(selectedCameraProjectionTypeChanged(int)));
			disconnect(m_selectedCamera, SIGNAL(drawChanged(bool)), this, SLOT(selectedCameraDrawChanged(bool)));
			disconnect(m_selectedCamera, SIGNAL(drawPathChanged(bool)), this, SLOT(selectedCameraDrawPathChanged(bool)));
		}

		QList<QListWidgetItem*> items = list_cameras->selectedItems();
		if(!items.empty())
		{
			QString selectedCameraName = items[0]->text();
			m_selectedCamera = m_schnapps->getCamera(selectedCameraName);

			connect(m_selectedCamera, SIGNAL(projectionTypeChanged(int)), this, SLOT(selectedCameraProjectionTypeChanged(int)));
			connect(m_selectedCamera, SIGNAL(drawChanged(bool)), this, SLOT(selectedCameraDrawChanged(bool)));
			connect(m_selectedCamera, SIGNAL(drawPathChanged(bool)), this, SLOT(selectedCameraDrawPathChanged(bool)));
		}
		else
			m_selectedCamera = NULL;

		updateSelectedCameraInfo();
	}
}

void ControlDock_CameraTab::cameraProjectionChanged(QAbstractButton* b)
{
	if (!b_updatingUI)
	{
		if(radio_orthographicProjection->isChecked())
			m_selectedCamera->setProjectionType(qglviewer::Camera::ORTHOGRAPHIC);
		else if(radio_perspectiveProjection->isChecked())
			m_selectedCamera->setProjectionType(qglviewer::Camera::PERSPECTIVE);
	}
}

void ControlDock_CameraTab::cameraDrawClicked(bool b)
{
	if (!b_updatingUI)
	{
		m_selectedCamera->setDraw(b);
	}
}

void ControlDock_CameraTab::cameraDrawPathClicked(bool b)
{
	if (!b_updatingUI)
	{
		m_selectedCamera->setDrawPath(b);
	}
}





void ControlDock_CameraTab::cameraAdded(Camera* c)
{
	b_updatingUI = true;
	list_cameras->addItem(c->getName());
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





void ControlDock_CameraTab::selectedCameraProjectionTypeChanged(int t)
{
	b_updatingUI = true;
	switch(t)
	{
		case qglviewer::Camera::ORTHOGRAPHIC : radio_orthographicProjection->setChecked(true); break;
		case qglviewer::Camera::PERSPECTIVE : radio_perspectiveProjection->setChecked(true); break;
	}
	b_updatingUI = false;
}

void ControlDock_CameraTab::selectedCameraDrawChanged(bool b)
{
	b_updatingUI = true;
	if(b)
		check_drawCamera->setCheckState(Qt::Checked);
	else
		check_drawCamera->setCheckState(Qt::Unchecked);
	b_updatingUI = false;
}

void ControlDock_CameraTab::selectedCameraDrawPathChanged(bool b)
{
	b_updatingUI = true;
	if(b)
		check_drawCameraPath->setCheckState(Qt::Checked);
	else
		check_drawCameraPath->setCheckState(Qt::Unchecked);
	b_updatingUI = false;
}





void ControlDock_CameraTab::updateSelectedCameraInfo()
{
	b_updatingUI = true;

	if(m_selectedCamera->getProjectionType() == qglviewer::Camera::PERSPECTIVE)
		radio_perspectiveProjection->setChecked(true);
	else if(m_selectedCamera->getProjectionType() == qglviewer::Camera::ORTHOGRAPHIC)
		radio_orthographicProjection->setChecked(true);

	check_drawCamera->setChecked(m_selectedCamera->getDraw());

	check_drawCameraPath->setChecked(m_selectedCamera->getDrawPath());

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
