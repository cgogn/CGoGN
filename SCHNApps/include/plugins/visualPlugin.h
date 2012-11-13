#ifndef _VISUALPLUGIN_H_
#define _VISUALPLUGIN_H_

#include "plugin.h"


class VisualPlugin : public QObject, public Plugin{
	Q_OBJECT
public:
	VisualPlugin();
	~VisualPlugin();

	virtual void cb_updateMatrix(View* view);

	virtual void cb_redraw(Scene* scene) =0;

	virtual void cb_initGL(Scene* scene) =0;

	void updateGL();
	void updateGL(Scene* scene);



	bool cb_keyPress(Scene* scene, int event) {return false;}

	bool cb_keyRelease(Scene* scene, int event) {return false;}

	bool cb_mousePress(Scene* scene, int button, int x, int y) {return false;}

	bool cb_mouseRelease(Scene* scene, int button, int x, int y) {return false;}

	bool cb_mouseClick(Scene* scene, int button, int x, int y) {return false;}

	bool cb_mouseMove(Scene* scene, int buttons, int x, int y) {return false;}

	bool cb_wheelEvent(Scene* scene, int delta, int x, int y) {return false;}


	virtual void cb_recievedScene(Scene* scene){}
	virtual void cb_removingScene(Scene* scene){}

	virtual void cb_recievedMap(MapHandler* map){}
	virtual void cb_removingMap(MapHandler* map){}



	QString getName() {return m_name;}

	QString getPluginFilePath() {return m_pluginPathFile;}

	void addDependantPlugin(Plugin* dependant);
	void deleteDependantLink(Plugin* dependantPlugin);

	bool hasDependantPlugins(){ return !l_dependantPlugins.isEmpty(); }

	QStringList getDependantPluginNames();



	bool isWaitingForScene(){return (m_waitedScene>0 || m_waitedScene==UNLIMITED_NUMBER_OF_SCENES);}
	void recieveScene(Scene* scene);

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

	bool hasManualLinkWith(Scene* scene){std::cout << "\ti" << std::endl;return l_recievedScene.contains(scene);}

	bool addMap(MapHandler* map);
	MapHandler* takeMap(MapHandler* map);
	QList<MapHandler*> workingMaps(){return l_map;}
	void setMapNumber(int n){if(n>=l_map.size() || n==UNLIMITED_MAP_NUMBER) m_mapNumber=n;}
	int remainingMapNumber(){return (m_mapNumber!=UNLIMITED_MAP_NUMBER)?(m_mapNumber-l_map.size()):(m_mapNumber);}


	enum {UNLIMITED_NUMBER_OF_SCENES=-1};
protected:

	template<typename T>
	T* getDependencie(QString dependencie){ return( (T*)(this->addDependencie(dependencie)) ); }


	std::list<Scene*> l_scene;

	bool associateScene(QString glv_name, Scene* &scene, bool cb_initGL=false);

	bool addNewScene(QString glv_name, Scene* &scene);
	bool addNewSceneDialog(QString name, Scene* &scene);

	bool addWidgetInDockTab(QWidget* newTabWidget, QString tabText);
	void removeTabInDock(QWidget* tabWidget);

	bool addReferencedMap(QString map_name, MapHandler* map);
	template<typename T>
	MapHandler* addNewReferencedMap(QString map_name, T* &map){
		if(m_window){
			MapHandler* vh= m_window->addNewReferencedMap<T>(map_name, map);
			if(vh) l_map.push_back(vh);
			return vh;
		}
		else{
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
			return NULL;
		}
	}

	MapHandler*  getReferencedMap(QString map_name);


	QList<QAction*> l_action;

	QList<QWidget*> l_customWidget;

	QAction* addMenuAction(QString menuPath, const char* method);
	bool addMenuAction(QString menuPath, QAction* action);
	void deleteActions();
	QAction* addToolbarAction(const char* method, QIcon icon= QIcon());
	bool addToolbarAction(QAction* action);


	int m_waitedScene;
	QList<Scene*> l_recievedScene;

	bool m_allowSameViewSeveralTime;
	void allowSamerViewSeveralTime(bool b){m_allowSameViewSeveralTime=b;};

	void waitingForScene(int nb=UNLIMITED_NUMBER_OF_SCENES);

	ViewButton* addSceneNewViewButton(Scene* scene, QString imagePath);
	bool addSceneViewButton(Scene* scene, ViewButton* viewButton);
	void takeSceneViewButton(Scene* scene, ViewButton* viewButton);

	QList<ViewButton*> l_viewButtons;

signals:
	void recievedScene(Scene*);
};

#endif
