#include "camera.h"


#include "scene.h"


Camera::Camera(View* v) :
	m_name("camera"),
	m_draw(false),
	m_drawFarPlane(false),
	m_drawScale(1.0),
	m_drawUserPath(false),
	m_drawUserPathAxis(false),
	m_drawUserPathScale(1.0),
	m_lastWorkingView(v),
	m_snapCount(0)
{
	if(v){
		l_views.push_back(v);
	}
	this->setZClippingCoefficient(100);
}

Camera::Camera(View* v, Camera c) : qglviewer::Camera(c),
		m_name("camera"),
		m_draw(false),
		m_drawFarPlane(false),
		m_drawScale(1.0),
		m_drawUserPath(false),
		m_drawUserPathAxis(false),
		m_drawUserPathScale(1.0),
		m_lastWorkingView(v),
		m_snapCount(0)
{
	if(v){
		l_views.push_back(v);
	}
	this->setZClippingCoefficient(100);
}

Camera::~Camera(){}


void Camera::draw(){
	if(m_draw){
		qglviewer::Camera::draw(m_drawFarPlane,m_drawScale);
		if(m_drawUserPath){
			int mask=1;
			if(m_drawUserPathAxis){
				mask= mask | 4;
			}
			if(this->keyFrameInterpolator(0)){
				this->keyFrameInterpolator(0)->drawPath(mask,6,m_drawUserPathScale);
			}
		}
	}
}

void Camera::takenFrom(View* v){
	int i= l_views.indexOf(v);
	if(i>=0){
		l_views.takeAt(i);
	}
}

void Camera::sharedWith(View* v){
	if(!l_views.contains(v)){
		l_views.push_back(v);
	}
}

void Camera::fitParamWith(View* v){
	if(v!=m_lastWorkingView){
		setScreenWidthAndHeight(v->width(), v->height());
		m_lastWorkingView= v;
	}
}

void Camera::saveSnapshot(QString snapPathName){
	foreach(View* view, l_views){
		view->saveSnapshot(snapPathName+view->getName()+'_'+QString::number(m_snapCount)+".jpeg", true);
	}
	m_snapCount++;
}

void Camera::updateGL(){
	foreach(View* view, l_views){
		view->updateGL();
	}
}

void Camera::viewShowButton(bool b){
	foreach(View* view, l_views){
		view->setShowButtons(b);
	}
}
