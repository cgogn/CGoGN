#include "camera.h"

namespace CGoGN
{

namespace SCHNApps
{

unsigned int Camera::cameraCount = 0;

Camera::Camera(const QString& name, SCHNApps* s) :
	m_name(name),
	m_schnapps(s),
	m_draw(false),
	m_drawPath(false)
{
	++cameraCount;
	connect(this->frame(), SIGNAL(modified()), this, SLOT(frameModified()));
}

Camera::~Camera()
{}

void Camera::setProjectionType(int t)
{
	setType(qglviewer::Camera::Type(t));
	emit(projectionTypeChanged(t));
	foreach(View* view, m_schnapps->getViewSet().values())
		view->updateGL();
}

void Camera::setDraw(bool b)
{
	m_draw = b;
	emit(drawChanged(b));
	foreach(View* view, m_schnapps->getViewSet().values())
		view->updateGL();
}

void Camera::setDrawPath(bool b)
{
	m_drawPath = b;
	emit(drawPathChanged(b));
	foreach(View* view, m_schnapps->getViewSet().values())
		view->updateGL();
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

void Camera::frameModified()
{
	if(m_draw || m_drawPath)
	{
		foreach(View* view, m_schnapps->getViewSet().values())
			view->updateGL();
	}
	else
	{
		foreach(View* view, l_views)
			view->updateGL();
	}
}

} // namespace SCHNApps

} // namespace CGoGN
