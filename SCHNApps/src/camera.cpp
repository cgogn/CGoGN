

#include "camera.h"
#include "mapHandler.h"

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
	DEBUG_EMIT("projectionTypeChanged");
	emit(projectionTypeChanged(t));
	foreach(View* view, m_schnapps->getViewSet().values())
		view->updateGL();
}

void Camera::setDraw(bool b)
{
	m_draw = b;
	DEBUG_EMIT("drawChanged");
	emit(drawChanged(b));
	foreach(View* view, m_schnapps->getViewSet().values())
		view->updateGL();
}

void Camera::setDrawPath(bool b)
{
	m_drawPath = b;
	DEBUG_EMIT("drawPathChanged");
	emit(drawPathChanged(b));
	foreach(View* view, m_schnapps->getViewSet().values())
		view->updateGL();
}

void Camera::linkView(View* view)
{
	if(view && !l_views.contains(view))
	{
		l_views.push_back(view);
		updateParams();
		connect(view, SIGNAL(boundingBoxChanged()), this, SLOT(updateParams()));
	}
}

void Camera::unlinkView(View* view)
{
	if (l_views.removeOne(view))
	{
		updateParams();
		disconnect(view, SIGNAL(boundingBoxChanged()), this, SLOT(updateParams()));
	}
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

void Camera::updateParams()
{
	qglviewer::Vec bbMin;
	qglviewer::Vec bbMax;

	if (!l_views.empty())
	{
		l_views.first()->getBB(bbMin, bbMax);

		foreach(View* v, l_views)
		{
			qglviewer::Vec minbb;
			qglviewer::Vec maxbb;
			v->getBB(minbb, maxbb);
			for(unsigned int dim = 0; dim < 3; ++dim)
			{
				if(minbb[dim] < bbMin[dim])
					bbMin[dim] = minbb[dim];
				if(maxbb[dim] > bbMax[dim])
					bbMax[dim] = maxbb[dim];
			}
		}
	}
	else
	{
		bbMin.setValue(0, 0, 0);
		bbMax.setValue(0, 0, 0);
	}

	this->setSceneBoundingBox(bbMin, bbMax);
	this->showEntireScene();
}

} // namespace SCHNApps

} // namespace CGoGN
