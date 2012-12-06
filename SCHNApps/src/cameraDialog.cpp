#include "cameraDialog.h"

#include "window.h"
#include "camera.h"
#include <QMessageBox>
#include <QMouseEvent>

CameraDialog::CameraDialog(Window* window) :
	QDialog(window),
	m_window(window)
{
	this->setupUi(this);
	this->setModal(false);

	connect(addCameraButton, SIGNAL(clicked()), this, SLOT(cb_addCamera()));
	connect(removeCameraButton, SIGNAL(clicked()), this, SLOT(cb_removeCamera()));

	connect(m_window, SIGNAL(cameraAdded(Camera*)), this, SLOT(cb_addCameraToList(Camera*)));
	connect(m_window, SIGNAL(cameraRemoved(Camera*)), this, SLOT(cb_removeCameraFromList(Camera*)));
}

CameraDialog::~CameraDialog()
{}

void CameraDialog::cb_addCamera()
{
	m_window->addCamera();
}

void CameraDialog::cb_removeCamera()
{
	const QString& cname = cameraList->currentItem()->text();
	Camera* c = m_window->getCamera(cname);
	if(!c->isUsed())
		m_window->removeCamera(cname);
	else
		QMessageBox::warning(this, tr("Warning"), "Camera is currently used");
}

void CameraDialog::cb_addCameraToList(Camera* c)
{
	cameraList->addItem(c->getName());
}

void CameraDialog::cb_removeCameraFromList(Camera* c)
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
