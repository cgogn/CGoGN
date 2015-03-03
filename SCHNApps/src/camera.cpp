

#include "camera.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

unsigned int Camera::cameraCount = 0;

Camera::Camera(const QString& name, SCHNApps* s) :
	m_drawBB(false),
	m_name(name),
	m_schnapps(s),
	m_draw(false),
	m_drawPath(false)
{
	++cameraCount;
	connect(this->frame(), SIGNAL(modified()), this, SLOT(frameModified()));

	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
	{
		// do the code of BBModified for each linked map
		if (isLinkedToMap(map))
			updateParams();
		foreach(View* v, l_views)
		{
			v->updateGL();
		}
		// and do the connection for map added
		mapAdded(map);
	}
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


void Camera::updateParams()
{
	// get all maps viewed by camera
	QSet<MapHandlerGen*> q_maps;
	foreach(View* view, l_views)
	{
		QList<MapHandlerGen*> l_maps = view->getLinkedMaps();
		foreach(MapHandlerGen* mhg, l_maps)
			q_maps.insert(mhg);
	}

	if (!q_maps.empty())
	{
// to help debug I store bb min/max in camera for camera bb drawing
//		qglviewer::Vec m_bbMin;
//		qglviewer::Vec m_bbMax;

		(*q_maps.begin())->transformedBB(m_bbMin,m_bbMax);

		foreach(MapHandlerGen* mhg, q_maps)
		{
			qglviewer::Vec minbb;
			qglviewer::Vec maxbb;
			mhg->transformedBB(minbb,maxbb);
			for(unsigned int dim = 0; dim < 3; ++dim)
			{
				if(minbb[dim] < m_bbMin[dim])
					m_bbMin[dim] = minbb[dim];
				if(maxbb[dim] > m_bbMax[dim])
					m_bbMax[dim] = maxbb[dim];
			}
		}
		this->setSceneBoundingBox(m_bbMin, m_bbMax);
		this->showEntireScene();
	}

}


bool Camera::isLinkedToMap(MapHandlerGen* mhg) const
{
	const QString& mapName = mhg->getName();
	foreach(View* v, l_views)
	{
		if (v->isLinkedToMap(mapName))
			return true;
	}
	return false;
}


void Camera::mapAdded(MapHandlerGen* mhg)
{
	// all maps signals to camera its existence
	// selection if done by BBModified
	connect(mhg, SIGNAL(BBisModified()), this, SLOT(BBModified()));
}

void Camera::mapRemoved(MapHandlerGen* mhg)
{
	disconnect(mhg, SIGNAL(BBisModified()), this, SLOT(BBModified()));
}

void Camera::BBModified()
{
	DEBUG_SLOT();
	MapHandlerGen* mhg = static_cast<MapHandlerGen*>(QObject::sender());

	if (isLinkedToMap(mhg))
		updateParams();
	foreach(View* v, l_views)
	{
		v->updateGL();
	}
}


void Camera::drawBBCam()
{
	if (!m_drawBB)
		return;

//TODO with Drawer ?

//	float shift = 0.01f*(m_bbMax - m_bbMin).norm();

//	glDisable(GL_LIGHTING);
//	glColor3f(0.5f,1.0f,0.0f);
//	glLineWidth(1.0f);
//	glBegin(GL_LINE_LOOP);
//	glVertex3f(m_bbMin[0]-shift, m_bbMin[1]-shift, m_bbMin[2]-shift);
//	glVertex3f(m_bbMin[0]-shift, m_bbMax[1]+shift, m_bbMin[2]-shift);
//	glVertex3f(m_bbMax[0]+shift, m_bbMax[1]+shift, m_bbMin[2]-shift);
//	glVertex3f(m_bbMax[0]+shift, m_bbMin[1]-shift, m_bbMin[2]-shift);
//	glVertex3f(m_bbMin[0]-shift, m_bbMin[1]-shift, m_bbMin[2]-shift);
//	glEnd();
//	glBegin(GL_LINE_LOOP);
//	glVertex3f(m_bbMax[0]+shift, m_bbMax[1]+shift, m_bbMax[2]+shift);
//	glVertex3f(m_bbMax[0]+shift, m_bbMin[1]-shift, m_bbMax[2]+shift);
//	glVertex3f(m_bbMin[0]-shift, m_bbMin[1]-shift, m_bbMax[2]+shift);
//	glVertex3f(m_bbMin[0]-shift, m_bbMax[1]+shift, m_bbMax[2]+shift);
//	glVertex3f(m_bbMax[0]+shift, m_bbMax[1]+shift, m_bbMax[2]+shift);
//	glEnd();
//	glBegin(GL_LINES);
//	glVertex3f(m_bbMin[0]-shift, m_bbMin[1]-shift, m_bbMin[2]-shift);
//	glVertex3f(m_bbMin[0]-shift, m_bbMin[1]-shift, m_bbMax[2]+shift);
//	glVertex3f(m_bbMin[0]-shift, m_bbMax[1]+shift, m_bbMin[2]-shift);
//	glVertex3f(m_bbMin[0]-shift, m_bbMax[1]+shift, m_bbMax[2]+shift);
//	glVertex3f(m_bbMax[0]+shift, m_bbMax[1]+shift, m_bbMin[2]-shift);
//	glVertex3f(m_bbMax[0]+shift, m_bbMax[1]+shift, m_bbMax[2]+shift);
//	glVertex3f(m_bbMax[0]+shift, m_bbMin[1]-shift, m_bbMin[2]-shift);
//	glVertex3f(m_bbMax[0]+shift, m_bbMin[1]-shift, m_bbMax[2]+shift);
//	glEnd();
//	glColor3f(1.0f,1.0f,1.0f);
//	glEnable(GL_LIGHTING);
}

} // namespace SCHNApps

} // namespace CGoGN
