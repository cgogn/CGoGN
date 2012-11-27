#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <QtPlugin>

#include <iostream>
#include <list>

#include "window.h"
#include "system.h"
#include "scene.h"
#include "view.h"
#include "camera.h"
#include "mapHandler.h"
#include "vboHandler.h"

class Plugin
{
public:
	enum { UNLIMITED_NUMBER_OF_MAPS = -1 };
	enum { UNLIMITED_NUMBER_OF_SCENES = -1 };

	Plugin(const QString& name, const QString& filePath, Window* window);
	virtual ~Plugin();

	virtual bool enable() = 0;
	virtual void disable() = 0;

	const QString& getName() { return m_name; }
	void setName(const QString& name) { m_name = name; }

	const QString& getFilePath() { return m_filePath; }
	void setFilePath(const QString& f) { m_filePath = f; }

	Window* getWindow() { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	void updateGL();
	void updateGL(Scene* scene);

	virtual void cb_initGL(Scene* scene) = 0;
	virtual void cb_updateMatrix(View* view) = 0;
	virtual void cb_redraw(Scene* scene) = 0;

	virtual void cb_keyPress(Scene* scene, int key) = 0;
	virtual void cb_keyRelease(Scene* scene, int key) = 0;
	virtual void cb_mousePress(Scene* scene, int button, int x, int y) = 0;
	virtual void cb_mouseRelease(Scene* scene, int button, int x, int y) = 0;
	virtual void cb_mouseClick(Scene* scene, int button, int x, int y) = 0;
	virtual void cb_mouseMove(Scene* scene, int buttons, int x, int y) = 0;
	virtual void cb_wheelEvent(Scene* scene, int delta, int x, int y) = 0;

	virtual void cb_mapAdded(MapHandler* map) = 0;
	virtual void cb_mapRemoved(MapHandler* map) = 0;

	virtual void cb_sceneAdded(Scene* s) = 0;
	virtual void cb_sceneRemoved(Scene* s) = 0;

	/*********************************************************
	 * MANAGE MAPS
	 *********************************************************/
	bool linkMap(MapHandler* map);
	void unlinkMap(MapHandler* map);
	bool isLinkedToMap(MapHandler* map);
	QList<MapHandler*> getLinkedMaps();
	void setMaxNumberOfLinkedMaps(int n);
	int getCurrentNumberOfLinkedMaps();
	int getRemainingNumberOfLinkedMaps();

	/*********************************************************
	 * MANAGE SCENES
	 *********************************************************/
	bool linkScene(Scene* scene);
	void unlinkScene(Scene* scene);
	bool isLinkedToScene(Scene* scene);
	QList<Scene*> getLinkedScenes();

	/*********************************************************
	 * MANAGE DOCK TABS
	 *********************************************************/
	bool addTabInDock(QWidget* tabWidget, const QString& tabText);
	void removeTabInDock(QWidget* tabWidget);

	/*********************************************************
	 * MANAGE MENU ACTIONS
	 *********************************************************/
	bool addMenuAction(const QString& menuPath, QAction* action);
	void removeMenuAction(QAction* action);

	/*********************************************************
	 * MANAGE TOOLBAR ACTIONS
	 *********************************************************/
	bool addToolbarAction(QAction* action);
	void removeToolbarAction(QAction* action);

protected:
	QString m_name;
	QString m_filePath;
	Window* m_window;

	int m_maxNumberOfMaps;
	QList<MapHandler*> l_maps;
	QList<Scene*> l_scenes;
	QList<QWidget*> l_tabWidgets;
	QList<QAction*> l_menuActions;
	QList<QAction*> l_toolbarActions;
//	QList<ViewButton*> l_viewButtons;

//	QList<Plugin*> l_dependencies;
//	QList<Plugin*> l_dependantPlugins;

/*
	void addDependantPlugin(Plugin* p) { l_dependantPlugins.push_back(p); }
	void removeDependantPlugin(Plugin* p) { l_dependantPlugins.removeAll(p); }
	void removeAllDependantPlugins() { l_dependantPlugins.clear(); }
	bool hasDependantPlugins() { return !l_dependantPlugins.isEmpty(); }
	QStringList getDependantPluginNames()
	{
		QStringList l;
		for(QList<Plugin*>::iterator it = l_dependantPlugins.begin(); it != l_dependantPlugins.end(); ++it)
			l.push_back((*it)->getName());
		return l;
	}

	Plugin* addDependency(QString dependency)
	{
		Plugin* plugin;
		if((plugin = m_window->checkPluginDependency(dependency, this)))
		{
			l_dependencies.push_back(plugin);
			return plugin;
		}
		else
			return NULL;
	}

	template<typename T>
	T* getDependency(QString dependency)
	{
		return (T*)(this->addDependency(dependency));
	}

	virtual void removeDependencyLink(Plugin* p)
	{
		p->removeDependantPlugin(this);
		l_dependencies.removeAll(p);
	}
	virtual void removeAllDependencyLinks()
	{
		foreach(Plugin* p, l_dependencies)
			p->removeDependantPlugin(this);
		l_dependencies.clear();
	}
	void removeDependantLink(Plugin* p)
	{
		int i = l_dependantPlugins.indexOf(p);
		if(i >= 0)
			p->removeDependencyLink(this);
	}
	void removeAllDependantLinks()
	{
		foreach(Plugin* p, l_dependantPlugins)
			p->removeDependencyLink(this);
	}

	virtual void unloadDependantPlugins()
	{
		while(!l_dependantPlugins.empty())
		{
			if(l_dependantPlugins.back())
				m_window->unloadPlugin(l_dependantPlugins.back()->getName());
		}
	}
*/
};

Q_DECLARE_INTERFACE(Plugin, "Window.Plugin")

#endif
