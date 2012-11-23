#include "simplePlugin.h"
#include "Utils/GLSLShader.h"

#include <set>

SimplePlugin::SimplePlugin()
{
	this->m_window= NULL;
	this->m_windowInitializing=false;
	this->m_mapNumber=UNLIMITED_MAP_NUMBER;
};

SimplePlugin::~SimplePlugin(){
};

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
