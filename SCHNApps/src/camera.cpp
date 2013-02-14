#include "camera.h"
#include "dialogs/camerasDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

unsigned int Camera::cameraCount = 0;

Camera::Camera(const QString& name, Window* window) :
	m_name(name),
	m_window(window),
	m_draw(false),
	m_drawPath(false),
	m_snapCount(0)
{
	++cameraCount;
}

Camera::~Camera()
{}

void Camera::changeType(qglviewer::Camera::Type type)
{
	setType(type);
	foreach(View* view, m_window->getViewsList())
		view->updateGL();

	m_window->getCamerasDialog()->setCameraType(m_name, type);
}

void Camera::setDraw(bool b)
{
	m_draw = b;
	foreach(View* view, m_window->getViewsList())
		view->updateGL();

	m_window->getCamerasDialog()->setDrawCamera(m_name, b);
}

void Camera::setDrawPath(bool b)
{
	m_drawPath = b;
	foreach(View* view, m_window->getViewsList())
		view->updateGL();

	m_window->getCamerasDialog()->setDrawCameraPath(m_name, b);
}

void Camera::linkView(View* view)
{
	if(view && !l_views.contains(view))
		l_views.push_back(view);
}

void Camera::unlinkView(View* view)
{
	l_views.removeOne(view);
}

void Camera::saveSnapshot(QString snapPathName)
{
	foreach(View* view, l_views)
		view->saveSnapshot(snapPathName + view->getName() + '_' + QString::number(m_snapCount) + ".jpg", true);
	++m_snapCount;
}

} // namespace SCHNApps

} // namespace CGoGN
