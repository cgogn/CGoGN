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
	connect(m_dockTab->button_refreshVBOs, SIGNAL(clicked()), this, SLOT(cb_refreshVBOs()));
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

	m_dockTab->refreshUI(params);
}

void RenderPlugin::viewUnlinked(View* view)
{
	h_viewParams.remove(view);

	View* current = m_window->getCurrentView();
	if(isLinkedToView(current))
		m_dockTab->refreshUI(h_viewParams[current]);
}

void RenderPlugin::currentViewChanged(View* view)
{
	assert(isLinkedToView(view));
	m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderPlugin::mapLinked(View* view, MapHandlerGen* m)
{
	assert(isLinkedToView(view));
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet p(m);
	params->perMap.insert(m->getName(), p);

	m_dockTab->refreshUI(params);
}

void RenderPlugin::mapUnlinked(View* view, MapHandlerGen* m)
{
	assert(isLinkedToView(view));
	ParameterSet* params = h_viewParams[view];
	params->perMap.remove(m->getName());

	if(params->selectedMap == m)
	{
		params->selectedMap = NULL;
		m_dockTab->refreshUI(params);
	}
}

void RenderPlugin::cb_selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[view];
		QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
		if(!currentItems.empty())
		{
			const QString& mapname = currentItems[0]->text();
			params->selectedMap = m_window->getMap(mapname);
			m_dockTab->refreshUI(params);
			view->updateGL();
		}
	}
}

void RenderPlugin::cb_positionVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* map = params->selectedMap;
		params->perMap[map->getName()].positionVBO = map->getVBO(m_dockTab->combo_positionVBO->currentText());
		current->updateGL();
	}
}

void RenderPlugin::cb_normalVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* map = params->selectedMap;
		params->perMap[map->getName()].normalVBO = map->getVBO(m_dockTab->combo_normalVBO->currentText());
		current->updateGL();
	}
}

void RenderPlugin::cb_refreshVBOs()
{
	View* current = m_window->getCurrentView();
	if(isLinkedToView(current))
		m_dockTab->refreshUI(h_viewParams[current]);
}

void RenderPlugin::cb_renderVerticesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* m = params->selectedMap;
		params->perMap[m->getName()].renderVertices = b;
		current->updateGL();
	}
}

void RenderPlugin::cb_verticesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* m = params->selectedMap;
		params->perMap[m->getName()].verticesScaleFactor = i / 50.0;
		current->updateGL();
	}
}

void RenderPlugin::cb_renderEdgesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* m = params->selectedMap;
		params->perMap[m->getName()].renderEdges = b;
		current->updateGL();
	}
}

void RenderPlugin::cb_renderFacesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* m = params->selectedMap;
		params->perMap[m->getName()].renderFaces = b;
		current->updateGL();
	}
}

void RenderPlugin::cb_faceStyleChanged(QAbstractButton* b)
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* m = params->selectedMap;
		if(m_dockTab->radio_flatShading->isChecked())
			params->perMap[m->getName()].faceStyle = FLAT;
		else if(m_dockTab->radio_phongShading->isChecked())
			params->perMap[m->getName()].faceStyle = PHONG;
		current->updateGL();
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
			for(int i = 0; i < vbos.count(); ++i)
			{
				combo_positionVBO->addItem(QString::fromStdString(vbos[i]->name()));
				if(p.positionVBO == NULL)
				{										// if nothing is specified in the parameter set
					if(vbos[i]->name() == "position")	// try to select a VBO named "position"
					{
						p.positionVBO = vbos[i];
						combo_positionVBO->setCurrentIndex(i);
					}
				}
				else if(vbos[i] == p.positionVBO)
					combo_positionVBO->setCurrentIndex(i);

				combo_normalVBO->addItem(QString::fromStdString(vbos[i]->name()));
				if(p.normalVBO == NULL)
				{									// if nothing is specified in the parameter set
					if(vbos[i]->name() == "normal")	// try to select a VBO named "normal"
					{
						p.normalVBO = vbos[i];
						combo_normalVBO->setCurrentIndex(i);
					}
				}
				else if(vbos[i] == p.normalVBO)
					combo_normalVBO->setCurrentIndex(i);
			}

			if(p.positionVBO == NULL && vbos.count() > 0)
			{
				p.positionVBO = vbos[0];
				combo_positionVBO->setCurrentIndex(0);
			}
			if(p.normalVBO == NULL && vbos.count() > 0)
			{
				p.normalVBO = vbos[0];
				combo_normalVBO->setCurrentIndex(0);
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
