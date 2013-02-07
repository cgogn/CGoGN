#include "renderExplod.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"


PerMapParameterSet::PerMapParameterSet(MapHandlerGen* map) :
	positionVBO(NULL),
	colorVBO(NULL),
	facesScaleFactor(1.0f),
	volumesScaleFactor(1.0f),
	renderEdges(false),
	renderFaces(true)
{
	QList<Utils::VBO*> vbos = map->getVBOList();
	for(int i = 0; i < vbos.count(); ++i)
	{
		if(vbos[i]->name() == "position") // try to select a VBO named "position"
			positionVBO = vbos[i];
		if(vbos[i]->name() == "color")	// try to select a VBO named "color"
			colorVBO = vbos[i];
	}

	if(positionVBO == NULL && vbos.count() > 0)
		positionVBO = vbos[0];
	if(colorVBO == NULL && vbos.count() > 0)
		colorVBO = vbos[0];
}


bool RenderExplodPlugin::enable()
{
	m_dockTab = new RenderExplodDockTab(this);
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

	connect(m_dockTab->mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));
	connect(m_dockTab->combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_positionVBOChanged(int)));
	connect(m_dockTab->combo_colorVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_colorVBOChanged(int)));
	connect(m_dockTab->check_renderEdges, SIGNAL(toggled(bool)), this, SLOT(cb_renderEdgesChanged(bool)));
	connect(m_dockTab->check_renderFaces, SIGNAL(toggled(bool)), this, SLOT(cb_renderFacesChanged(bool)));
	connect(m_dockTab->slider_facesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(cb_facesScaleFactorChanged(int)));
	connect(m_dockTab->slider_volumesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(cb_volumesScaleFactorChanged(int)));

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

void RenderExplodPlugin::changeSelectedMap(View* view, MapHandlerGen* map)
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

void RenderExplodPlugin::changePositionVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].positionVBO = vbo;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeColorVBO(View* view, MapHandlerGen* map, Utils::VBO* vbo)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].colorVBO = vbo;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeRenderEdges(View* view, MapHandlerGen* map, bool b)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].renderEdges = b;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeRenderFaces(View* view, MapHandlerGen* map, bool b)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].renderFaces = b;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeFacesScaleFactor(View* view, MapHandlerGen* map, int i)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].facesScaleFactor = i / 50.0;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::changeVolumesScaleFactor(View* view, MapHandlerGen* map, int i)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].volumesScaleFactor = i / 50.0;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void RenderExplodPlugin::cb_selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
		if(!currentItems.empty())
			changeSelectedMap(m_window->getCurrentView(), m_window->getMap(currentItems[0]->text()));
	}
}

void RenderExplodPlugin::cb_positionVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changePositionVBO(view, map, map->getVBO(m_dockTab->combo_positionVBO->currentText()));
	}
}

void RenderExplodPlugin::cb_colorVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeColorVBO(view, map, map->getVBO(m_dockTab->combo_positionVBO->currentText()));
	}
}

void RenderExplodPlugin::cb_renderEdgesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeRenderEdges(view, map, b);
	}
}

void RenderExplodPlugin::cb_renderFacesChanged(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeRenderFaces(view, map, b);
	}
}

void RenderExplodPlugin::cb_facesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeFacesScaleFactor(view, map, i);
	}
}

void RenderExplodPlugin::cb_volumesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeVolumesScaleFactor(view, map, i);
	}
}


void RenderExplodDockTab::refreshUI(ParameterSet* params)
{
	plugin->setRefreshingUI(true);

	mapList->clear();
	combo_positionVBO->clear();
	combo_colorVBO->clear();

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

					combo_colorVBO->addItem(QString::fromStdString(vbos[i]->name()));
					if(p.colorVBO == NULL)
					{									// if nothing is specified in the parameter set
						if(vbos[i]->name() == "color")	// try to select a VBO named "color"
						{
							p.colorVBO = vbos[i];
							combo_colorVBO->setCurrentIndex(j);
						}
					}
					else if(vbos[i] == p.colorVBO)
						combo_colorVBO->setCurrentIndex(j);

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
			if(p.colorVBO == NULL && vbos.count() > 0)
			{
				int i = 0;
				bool found = false;
				while(i < vbos.count() && !found)
				{
					if(vbos[i]->dataSize() == 3)
					{
						p.colorVBO = vbos[i];
						combo_colorVBO->setCurrentIndex(i);
						found = true;
					}
					++i;
				}
			}

			check_renderEdges->setChecked(p.renderEdges);
			check_renderFaces->setChecked(p.renderFaces);
			slider_facesScaleFactor->setSliderPosition(p.facesScaleFactor * 50.0);
			slider_volumesScaleFactor->setSliderPosition(p.volumesScaleFactor * 50.0);
		}
		++i;
	}

	plugin->setRefreshingUI(false);
}



#ifndef DEBUG
Q_EXPORT_PLUGIN2(RenderExplodPlugin, RenderExplodPlugin)
#else
Q_EXPORT_PLUGIN2(RenderExplodPluginD, RenderExplodPlugin)
#endif
