#include "dialogs/camerasDialog.h"

#include "window.h"
#include "camera.h"

#include <QMessageBox>
#include <QMouseEvent>

namespace CGoGN
{

namespace SCHNApps
{

CamerasDialog::CamerasDialog(Window* window) :
	QDialog(window),
	m_window(window)
{
	this->setupUi(this);
	this->setModal(false);

	connect(addCameraButton, SIGNAL(clicked()), this, SLOT(cb_addCamera()));
	connect(removeCamerasButton, SIGNAL(clicked()), this, SLOT(cb_removeCameras()));

	connect(m_window, SIGNAL(cameraAdded(Camera*)), this, SLOT(cb_addCameraToList(Camera*)));
	connect(m_window, SIGNAL(cameraRemoved(Camera*)), this, SLOT(cb_removeCameraFromList(Camera*)));
}

CamerasDialog::~CamerasDialog()
{}

void CamerasDialog::cb_addCamera()
{
	m_window->addCamera();
}

void CamerasDialog::cb_removeCameras()
{
	QList<QListWidgetItem*> currentItems = cameraList->selectedItems();
	foreach(QListWidgetItem* item, currentItems)
	{
		const QString& cname = item->text();
		Camera* c = m_window->getCamera(cname);
		if(!c->isUsed())
			m_window->removeCamera(cname);
		else
			QMessageBox::warning(this, tr("Warning"), "Camera " + cname + " is currently used");
	}
}

void CamerasDialog::cb_addCameraToList(Camera* c)
{
	cameraList->addItem(c->getName());
}

void CamerasDialog::cb_removeCameraFromList(Camera* c)
{
	for(int i = 0; i < cameraList->count(); ++i)
	{
		if(cameraList->item(i)->text() == c->getName())
		{
			delete cameraList->item(i);
			return;
		}
	}
}

} // namespace SCHNApps

} // namespace CGoGN
