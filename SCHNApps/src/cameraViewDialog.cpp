#include "cameraViewDialog.h"

#include "window.h"
#include "view.h"
#include "camera.h"

#include <QMessageBox>
#include <QMouseEvent>

CameraViewDialog::CameraViewDialog(Window* window, View* view) :
	QDialog(view),
	m_window(window),
	m_view(view)
{
	this->setupUi(this);
	this->setWindowTitle(m_view->getName() + QString(" : camera"));

	connect(cameraList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedCameraChanged()));

	connect(m_window, SIGNAL(cameraAdded(Camera*)), this, SLOT(cb_addCameraToList(Camera*)));
	connect(m_window, SIGNAL(cameraRemoved(Camera*)), this, SLOT(cb_removeCameraFromList(Camera*)));

	QList<Camera*> cameras = m_window->getCamerasList();
	foreach(Camera* c, cameras)
		cameraList->addItem(c->getName());

	selectCurrentCamera();
}

CameraViewDialog::~CameraViewDialog()
{}

void CameraViewDialog::selectCurrentCamera()
{
	Camera* current = m_view->getCurrentCamera();
	for(int i = 0; i < cameraList->count(); ++i)
	{
		if(cameraList->item(i)->text() == current->getName())
			cameraList->item(i)->setSelected(true);
		else
			cameraList->item(i)->setSelected(false);
	}
}

void CameraViewDialog::cb_selectedCameraChanged()
{
	QList<QListWidgetItem*> currentItems = cameraList->selectedItems();
	if(currentItems.empty())
		selectCurrentCamera();
	else
	{
		const QString& cname = currentItems[0]->text();
		Camera* c = m_window->getCamera(cname);
		m_view->getCurrentCamera()->unlinkView(m_view);
		m_view->setCurrentCamera(c);
		c->linkView(m_view);
	}
}

void CameraViewDialog::cb_addCameraToList(Camera* c)
{
	cameraList->addItem(c->getName());
}

void CameraViewDialog::cb_removeCameraFromList(Camera* c)
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
