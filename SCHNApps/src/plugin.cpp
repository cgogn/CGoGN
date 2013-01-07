#include "plugin.h"

#include <GL/glew.h>

#include "window.h"
#include "Utils/GLSLShader.h"

namespace CGoGN
{

namespace SCHNApps
{

Plugin::Plugin() :
	m_window(NULL),
	b_providesRendering(false)
{
	glewInit();
}

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
 * MANAGE LINKED VIEWS
 *********************************************************/

bool Plugin::linkView(View* view)
{
	if(view && !l_views.contains(view))
	{
		l_views.push_back(view);
		viewLinked(view);
		return true;
	}
	else
		return false;
}

void Plugin::unlinkView(View* view)
{
	if(l_views.removeOne(view))
		viewUnlinked(view);
}

/*********************************************************
 * MANAGE SHADERS
 *********************************************************/

void Plugin::registerShader(Utils::GLSLShader* shader)
{
	if(shader && !l_shaders.contains(shader))
		l_shaders.push_back(shader);
}

void Plugin::unregisterShader(Utils::GLSLShader* shader)
{
	l_shaders.removeOne(shader);
}

/*********************************************************
 * MANAGE DOCK TABS
 *********************************************************/

bool Plugin::addTabInDock(QWidget* tabWidget, const QString& tabText)
{
	if(tabWidget && !l_tabWidgets.contains(tabWidget))
	{
		// tab created by plugins that do not provide rendering are always enabled,
		// the other are enabled only if they are linked to the current view
		m_window->addTabInDock(tabWidget, tabText, !b_providesRendering);
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

} // namespace SCHNApps

} // namespace CGoGN
