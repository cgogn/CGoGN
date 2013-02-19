#include "renderTopoSurface.h"

#include "mapHandler.h"


bool RenderTopoSurfacePlugin::enable()
{
	m_dockTab = new RenderTopoSurfaceDockTab(this);
	addTabInDock(m_dockTab, "RenderTopo");

	connect(m_dockTab->mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));
	connect(m_dockTab->slider_edgesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(cb_edgesScaleFactorChanged(int)));
	connect(m_dockTab->slider_facesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(cb_facesScaleFactorChanged(int)));
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
		const PerMapParameterSet& p = params->perMap[m->getName()];

		if(p.m_renderTopo != NULL)
		{
			p.m_renderTopo->drawTopo();
		}
	}
}

void RenderTopoSurfacePlugin::viewLinked(View* view, Plugin* plugin)
{
	if(plugin == this)
	{
		ParameterSet* params = new ParameterSet();
		h_viewParams.insert(view, params);
		const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
		foreach(MapHandlerGen* map, maps)
		{
			PerMapParameterSet p(map);
			params->perMap.insert(map->getName(), p);
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
		m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderTopoSurfacePlugin::mapLinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));

	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet p(m);
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

void RenderTopoSurfacePlugin::changeSelectedMap(View* view, MapHandlerGen* map)
{
	ParameterSet* params = h_viewParams[view];

	MapHandlerGen* prev = params->selectedMap;
	params->selectedMap = map;

	if(view->isCurrentView())
	{
		if(prev)
			disconnect(map, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(vboAdded(Utils::VBO*)));
		if(map)
			connect(map, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(vboAdded(Utils::VBO*)));

		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::changeEdgesScaleFactor(View* view, MapHandlerGen* map, int i)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].edgesScaleFactor = i / 50.0;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::changeFacesScaleFactor(View* view, MapHandlerGen* map, int i)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].facesScaleFactor = i / 50.0;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderTopoSurfacePlugin::cb_selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
		if(!currentItems.empty())
			changeSelectedMap(m_window->getCurrentView(), m_window->getMap(currentItems[0]->text()));
	}
}

void RenderTopoSurfacePlugin::cb_edgesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeEdgesScaleFactor(view, map, i);
	}
}

void RenderTopoSurfacePlugin::cb_facesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeFacesScaleFactor(view, map, i);
	}
}

void RenderDockTab::refreshUI(ParameterSet* params)
{
	plugin->setRefreshingUI(true);

	mapList->clear();
	combo_positionVBO->clear();
	combo_normalVBO->clear();

	MapHandlerGen* map = params->selectedMap;

	QHash<QString, PerMapParameterSet>::const_iterator i = params->perMap.constBegin();
	while (i != params->perMap.constEnd())
	{
		mapList->addItem(i.key());
		if(map != NULL && i.key() == map->getName())
		{
			QList<QListWidgetItem*> item = mapList->findItems(map->getName(), Qt::MatchExactly);
			item[0]->setSelected(true);

			PerMapParameterSet& p = params->perMap[map->getName()];

			slider_edgesScaleFactor->setSliderPosition(p.edgesScaleFactor * 50.0);
			slider_facesScaleFactor->setSliderPosition(p.facesScaleFactor * 50.0);
		}
		++i;
	}

	plugin->setRefreshingUI(false);
}



#ifndef DEBUG
Q_EXPORT_PLUGIN2(RenderTopoSurfacePlugin, RenderTopoSurfacePlugin)
#else
Q_EXPORT_PLUGIN2(RenderTopoSurfacePluginD, RenderTopoSurfacePlugin)
#endif
