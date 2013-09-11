#include "surface_renderTopo.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

bool Surface_RenderTopo_Plugin::enable()
{
	m_dockTab = new Surface_RenderTopo_DockTab(m_schnapps, this);
	m_schnapps->addPluginDockTab(this, m_dockTab, "Surface_RenderTopo");

	connect(m_schnapps, SIGNAL(selectedViewChanged(View*, View*)), this, SLOT(selectedViewChanged(View*, View*)));
	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapAdded(map);

	m_dockTab->updateMapParameters();

	return true;
}

void Surface_RenderTopo_Plugin::disable()
{
	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapRemoved(map);
}

void Surface_RenderTopo_Plugin::drawMap(View* view, MapHandlerGen* map)
{
	const MapParameters& p = h_viewParameterSet[view][map];

//	if(p.positionAttribute.isValid())
//	{
//		if(p.drawDarts)
//			map->drawTopo(DARTS);
//		if(p.drawPhi1)
//			map->drawTopo(RELATION1);
//		if(p.drawPhi2)
//			map->drawTopo(RELATION2);
//	}
}





void Surface_RenderTopo_Plugin::selectedViewChanged(View *prev, View *cur)
{
	m_dockTab->updateMapParameters();
}

void Surface_RenderTopo_Plugin::selectedMapChanged(MapHandlerGen *prev, MapHandlerGen *cur)
{
	m_dockTab->updateMapParameters();
}

void Surface_RenderTopo_Plugin::mapAdded(MapHandlerGen* map)
{
	connect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
}

void Surface_RenderTopo_Plugin::mapRemoved(MapHandlerGen* map)
{
	disconnect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
}





void Surface_RenderTopo_Plugin::attributeAdded(unsigned int orbit, const QString& nameAttr)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());
	if(map == m_schnapps->getSelectedMap())
		m_dockTab->addAttributeToList(orbit, nameAttr);
}





//void Surface_RenderTopo_Plugin::changePositionAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP2::VEC3> attribute, bool fromUI)
//{
//	ParameterSet* params = h_viewParams[view];
//	PerMapParameterSet* perMap = params->perMap[map->getName()];
//	perMap->positionAttribute = attribute;
//	perMap->updateRender();

//	if(view->isCurrentView())
//	{
//		if(!fromUI)
//			m_dockTab->refreshUI(params);
//		view->updateGL();
//	}
//}

//void Surface_RenderTopo_Plugin::changeDrawDarts(View* view, MapHandlerGen* map, bool b, bool fromUI)
//{
//	ParameterSet* params = h_viewParams[view];
//	params->perMap[map->getName()]->drawDarts = b;

//	if(view->isCurrentView())
//	{
//		if(!fromUI)
//			m_dockTab->refreshUI(params);
//		view->updateGL();
//	}
//}

//void Surface_RenderTopo_Plugin::changeDartsColor(View* view, MapHandlerGen* map, QColor c, bool fromUI)
//{
//	ParameterSet* params = h_viewParams[view];
//	PerMapParameterSet* perMap = params->perMap[map->getName()];
//	perMap->dartsColor = c;
//	perMap->m_renderTopo->setInitialDartsColor(c.redF(), c.greenF(), c.blueF());
//	perMap->m_renderTopo->setAllDartsColor(c.redF(), c.greenF(), c.blueF());

//	if(view->isCurrentView())
//	{
//		if(!fromUI)
//			m_dockTab->refreshUI(params);
//		view->updateGL();
//	}
//}

//void Surface_RenderTopo_Plugin::changeDrawPhi1(View* view, MapHandlerGen* map, bool b, bool fromUI)
//{
//	ParameterSet* params = h_viewParams[view];
//	params->perMap[map->getName()]->drawPhi1 = b;

//	if(view->isCurrentView())
//	{
//		if(!fromUI)
//			m_dockTab->refreshUI(params);
//		view->updateGL();
//	}
//}

//void Surface_RenderTopo_Plugin::changePhi1Color(View* view, MapHandlerGen* map, QColor c, bool fromUI)
//{
//	ParameterSet* params = h_viewParams[view];
//	params->perMap[map->getName()]->phi1Color = c;

//	if(view->isCurrentView())
//	{
//		if(!fromUI)
//			m_dockTab->refreshUI(params);
//		view->updateGL();
//	}
//}

//void Surface_RenderTopo_Plugin::changeDrawPhi2(View* view, MapHandlerGen* map, bool b, bool fromUI)
//{
//	ParameterSet* params = h_viewParams[view];
//	params->perMap[map->getName()]->drawPhi2 = b;

//	if(view->isCurrentView())
//	{
//		if(!fromUI)
//			m_dockTab->refreshUI(params);
//		view->updateGL();
//	}
//}

//void Surface_RenderTopo_Plugin::changePhi2Color(View* view, MapHandlerGen* map, QColor c, bool fromUI)
//{
//	ParameterSet* params = h_viewParams[view];
//	params->perMap[map->getName()]->phi2Color = c;

//	if(view->isCurrentView())
//	{
//		if(!fromUI)
//			m_dockTab->refreshUI(params);
//		view->updateGL();
//	}
//}

//void Surface_RenderTopo_Plugin::changeEdgesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI)
//{
//	ParameterSet* params = h_viewParams[view];
//	PerMapParameterSet* perMap = params->perMap[map->getName()];
//	perMap->edgesScaleFactor = i / 100.0;
//	perMap->updateRender();

//	if(view->isCurrentView())
//	{
//		if(!fromUI)
//			m_dockTab->refreshUI(params);
//		view->updateGL();
//	}
//}

//void Surface_RenderTopo_Plugin::changeFacesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI)
//{
//	ParameterSet* params = h_viewParams[view];
//	PerMapParameterSet* perMap = params->perMap[map->getName()];
//	perMap->facesScaleFactor = i / 100.0;
//	perMap->updateRender();

//	if(view->isCurrentView())
//	{
//		if(!fromUI)
//			m_dockTab->refreshUI(params);
//		view->updateGL();
//	}
//}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(Surface_RenderTopo_Plugin, Surface_RenderTopo_Plugin)
#else
Q_EXPORT_PLUGIN2(Surface_RenderTopo_PluginD, Surface_RenderTopo_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
