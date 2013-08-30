#include "renderTopoSurface.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

PerMapParameterSet::PerMapParameterSet(MapHandlerGen* m) :
	mh(m),
	drawDarts(true),
	dartsColor("white"),
	drawPhi1(true),
	phi1Color("cyan"),
	drawPhi2(true),
	phi2Color("red"),
	edgesScaleFactor(1.0f),
	facesScaleFactor(1.0f)
{
	m_renderTopo = new Algo::Render::GL2::TopoRender();

	QString positionName;

	QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));

	const AttributeHash& attribs = mh->getAttributesList(VERTEX);
	for(AttributeHash::const_iterator i = attribs.constBegin(); i != attribs.constEnd(); ++i)
	{
		if(i.value() == vec3TypeName)
		{
			if(positionName != "position")	// try to select an attribute named "position"
				positionName = i.key();		// or anything else if not found
		}
	}
	positionAttribute = mh->getAttribute<PFP2::VEC3, VERTEX>(positionName);

	updateRender();
}

PerMapParameterSet::~PerMapParameterSet()
{
	delete m_renderTopo;
}

void PerMapParameterSet::updateRender()
{
	PFP2::MAP* m = static_cast<MapHandler<PFP2>*>(mh)->getMap();
	m_renderTopo->updateData<PFP2>(*m, positionAttribute, edgesScaleFactor, facesScaleFactor);
}


bool RenderTopoSurfacePlugin::enable()
{
	m_dockTab = new RenderTopoSurfaceDockTab(m_window, this);
	addTabInDock(m_dockTab, "RenderTopoSurface");

	connect(m_window, SIGNAL(viewAndPluginLinked(View*, Plugin*)), this, SLOT(viewLinked(View*, Plugin*)));
	connect(m_window, SIGNAL(viewAndPluginUnlinked(View*, Plugin*)), this, SLOT(viewUnlinked(View*, Plugin*)));
	connect(m_window, SIGNAL(currentViewChanged(View*)), this, SLOT(currentViewChanged(View*)));

	return true;
}

void RenderTopoSurfacePlugin::disable()
{

}

void RenderTopoSurfacePlugin::redraw(View* view)
{
	ParameterSet* params = h_viewParams[view];

	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	foreach(MapHandlerGen* m, maps)
	{
		PerMapParameterSet* p = params->perMap[m->getName()];

		if(p->drawDarts)
			p->m_renderTopo->drawDarts();
		if(p->drawPhi1)
			p->m_renderTopo->drawRelation1();
		if(p->drawPhi2)
			p->m_renderTopo->drawRelation2();
	}
}

void RenderTopoSurfacePlugin::mousePress(View* view, QMouseEvent* event)
{
//  if (Shift())
//  {
//    ParameterSet* params = h_viewParams[view];

//    m_renderTopo->updateData<PFP>(myMap, position, m_coeff_topo_explod[0], m_coeff_topo_explod[1], m_coeff_topo_explod[2], *m_selNoBoundary)	;

//    Dart d = m_renderTopo->picking<PFP>(myMap, x,  y, *m_selNoBoundary);
//    if (d != Dart::nil())
//    {
//      emit(dartSelected(d);
//    }
//  }
}

void RenderTopoSurfacePlugin::viewLinked(View* view, Plugin* plugin)
{
	if(plugin == this)
	{
		ParameterSet* params = new ParameterSet();
		h_viewParams.insert(view, params);

		const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
		foreach(MapHandlerGen* mh, maps)
			addManagedMap(view, mh);

		connect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
		connect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));

		if(view->isCurrentView())
			m_dockTab->refreshUI(params);
	}
}

void RenderTopoSurfacePlugin::viewUnlinked(View* view, Plugin* plugin)
{
	if(plugin == this)
	{
		const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
		foreach(MapHandlerGen* mh, maps)
			removeManagedMap(view, mh);

		ParameterSet* params = h_viewParams[view];
		delete params;
		h_viewParams.remove(view);

		disconnect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
		disconnect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));
	}
}

void RenderTopoSurfacePlugin::currentViewChanged(View* view)
{
	if(isLinkedToView(view))
		m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderTopoSurfacePlugin::mapLinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));
	addManagedMap(view, m);
}

void RenderTopoSurfacePlugin::mapUnlinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));
	removeManagedMap(view, m);
}

