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
	connect(m_dockTab->list_vectorVBO, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedVectorVBOChanged()));
	connect(m_dockTab->button_refreshVBOs, SIGNAL(clicked()), this, SLOT(cb_refreshVBOs()));
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
		params->selectedMap = maps[0];

	m_dockTab->refreshUI(params);
}

void RenderVectorPlugin::viewUnlinked(View* view)
{
	h_viewParams.remove(view);

	View* current = m_window->getCurrentView();
	if(isLinkedToView(current))
		m_dockTab->refreshUI(h_viewParams[current]);
}

void RenderVectorPlugin::currentViewChanged(View* view)
{
	assert(isLinkedToView(view));
	m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderVectorPlugin::mapLinked(View* view, MapHandlerGen* m)
{
	assert(isLinkedToView(view));
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet p(m);
	params->perMap.insert(m->getName(), p);
	if(params->perMap.count() == 1)
		params->selectedMap = m;

	m_dockTab->refreshUI(params);
}

void RenderVectorPlugin::mapUnlinked(View* view, MapHandlerGen* m)
{
	assert(isLinkedToView(view));
	ParameterSet* params = h_viewParams[view];
	params->perMap.remove(m->getName());

	if(params->selectedMap == m)
	{
		if(!params->perMap.empty())
			params->selectedMap = m_window->getMap(params->perMap.begin().key());
		else
			params->selectedMap = NULL;
		m_dockTab->refreshUI(params);
	}
}

void RenderVectorPlugin::cb_selectedMapChanged()
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

void RenderVectorPlugin::cb_positionVBOChanged(int index)
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

void RenderVectorPlugin::cb_selectedVectorVBOChanged()
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* map = params->selectedMap;
		PerMapParameterSet& mapParam = params->perMap[map->getName()];

		mapParam.vectorVBO.clear();
		QList<QListWidgetItem*> currentItems = m_dockTab->list_vectorVBO->selectedItems();
		foreach(QListWidgetItem* item, currentItems)
			mapParam.vectorVBO.push_back(map->getVBO(item->text()));

		current->updateGL();
	}
}

void RenderVectorPlugin::cb_refreshVBOs()
{
	View* current = m_window->getCurrentView();
	if(isLinkedToView(current))
		m_dockTab->refreshUI(h_viewParams[current]);
}

void RenderVectorPlugin::cb_vectorsScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* m = params->selectedMap;
		params->perMap[m->getName()].vectorsScaleFactor = i / 50.0;
		current->updateGL();
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

//			QList<Utils::VBO*> vbos = map->getVBOList(nameOfType(VEC3()));
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

				list_vectorVBO->addItem(QString::fromStdString(vbos[i]->name()));
				if(std::find(p.vectorVBO.begin(), p.vectorVBO.end(), vbos[i]) != p.vectorVBO.end())
					list_vectorVBO->item(i)->setSelected(true);
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
