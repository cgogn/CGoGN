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

	list_cameras->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(button_addCamera, SIGNAL(clicked()), this, SLOT(addCameraButtonClicked()));
	connect(list_cameras, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(cameraCheckStateChanged(QListWidgetItem*)));
	connect(list_cameras, SIGNAL(itemSelectionChanged()), this, SLOT(cameraSelectionChanged()));
	connect(group_projectionType, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(cameraProjectionChanged(QAbstractButton*)));
	connect(check_drawCamera, SIGNAL(clicked(bool)), this, SLOT(cameraDrawClicked(bool)));
	connect(check_drawCameraPath, SIGNAL(clicked(bool)), this, SLOT(cameraDrawPathClicked(bool)));

	connect(m_schnapps, SIGNAL(cameraAdded(Camera*)), this, SLOT(cameraAdded(Camera*)));
	connect(m_schnapps, SIGNAL(cameraRemoved(Camera*)), this, SLOT(cameraRemoved(Camera*)));
}


Camera* ControlDock_CameraTab::currentCam()
{
	QList<QListWidgetItem *> sels = list_cameras->selectedItems();
	if (!sels.empty())
		return m_schnapps->getCamera(sels[0]->text());
	//else
	return m_schnapps->getCamera(list_cameras->item(0)->text());
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
		m_schnapps->getCamera(item->text())->m_drawBB = (item->checkState() == Qt::Checked);
		m_schnapps->redrawAllViews();
	}
}

void ControlDock_CameraTab::cameraProjectionChanged(QAbstractButton* b)
{
	if(!b_updatingUI)
	{
		Camera* c =currentCam();
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
		Camera* c = currentCam();
		c->setDraw(b);
	}
}

void ControlDock_CameraTab::cameraDrawPathClicked(bool b)
{
	if(!b_updatingUI)
	{
		Camera* c = currentCam();
		c->setDrawPath(b);
	}
}



void ControlDock_CameraTab::cameraAdded(Camera* c)
{
	QListWidgetItem* item = new QListWidgetItem(c->getName(), list_cameras);
	b_updatingUI = true;
//	item->setCheckState(Qt::Unchecked);
	item->setSelected(true);
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

void ControlDock_CameraTab::cameraSelectionChanged()
{
	updateCurrentCameraInfo();
}



void ControlDock_CameraTab::updateCurrentCameraInfo()
{
	b_updatingUI = true;
	Camera* c = currentCam();

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