void RenderTopoSurfacePlugin::addManagedMap(View* v, MapHandlerGen *m)
{
	connect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
	connect(m, SIGNAL(connectivityModified()), this, SLOT(connectivityModified()));

	ParameterSet* params = h_viewParams[v];
	PerMapParameterSet* perMap = new PerMapParameterSet(m);

	registerShader(perMap->m_renderTopo->shader1());
	registerShader(perMap->m_renderTopo->shader2());

	params->perMap.insert(m->getName(), perMap);

	if(params->selectedMap == NULL || params->perMap.count() == 1)
		changeSelectedMap(v, m);
	else
		m_dockTab->refreshUI(params);
}

void RenderTopoSurfacePlugin::removeManagedMap(View *v, MapHandlerGen *m)
{
	disconnect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
	disconnect(m, SIGNAL(connectivityModified()), this, SLOT(connectivityModified()));

	ParameterSet* params = h_viewParams[v];
	PerMapParameterSet* perMap = params->perMap[m->getName()];

	unregisterShader(perMap->m_renderTopo->shader1());
	unregisterShader(perMap->m_renderTopo->shader2());

	delete perMap;
	params->perMap.remove(m->getName());

	if(params->selectedMap == m)
	{
		if(!params->perMap.empty())
			changeSelectedMap(v, m_window->getMap(params->perMap.begin().key()));
		else
			changeSelectedMap(v, NULL);
	}
	else
		m_dockTab->refreshUI(params);
}

void RenderTopoSurfacePlugin::changeSelectedMap(View* view, MapHandlerGen* map)
{
	ParameterSet* params = h_viewParams[view];

	MapHandlerGen* prev = params->selectedMap;
	params->selectedMap = map;

	if(view->isCurrentView())
	{
		if(prev)
			disconnect(prev, SIGNAL(attributeAdded(unsigned int, const QString&)), m_dockTab, SLOT(addAttributeToList(unsigned int, const QString&)));
		if(map)
			connect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), m_dockTab, SLOT(addAttributeToList(unsigned int, const QString&)));

		m_dockTab->refreshUI(params);
	}
}

void RenderTopoSurfacePlugin::changePositionAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP2::VEC3> attribute, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet* perMap = params->perMap[map->getName()];
	perMap->positionAttribute = attribute;
	perMap->updateRender();

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::changeDrawDarts(View* view, MapHandlerGen* map, bool b, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->drawDarts = b;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::changeDartsColor(View* view, MapHandlerGen* map, QColor c, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet* perMap = params->perMap[map->getName()];
	perMap->dartsColor = c;
	perMap->m_renderTopo->setInitialDartsColor(c.redF(), c.greenF(), c.blueF());
	perMap->m_renderTopo->setAllDartsColor(c.redF(), c.greenF(), c.blueF());

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::changeDrawPhi1(View* view, MapHandlerGen* map, bool b, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->drawPhi1 = b;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::changePhi1Color(View* view, MapHandlerGen* map, QColor c, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->phi1Color = c;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::changeDrawPhi2(View* view, MapHandlerGen* map, bool b, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->drawPhi2 = b;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::changePhi2Color(View* view, MapHandlerGen* map, QColor c, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->phi2Color = c;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::changeEdgesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet* perMap = params->perMap[map->getName()];
	perMap->edgesScaleFactor = i / 100.0;
	perMap->updateRender();

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::changeFacesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet* perMap = params->perMap[map->getName()];
	perMap->facesScaleFactor = i / 100.0;
	perMap->updateRender();

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::attributeModified(unsigned int orbit, QString nameAttr)
{
	if(orbit == VERTEX)
	{
		MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());
		foreach(View* view, l_views)
		{
			ParameterSet* params = h_viewParams[view];
			if(params->perMap.contains(map->getName()))
			{
				PerMapParameterSet* perMap = params->perMap[map->getName()];
				if(nameAttr == QString::fromStdString(perMap->positionAttribute.name()))
					perMap->updateRender();
			}
		}
	}
}

void RenderTopoSurfacePlugin::connectivityModified()
{

}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(RenderTopoSurfacePlugin, RenderTopoSurfacePlugin)
#else
Q_EXPORT_PLUGIN2(RenderTopoSurfacePluginD, RenderTopoSurfacePlugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
