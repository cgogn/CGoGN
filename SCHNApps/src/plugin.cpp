#include "plugin.h"

Plugin::Plugin() :
	m_window(NULL)
{}

Plugin::Plugin(const QString& name, const QString& filePath, Window* window) :
	m_name(name),
	m_filePath(filePath),
	m_window(window)
{}

Plugin::~Plugin()
{
	foreach(View* view, l_views)
		unlinkView(view);

	foreach(QWidget* tabWidget, l_tabWidgets)
		removeTabInDock(tabWidget);

	foreach(QAction* action, l_menuActions)
		removeMenuAction(action);

	foreach(QAction* action, l_toolbarActions)
		removeToolbarAction(action);

//	unloadDependantPlugins();
//	removeAllDependencyLinks();
}

/*********************************************************
 * MANAGE SCENES
 *********************************************************/

bool Plugin::linkView(View* view)
{
	if(view && !l_views.contains(view))
	{
		l_views.push_back(view);
		view->linkPlugin(this);
		view->updateGL();
		cb_viewAdded(view);
		return true;
	}
	else
		return false;
}

void Plugin::unlinkView(View* view)
{
	if(l_views.removeOne(view))
	{
		view->unlinkPlugin(this);
		view->updateGL();
		cb_viewRemoved(view);
	}
}

/*********************************************************
 * MANAGE DOCK TABS
 *********************************************************/

bool Plugin::addTabInDock(QWidget* tabWidget, const QString& tabText)
{
	if(tabWidget && !l_tabWidgets.contains(tabWidget))
	{
		m_window->addTabInDock(tabWidget, tabText);
		l_tabWidgets.push_back(tabWidget);
		return true;
	}
	else
		return false;
}

void Plugin::removeTabInDock(QWidget* tabWidget)
{
	if(l_tabWidgets.removeOne(tabWidget))
		m_window->removeTabInDock(tabWidget);
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
