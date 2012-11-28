#include "cameraViewDialog.h"

#include "window.h"
#include "camera.h"
#include <QMessageBox>

CameraViewDialog::CameraViewDialog(Window* window) :
	m_window(window)
{
	this->setupUi(this);

	connect(addCameraButton, SIGNAL(clicked()), this, SLOT(cb_addCamera()));
	connect(removeCameraButton, SIGNAL(clicked()), this, SLOT(cb_removeCamera()));

	connect(viewList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(cb_selectedViewChanged(QListWidgetItem*, QListWidgetItem*)));
	connect(cameraList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(cb_selectedCameraChanged(QListWidgetItem*, QListWidgetItem*)));

	updateCameraList();
	updateViewList();
}

CameraViewDialog::~CameraViewDialog()
{}

void CameraViewDialog::updateCameraList()
{
	cameraList->clear();
	QList<Camera*> cameras = m_window->getCamerasList();
	foreach(Camera* c, cameras)
		cameraList->addItem(c->getName()); // + QString(c->isUsed() ? " (used)" : ""));
}

void CameraViewDialog::updateViewList()
{
	viewList->clear();
	QList<View*> views = m_window->getViewsList();
	foreach(View* v, views)
		viewList->addItem(v->getName());
}

void CameraViewDialog::cb_addCamera()
{
	Camera* c = m_window->addCamera();
	cameraList->addItem(c->getName());
}

void CameraViewDialog::cb_removeCamera()
{
	const QString& cname = cameraList->currentItem()->text();
	Camera* c = m_window->getCamera(cname);
	if(!c->isUsed())
	{
		m_window->removeCamera(cname);
		delete cameraList->currentItem();
	}
	else
		QMessageBox::warning(this, tr("Warning"), "Camera is currently used");
}

void CameraViewDialog::cb_selectedViewChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	const QString& vname = current->text();
	View* v = m_window->getView(vname);
	Camera* c = v->getCurrentCamera();
	for(int i = 0; i < cameraList->count(); ++i)
	{
		if(cameraList->item(i)->text() == c->getName())
			cameraList->item(i)->setSelected(true);
		else
			cameraList->item(i)->setSelected(false);
	}
}

void CameraViewDialog::cb_selectedCameraChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	const QString& cname = current->text();
	Camera* c = m_window->getCamera(cname);
	if(viewList->currentItem() != NULL)
	{
		const QString& vname = viewList->currentItem()->text();
		View* v = m_window->getView(vname);
		v->setCurrentCamera(c);
	}
}
