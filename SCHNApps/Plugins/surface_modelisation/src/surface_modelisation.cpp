#include "surface_modelisation.h"

#include "schnapps.h"
#include "view.h"
#include "mapHandler.h"

#include "Algo/Modelisation/polyhedron.h"

namespace CGoGN
{

namespace SCHNApps
{

Surface_Modelisation_Plugin::Surface_Modelisation_Plugin()
{}

bool Surface_Modelisation_Plugin::enable()
{
	m_dockTab = new Surface_Modelisation_DockTab(m_schnapps, this);
	m_schnapps->addPluginDockTab(this, m_dockTab, "Surface_Modelisation");

	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapAdded(map);

	m_dockTab->updateMapParameters();

	return true;
}

void Surface_Modelisation_Plugin::disable()
{
	disconnect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
}





void Surface_Modelisation_Plugin::selectedMapChanged(MapHandlerGen *prev, MapHandlerGen *cur)
{
	m_dockTab->updateMapParameters();
}

void Surface_Modelisation_Plugin::mapAdded(MapHandlerGen* map)
{
	connect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
}

void Surface_Modelisation_Plugin::mapRemoved(MapHandlerGen* map)
{
	disconnect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
}





void Surface_Modelisation_Plugin::attributeAdded(unsigned int orbit, const QString& name)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());
	if(orbit == VERTEX && map->isSelectedMap())
		m_dockTab->addVertexAttribute(name);
}





void Surface_Modelisation_Plugin::changePositionAttribute(const QString& view, const QString& map, const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		h_parameterSet[m].positionAttribute = m->getAttribute<PFP2::VEC3, VERTEX>(name);
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}

void Surface_Modelisation_Plugin::changeVertexSelector(const QString& map, const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		MapParameters& p = h_parameterSet[m];
		p.vertexSelector = m->getCellSelector<VERTEX>(name);
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}

void Surface_Modelisation_Plugin::changeEdgeSelector(const QString& map, const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		MapParameters& p = h_parameterSet[m];
		p.edgeSelector = m->getCellSelector<EDGE>(name);
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}

void Surface_Modelisation_Plugin::changeFaceSelector(const QString& map, const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		MapParameters& p = h_parameterSet[m];
		p.faceSelector = m->getCellSelector<FACE>(name);
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}





void Surface_Modelisation_Plugin::createCube(MapHandlerGen *mhg)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
	PFP2::MAP* map = mh->getMap();

	VertexAttribute<PFP2::VEC3>& position = h_parameterSet[mhg].positionAttribute;

	Algo::Surface::Modelisation::embedPrism<PFP2>(*map, position, 4, false, 1.0f, 1.0f, 1.0f);

	mh->notifyAttributeModification(position);
	mh->notifyConnectivityModification();
}

void Surface_Modelisation_Plugin::flipEdge(MapHandlerGen *mhg)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
	PFP2::MAP* map = mh->getMap();

	const MapParameters& p = h_parameterSet[mhg];
	if(p.edgeSelector && !p.edgeSelector->getSelectedCells().empty())
	{
		const std::vector<Dart>& edge = p.edgeSelector->getSelectedCells();
		map->flipEdge(edge[0]);
	}

	mh->notifyConnectivityModification();
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(Surface_Modelisation_Plugin, Surface_Modelisation_Plugin)
#else
Q_EXPORT_PLUGIN2(Surface_Modelisation_PluginD, Surface_Modelisation_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
