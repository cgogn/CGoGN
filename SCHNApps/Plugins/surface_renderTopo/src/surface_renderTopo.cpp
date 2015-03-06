#include "surface_renderTopo.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

bool Surface_RenderTopo_Plugin::enable()
{
	//	magic line that init static variables of GenericMap in the plugins
	GenericMap::copyAllStatics(m_schnapps->getStaticPointers());

	m_dockTab = new Surface_RenderTopo_DockTab(m_schnapps, this);
	m_schnapps->addPluginDockTab(this, m_dockTab, "Surface_RenderTopo");

	connect(m_schnapps, SIGNAL(selectedViewChanged(View*, View*)), this, SLOT(selectedViewChanged(View*, View*)));
	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	m_shaderTopo1 = new CGoGN::Utils::ShaderSimpleColor();
	registerShader(m_shaderTopo1);

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
	{
		mapAdded(map);
	}

	m_dockTab->updateMapParameters();

	return true;
}

void Surface_RenderTopo_Plugin::disable()
{
	disconnect(m_schnapps, SIGNAL(selectedViewChanged(View*, View*)), this, SLOT(selectedViewChanged(View*, View*)));
	disconnect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapRemoved(map);
}

void Surface_RenderTopo_Plugin::drawMap(View* view, MapHandlerGen* map)
{
	ViewMapParam& p = h_viewParameterSet[view][map];

	map->getTopoRender()->setInitialDartsColor(p.dartsColor.redF(),p.dartsColor.greenF(),p.dartsColor.blueF());
	map->getTopoRender()->setColorPhi1(Geom::Vec4f(p.phi1Color.redF(),p.phi1Color.greenF(),p.phi1Color.blueF(),0.0f));
	map->getTopoRender()->setColorPhi2(Geom::Vec4f(p.phi2Color.redF(),p.phi2Color.greenF(),p.phi2Color.blueF(),0.0f));

	MapParam& pm = h_parameterSet[map];

	if (pm.needUpdate)
	{
		map->getTopoRender()->setExplodeEdge(pm.edgesScaleFactor);
		map->getTopoRender()->setExplodeFace(pm.facesScaleFactor);
		QString pos = m_dockTab->combo_positionAttribute->currentText();
		map->updateTopoRender(pos);
		pm.needUpdate = false;
	}

	int code=0;
	if(p.drawDarts)
		code |= 1;
	if(p.drawPhi1)
		code |= 2;
	if(p.drawPhi2)
		code |= 4;
	map->drawTopoRender(code);
}





void Surface_RenderTopo_Plugin::selectedViewChanged(View *prev, View *cur)
{
	m_dockTab->updateMapParameters();
}

void Surface_RenderTopo_Plugin::selectedMapChanged(MapHandlerGen *prev, MapHandlerGen *cur)
{
	m_dockTab->updateMapParameters();

	if (cur==NULL)
		m_dockTab->setDisabled(true);
	else
		m_dockTab->setDisabled(false);
}

void Surface_RenderTopo_Plugin::mapAdded(MapHandlerGen* map)
{
	connect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
	map->createTopoRender(m_shaderTopo1);
}

void Surface_RenderTopo_Plugin::mapRemoved(MapHandlerGen* map)
{
	disconnect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
}





void Surface_RenderTopo_Plugin::attributeAdded(unsigned int orbit, const QString& name)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());
	if(orbit == VERTEX && map == m_schnapps->getSelectedMap())
	{
		m_dockTab->addVertexAttribute(name);
	}
}





//void Surface_RenderTopo_Plugin::changePositionAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP2::VEC3, PFP2::MAP> attribute, bool fromUI)
//{
////	map->updateTopoRender

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


#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#else
	Q_EXPORT_PLUGIN2(Surface_RenderTopo_Plugin, Surface_RenderTopo_Plugin)
#endif


} // namespace SCHNApps

} // namespace CGoGN
