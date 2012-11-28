#include "camera.h"

unsigned int Camera::cameraCount = 0;

Camera::Camera(const QString& name, Window* window) :
	m_name(name),
	m_window(window),
	m_draw(false),
	m_drawFarPlane(false),
	m_drawScale(1.0),
	m_drawPath(false),
	m_drawPathAxis(false),
	m_drawPathScale(1.0),
	m_snapCount(0)
{
	++cameraCount;
	this->setZClippingCoefficient(100);
}

Camera::~Camera()
{}

void Camera::draw()
{
	if(m_draw)
	{
		qglviewer::Camera::draw(m_drawFarPlane, m_drawScale);
		if(m_drawPath)
		{
			int mask = 1;
			if(m_drawPathAxis)
				mask = mask | 4;
			if(this->keyFrameInterpolator(0))
				this->keyFrameInterpolator(0)->drawPath(mask, 6, m_drawPathScale);
		}
	}
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

bool Camera::isLinkedWithView(View* view)
{
	return l_views.contains(view);
}

void Camera::fitParamWith(View* view)
{
	setScreenWidthAndHeight(view->width(), view->height());
}

void Camera::saveSnapshot(QString snapPathName)
{
	foreach(View* view, l_views)
		view->saveSnapshot(snapPathName + view->getName() + '_' + QString::number(m_snapCount) + ".jpg", true);
	++m_snapCount;
}

void Camera::updateGL()
{
	foreach(View* view, l_views)
		view->updateGL();
}

//void Camera::viewShowButton(bool b)
//{
//	foreach(View* view, l_views)
//		view->setShowButtons(b);
//}
