#include "plugin.h"

Plugin::Plugin(const QString& name, const QString& filePath)
{

}

Plugin::~Plugin()
{
	foreach(Scene* scene, l_scenes)
		removeScene(scene);

	foreach(QWidget* tabWidget, l_tabWidgets)
		removeTabInDock(tabWidget);

	foreach(QAction* action, l_menuActions)
		removeMenuAction(action);

	foreach(QAction* action, l_toolbarActions)
		removeToolbarAction(action);

//	unloadDependantPlugins();
//	removeAllDependencyLinks();
}

void Plugin::updateGL()
{
	foreach(Scene* s, l_scenes)
		s->updateGL();
}

void Plugin::updateGL(Scene* s)
{
	s->updateGL();
}

/*********************************************************
 * MANAGE MAPS
 *********************************************************/

bool Plugin::addMap(MapHandler* map)
{
	if(
		(m_maxNumberOfMaps == UNLIMITED_NUMBER_OF_MAPS || l_maps.size() < m_maxNumberOfMaps)
		&& map
		&& !l_maps.contains(map)
	)
	{
		l_maps.push_back(map);
		cb_mapAdded(map);
		return true;
	}
	else
		return false;
}

void Plugin::removeMap(MapHandler* map)
{
	if(l_maps.removeOne(map))
		cb_mapRemoved(map);
}

bool Plugin::hasMap(MapHandler* map)
{
	return l_maps.contains(map);
}

QList<MapHandler*> Plugin::getMaps()
{
	return l_maps;
}

void Plugin::setMaxNumberOfMaps(int n)
{
	if(n >= l_maps.size() || n == UNLIMITED_NUMBER_OF_MAPS)
		m_maxNumberOfMaps = n;
}

int Plugin::getCurrentNumberOfMaps()
{
	return l_maps.size();
}

int Plugin::getRemainingNumberOfMaps()
{
	if(m_maxNumberOfMaps != UNLIMITED_NUMBER_OF_MAPS)
		return m_maxNumberOfMaps - l_maps.size();
	else
		return UNLIMITED_NUMBER_OF_MAPS;
}

/*********************************************************
 * MANAGE SCENES
 *********************************************************/

bool Plugin::addScene(Scene* scene)
{
	if(s && !l_scenes.contains(scene))
	{
		l_scenes.push_back(scene);
		scene->addPlugin(this);
		scene->updateGL();
		cb_sceneAdded(scene);
		return true;
	}
	else
		return false;
}

void Plugin::removeScene(Scene* scene)
{
	if(l_scenes.removeOne(scene))
	{
		scene->removePlugin(this);
		scene->updateGL();
		cb_sceneRemoved(scene);
	}
}

bool Plugin::hasScene(Scene* scene)
{
	return l_scenes.contains(scene);
}

QList<Scene*> Plugin::getScenes()
{
	return l_scenes;
}

/*********************************************************
 * MANAGE DOCK TABS
 *********************************************************/

bool Plugin::addTabInDock(QWidget* tabWidget, const QString& tabText)
{
	if(m_window)
	{
		l_tabWidgets.push_back(tabWidget);
		m_window->addTabInDock(tabWidget, tabText);
		return true;
	}
	else
	{
		System::Error::code = System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
		return false;
	}
}

void Plugin::removeTabInDock(QWidget* tabWidget)
{
	if(m_window)
	{
		l_tabWidgets.removeOne(tabWidget);
		m_window->removeTabInDock(tabWidget);
	}
}

/*********************************************************
 * MANAGE MENU ACTIONS
 *********************************************************/

bool Plugin::addMenuAction(const QString& menuPath, QAction* action)
{
	if(action && !l_menuActions.contains(action))
	{
		m_window->addMenuAction(menuPath, action);
		l_menuActions.push_back(action);
		return true;
	}
	else
		return false;
}

void Plugin::removeMenuAction(QAction* action)
{
	if(l_menuActions.removeOne(action))
		m_window->removeMenuAction(action);
}

/*********************************************************
 * MANAGE TOOLBAR ACTIONS
 *********************************************************/

bool Plugin::addToolbarAction(QAction* action)
{
	if(action && !l_toolbarActions.contains(action))
	{
		m_window->addToolbarAction(action);
		l_toolbarActions.push_back(action);
		return true;
	}
	else
		return false;
}

void Plugin::removeToolbarAction(QAction* action)
{
	if(l_toolbarActions.removeOne(action))
		m_window->removeToolbarAction(action);
}

/*********************************************************
 * MANAGE SCENE VIEW BUTTONS
 *********************************************************/
/*
bool Plugin::addSceneViewButton(Scene* scene, ViewButton* viewButton)
{
	if(scene && viewButton)
		return scene->addViewButton(viewButton);
	return false;
}

void Plugin::removeSceneViewButton(Scene* scene, ViewButton* viewButton)
{
	if(scene && viewButton)
		scene->removeViewButton(viewButton);
}
*/
