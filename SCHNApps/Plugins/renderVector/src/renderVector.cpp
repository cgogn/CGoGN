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
		PerMapParameterSet* p = params->perMap[m->getName()];
		m_vectorShader->setScale(m->getBBdiagSize() / 100.0f * p->vectorsScaleFactor) ;
		if(p->positionVBO != NULL)
		{
			m_vectorShader->setAttributePosition(p->positionVBO) ;
			for(std::vector<Utils::VBO*>::const_iterator it = p->vectorVBO.begin(); it != p->vectorVBO.end(); ++it)
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
		foreach(MapHandlerGen* mh, maps)
			addManagedMap(view, mh);

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

void RenderVectorPlugin::currentViewChanged(View* view)
{
	if(isLinkedToView(view))
		m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderVectorPlugin::mapLinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));
	addManagedMap(view, m);
}

void RenderVectorPlugin::mapUnlinked(MapHandlerGen* m)
{
	View* view = static_cast<View*>(QObject::sender());
	assert(isLinkedToView(view));
	removeManagedMap(view, m);
}

void RenderVectorPlugin::addManagedMap(View* v, MapHandlerGen *m)
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

void RenderVectorPlugin::removeManagedMap(View *v, MapHandlerGen *m)
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

void RenderVectorPlugin::changeSelectedMap(View* view, MapHandlerGen* map)
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

void RenderVectorPlugin::changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo, bool fromUI)
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

void RenderVectorPlugin::changeSelectedVectorsVBO(View* view, MapHandlerGen* map, const std::vector<Utils::VBO*>& vbos, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->vectorVBO = vbos;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderVectorPlugin::changeVectorsScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->vectorsScaleFactor = i / 50.0;

	if(view->isCurrentView())
	{
		if(!fromUI)
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
