#include "render.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

PerMapParameterSet::PerMapParameterSet(MapHandlerGen* map) :
	positionVBO(NULL),
	normalVBO(NULL),
	verticesScaleFactor(1.0f),
	renderVertices(false),
	renderEdges(false),
	renderFaces(true),
	faceStyle(FLAT)
{
	bool positionFound = false;
	bool normalFound = false;

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

			if(!normalFound) normalVBO = vbos[i];
			if(vbos[i]->name() == "normal")	// try to select a VBO named "normal"
			{
				normalVBO = vbos[i];
				normalFound = true;
			}
		}
	}
}


bool RenderPlugin::enable()
{
	m_dockTab = new RenderDockTab(m_window, this);
	addTabInDock(m_dockTab, "Render");

	m_flatShader = new CGoGN::Utils::ShaderFlat();
	m_flatShader->setAmbiant(CGoGN::Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f));
	m_flatShader->setDiffuse(CGoGN::Geom::Vec4f(0.8f, 0.9f, 0.7f, 1.0f));
	m_flatShader->setExplode(1.0f);

	m_phongShader = new CGoGN::Utils::ShaderPhong() ;
	m_phongShader->setAmbiant(CGoGN::Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f)) ;
	m_phongShader->setDiffuse(CGoGN::Geom::Vec4f(0.8f, 0.9f, 0.7f, 1.0f)) ;
	m_phongShader->setSpecular(CGoGN::Geom::Vec4f(0.9f, 0.9f, 0.9f, 1.0f)) ;
	m_phongShader->setShininess(80.0f) ;

	m_simpleColorShader = new CGoGN::Utils::ShaderSimpleColor();
	CGoGN::Geom::Vec4f c(0.1f, 0.1f, 0.1f, 1.0f);
	m_simpleColorShader->setColor(c);

	m_pointSprite = new CGoGN::Utils::PointSprite();

	registerShader(m_flatShader);
	registerShader(m_phongShader);
	registerShader(m_simpleColorShader);
	registerShader(m_pointSprite);

	connect(m_window, SIGNAL(viewAndPluginLinked(View*, Plugin*)), this, SLOT(viewLinked(View*, Plugin*)));
	connect(m_window, SIGNAL(viewAndPluginUnlinked(View*, Plugin*)), this, SLOT(viewUnlinked(View*, Plugin*)));
	connect(m_window, SIGNAL(currentViewChanged(View*)), this, SLOT(currentViewChanged(View*)));

	return true;
}

void RenderPlugin::disable()
{
	delete m_flatShader;
	delete m_phongShader;
	delete m_simpleColorShader;
	delete m_pointSprite;
}

void RenderPlugin::redraw(View* view)
{
	ParameterSet* params = h_viewParams[view];

	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	foreach(MapHandlerGen* m, maps)
	{
		const PerMapParameterSet& p = params->perMap[m->getName()];
		if(p.positionVBO != NULL)
		{
			if(p.renderVertices)
			{
				m_pointSprite->setSize(m->getBBdiagSize() / 200.0f * p.verticesScaleFactor);
				m_pointSprite->setAttributePosition(p.positionVBO);
				m_pointSprite->predraw(CGoGN::Geom::Vec3f(0.0f, 0.0f, 1.0f));
				m->draw(m_pointSprite, CGoGN::Algo::Render::GL2::POINTS);
				m_pointSprite->postdraw();
			}
			if(p.renderEdges)
			{
				glLineWidth(1.0f);
				m_simpleColorShader->setAttributePosition(p.positionVBO);
				m->draw(m_simpleColorShader, CGoGN::Algo::Render::GL2::LINES);
			}
			if(p.renderFaces)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_LIGHTING);
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0f, 1.0f);
				switch(p.faceStyle)
				{
					case FLAT :
						m_flatShader->setAttributePosition(p.positionVBO);
						m->draw(m_flatShader, CGoGN::Algo::Render::GL2::TRIANGLES);
						break ;
					case PHONG :
						if(p.normalVBO != NULL)
						{
							m_phongShader->setAttributePosition(p.positionVBO) ;
							m_phongShader->setAttributeNormal(p.normalVBO) ;
							m->draw(m_phongShader, CGoGN::Algo::Render::GL2::TRIANGLES);
						}
						break ;
				}
				glDisable(GL_POLYGON_OFFSET_FILL);
			}
		}
	}
}

void RenderPlugin::viewLinked(View* view, Plugin* plugin)
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

void RenderPlugin::viewUnlinked(View* view, Plugin* plugin)
{
	if(plugin == this)
	{
		h_viewParams.remove(view);

		disconnect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
		disconnect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));
	}
}

void RenderPlugin::currentViewChanged(View* view)
{
	if(isLinkedToView(view))
	{
		ParameterSet* params = h_viewParams[view];
		changeSelectedMap(view, params->selectedMap);
		m_dockTab->refreshUI(h_viewParams[view]);
	}
}

void RenderPlugin::mapLinked(MapHandlerGen* m)
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

void RenderPlugin::mapUnlinked(MapHandlerGen* m)
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

void RenderPlugin::vboAdded(Utils::VBO* vbo)
{
	assert(h_viewParams[m_window->getCurrentView()]->selectedMap == static_cast<MapHandlerGen*>(QObject::sender()));
	if(vbo->dataSize() == 3)
		m_dockTab->addVBOToList(QString::fromStdString(vbo->name()));
}

void RenderPlugin::vboRemoved(Utils::VBO* vbo)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	View* view = m_window->getCurrentView();
	ParameterSet* params = h_viewParams[view];
	if(params->perMap[map->getName()].positionVBO == vbo)
		changePositionVBO(view, map, NULL);
	if(params->perMap[map->getName()].normalVBO == vbo)
		changeNormalVBO(view, map, NULL);

	m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderPlugin::changeSelectedMap(View* view, MapHandlerGen* map, bool fromUI)
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
	}
}

void RenderPlugin::changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo, bool fromUI)
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

void RenderPlugin::changeNormalVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].normalVBO = vbo;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderPlugin::changeRenderVertices(View* view, MapHandlerGen* map, bool b, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].renderVertices = b;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderPlugin::changeVerticesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].verticesScaleFactor = i / 50.0;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderPlugin::changeRenderEdges(View* view, MapHandlerGen* map, bool b, bool fromUI)
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

void RenderPlugin::changeRenderFaces(View* view, MapHandlerGen* map, bool b, bool fromUI)
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

void RenderPlugin::changeFacesStyle(View* view, MapHandlerGen* map, FaceShadingStyle style, bool fromUI)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].faceStyle = style;

	if(view->isCurrentView())
	{
		if(!fromUI)
			m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(RenderPlugin, RenderPlugin)
#else
Q_EXPORT_PLUGIN2(RenderPluginD, RenderPlugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
