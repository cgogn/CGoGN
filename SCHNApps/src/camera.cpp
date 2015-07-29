

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
	b_draw(false),
	b_drawPath(false),
	b_fitToViewsBoundingBox(true)
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
	b_draw = b;
	DEBUG_EMIT("drawChanged");
	emit(drawChanged(b));
	foreach(View* view, m_schnapps->getViewSet().values())
		view->updateGL();
}

void Camera::setDrawPath(bool b)
{
	b_drawPath = b;
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
		fitToViewsBoundingBox();
		connect(view, SIGNAL(boundingBoxChanged()), this, SLOT(fitToViewsBoundingBox()));
	}
}

void Camera::unlinkView(View* view)
{
	if (l_views.removeOne(view))
	{
		fitToViewsBoundingBox();
		disconnect(view, SIGNAL(boundingBoxChanged()), this, SLOT(fitToViewsBoundingBox()));
	}
}

void Camera::frameModified()
{
	if(b_draw || b_drawPath)
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

void Camera::fitToViewsBoundingBox()
{
	if (b_fitToViewsBoundingBox)
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
}


QString Camera::toString()
{
	QString res;
	QTextStream str(&res);
	qglviewer::Vec pos = this->position();
	qglviewer::Quaternion ori = this->orientation();
	str << pos[0] << " " << pos[1] << " " << pos[2] << " ";
	str << ori[0] << " " << ori[1] << " " << ori[2] << " " << ori[3];
	return res;
}

void Camera::fromString(QString cam)
{
	QTextStream str(&cam);
	qglviewer::Vec pos = this->position();
	qglviewer::Quaternion ori = this->orientation();
	str >> pos[0];
	str >> pos[1];
	str >> pos[2];
	str >> ori[0];
	str >> ori[1];
	str >> ori[2];
	str >> ori[3];
	this->setPosition(pos);
	this->setOrientation(ori);
}


QString Camera::getName()
{
	return m_name;
}

SCHNApps* Camera::getSCHNApps() const
{
	return m_schnapps;
}

bool Camera::isUsed() const
{
	return !l_views.empty();
}

bool Camera::isShared()	const
{
	return l_views.size() > 1;
}

qglviewer::Camera::Type Camera::getProjectionType()
{
	return type();
}

bool Camera::getDraw() const
{
	return b_draw;
}

bool Camera::getDrawPath() const
{
	return b_drawPath;
}

const QList<View*>& Camera::getLinkedViews() const 
{
	return l_views;
}

bool Camera::isLinkedToView(View* view) const
{
	return l_views.contains(view);
}

void Camera::enableViewsBoundingBoxFitting()
{
	b_fitToViewsBoundingBox = true;
}

void Camera::disableViewsBoundingBoxFitting()
{
	b_fitToViewsBoundingBox = false;
}









} // namespace SCHNApps

} // namespace CGoGN
