#include "visualPlugin.h"

#include <set>
#include <Utils/GLSLShader.h>

VisualPlugin::VisualPlugin() :
	m_waitedScene(UNLIMITED_NUMBER_OF_SCENES),
	m_allowSameViewSeveralTime(false)
{
	this->m_window= NULL;
	this->m_windowInitializing=false;
	this->m_mapNumber=UNLIMITED_MAP_NUMBER;
};

VisualPlugin::~VisualPlugin(){
	foreach(Scene* scene, l_scene){
		scene->suppressLinkWith(this);
	}

	foreach(Scene* scene, l_recievedScene){
		scene->suppressLinkWith(this);
	}
	this->unloadDependantPlugins();
	this->suppressAllDependencieLinks();

	foreach(QWidget* customWidget, l_customWidget){
		this->removeTabInDock(customWidget);
	}

	this->deleteActions();
};

void VisualPlugin::cb_updateMatrix(View* view){
	if(view){
		glm::mat4 model(view->getCurrentModelViewMatrice());
		glm::mat4 proj(view->getCurrentProjectionMatrice());

		for(std::set< std::pair<void*, Utils::GLSLShader*> >::iterator it = Utils::GLSLShader::m_registeredShaders.begin();
			it != Utils::GLSLShader::m_registeredShaders.end();
			++it)
		{
			if ((it->first == NULL) || (it->first == this))
			{
				it->second->updateMatrices(proj, model);
			}
		}
	}
}

void VisualPlugin::updateGL(){
	for(std::list<Scene*>::iterator it= l_scene.begin(); it!=l_scene.end(); ++it){
		(*it)->updateGL();
	}
	for(QList<Scene*>::iterator it= l_recievedScene.begin(); it!=l_recievedScene.end(); ++it){
		(*it)->updateGL();
	}
}

void VisualPlugin::updateGL(Scene* scene){
	scene->updateGL();
}

void VisualPlugin::setWindow(Window* window){
	this->m_window= window;
}

void VisualPlugin::setName(QString name){
	this->m_name= name;
}

void VisualPlugin::setPluginFilePath(QString path){
	this->m_pluginPathFile= path;
}

bool VisualPlugin::activate(bool initializing){
	this->m_windowInitializing= initializing;
	bool r= activate();
	this->m_windowInitializing= false;
	return r;
}

bool VisualPlugin::associateScene(QString glv_name, Scene* &scene, bool cb_initGL){
	if(m_window){
		if(m_window->associateSceneWithPlugin(glv_name, this, scene, cb_initGL)){
			l_scene.push_back(scene);
			return true;
		}
		else{
			return false;
		}
	}
	else{
		if(!m_name.isEmpty()){
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
		}
		else{
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW;
		}
		return false;
	}
}


bool VisualPlugin::addNewScene(QString name, Scene* &scene){
	if(m_window){
		m_window->addNewSceneFromPlugin(name, this, scene);
		if(scene){
			l_scene.push_back(scene);
			return true;
		}
		return false;
	}
	else{
		if(!m_name.isEmpty()){
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
		}
		else{
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW;
		}
		return false;
	}
}

bool VisualPlugin::addNewSceneDialog(QString name, Scene* &scene){
	if(m_window){
		if(!m_windowInitializing)
			m_window->addNewSceneFromPluginDialog(name, this, scene);
		else
			m_window->addNewSceneFromPlugin(name, this, scene);
		if(scene){
			l_scene.push_back(scene);
			return true;
		}
		return false;
	}
	else{
		if(!name.isEmpty()){
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(name);
		}
		else{
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW;
		}
		return false;
	}
}

