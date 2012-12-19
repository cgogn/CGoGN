#include "subdivideSurface.h"

#include "mapHandler.h"

bool SubdivideSurfacePlugin::enable()
{
	m_dockTab = new SubdivideSurfaceDockTab();
	addTabInDock(m_dockTab, "SubdivideSurface");

	connect(m_dockTab->mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));

	connect(m_window, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(cb_addMapToList(MapHandlerGen*)));
	connect(m_window, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(cb_removeMapFromList(MapHandlerGen*)));

	QList<MapHandlerGen*> maps = m_window->getMapsList();
	foreach(MapHandlerGen* m, maps)
		m_dockTab->mapList->addItem(m->getName());

	connect(m_dockTab->button_trianguleFaces, SIGNAL(clicked()), this, SLOT(cb_trianguleFaces()));
	return true;
}

void SubdivideSurfacePlugin::cb_selectedMapChanged()
{

}

void SubdivideSurfacePlugin::cb_addMapToList(MapHandlerGen* m)
{
	m_dockTab->mapList->addItem(m->getName());
}

void SubdivideSurfacePlugin::cb_removeMapFromList(MapHandlerGen* m)
{
	for(int i = 0; i < m_dockTab->mapList->count(); ++i)
	{
		if(m_dockTab->mapList->item(i)->text() == m->getName())
		{
			delete m_dockTab->mapList->item(i);
			return;
		}
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(SubdivideSurfacePlugin, SubdivideSurfacePlugin)
#else
Q_EXPORT_PLUGIN2(SubdivideSurfacePluginD, SubdivideSurfacePlugin)
#endif
