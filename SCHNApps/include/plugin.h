#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <QtPlugin>

#include <iostream>
#include <list>

#include "window.h"
#include "system.h"
#include "view.h"
#include "camera.h"
#include "mapHandler.h"
#include "vboHandler.h"
#include "scene.h"

class Plugin
{
public:
	enum { UNLIMITED_NUMBER_OF_MAPS = -1 };
	enum { UNLIMITED_NUMBER_OF_SCENES = -1 };

	virtual ~Plugin()
	{}

	virtual void cb_updateMatrix(View* view) = 0;
	virtual void cb_redraw(Scene* scene) = 0;
	virtual void cb_initGL(Scene* scene) = 0;

	void updateGL()	= 0;
	void updateGL(Scene* scene)	= 0;

	virtual bool cb_keyPress(Scene* scene, int event) = 0;
	virtual bool cb_keyRelease(Scene* scene, int event) = 0;
	virtual bool cb_mousePress(Scene* scene, int button, int x, int y) = 0;
	virtual bool cb_mouseRelease(Scene* scene, int button, int x, int y) = 0;
	virtual bool cb_mouseClick(Scene* scene, int button, int x, int y) = 0;
	virtual bool cb_mouseMove(Scene* scene, int buttons, int x, int y) = 0;
	virtual bool cb_wheelEvent(Scene* scene, int delta, int x, int y) = 0;

	virtual void cb_recievedMap(MapHandler* map) = 0;
	virtual void cb_removingMap(MapHandler* map) = 0;

	virtual bool isWaitingForScene() = 0;
	virtual void recieveScene(Scene* scene) = 0;
	virtual void deleteLinkWithScene(Scene* scene) = 0;
	virtual bool hasManualLinkWithScene(Scene* scene) = 0;

	const QString& getName() { return m_name; }
	void setName(QString name) { m_name = name; }

	const QString& getPluginFilePath() { return m_pluginPathFile; }
	void setPluginFilePath(QString path) { m_pluginPathFile = path; }

	Window* getWindow() { return m_window; }
	void setWindow(Window* window) { m_window = window; }

	virtual bool activate(bool initializing)
	{
		m_windowInitializing = initializing;
		bool r = activate();
		m_windowInitializing = false;
		return r;
	}

	virtual void activate() = 0;
	virtual void disable() = 0;

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

	bool addMap(MapHandler* map)
	{
		if((m_mapNumber == UNLIMITED_NUMBER_OF_MAPS || l_map.size() < m_mapNumber) && map && !l_map.contains(map))
		{
			l_map.push_back(map);
			cb_recievedMap(map);
			return true;
		}
		else
			return false;
	}
	MapHandler* takeMap(MapHandler* map)
	{
		int i = l_map.indexOf(map);
		if(i >= 0)
		{
			cb_removingMap(map);
			return l_map.takeAt(i);
		}
		else
			return NULL;
	}
	QList<MapHandler*> workingMaps() { return l_map; }
	void setMapNumber(int n)
	{
		if(n >= l_map.size() || n == UNLIMITED_NUMBER_OF_MAPS)
			m_mapNumber = n;
	}
	virtual int remainingMapNumber()
	{
		return m_mapNumber != UNLIMITED_NUMBER_OF_MAPS ? m_mapNumber - l_map.size() : m_mapNumber;
	}

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
