#include "renderVector.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"

bool RenderVectorPlugin::enable()
{
	m_dockTab = new RenderVectorDockTab(this);
	addTabInDock(m_dockTab, "RenderVector");

	m_vectorShader = new Utils::ShaderVectorPerVertex() ;
	m_vectorShader->setColor(Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f)) ;

	registerShader(m_vectorShader);

	connect(m_dockTab->mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));
	connect(m_dockTab->combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_positionVBOChanged(int)));
	connect(m_dockTab->combo_vectorVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_vectorVBOChanged(int)));
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
		if(p.positionVBO != NULL && p.vectorVBO != NULL)
		{
			m_vectorShader->setScale(m->getBBdiagSize() / 100.0f * p.vectorsScaleFactor) ;
			m_vectorShader->setAttributePosition(p.positionVBO) ;
			m_vectorShader->setAttributeVector(p.vectorVBO) ;
			glLineWidth(1.0f) ;
			m->draw(m_vectorShader, Algo::Render::GL2::POINTS) ;
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
		PerMapParameterSet p;
		params->perMap.insert(map->getName(), p);
	}

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
	m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderVectorPlugin::mapLinked(View* view, MapHandlerGen* m)
{
	assert(isLinkedToView(view));
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet p;
	params->perMap.insert(m->getName(), p);

	m_dockTab->refreshUI(params);
}

void RenderVectorPlugin::mapUnlinked(View* view, MapHandlerGen* m)
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

void RenderVectorPlugin::cb_vectorVBOChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* map = params->selectedMap;
		params->perMap[map->getName()].vectorVBO = map->getVBO(m_dockTab->combo_vectorVBO->currentText());
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
	combo_vectorVBO->clear();

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

			slider_vectorsScaleFactor->setSliderPosition(p.vectorsScaleFactor * 50.0);

			QList<Utils::VBO*> vbos = map->getVBOList();
			for(int i = 0; i < vbos.count(); ++i)
			{
				combo_positionVBO->addItem(QString::fromStdString(vbos[i]->name()));
				if(vbos[i] == p.positionVBO)
					combo_positionVBO->setCurrentIndex(i);
				combo_vectorVBO->addItem(QString::fromStdString(vbos[i]->name()));
				if(vbos[i] == p.vectorVBO)
					combo_vectorVBO->setCurrentIndex(i);
			}
			if(p.positionVBO == NULL && vbos.count() > 0)
			{
				p.positionVBO = vbos[0];
				combo_positionVBO->setCurrentIndex(0);
			}
			if(p.vectorVBO == NULL && vbos.count() > 0)
			{
				p.vectorVBO = vbos[0];
				combo_vectorVBO->setCurrentIndex(0);
			}
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
