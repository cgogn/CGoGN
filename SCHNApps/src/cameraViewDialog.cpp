#include "cameraViewDialog.h"

#include "window.h"
#include "view.h"
#include "camera.h"

#include <QMessageBox>
#include <QMouseEvent>

CameraViewDialog::CameraViewDialog(Window* window, View* view) :
	QDialog(window),
	m_window(window),
	m_view(view)
{
	this->setupUi(this);
	this->setWindowTitle(QString("Select camera for view ") + m_view->getName());
	connect(cameraList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(cb_selectedCameraChanged(QListWidgetItem*, QListWidgetItem*)));

	QList<Camera*> cameras = m_window->getCamerasList();
	foreach(Camera* c, cameras)
		cameraList->addItem(c->getName());
}

CameraViewDialog::~CameraViewDialog()
{}

void CameraViewDialog::cb_selectedCameraChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	const QString& cname = current->text();
	Camera* c = m_window->getCamera(cname);
	m_view->setCurrentCamera(c);
}
