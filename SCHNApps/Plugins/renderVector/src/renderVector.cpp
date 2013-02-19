#include "renderVector.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

PerMapParameterSet::PerMapParameterSet(MapHandlerGen* map) :
	positionVBO(NULL),
	vectorsScaleFactor(1.0f)
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


bool RenderVectorPlugin::enable()
{
	m_dockTab = new RenderVectorDockTab(m_window, this);
	addTabInDock(m_dockTab, "RenderVector");

	m_vectorShader = new Utils::ShaderVectorPerVertex();
	m_vectorShader->setColor(Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));

	registerShader(m_vectorShader);

	connect(m_window, SIGNAL(viewAndPluginLinked(View*, Plugin*)), this, SLOT(viewLinked(View*, Plugin*)));
	connect(m_window, SIGNAL(viewAndPluginUnlinked(View*, Plugin*)), this, SLOT(viewUnlinked(View*, Plugin*)));
	connect(m_window, SIGNAL(currentViewChanged(View*)), this, SLOT(currentViewChanged(View*)));

	return true;
}

void RenderVectorPlugin::disable()
{
	delete m_vectorShader;
}

void RenderVectorPlugin::redraw(View* view)
{
	ParameterSet* params = h_viewParams[view];

	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	foreach(MapHandlerGen* m, maps)
	{
		const PerMapParameterSet& p = params->perMap[m->getName()];
		m_vectorShader->setScale(m->getBBdiagSize() / 100.0f * p.vectorsScaleFactor) ;
		if(p.positionVBO != NULL)
		{
			m_vectorShader->setAttributePosition(p.positionVBO) ;
			for(std::vector<Utils::VBO*>::const_iterator it = p.vectorVBO.begin(); it != p.vectorVBO.end(); ++it)
			{
				m_vectorShader->setAttributeVector(*it) ;
				glLineWidth(1.0f) ;
				m->draw(m_vectorShader, Algo::Render::GL2::POINTS) ;
			}
		}
	}
}

void RenderVectorPlugin::viewLinked(View* view, Plugin* plugin)
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

void RenderVectorPlugin::viewUnlinked(View* view, Plugin* plugin)
{
	if(plugin == this)
	{
		h_viewParams.remove(view);

		disconnect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
		disconnect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));
	}
}

void RenderVectorPlugin::currentViewChanged(View* view)
{
	if(isLinkedToView(view))
		m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderVectorPlugin::mapLinked(MapHandlerGen* m)
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

void RenderVectorPlugin::mapUnlinked(MapHandlerGen* m)
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

void RenderVectorPlugin::vboAdded(Utils::VBO* vbo)
{
	m_dockTab->refreshUI(h_viewParams[m_window->getCurrentView()]);
}

void RenderVectorPlugin::vboRemoved(Utils::VBO* vbo)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	View* view = m_window->getCurrentView();
	ParameterSet* params = h_viewParams[view];
	if(params->perMap[map->getName()].positionVBO == vbo)
		changePositionVBO(view, map, NULL);

	m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderVectorPlugin::changeSelectedMap(View* view, MapHandlerGen* map)
{
	ParameterSet* params = h_viewParams[view];

	MapHandlerGen* prev = params->selectedMap;
	params->selectedMap = map;

	if(view->isCurrentView())
	{
		if(prev)
			disconnect(prev, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(vboAdded(Utils::VBO*)));
		if(map)
			connect(map, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(vboAdded(Utils::VBO*)));

		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderVectorPlugin::changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].positionVBO = vbo;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderVectorPlugin::changeSelectedVectorsVBO(View* view, MapHandlerGen* map, const std::vector<Utils::VBO*>& vbos)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].vectorVBO = vbos;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderVectorPlugin::changeVectorsScaleFactor(View* view, MapHandlerGen* map, int i)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].vectorsScaleFactor = i / 50.0;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(RenderVectorPlugin, RenderVectorPlugin)
#else
Q_EXPORT_PLUGIN2(RenderVectorPluginD, RenderVectorPlugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
