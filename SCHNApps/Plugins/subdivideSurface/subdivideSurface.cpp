#include "subdivideSurface.h"

#include "mapHandler.h"

#include "Algo/Modelisation/subdivision.h"

bool SubdivideSurfacePlugin::enable()
{
	m_dockTab = new SubdivideSurfaceDockTab();
	addTabInDock(m_dockTab, "SubdivideSurface");

	connect(m_window, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(cb_addMapToList(MapHandlerGen*)));
	connect(m_window, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(cb_removeMapFromList(MapHandlerGen*)));

	connect(m_dockTab->button_trianguleFaces, SIGNAL(clicked()), this, SLOT(cb_trianguleFaces()));

	QList<MapHandlerGen*> maps = m_window->getMapsList();
	foreach(MapHandlerGen* m, maps)
		m_dockTab->mapList->addItem(m->getName());

	return true;
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

void SubdivideSurfacePlugin::cb_trianguleFaces()
{
	QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandler<PFP>* mh = reinterpret_cast<MapHandler<PFP>*>(m_window->getMap(mapname));
		MAP* map = mh->getMap();
		VertexAttribute<VEC3> position = map->getAttribute<VEC3, VERTEX>("position");
		Algo::Modelisation::trianguleFaces<PFP>(*map, position);
		mh->updatePrimitives(Algo::Render::GL2::POINTS);
		mh->updatePrimitives(Algo::Render::GL2::LINES);
		mh->updatePrimitives(Algo::Render::GL2::TRIANGLES);
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(SubdivideSurfacePlugin, SubdivideSurfacePlugin)
#else
Q_EXPORT_PLUGIN2(SubdivideSurfacePluginD, SubdivideSurfacePlugin)
#endif
