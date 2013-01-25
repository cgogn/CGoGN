#include "render.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"


PerMapParameterSet::PerMapParameterSet(MapHandlerGen* map) :
	positionVBO(NULL),
	normalVBO(NULL),
	verticesScaleFactor(1.0f),
	renderVertices(false),
	renderEdges(false),
	renderFaces(true),
	faceStyle(FLAT)
{
	QList<Utils::VBO*> vbos = map->getVBOList();
	for(int i = 0; i < vbos.count(); ++i)
	{
		if(vbos[i]->name() == "position") // try to select a VBO named "position"
			positionVBO = vbos[i];
		if(vbos[i]->name() == "normal")	// try to select a VBO named "normal"
			normalVBO = vbos[i];
	}

	if(positionVBO == NULL && vbos.count() > 0)
		positionVBO = vbos[0];
	if(normalVBO == NULL && vbos.count() > 0)
		normalVBO = vbos[0];
}


bool RenderPlugin::enable()
{
	m_dockTab = new RenderDockTab(this);
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

	connect(m_dockTab->mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));
	connect(m_dockTab->combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_positionVBOChanged(int)));
	connect(m_dockTab->combo_normalVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_normalVBOChanged(int)));
	connect(m_dockTab->check_renderVertices, SIGNAL(toggled(bool)), this, SLOT(cb_renderVerticesChanged(bool)));
	connect(m_dockTab->slider_verticesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(cb_verticesScaleFactorChanged(int)));
	connect(m_dockTab->check_renderEdges, SIGNAL(toggled(bool)), this, SLOT(cb_renderEdgesChanged(bool)));
	connect(m_dockTab->check_renderFaces, SIGNAL(toggled(bool)), this, SLOT(cb_renderFacesChanged(bool)));
	connect(m_dockTab->group_faceShading, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(cb_faceStyleChanged(QAbstractButton*)));

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

void RenderPlugin::viewLinked(View* view)
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
		params->selectedMap = maps[0];

	connect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
	connect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));

	if(view->isCurrentView())
		m_dockTab->refreshUI(params);
}

void RenderPlugin::viewUnlinked(View* view)
{
	h_viewParams.remove(view);

	disconnect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
	disconnect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));
}

void RenderPlugin::currentViewChanged(View* view)
{
	assert(isLinkedToView(view));
	m_dockTab->refreshUI(h_viewParams[view]);
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
	m_dockTab->refreshUI(h_viewParams[m_window->getCurrentView()]);
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
}

void RenderPlugin::changeSelectedMap(View* view, MapHandlerGen* map)
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

void RenderPlugin::changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].positionVBO = vbo;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderPlugin::changeNormalVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].normalVBO = vbo;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderPlugin::changeRenderVertices(View* view, MapHandlerGen* map, bool b)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].renderVertices = b;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderPlugin::changeVerticesScaleFactor(View* view, MapHandlerGen* map, int i)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].verticesScaleFactor = i / 50.0;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderPlugin::changeRenderEdges(View* view, MapHandlerGen* map, bool b)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].renderEdges = b;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderPlugin::changeRenderFaces(View* view, MapHandlerGen* map, bool b)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].renderFaces = b;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderPlugin::changeFacesStyle(View* view, MapHandlerGen* map, FaceShadingStyle style)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].faceStyle = style;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderPlugin::cb_selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
		if(!currentItems.empty())
			changeSelectedMap(m_window->getCurrentView(), m_window->getMap(currentItems[0]->text()));
	}
}

void RenderPlugin::cb_positionVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changePositionVBO(view, map, map->getVBO(m_dockTab->combo_positionVBO->currentText()));
	}
}

void RenderPlugin::cb_normalVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeNormalVBO(view, map, map->getVBO(m_dockTab->combo_normalVBO->currentText()));
	}
}

void RenderPlugin::cb_renderVerticesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeRenderVertices(view, map, b);
	}
}

void RenderPlugin::cb_verticesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeVerticesScaleFactor(view, map, i);
	}
}

void RenderPlugin::cb_renderEdgesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeRenderEdges(view, map, b);
	}
}

void RenderPlugin::cb_renderFacesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeRenderFaces(view, map, b);
	}
}

void RenderPlugin::cb_faceStyleChanged(QAbstractButton* b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		if(m_dockTab->radio_flatShading->isChecked())
			changeFacesStyle(view, map, FLAT);
		else if(m_dockTab->radio_phongShading->isChecked())
			changeFacesStyle(view, map, PHONG);
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

			QList<Utils::VBO*> vbos = map->getVBOList();
			unsigned int j = 0;
			for(int i = 0; i < vbos.count(); ++i)
			{
				if(vbos[i]->dataSize() == 3)
				{
					combo_positionVBO->addItem(QString::fromStdString(vbos[i]->name()));
					if(p.positionVBO == NULL)
					{										// if nothing is specified in the parameter set
						if(vbos[i]->name() == "position")	// try to select a VBO named "position"
						{
							p.positionVBO = vbos[i];
							combo_positionVBO->setCurrentIndex(j);
						}
					}
					else if(vbos[i] == p.positionVBO)
						combo_positionVBO->setCurrentIndex(j);

					combo_normalVBO->addItem(QString::fromStdString(vbos[i]->name()));
					if(p.normalVBO == NULL)
					{									// if nothing is specified in the parameter set
						if(vbos[i]->name() == "normal")	// try to select a VBO named "normal"
						{
							p.normalVBO = vbos[i];
							combo_normalVBO->setCurrentIndex(j);
						}
					}
					else if(vbos[i] == p.normalVBO)
						combo_normalVBO->setCurrentIndex(j);

					++j;
				}
			}

			if(p.positionVBO == NULL && vbos.count() > 0)
			{
				int i = 0;
				bool found = false;
				while(i < vbos.count() && !found)
				{
					if(vbos[i]->dataSize() == 3)
					{
						p.positionVBO = vbos[i];
						combo_positionVBO->setCurrentIndex(i);
						found = true;
					}
					++i;
				}
			}
			if(p.normalVBO == NULL && vbos.count() > 0)
			{
				int i = 0;
				bool found = false;
				while(i < vbos.count() && !found)
				{
					if(vbos[i]->dataSize() == 3)
					{
						p.normalVBO = vbos[i];
						combo_normalVBO->setCurrentIndex(i);
						found = true;
					}
					++i;
				}
			}

			check_renderVertices->setChecked(p.renderVertices);
			slider_verticesScaleFactor->setSliderPosition(p.verticesScaleFactor * 50.0);
			check_renderEdges->setChecked(p.renderEdges);
			check_renderFaces->setChecked(p.renderFaces);
			radio_flatShading->setChecked(p.faceStyle == FLAT);
			radio_phongShading->setChecked(p.faceStyle == PHONG);
		}
		++i;
	}

	plugin->setRefreshingUI(false);
}



#ifndef DEBUG
Q_EXPORT_PLUGIN2(RenderPlugin, RenderPlugin)
#else
Q_EXPORT_PLUGIN2(RenderPluginD, RenderPlugin)
#endif
