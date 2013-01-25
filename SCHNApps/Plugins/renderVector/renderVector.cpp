#include "renderVector.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"


PerMapParameterSet::PerMapParameterSet(MapHandlerGen* map) :
	positionVBO(NULL),
	vectorsScaleFactor(1.0f)
{
	QList<Utils::VBO*> vbos = map->getVBOList();
	for(int i = 0; i < vbos.count(); ++i)
	{
		if(vbos[i]->name() == "position") // try to select a VBO named "position"
			positionVBO = vbos[i];
	}

	if(positionVBO == NULL && vbos.count() > 0)
		positionVBO = vbos[0];
}


bool RenderVectorPlugin::enable()
{
	m_dockTab = new RenderVectorDockTab(this);
	addTabInDock(m_dockTab, "RenderVector");

	m_vectorShader = new Utils::ShaderVectorPerVertex();
	m_vectorShader->setColor(Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));

	registerShader(m_vectorShader);

	connect(m_dockTab->mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));
	connect(m_dockTab->combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_positionVBOChanged(int)));
	connect(m_dockTab->list_vectorVBO, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedVectorsVBOChanged()));
	connect(m_dockTab->slider_vectorsScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(cb_vectorsScaleFactorChanged(int)));

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

void RenderVectorPlugin::viewLinked(View* view)
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

void RenderVectorPlugin::viewUnlinked(View* view)
{
	h_viewParams.remove(view);

	disconnect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
	disconnect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));
}

void RenderVectorPlugin::currentViewChanged(View* view)
{
	assert(isLinkedToView(view));
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
}

void RenderVectorPlugin::changeSelectedMap(View* view, MapHandlerGen* map)
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

void RenderVectorPlugin::cb_selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
		if(!currentItems.empty())
			changeSelectedMap(m_window->getCurrentView(), m_window->getMap(currentItems[0]->text()));
	}
}

void RenderVectorPlugin::cb_positionVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changePositionVBO(view, map, map->getVBO(m_dockTab->combo_positionVBO->currentText()));
	}
}

void RenderVectorPlugin::cb_selectedVectorsVBOChanged()
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		QList<QListWidgetItem*> currentItems = m_dockTab->list_vectorVBO->selectedItems();
		std::vector<Utils::VBO*> vbos;
		foreach(QListWidgetItem* item, currentItems)
			vbos.push_back(map->getVBO(item->text()));
		changeSelectedVectorsVBO(view, map, vbos);
	}
}

void RenderVectorPlugin::cb_vectorsScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeVectorsScaleFactor(view, map, i);
	}
}



void RenderVectorDockTab::refreshUI(ParameterSet* params)
{
	plugin->setRefreshingUI(true);

	mapList->clear();
	combo_positionVBO->clear();
	list_vectorVBO->clear();

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

					list_vectorVBO->addItem(QString::fromStdString(vbos[i]->name()));
					if(std::find(p.vectorVBO.begin(), p.vectorVBO.end(), vbos[i]) != p.vectorVBO.end())
						list_vectorVBO->item(j)->setSelected(true);

					++j;
				}
			}

			if(p.positionVBO == NULL && vbos.count() > 0)
			{
				p.positionVBO = vbos[0];
				combo_positionVBO->setCurrentIndex(0);
			}

			slider_vectorsScaleFactor->setSliderPosition(p.vectorsScaleFactor * 50.0);
		}
		++i;
	}

	plugin->setRefreshingUI(false);
}



#ifndef DEBUG
Q_EXPORT_PLUGIN2(RenderVectorPlugin, RenderVectorPlugin)
#else
Q_EXPORT_PLUGIN2(RenderVectorPluginD, RenderVectorPlugin)
#endif
