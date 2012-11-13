#include "simplePlugin.h"

#include <set>
#include <Utils/GLSLShader.h>

SimplePlugin::SimplePlugin()
{
	this->m_window= NULL;
	this->m_windowInitializing=false;
	this->m_mapNumber=UNLIMITED_MAP_NUMBER;
};

SimplePlugin::~SimplePlugin(){
};

void SimplePlugin::setWindow(Window* window){
	this->m_window= window;
}

void SimplePlugin::setName(QString name){
	this->m_name= name;
}

void SimplePlugin::setPluginFilePath(QString path){
	this->m_pluginPathFile= path;
}

bool SimplePlugin::activate(bool initializing){
	this->m_windowInitializing= initializing;
	bool r= activate();
	this->m_windowInitializing= false;
	return r;
}




void SimplePlugin::addDependantPlugin(Plugin* dependant){
	this->l_dependantPlugins.push_back(dependant);
}

bool SimplePlugin::addWidgetInDockTab(QWidget* newTabWidget, QString tabText){
	if(m_window){
		m_window->addWidgetInDockTab(newTabWidget, tabText);
		return true;
	}
	else{
		System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
		return false;
	}
}

void SimplePlugin::removeTabInDock(QWidget* tabWidget){
	if(m_window){
		m_window->removeTabInDock(tabWidget);
	}
}

bool SimplePlugin::addReferencedMap(QString map_name, CGoGN::GenericMap* map){
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

Plugin* SimplePlugin::addDependencie(QString dependencie){
	Plugin* plugin;
	if((plugin=m_window->checkPluginDependencie(dependencie, this))){
		this->l_dependencies.push_back(plugin);
		return plugin;
	}
	else{
		return NULL;
	}
}

void SimplePlugin::deleteDependantLink(Plugin* dependantPlugin){
	l_dependantPlugins.removeAll(dependantPlugin);
}

QStringList SimplePlugin::getDependantPluginNames(){
	QStringList l;
	for(QList<Plugin*>::iterator it= l_dependantPlugins.begin(); it!=l_dependantPlugins.end(); ++it){
		l.push_back((*it)->getName());
	}
	return l;
}


void SimplePlugin::suppressDependencieLink(Plugin* dependencie){
	dependencie->deleteDependantLink(this);
	l_dependencies.removeAll(dependencie);
}

void SimplePlugin::suppressAllDependencieLinks(){
	foreach(Plugin* plugin, l_dependencies){
		this->suppressDependencieLink(plugin);
	}
}

void SimplePlugin::unloadDependantPlugins(){
	while(!l_dependantPlugins.empty()){
		if(l_dependantPlugins.back())
			m_window->unloadPlugin(l_dependantPlugins.back()->getName());
	}
}

bool SimplePlugin::addMap(CGoGN::GenericMap* map){
	if((m_mapNumber==UNLIMITED_MAP_NUMBER || l_map.size()<m_mapNumber) && map && !l_map.contains(map)){
		l_map.push_back(map);
		cb_recievedMap(map);
		return true;
	}
	else{
		return false;
	}
}

CGoGN::GenericMap* SimplePlugin::takeMap(CGoGN::GenericMap* map){
	int i= l_map.indexOf(map);
	if(i>=0){
		cb_removingMap(map);
		return l_map.takeAt(i);
	}
	else{
		return NULL;
	}
}

void SimplePlugin::suppressDependantLink(Plugin* dependant){
	int i= l_dependantPlugins.indexOf(dependant);
	if(i>=0){
		dependant->suppressDependencieLink(this);
	}
}

void SimplePlugin::suppressAllDependantLinks(){
	foreach(Plugin* plugin, l_dependantPlugins){
		plugin->suppressDependencieLink(this);
	}
}


QAction* SimplePlugin::addMenuAction(QString menuPath, const char* method){
	QAction* action= new QAction(m_window);
	QObject::connect(action, SIGNAL(triggered()), this, method);
	if(m_window->addMenuAction(menuPath,action)){
		l_action.push_back(action);
		return action;
	}
	else{
		QObject::disconnect(action, SIGNAL(triggered()), m_window, method);
		delete action;
		return NULL;
	}
}

void SimplePlugin::deleteMenuActions(){
	for(QList<QAction*>::iterator it= l_action.begin(); it!=l_action.end(); ++it){
		m_window->deleteMenuAction((*it));
	}
}
