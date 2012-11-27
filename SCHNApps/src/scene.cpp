#include "scene.h"

#include <QGLContext>
#include <iostream>

#include "camera.h"
#include "view.h"
//#include "context.h"
#include "vboHandler.h"
#include "window.h"
#include "plugin.h"

unsigned int Scene::sceneCount = 0;

Scene::Scene(const QString& name, Window* window) :
	m_name(name),
	m_window(window),
	m_context(window->getContext())
{
	++sceneCount;
}

Scene::~Scene()
{
	foreach(View* v, l_views)
		unlinkView(v);

	foreach(Plugin* p, l_plugins)
		unlinkPlugin(p);

	// view buttons
}

void Scene::initGL()
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_initGL(this);
}

void Scene::updateGL()
{
	foreach(View* v, l_views)
		v->updateGL();
}

void Scene::draw(View *v)
{
	QList<Camera*> cameras = m_window->getCameras();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	foreach(Camera* c, cameras)
		c->draw();
	glPopAttrib();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	foreach(Plugin* plugin, l_plugins)
	{
		plugin->cb_updateMatrix(v);
		plugin->cb_redraw(this);
	}
	glPopAttrib();
}

void Scene::keyPressEvent(QKeyEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_keyPress(this, event->key());
}

void Scene::keyReleaseEvent(QKeyEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_keyRelease(this, event->key());
}

void Scene::mousePressEvent(QMouseEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_mousePress(this, event->button(), event->x(), event->y());
}

void Scene::mouseReleaseEvent(QMouseEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_mouseRelease(this, event->button(), event->x(), event->y());
}

void Scene::mouseMoveEvent(QMouseEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_mouseMove(this, event->button(), event->x(), event->y());
}

void Scene::wheelEvent(QWheelEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_wheelEvent(this,event->delta(),event->x(),event->y());
}

/*********************************************************
 * MANAGE VIEWS
 *********************************************************/

void Scene::linkView(View* view)
{
	if(view)
	{
		l_views.push_back(view);
		view->setScene(this);
/*
		if(m_window)
			m_window->addNewSceneView(this, view);

		view->enableCameraGesture();
		view->enableViewClose();
		view->enableLinking();
		if(!l_plugins.isEmpty())
			view->enableUnlinking();

		foreach(ViewButton* button, l_viewButtons)
			view->addViewButton(button);
*/
	}
}

void Scene::unlinkView(View* view)
{
	l_views.removeOne(view);
	if(view)
		delete view;
}

View* Scene::getLinkedView(int num)
{
	if(num > l_views.size())
		return NULL;
	else
		return l_views[num];
}

/*********************************************************
 * MANAGE PLUGINS
 *********************************************************/

void Scene::linkPlugin(Plugin* plugin, bool callInitGL)
{
	if(plugin && !l_plugins.contains(plugin))
	{
		l_plugins.push_back(plugin);

		// if the caller desires so (callBackInitGL = true), a call to the plugin's
		// initGL method is made
		if(callInitGL)
			plugin->cb_initGL(this);

		updateGL();
	}
}

void Scene::unlinkPlugin(Plugin* plugin)
{
	if(l_plugins.removeOne(plugin))
		plugin->unlinkScene(this);
}

///*********************************************************
// * MANAGE VIEW BUTTONS
// *********************************************************/
//
//void Scene::addViewButton(ViewButton* viewButton)
//{
//	if(viewButton && !l_viewButtons.contains(viewButton))
//	{
//		foreach(View* view, l_views)
//			view->addViewButton(viewButton);
//
//		l_viewButtons.push_back((viewButton));
//	}
//}
//
//void Scene::removeViewButton(ViewButton* viewButton)
//{
//	if(l_viewButtons.removeOne(viewButton))
//	{
//		foreach(View* view, l_views)
//			view->removeViewButton(viewButton);
//	}
//}



//VBOHandler* Scene::addNewVBO(QString name){
//	foreach(VBOHandler* vbo, l_vbo){
//		if((*vbo)==name){
//			System::Error::code= System::Error::VBO_EXISTS_f(name, m_name);
//
//			return NULL;
//		}
//	}
//	VBOHandler* newVBO= new VBOHandler(name);
//	newVBO->shareWith(this);
//
//	return newVBO;
//}
//
//void Scene::addVBO(VBOHandler* vbo){
//	if(!l_vbo.contains(vbo)){
//		l_vbo.push_back(vbo);
//	}
//}
//
//VBOHandler* Scene::findVBO(QString name){
//	foreach(VBOHandler* vbo, l_vbo){
//		if(vbo->getName()==name){
//			return vbo;
//		}
//	}
//	return NULL;
//}
//
//VBOHandler* Scene::findFirstVBOMatching(QRegExp regexp){
//	foreach(VBOHandler* vbo, l_vbo){
//		if(vbo->getName().contains(regexp)){
//			return vbo;
//		}
//	}
//
//	return NULL;
//}
//
//QList<VBOHandler*> Scene::findVBOsMatching(QRegExp regexp){
//	QList<VBOHandler*> rlist;
//	foreach(VBOHandler* vbo, l_vbo){
//		if(vbo->getName().contains(regexp)){
//			rlist.push_back(vbo);
//		}
//	}
//
//	return rlist;
//}
//
//
//VBOHandler* Scene::takeVBO(VBOHandler* vbo){
//	int i= l_vbo.indexOf(vbo);
//	if(i>=0){
//		return l_vbo.takeAt(i);
//	}
//	else{
//		return NULL;
//	}
//}

void Scene::firstViewFitSphere(float x, float y, float z, float radius)
{
	if(!l_views.empty())
		l_views.first()->getCurrentCamera()->fitSphere(qglviewer::Vec(x,y,z), radius);
}
