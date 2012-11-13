#include "scene.h"

#include <QGLContext>
#include <iostream>

#include "camera.h"
#include "view.h"
#include "context.h"
#include "window.h"
#include "plugin.h"
#include "vboHandler.h"


Scene::Scene(QString name, Window* window, Camera* sharedCamera) :
	m_window(window),
	m_name(name),
	m_creator(NULL),
	m_context(window->context())
{


	View* view= new View(this,name+"_view1",sharedCamera,0,m_context);

	l_view.push_back(view);

	view->enableLinking();
	view->enableCameraGesture();
	view->enableViewClose();
}

Scene::Scene(QString name, Plugin* plugin, Window* window) :
			m_window(window),
			m_name(name),
			m_creator(plugin),
			m_context(window->context())
{
	View* view= new View(this,name+"_view1",0,0,m_context);

	l_view.push_back(view);

	l_plugin.push_back(plugin);

	view->enableLinking();
	view->enableUnlinking();
	view->enableCameraGesture();
}

Scene::~Scene(){
	while(!l_view.isEmpty()){
		View* view= l_view.takeFirst();
		delete view;
	}
//	while(!l_vbo.isEmpty()){
//		VBOHandler* vbo= l_vbo.first();
//		if(!vbo->isShared()){
//			vbo->unshareWith(this);
//			delete vbo;
//		}
//		else{
//			vbo->unshareWith(this);
//		}
//	}
	while(!l_plugin.isEmpty()){
		suppressLinkWith(l_plugin.last());
	}
}

void Scene::updateGL(View* view){
	foreach(View* v, l_view){
		std::cout << "a ___" << std::endl;
		if(v!=view){
			v->simpleUpdate();
		}
	}
}

void Scene::draw(View* view){
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	foreach(View* v, l_view){
		v->drawCameras(view);
	}
	glPopAttrib();


	glPushAttrib(GL_ALL_ATTRIB_BITS);
	foreach(Plugin* plugin, l_plugin){
		plugin->cb_updateMatrix(view);
		plugin->cb_redraw(this);
	}
	glPopAttrib();
}

void Scene::init(){
	foreach(Plugin* plugin, l_plugin){
		plugin->cb_initGL(this);
	}
}

bool Scene::keyPressEvent(QKeyEvent* event){
	int k= event->key();

	//for each plugin that operated on this view,
	//a callback is made on their corresponding method.
	//if none of the methods operates on this view (they return false)
	//the the default behavior is adopted
	foreach(Plugin* plugin, l_plugin){
		if(plugin->cb_keyPress(this,k)){
			return true;
		}
	}
	return false;
}

bool Scene::keyReleaseEvent(QKeyEvent* event){
	int k= event->key();

	//for each plugin that operated on this view,
	//a callback is made on their corresponding method.
	//if none of the methods operates on this view (they return false)
	//the the default behavior is adopted
	foreach(Plugin* plugin, l_plugin){
		if(plugin->cb_keyRelease(this,k)){
			return true;
		}
	}
	return false;


}

bool Scene::mousePressEvent(QMouseEvent* event)
{
	//for each plugin that operated on this view,
	//a callback is made on their corresponding method.
	//if none of the methods operates on this view (they return false)
	//the the default behavior is adopted
	foreach(Plugin* plugin, l_plugin){
		if(plugin->cb_mousePress(this,event->button(),event->x(),event->y())){
			return true;
		}
	}
	return false;

}

bool Scene::mouseReleaseEvent(QMouseEvent* event){

	//for each plugin that operated on this view,
	//a callback is made on their corresponding method.
	//if none of the methods operates on this view (they return false)
	//the the default behavior is adopted
	foreach(Plugin* plugin, l_plugin){
		if(plugin->cb_mouseRelease(this,event->button(),event->x(),event->y())){
			return true;
		}
	}
	return false;
}

bool Scene::mouseMoveEvent(QMouseEvent* event){

	//for each plugin that operated on this view,
	//a callback is made on their corresponding method.
	//if none of the methods operates on this view (they return false)
	//the the default behavior is adopted
	foreach(Plugin* plugin, l_plugin){
		if(plugin->cb_mouseMove(this,event->button(),event->x(),event->y())){
			return true;
		}
	}
	return false;
}

bool Scene::wheelEvent(QWheelEvent* event){

	//for each plugin that operated on this view,
	//a callback is made on their corresponding method.
	//if none of the methods operates on this view (they return false)
	//the the default behavior is adopted
	foreach(Plugin* plugin, l_plugin){
		if(plugin->cb_wheelEvent(this,event->delta(),event->x(),event->y())){
			return true;
		}
	}
	return false;

}