void VisualPlugin::deleteLinkWith(Scene* scene){
	bool b=false;
	for(std::list<Scene*>::iterator it= l_scene.begin(); it!= l_scene.end(); ++it){
		if((*it)==scene){
			l_scene.erase(it);
//			if(scene->creator()==((Plugin*)this)){
//				m_window->removeScene(scene->getName());
//			}
			b=true;
			break;
		}
	}

	if(!b){
		int i= l_recievedScene.indexOf(scene);
		if(i>=0){
			cb_removingScene(scene);
			l_recievedScene.takeAt(i);
			if(m_waitedScene!=UNLIMITED_NUMBER_OF_SCENES){
				++m_waitedScene;
			}
		}
	}
}

void VisualPlugin::addDependantPlugin(Plugin* dependant){
	this->l_dependantPlugins.push_back(dependant);
}

bool VisualPlugin::addWidgetInDockTab(QWidget* newTabWidget, QString tabText){
	if(m_window){
		m_window->addWidgetInDockTab(newTabWidget, tabText);
		l_customWidget.push_back(newTabWidget);
		return true;
	}
	else{
		System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
		return false;
	}
}

void VisualPlugin::removeTabInDock(QWidget* tabWidget){
	if(m_window){
		int i;
		if((i=l_customWidget.indexOf(tabWidget))>=0){
			l_customWidget.takeAt(i);
		}
		m_window->removeTabInDock(tabWidget);
	}
}

bool VisualPlugin::addReferencedMap(QString map_name, MapHandler* map){
	if(m_window){
		bool r= m_window->addReferencedMap(map_name, map);
		if(r) l_map.push_back(map);
		return r;
	}
	else{
		System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
		return false;
	}
}

MapHandler*  VisualPlugin::getReferencedMap(QString map_name){
	if(m_window){
		return m_window->getReferencedMap(map_name);
	}
	else{
		System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
		return NULL;
	}
}

Plugin* VisualPlugin::addDependencie(QString dependencie){
	Plugin* plugin;
	if((plugin=m_window->checkPluginDependencie(dependencie, this))){
		this->l_dependencies.push_back(plugin);
		return plugin;
	}
	else{
		return NULL;
	}
}

void VisualPlugin::deleteDependantLink(Plugin* dependantPlugin){
	l_dependantPlugins.removeAll(dependantPlugin);
}

QStringList VisualPlugin::getDependantPluginNames(){
	QStringList l;
	for(QList<Plugin*>::iterator it= l_dependantPlugins.begin(); it!=l_dependantPlugins.end(); ++it){
		l.push_back((*it)->getName());
	}
	return l;
}

void VisualPlugin::recieveScene(Scene* scene){
	if(scene!=NULL && (m_waitedScene>0 || m_waitedScene==UNLIMITED_NUMBER_OF_SCENES) &&
			( !scene->isLinkedWith(this) || m_allowSameViewSeveralTime))
	{
		std::cout << "LINK" << std::endl;
		if(m_waitedScene!=UNLIMITED_NUMBER_OF_SCENES){
			--m_waitedScene;
		}
		std::cout << "first index of null: " << l_recievedScene.indexOf(NULL) << std::endl;
		std::cout << "size: " << l_recievedScene.size() << std::endl;
		int i= l_recievedScene.indexOf(NULL);
		if(i>=0){
			l_recievedScene.replace(l_recievedScene.indexOf(NULL),scene);
		}
		else{
			l_recievedScene.push_back(scene);
		}

		scene->associateNewPlugin(this);
		scene->updateGL();

		emit recievedScene(scene);
		cb_recievedScene(scene);
	}
}

void VisualPlugin::waitingForScene(int nb){
	if(nb>=0 && l_recievedScene.size()<=nb){
		m_waitedScene= nb-l_recievedScene.size();
		int t= l_recievedScene.size();
		std::cout << "duh?" << std::endl;
		for(int i=t; i<nb; ++i){
			std::cout << i << " : push_back(NULL)" << std::endl;
			l_recievedScene.push_back(NULL);
		}
		std::cout << "size: " << l_recievedScene.size() << std::endl;
	}
	else if(nb==UNLIMITED_NUMBER_OF_SCENES){
		m_waitedScene= UNLIMITED_NUMBER_OF_SCENES;
	}
}

