#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <QtPlugin>


#include <iostream>
#include <list>

#include "window.h"
#include "view.h"
#include "camera.h"
#include "system.h"
#include "mapHandler.h"
#include "vboHandler.h"
#include "scene.h"

class Plugin{
public:
	enum {UNLIMITED_MAP_NUMBER=-1};

	virtual ~Plugin(){}

	virtual void cb_updateMatrix(View* view) =0;

	virtual void cb_redraw(Scene* scene) =0;

	virtual void cb_initGL(Scene* scene) =0;



	virtual bool cb_keyPress(Scene* scene, int event) =0;

	virtual bool cb_keyRelease(Scene* scene, int event) =0;

	virtual bool cb_mousePress(Scene* scene, int button, int x, int y) =0;

	virtual bool cb_mouseRelease(Scene* scene, int button, int x, int y) =0;

	virtual bool cb_mouseClick(Scene* scene, int button, int x, int y) =0;

	virtual bool cb_mouseMove(Scene* scene, int buttons, int x, int y) =0;

	virtual bool cb_wheelEvent(Scene* scene, int delta, int x, int y) =0;


	virtual void cb_recievedMap(MapHandler* map)=0;

	virtual void cb_removingMap(MapHandler* map)=0;



	virtual QString getName() =0;

	virtual QString getPluginFilePath() =0;

	virtual bool hasDependantPlugins() =0;
	virtual QStringList getDependantPluginNames() =0;



	virtual bool isWaitingForScene() =0;
	virtual void recieveScene(Scene* scene) =0;



	virtual void setWindow(Window* window) =0;

	virtual void setName(QString name) =0;

	virtual void setPluginFilePath(QString path)= 0;

	virtual bool activate(bool initializing= false) =0;
	virtual void disable() =0;

	virtual void deleteLinkWith(Scene* scene) =0;

	virtual void addDependantPlugin(Plugin* dependant) =0;
	virtual void deleteDependantLink(Plugin* dependantPlugin) =0;

	virtual Plugin* addDependencie(QString dependencie) =0;
	virtual void suppressDependencieLink(Plugin* dependencie) =0;
	virtual void suppressAllDependencieLinks() =0;
	virtual void suppressDependantLink(Plugin* dependant) =0;
	virtual void suppressAllDependantLinks() =0;

	virtual void unloadDependantPlugins() =0;

	virtual bool hasManualLinkWith(Scene* scene)=0;

	virtual bool addMap(MapHandler* map)=0;
	virtual MapHandler* takeMap(MapHandler* map)=0;
	virtual QList<MapHandler*> workingMaps()=0;
	virtual void setMapNumber(int n)=0;
	virtual int remainingMapNumber()=0;

protected:
	Window* m_window;
	QString m_name;
	QString m_pluginPathFile;
	QList<Plugin*> l_dependencies;
	QList<Plugin*> l_dependantPlugins;
	bool m_windowInitializing;
	int m_mapNumber;
	QList<MapHandler*> l_map;
};

Q_DECLARE_INTERFACE(Plugin, "Window.Plugin")

#endif
