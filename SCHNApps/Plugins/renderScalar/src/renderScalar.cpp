#include "renderScalar.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

PerMapParameterSet::PerMapParameterSet(MapHandlerGen* map) :
	positionVBO(NULL),
	scalarVBO(NULL),
	expansion(0)
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
		if(p->positionVBO && p->scalarVBO)
		{
			m_scalarShader->setAttributePosition(p->positionVBO);
			m_scalarShader->setAttributeScalar(p->scalarVBO);
			m_scalarShader->setMinValue(p->scalarMin);
			m_scalarShader->setMaxValue(p->scalarMax);
			m_scalarShader->setExpansion(p->expansion);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 1.0f);
			m->draw(m_scalarShader, Algo::Render::GL2::TRIANGLES);
			glDisable(GL_POLYGON_OFFSET_FILL);
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
	connect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
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
	disconnect(m, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
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

void RenderScalarPlugin::changeScalarVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet* perMap = params->perMap[map->getName()];

	perMap->scalarVBO = vbo;

	if(vbo != NULL)
	{
		const VertexAttribute<PFP2::REAL>& attr = map->getAttribute<PFP2::REAL, VERTEX>(QString::fromStdString(vbo->name()));
		perMap->scalarMin = 1e20;
		perMap->scalarMax = -1e20;
		for(unsigned int i = attr.begin(); i != attr.end(); attr.next(i))
		{
			perMap->scalarMin = attr[i] < perMap->scalarMin ? attr[i] : perMap->scalarMin;
			perMap->scalarMax = attr[i] > perMap->scalarMax ? attr[i] : perMap->scalarMax;
		}
	}

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderScalarPlugin::changeExpansion(View* view, MapHandlerGen* map, int i, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->expansion = i;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderScalarPlugin::attributeModified(unsigned int orbit, QString nameAttr)
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
				if(perMap->scalarVBO && nameAttr == QString::fromStdString(perMap->scalarVBO->name()))
				{
					const VertexAttribute<PFP2::REAL>& attr = map->getAttribute<PFP2::REAL, VERTEX>(nameAttr);
					perMap->scalarMin = std::numeric_limits<float>::max();
					perMap->scalarMax = std::numeric_limits<float>::min();
					for(unsigned int i = attr.begin(); i != attr.end(); attr.next(i))
					{
						perMap->scalarMin = attr[i] < perMap->scalarMin ? attr[i] : perMap->scalarMin;
						perMap->scalarMax = attr[i] > perMap->scalarMax ? attr[i] : perMap->scalarMax;
					}
				}
			}
		}
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(RenderScalarPlugin, RenderScalarPlugin)
#else
Q_EXPORT_PLUGIN2(RenderScalarPluginD, RenderScalarPlugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
