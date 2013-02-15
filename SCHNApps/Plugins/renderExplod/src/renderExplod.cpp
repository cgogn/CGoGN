#include "renderExplod.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

PerMapParameterSet::PerMapParameterSet(MapHandlerGen* map) :
	positionVBO(NULL),
	colorVBO(NULL),
	facesScaleFactor(1.0f),
	volumesScaleFactor(1.0f),
	renderEdges(false),
	renderFaces(true)
{
	bool positionFound = false;
	bool colorFound = false;

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

			if(!colorFound) colorVBO = vbos[i];
			if(vbos[i]->name() == "color")	// try to select a VBO named "color"
			{
				colorVBO = vbos[i];
				colorFound = true;
			}
		}
	}
}


bool RenderExplodPlugin::enable()
{
	m_dockTab = new RenderExplodDockTab(m_window, this);
	addTabInDock(m_dockTab, "RenderExplod");

//	m_renderExplod = new Algo::Render::GL2::ExplodeVolumeRender(true);
//
//	m_renderExplod->setNoClippingPlane();
//	m_renderExplod->setExplodeVolumes(0.9f);
//	m_renderExplod->setExplodeFaces(1.0f);
//
//	//m_renderExplod->setAmbiant(Geom::Vec4f(0.1f,0.1f,0.1f,0.0f));
//	//m_renderExplod->setBackColor(Geom::Vec4f(0.1f,1.0f,0.1f,0.0f));
//	//m_renderExplod->setColorLine(Geom::Vec4f(0.1f,0.1f,0.1f,0.0f));
//	//m_renderExplod->setAmbiant(Geom::Vec4f(0.9f, 0.5f, 0.0f, 0.0f));
//	m_renderExplod->setBackColor(Geom::Vec4f(0.9f, 0.5f, 0.0f, 0.0f));
//	//m_renderExplod->setColorLine(Geom::Vec4f(0.9f, 0.5f, 0.0f, 0.0f));
//	m_renderExplod->setColorLine(Geom::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
//
//	registerShader(m_renderExplod->shaderFaces());
//	registerShader(m_renderExplod->shaderLines());

	connect(m_window, SIGNAL(viewAndPluginLinked(View*, Plugin*)), this, SLOT(viewLinked(View*, Plugin*)));
	connect(m_window, SIGNAL(viewAndPluginUnlinked(View*, Plugin*)), this, SLOT(viewUnlinked(View*, Plugin*)));
	connect(m_window, SIGNAL(currentViewChanged(View*)), this, SLOT(currentViewChanged(View*)));

	return true;
}

void RenderExplodPlugin::disable()
{

}

void RenderExplodPlugin::redraw(View* view)
{
	ParameterSet* params = h_viewParams[view];

	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	foreach(MapHandlerGen* m, maps)
	{
		const PerMapParameterSet& p = params->perMap[m->getName()];
		if(p.positionVBO != NULL)
		{
			if(p.renderEdges)
			{
//				m_renderExplod->drawEdges();
			}
			if(p.renderFaces)
			{
//				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//				glEnable(GL_LIGHTING);
//				glEnable(GL_POLYGON_OFFSET_FILL);
//			    glPolygonOffset(1.0f, 1.0f) ;
//			    m_renderExplod->drawFaces();
//				glDisable(GL_POLYGON_OFFSET_FILL);
			}
		}
	}
}

void RenderExplodPlugin::viewLinked(View* view, Plugin* plugin)
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

void RenderExplodPlugin::viewUnlinked(View* view, Plugin* plugin)
{
	if(plugin == this)
	{
		h_viewParams.remove(view);

		disconnect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
		disconnect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));
	}
}

void RenderExplodPlugin::currentViewChanged(View* view)
{
	if(isLinkedToView(view))
		m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderExplodPlugin::mapLinked(MapHandlerGen* m)
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

void RenderExplodPlugin::mapUnlinked(MapHandlerGen* m)
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

void RenderExplodPlugin::vboAdded(Utils::VBO* vbo)
{
	m_dockTab->refreshUI(h_viewParams[m_window->getCurrentView()]);
}

void RenderExplodPlugin::vboRemoved(Utils::VBO* vbo)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	View* view = m_window->getCurrentView();
	ParameterSet* params = h_viewParams[view];
	if(params->perMap[map->getName()].positionVBO == vbo)
		changePositionVBO(view, map, NULL);
	if(params->perMap[map->getName()].colorVBO == vbo)
		changeColorVBO(view, map, NULL);

	m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderExplodPlugin::changeSelectedMap(View* view, MapHandlerGen* map, bool fromUI)
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

		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].positionVBO = vbo;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeColorVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].colorVBO = vbo;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeRenderEdges(View* view, MapHandlerGen* map, bool b, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].renderEdges = b;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeRenderFaces(View* view, MapHandlerGen* map, bool b, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].renderFaces = b;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeFacesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].facesScaleFactor = i / 50.0;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeVolumesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].volumesScaleFactor = i / 50.0;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(RenderExplodPlugin, RenderExplodPlugin)
#else
Q_EXPORT_PLUGIN2(RenderExplodPluginD, RenderExplodPlugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
