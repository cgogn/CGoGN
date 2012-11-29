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

//	connect(cameraList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(cb_selectedCameraChanged(QListWidgetItem*, QListWidgetItem*)));
}

CameraDialog::~CameraDialog()
{}

void CameraDialog::addCameraToList(const QString& name)
{
	cameraList->addItem(name);
}

void CameraDialog::cb_addCamera()
{
	m_window->addCamera();
}

void CameraDialog::cb_removeCamera()
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

//void CameraDialog::cb_selectedViewChanged(QListWidgetItem* current, QListWidgetItem* previous)
//{
//	const QString& vname = current->text();
//	View* v = m_window->getView(vname);
//	Camera* c = v->getCurrentCamera();
//	for(int i = 0; i < cameraList->count(); ++i)
//	{
//		if(cameraList->item(i)->text() == c->getName())
//			cameraList->item(i)->setSelected(true);
//		else
//			cameraList->item(i)->setSelected(false);
//	}
//}
//
//void CameraDialog::cb_selectedCameraChanged(QListWidgetItem* current, QListWidgetItem* previous)
//{
//	const QString& cname = current->text();
//	Camera* c = m_window->getCamera(cname);
//	if(viewList->currentItem() != NULL)
//	{
//		const QString& vname = viewList->currentItem()->text();
//		View* v = m_window->getView(vname);
//		v->setCurrentCamera(c);
//	}
//}