View* Scene::getView(int num){
	if(num<0 || num>l_view.size()){
		return NULL;
	}
	else{
		return l_view[num];
	}
}

View* Scene::addNewView(Camera* c){
	View* view= new View(this,m_name+"_view1"+QString::number(countViews()),c,l_view.first(), m_context);

	l_view.push_back(view);

	if(m_window){
		m_window->addNewSceneView(this, view);
	}

	view->enableCameraGesture();
	view->enableViewClose();
	if(!isManual() && l_view.size()==2){
		l_view.first()->enableViewClose();
	}
	view->enableLinking();
	if(!l_plugin.isEmpty()){
		view->enableUnlinking();
	}

	foreach(ViewButton* button, l_viewButton){
		view->addCustomViewButton(button);
	}

	return view;
}

void Scene::deleteView(View* view){
	if(l_view.size()==1 && view==l_view.first()){
		m_window->removeScene(m_name);
	}
	else{
		int i= l_view.indexOf(view);
		if(i>=0){
			std::cout << "A" << std::endl;
			delete (l_view.takeAt(i));
			if(!isManual()){
				std::cout << "B" << std::endl;
				if(l_view.size()==1){
					l_view.first()->enableViewClose(false);
				}
			}
		}
	}
}

void Scene::viewClickedButton(View* view, ViewButton* viewButton){
	if(view && viewButton){
		emit(viewButtonClicked(view, viewButton));
	}
}

void Scene::associateNewPlugin(Plugin* plugin, bool callBackInitGL){
	//the plugin is put in the operating plugin list
	l_plugin.push_back(plugin);

	//if the caller desires so (callBackInitGL=true), a call back on the new operating plugin's
	//initGL methode is made
	if(callBackInitGL){
		plugin->cb_initGL(this);
	}

	this->updateGL();
}

void Scene::suppressLinkWith(Plugin* plugin){
	//if the given plugin is the creator of this view
	if( !this->isManual() && m_creator==plugin){
		//this view will be destroyed
		//so we first remove all the other plugins (starting from the end of the list)
		//and we suppress their link with this view
		while(!l_plugin.empty()){
			Plugin* plug_t= l_plugin.back();
			l_plugin.pop_back();
			plug_t->deleteLinkWith(this);
		}
		m_window->removeScene(m_name);
	}
	//if the given plugin is not the creator
	else{
		int i= l_plugin.indexOf(plugin);
		if(i>=0){
			plugin->deleteLinkWith(this);
			l_plugin.takeAt(i);
		}

		int nbManualPlugin=0;
		foreach(Plugin* plugin, l_plugin){
			if(plugin->hasManualLinkWith(this)){
				++nbManualPlugin;
			}
		}
		if(nbManualPlugin<1){
			foreach(View* view, l_view){
				view->enableUnlinking(false);
			}
		}
	}
}

void Scene::linkWithPlugin(){
	m_window->linkDialog(this);
	if((isManual() && l_plugin.size()>0) || (!isManual() && l_plugin.size()>1)){
		foreach(View* view, l_view){
			view->enableUnlinking();
		}
	}
}

void Scene::unlinkPlugin(){
	QList<Plugin*> dependingPlugins(l_plugin);
	QList<Plugin*>::iterator it= dependingPlugins.begin();
	while(it!=dependingPlugins.end()){
		if(!(*it)->hasManualLinkWith(this)){
			it= dependingPlugins.erase(it);
		}
		else{
			++it;
		}
	}

	m_window->unlinkDialog(this, dependingPlugins);
	if((isManual() && l_plugin.size()<=0) || (!isManual() && l_plugin.size()<=1)){
		foreach(View* view, l_view){
			view->enableUnlinking(false);
		}
	}
}

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

void Scene::firstViewFitSphere(float x, float y, float z, float radius){
	if(!l_view.empty()){
		l_view.first()->currentCamera()->fitSphere(qglviewer::Vec(x,y,z), radius);
	}
}

bool Scene::addCustomViewButton(ViewButton* viewButton){
	if(viewButton && !l_viewButton.contains(viewButton)){
		foreach(View* view, l_view){
			view->addCustomViewButton(viewButton);
		}

		l_viewButton.push_back((viewButton));

		return true;
	}

	return false;
}

ViewButton* Scene::takeCustomViewButton(ViewButton* viewButton){
	int i= l_viewButton.indexOf(viewButton);
	if(i>=0){
		foreach(View* view, l_view){
			view->removeCustomViewButton(viewButton);
		}

		return l_viewButton.takeAt(i);
	}
	else{
		return NULL;
	}
}