void VisualPlugin::suppressDependencieLink(Plugin* dependencie){
	dependencie->deleteDependantLink(this);
	l_dependencies.removeAll(dependencie);
}

void VisualPlugin::suppressAllDependencieLinks(){
	foreach(Plugin* plugin, l_dependencies){
		this->suppressDependencieLink(plugin);
	}
}

void VisualPlugin::unloadDependantPlugins(){
	while(!l_dependantPlugins.empty()){
		if(l_dependantPlugins.back())
			m_window->unloadPlugin(l_dependantPlugins.back()->getName());
	}
}

bool VisualPlugin::addMap(MapHandler* map){
	if((m_mapNumber==UNLIMITED_MAP_NUMBER || l_map.size()<m_mapNumber) && map && !l_map.contains(map)){
		l_map.push_back(map);
		cb_recievedMap(map);
		return true;
	}
	else{
		return false;
	}
}

MapHandler* VisualPlugin::takeMap(MapHandler* map){
	int i= l_map.indexOf(map);
	if(i>=0){
		cb_removingMap(map);
		return l_map.takeAt(i);
	}
	else{
		return NULL;
	}
}

void VisualPlugin::suppressDependantLink(Plugin* dependant){
	int i= l_dependantPlugins.indexOf(dependant);
	if(i>=0){
		dependant->suppressDependencieLink(this);
	}
}

void VisualPlugin::suppressAllDependantLinks(){
	foreach(Plugin* plugin, l_dependantPlugins){
		plugin->suppressDependencieLink(this);
	}
}


QAction* VisualPlugin::addMenuAction(QString menuPath, const char* method){
	QAction* action= new QAction(m_window);
	QObject::connect(action, SIGNAL(triggered()), this, method);
	if(m_window->addMenuAction(menuPath,action)){
		if(!l_action.contains(action))
			l_action.push_back(action);
		return action;
	}
	else{
		QObject::disconnect(action, SIGNAL(triggered()), m_window, method);
		delete action;
		return NULL;
	}
}

bool VisualPlugin::addMenuAction(QString menuPath, QAction* action){
	if(m_window->addMenuAction(menuPath,action)){
		if(!l_action.contains(action))
			l_action.push_back(action);
		return true;
	}
	return false;
}

void VisualPlugin::deleteActions(){
	for(QList<QAction*>::iterator it= l_action.begin(); it!=l_action.end(); ++it){
		if((*it)!=NULL){
			m_window->deleteMenuAction((*it));
			(*it)=NULL;
		}
	}
}

QAction* VisualPlugin::addToolbarAction(const char* method, QIcon icon){
	QAction* action= new QAction(m_window);
	QObject::connect(action, SIGNAL(triggered()), this, method);
	if(m_window->addToolbarAction(action)){
		action->setIcon(icon);
		if(!l_action.contains(action)){
			l_action.push_back(action);
		}
		return action;
	}
	else{
		QObject::disconnect(action, SIGNAL(triggered()), m_window, method);
		delete action;
		return NULL;
	}
}

bool VisualPlugin::addToolbarAction(QAction* action){
	if(m_window->addToolbarAction(action)){
		if(!l_action.contains(action))
			l_action.push_back(action);
		return true;
	}
	return false;
}

ViewButton* VisualPlugin::addSceneNewViewButton(Scene* scene, QString imagePath){
	ViewButton* vb= new ViewButton(imagePath,scene->getView(0));
	if(scene->addCustomViewButton(vb)){
		return vb;
	}
	return NULL;
}

bool VisualPlugin::addSceneViewButton(Scene* scene, ViewButton* viewButton){
	if(scene && viewButton){
		return (scene->addCustomViewButton(viewButton));
	}
	return false;
}

void VisualPlugin::takeSceneViewButton(Scene* scene, ViewButton* viewButton){
	if(scene){
		scene->takeCustomViewButton(viewButton);
	}
}
