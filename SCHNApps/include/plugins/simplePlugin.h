#ifndef _SIMPLEPLUGIN_H_
#define _SIMPLEPLUGIN_H_

#include "plugin.h"


class SimplePlugin : public QObject, public Plugin{
	Q_OBJECT
public:
	SimplePlugin();
	~SimplePlugin();

	void cb_updateMatrix(View* view){}

	void cb_redraw(Scene* scene){}

	void cb_initGL(Scene* scene){}

	void updateGL(){}
	void updateGL(Scene* scene){}



	bool cb_keyPress(Scene* scene, int event) {return false;}

	bool cb_keyRelease(Scene* scene, int event) {return false;}

	bool cb_mousePress(Scene* scene, int button, int x, int y) {return false;}

	bool cb_mouseRelease(Scene* scene, int button, int x, int y) {return false;}

	bool cb_mouseClick(Scene* scene, int button, int x, int y) {return false;}

	bool cb_mouseMove(Scene* scene, int buttons, int x, int y) {return false;}

	bool cb_wheelEvent(Scene* scene, int delta, int x, int y) {return false;}


	void cb_recievedMap(CGoGN::GenericMap* map){}

	void cb_removingMap(CGoGN::GenericMap* map){}



	QString getName() {return m_name;}

	QString getPluginFilePath() {return m_pluginPathFile;}

	void addDependantPlugin(Plugin* dependant);
	void deleteDependantLink(Plugin* dependantPlugin);

	bool hasDependantPlugins(){ return !l_dependantPlugins.isEmpty(); }

	QStringList getDependantPluginNames();



	bool isWaitingForScene(){return false;}
	void recieveScene(Scene* scene){}

	void setWindow(Window* window);

	void setName(QString name);

	void setPluginFilePath(QString path);


	bool activate(bool initializing);
	virtual	bool activate() =0;
	virtual	void disable() =0;

	void deleteLinkWith(Scene* scene);


	Plugin* addDependencie(QString dependencie);
	void suppressDependencieLink(Plugin* dependencie);
	void suppressAllDependencieLinks();
	void suppressDependantLink(Plugin* dependant);
	void suppressAllDependantLinks();

	void unloadDependantPlugins();

	bool hasManualLinkWith(Scene* scene){return false;}

	bool addMap(CGoGN::GenericMap* map);
	CGoGN::GenericMap* takeMap(CGoGN::GenericMap* map);
	QList<CGoGN::GenericMap*> workingMaps(){return l_map;}
	void setMapNumber(int n){if(n>=l_map.size() || n==UNLIMITED_MAP_NUMBER) m_mapNumber=n;}
	int remainingMapNumber(){return (m_mapNumber!=UNLIMITED_MAP_NUMBER)?(m_mapNumber-l_map.size()):(m_mapNumber);}


	enum {UNLIMITED_NUMBER_OF_SCENES=-1};
protected:

	template<typename T>
	T* getDependencie(QString dependencie){ return( (T*)(this->addDependencie(dependencie)) ); }


	bool addWidgetInDockTab(QWidget* newTabWidget, QString tabText);
	void removeTabInDock(QWidget* tabWidget);

	bool addReferencedMap(QString map_name, CGoGN::GenericMap* map);
	template<typename T>
	bool addNewReferencedMap(QString map_name, T* &map){
		if(m_window){
			bool r= m_window->addNewReferencedMap<T>(map_name, map);
			if(r) l_map.push_back(map);
			return r;
		}
		else{
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
			return false;
		}
	}
	template<typename T>
	T* getReferencedMap(QString map_name){
		if(m_window){
			return m_window->getReferencedMap<T>(map_name);
		}
		else{
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
			return NULL;
		}
	}


	QList<QAction*> l_action;

	QAction* addMenuAction(QString menuPath, const char* method);
	void deleteMenuActions();

};
