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
	setupUi(this);
	setModal(false);

	connect(addCameraButton, SIGNAL(clicked()), this, SLOT(cb_addCamera()));
	connect(removeCamerasButton, SIGNAL(clicked()), this, SLOT(cb_removeCameras()));

	connect(m_window, SIGNAL(cameraAdded(Camera*)), this, SLOT(cb_addCameraToList(Camera*)));
	connect(m_window, SIGNAL(cameraRemoved(Camera*)), this, SLOT(cb_removeCameraFromList(Camera*)));

	connect(cameraList, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(cb_itemChanged(QTreeWidgetItem*,int)));

	cameraList->resizeColumnToContents(1);
	cameraList->resizeColumnToContents(2);
	cameraList->resizeColumnToContents(3);
}

CamerasDialog::~CamerasDialog()
{}

void CamerasDialog::setCameraType(const QString& name, qglviewer::Camera::Type type)
{
	b_refreshingUI = true;
	QList<QTreeWidgetItem*> items = cameraList->findItems(name, Qt::MatchExactly);
	if(!items.empty())
	{
		if(type == qglviewer::Camera::ORTHOGRAPHIC)
			items[0]->setCheckState(1, Qt::Checked);
		else
			items[0]->setCheckState(1, Qt::Unchecked);
	}
	b_refreshingUI = false;
}

void CamerasDialog::setDrawCamera(const QString& name, bool b)
{
	b_refreshingUI = true;
	QList<QTreeWidgetItem*> items = cameraList->findItems(name, Qt::MatchExactly);
	if(!items.empty())
	{
		if(b)
			items[0]->setCheckState(2, Qt::Checked);
		else
			items[0]->setCheckState(2, Qt::Unchecked);
	}
	b_refreshingUI = false;
}

void CamerasDialog::setDrawCameraPath(const QString& name, bool b)
{
	b_refreshingUI = true;
	QList<QTreeWidgetItem*> items = cameraList->findItems(name, Qt::MatchExactly);
	if(!items.empty())
	{
		if(b)
			items[0]->setCheckState(3, Qt::Checked);
		else
			items[0]->setCheckState(3, Qt::Unchecked);
	}
	b_refreshingUI = false;
}

void CamerasDialog::cb_addCamera()
{
	m_window->addCamera();
}

void CamerasDialog::cb_removeCameras()
{
	QList<QTreeWidgetItem*> currentItems = cameraList->selectedItems();
	foreach(QTreeWidgetItem* item, currentItems)
	{
		const QString& cname = item->text(0);
		Camera* c = m_window->getCamera(cname);
		if(!c->isUsed())
			m_window->removeCamera(cname);
		else
			QMessageBox::warning(this, tr("Warning"), "Camera " + cname + " is currently used");
	}
}

void CamerasDialog::cb_addCameraToList(Camera* c)
{
	QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(c->getName()));
	item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
	item->setCheckState(1, Qt::Unchecked);
	item->setCheckState(2, Qt::Unchecked);
	item->setCheckState(3, Qt::Unchecked);
	cameraList->addTopLevelItem(item);
}

void CamerasDialog::cb_removeCameraFromList(Camera* c)
{
	QList<QTreeWidgetItem*> items = cameraList->findItems(c->getName(), Qt::MatchExactly);
	if(!items.empty())
		delete items[0];
}

void CamerasDialog::cb_itemChanged(QTreeWidgetItem* item, int column)
{
	if(!b_refreshingUI)
	{
		Camera *c = m_window->getCamera(item->text(0));
		switch(column)
		{
			case 1 :
				if(item->checkState(1) == Qt::Checked)
					c->changeType(qglviewer::Camera::ORTHOGRAPHIC);
				else
					c->changeType(qglviewer::Camera::PERSPECTIVE);
				break;
			case 2 :
				if(item->checkState(2) == Qt::Checked)
					c->setDraw(true);
				else
					c->setDraw(false);
				break;
			case 3 :
				if(item->checkState(3) == Qt::Checked)
					c->setDrawPath(true);
				else
					c->setDrawPath(false);
				break;
		}
	}
}

} // namespace SCHNApps

} // namespace CGoGN
