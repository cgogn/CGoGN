#include "renderTopoSurface.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

PerMapParameterSet::PerMapParameterSet(MapHandlerGen* m) :
	mh(m),
	edgesScaleFactor(1.0f),
	facesScaleFactor(1.0f)
{
	m_renderTopo = new Algo::Render::GL2::TopoRender();

	std::cout << "plop" << std::endl;

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
	MapHandler<PFP2>* mh2 = static_cast<MapHandler<PFP2>*>(mh);
	if(mh2 == NULL)
		return;

	PFP2::MAP* m = mh2->getMap();

	m_renderTopo->updateData<PFP2>(*m,positionAttribute,edgesScaleFactor,facesScaleFactor);

	std::cout << "update render " << std::endl;
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

		if(p != NULL)
			p->m_renderTopo->drawTopo();

//		if (p->m_selectedDart != NIL)
//			p->m_renderTopo->overdrawDart(m_selectedDart, 4.0f, 1.0f, 0.0f, 0.0f) ;

	}
}

void RenderTopoSurfacePlugin::viewLinked(View* view, Plugin* plugin)
{
	if(plugin == this)
	{
		ParameterSet* params = new ParameterSet();
		h_viewParams.insert(view, params);
		const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
		foreach(MapHandlerGen* mh, maps)
		{
			PerMapParameterSet* p = new PerMapParameterSet(mh);
			registerShader(p->m_renderTopo->shader1());
			registerShader(p->m_renderTopo->shader2());
			params->perMap.insert(mh->getName(), p);
		}
		if (!maps.empty())
			changeSelectedMap(view, maps[0]);

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
		h_viewParams.remove(view);

		disconnect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
		disconnect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));
	}
}

void RenderTopoSurfacePlugin::currentViewChanged(View* view)
{
	if(isLinkedToView(view))
	{
//		ParameterSet* params = h_viewParams[view];
//		changeSelectedMap(view, params->selectedMap);
		m_dockTab->refreshUI(h_viewParams[view]);
	}
}

void RenderTopoSurfacePlugin::mapLinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));

	connect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
	connect(m, SIGNAL(connectivityModified()), this, SLOT(connectivityModified()));

	ParameterSet* params = h_viewParams[view];

	PerMapParameterSet* p = new PerMapParameterSet(m);

	registerShader(p->m_renderTopo->shader1());
	registerShader(p->m_renderTopo->shader2());

	params->perMap.insert(m->getName(), p);

	if(params->selectedMap == NULL || params->perMap.count() == 1)
		changeSelectedMap(view, m);
	else
		m_dockTab->refreshUI(params);
}

void RenderTopoSurfacePlugin::mapUnlinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));

	disconnect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
	disconnect(m, SIGNAL(connectivityModified()), this, SLOT(connectivityModified()));

	ParameterSet* params = h_viewParams[view];
	params->perMap.remove(m->getName());

	if(params->selectedMap == m)
	{
		if(!params->perMap.empty())
			changeSelectedMap(view, m_window->getMap(params->perMap.begin().key()));
		else
			changeSelectedMap(view, NULL);
	}
	else
		m_dockTab->refreshUI(params);
}

void RenderTopoSurfacePlugin::changeSelectedMap(View* view, MapHandlerGen* map, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];

	MapHandlerGen* prev = params->selectedMap;
	params->selectedMap = map;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
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


void RenderTopoSurfacePlugin::changeEdgesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet* perMap = params->perMap[map->getName()];
	perMap->edgesScaleFactor = i / 50.0;
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
	perMap->facesScaleFactor = i / 50.0;
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
