#include "renderScalar.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

PerMapParameterSet::PerMapParameterSet(MapHandlerGen* map) :
	positionVBO(NULL)
{
	bool positionFound = false;

	QList<Utils::VBO*> vbos = map->getVBOList();
	for(int i = 0; i < vbos.count(); ++i)
	{
		if(vbos[i]->dataSize() == 3)
		{
			if(!positionFound) positionVBO = vbos[i];
			if(vbos[i]->name() == "position") // try to select a VBO named "position"
			{
				positionVBO = vbos[i];
				positionFound = true;
			}
		}
	}
}


bool RenderScalarPlugin::enable()
{
	m_dockTab = new RenderScalarDockTab(m_window, this);
	addTabInDock(m_dockTab, "RenderScalar");

	m_scalarShader = new Utils::ShaderScalarField();

	registerShader(m_scalarShader);

	connect(m_window, SIGNAL(viewAndPluginLinked(View*, Plugin*)), this, SLOT(viewLinked(View*, Plugin*)));
	connect(m_window, SIGNAL(viewAndPluginUnlinked(View*, Plugin*)), this, SLOT(viewUnlinked(View*, Plugin*)));
	connect(m_window, SIGNAL(currentViewChanged(View*)), this, SLOT(currentViewChanged(View*)));

	return true;
}

void RenderScalarPlugin::disable()
{
	delete m_scalarShader;
}

void RenderScalarPlugin::redraw(View* view)
{
	ParameterSet* params = h_viewParams[view];

	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	foreach(MapHandlerGen* m, maps)
	{
		PerMapParameterSet* p = params->perMap[m->getName()];
		if(p->positionVBO != NULL)
		{
			m_scalarShader->setAttributePosition(p->positionVBO) ;
			for(std::vector<Utils::VBO*>::const_iterator it = p->scalarVBO.begin(); it != p->scalarVBO.end(); ++it)
			{
				m_scalarShader->setAttributeScalar(*it) ;
				m->draw(m_scalarShader, Algo::Render::GL2::TRIANGLES) ;
			}
		}
	}
}

void RenderScalarPlugin::viewLinked(View* view, Plugin* plugin)
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

void RenderScalarPlugin::viewUnlinked(View* view, Plugin* plugin)
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

void RenderScalarPlugin::currentViewChanged(View* view)
{
	if(isLinkedToView(view))
		m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderScalarPlugin::mapLinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));
	addManagedMap(view, m);
}

void RenderScalarPlugin::mapUnlinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));
	removeManagedMap(view, m);
}

void RenderScalarPlugin::addManagedMap(View* v, MapHandlerGen *m)
{
//	connect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
//	connect(m, SIGNAL(connectivityModified()), this, SLOT(connectivityModified()));

	ParameterSet* params = h_viewParams[v];
	PerMapParameterSet* perMap = new PerMapParameterSet(m);

	params->perMap.insert(m->getName(), perMap);

	if(params->selectedMap == NULL || params->perMap.count() == 1)
		changeSelectedMap(v, m);
	else
		m_dockTab->refreshUI(params);
}

void RenderScalarPlugin::removeManagedMap(View *v, MapHandlerGen *m)
{
//	disconnect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
//	disconnect(m, SIGNAL(connectivityModified()), this, SLOT(connectivityModified()));

	ParameterSet* params = h_viewParams[v];
	PerMapParameterSet* perMap = params->perMap[m->getName()];

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

void RenderScalarPlugin::changeSelectedMap(View* view, MapHandlerGen* map)
{
	ParameterSet* params = h_viewParams[view];

	MapHandlerGen* prev = params->selectedMap;
	params->selectedMap = map;

	if(view->isCurrentView())
	{
		if(prev)
			disconnect(prev, SIGNAL(vboAdded(Utils::VBO*)), m_dockTab, SLOT(addVBOToList(Utils::VBO*)));
		if(map)
			connect(map, SIGNAL(vboAdded(Utils::VBO*)), m_dockTab, SLOT(addVBOToList(Utils::VBO*)));

		m_dockTab->refreshUI(params);
	}
}

void RenderScalarPlugin::changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->positionVBO = vbo;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderScalarPlugin::changeSelectedScalarsVBO(View* view, MapHandlerGen* map, const std::vector<Utils::VBO*>& vbos, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->scalarVBO = vbos;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(RenderScalarPlugin, RenderScalarPlugin)
#else
Q_EXPORT_PLUGIN2(RenderScalarPluginD, RenderScalarPlugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
