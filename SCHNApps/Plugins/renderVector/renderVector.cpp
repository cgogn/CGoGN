#include "renderVector.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"

bool RenderVectorPlugin::enable()
{
	m_dockTab = new RenderVectorDockTab();
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
	TabParams* params = h_viewParams[view];

	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	foreach(MapHandlerGen* m, maps)
	{
		std::cout << "draw map " << m->getName().toUtf8().constData() << std::endl;
		const PerMapTabParams& p = params->perMap[m->getName()];
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
	TabParams* params = new TabParams();
	h_viewParams.insert(view, params);

	m_dockTab->mapList->clear();
	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	if(maps.empty())
		params->selectedMap = NULL;
	else
	{
		params->selectedMap = maps[0];
		for(int i = 0; i < maps.count(); ++i)
		{
			PerMapTabParams p(NULL, NULL, 1.0f);
			params->perMap.insert(maps[i]->getName(), p);

			m_dockTab->mapList->addItem(maps[i]->getName());
			if(maps[i] == params->selectedMap)
			{
				m_dockTab->mapList->item(i)->setSelected(true);
				m_dockTab->combo_positionVBO->clear();
				m_dockTab->combo_vectorVBO->clear();
				QList<Utils::VBO*> vbos = maps[i]->getVBOList();
				for(int j = 0; j < vbos.count(); ++j)
				{
					m_dockTab->combo_positionVBO->addItem(QString::fromStdString(vbos[j]->name()));
					m_dockTab->combo_vectorVBO->addItem(QString::fromStdString(vbos[j]->name()));
				}
				if(vbos.count() > 0)
				{
					p.positionVBO = vbos[0];
					m_dockTab->combo_positionVBO->setCurrentIndex(0);
					p.vectorVBO = vbos[0];
					m_dockTab->combo_vectorVBO->setCurrentIndex(0);
				}
			}
		}
	}
}

void RenderVectorPlugin::viewUnlinked(View* view)
{
	h_viewParams.remove(view);
}

void RenderVectorPlugin::mapLinked(View* view, MapHandlerGen* m)
{
	assert(isLinkedToView(view));
	TabParams* params = h_viewParams[view];
	PerMapTabParams p(NULL, NULL, 1.0f);
	params->perMap.insert(m->getName(), p);
	m_dockTab->mapList->addItem(m->getName());
}

void RenderVectorPlugin::mapUnlinked(View* view, MapHandlerGen* m)
{
	assert(isLinkedToView(view));
	TabParams* params = h_viewParams[view];
	params->perMap.remove(m->getName());
//	for(int i = 0; i < m_dockTab->mapList->count(); ++i)
//	{
//		if(m_dockTab->mapList->item(i)->text() == m->getName())
//		{
//			delete m_dockTab->mapList->item(i);
//			return;
//		}
//	}
}

void RenderVectorPlugin::currentViewChanged(View* view)
{
	TabParams* params = h_viewParams[view];

	m_dockTab->mapList->clear();
	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	for(int i = 0; i < maps.count(); ++i)
	{
		m_dockTab->mapList->addItem(maps[i]->getName());
		if(maps[i] == params->selectedMap)
		{
			m_dockTab->mapList->item(i)->setSelected(true);
			PerMapTabParams p = params->perMap[maps[i]->getName()];
			m_dockTab->slider_vectorsScaleFactor->setSliderPosition(p.vectorsScaleFactor * 50.0);
			m_dockTab->combo_positionVBO->clear();
			m_dockTab->combo_vectorVBO->clear();
			QList<Utils::VBO*> vbos = maps[i]->getVBOList();
			for(int j = 0; j < vbos.count(); ++j)
			{
				m_dockTab->combo_positionVBO->addItem(QString::fromStdString(vbos[j]->name()));
				if(vbos[j] == p.positionVBO)
					m_dockTab->combo_positionVBO->setCurrentIndex(j);
				m_dockTab->combo_vectorVBO->addItem(QString::fromStdString(vbos[j]->name()));
				if(vbos[j] == p.vectorVBO)
					m_dockTab->combo_vectorVBO->setCurrentIndex(j);
			}
			if(p.positionVBO == NULL && vbos.count() > 0)
				m_dockTab->combo_positionVBO->setCurrentIndex(0);
			if(p.vectorVBO == NULL && vbos.count() > 0)
				m_dockTab->combo_vectorVBO->setCurrentIndex(0);
		}
	}
}

void RenderVectorPlugin::refreshTabInfo()
{
	m_dockTab->mapList->clear();
	m_dockTab->combo_positionVBO->clear();
	m_dockTab->combo_vectorVBO->clear();
	View* view = m_window->getCurrentView();

	assert(isLinkedToView(view));

	TabParams* params = h_viewParams[view];
	MapHandlerGen* map = params->selectedMap;

	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	for(int i = 0; i < maps.count(); ++i)
	{
		m_dockTab->mapList->addItem(maps[i]->getName());
		if(maps[i] == map)
			m_dockTab->mapList->item(i)->setSelected(true);
	}

	if(map != NULL)
	{
		PerMapTabParams p = params->perMap[map->getName()];
		m_dockTab->slider_vectorsScaleFactor->setSliderPosition(p.vectorsScaleFactor * 50.0);
		QList<Utils::VBO*> vbos = map->getVBOList();
		for(int i = 0; i < vbos.count(); ++i)
		{
			m_dockTab->combo_positionVBO->addItem(QString::fromStdString(vbos[i]->name()));
			if(vbos[i] == p.positionVBO)
				m_dockTab->combo_positionVBO->setCurrentIndex(i);
			m_dockTab->combo_vectorVBO->addItem(QString::fromStdString(vbos[i]->name()));
			if(vbos[i] == p.vectorVBO)
				m_dockTab->combo_vectorVBO->setCurrentIndex(i);
		}
		if(p.positionVBO == NULL && vbos.count() > 0)
		{
			p.positionVBO = vbos[0];
			m_dockTab->combo_positionVBO->setCurrentIndex(0);
		}
		if(p.vectorVBO == NULL && vbos.count() > 0)
		{
			p.vectorVBO = vbos[0];
			m_dockTab->combo_vectorVBO->setCurrentIndex(0);
		}
	}
}

void RenderVectorPlugin::cb_selectedMapChanged()
{
	View* current = m_window->getCurrentView();
	assert(isLinkedToView(current));

	TabParams* params = h_viewParams[current];

	QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandlerGen* m = m_window->getMap(mapname);

		params->selectedMap = m;
		PerMapTabParams p = params->perMap[m->getName()];
		m_dockTab->combo_positionVBO->clear();
		m_dockTab->combo_vectorVBO->clear();
		QList<Utils::VBO*> vbos = m->getVBOList();
		for(int j = 0; j < vbos.count(); ++j)
		{
			m_dockTab->combo_positionVBO->addItem(QString::fromStdString(vbos[j]->name()));
			if(vbos[j] == p.positionVBO)
				m_dockTab->combo_positionVBO->setCurrentIndex(j);
			m_dockTab->combo_vectorVBO->addItem(QString::fromStdString(vbos[j]->name()));
			if(vbos[j] == p.vectorVBO)
				m_dockTab->combo_vectorVBO->setCurrentIndex(j);
		}
		if(p.positionVBO == NULL && vbos.count() > 0)
			m_dockTab->combo_positionVBO->setCurrentIndex(0);
		if(p.vectorVBO == NULL && vbos.count() > 0)
			m_dockTab->combo_vectorVBO->setCurrentIndex(0);
	}
}

void RenderVectorPlugin::cb_positionVBOChanged(int index)
{
	View* current = m_window->getCurrentView();
	assert(isLinkedToView(current));

	TabParams* params = h_viewParams[current];
	MapHandlerGen* m = params->selectedMap;
	params->perMap[m->getName()].positionVBO = m->getVBO(m_dockTab->combo_positionVBO->currentText());
	current->updateGL();
}

void RenderVectorPlugin::cb_vectorVBOChanged(int index)
{
	View* current = m_window->getCurrentView();
	assert(isLinkedToView(current));

	TabParams* params = h_viewParams[current];
	MapHandlerGen* m = params->selectedMap;
	params->perMap[m->getName()].vectorVBO = m->getVBO(m_dockTab->combo_vectorVBO->currentText());
	current->updateGL();
}

void RenderVectorPlugin::cb_refreshVBOs()
{
	View* current = m_window->getCurrentView();
	assert(isLinkedToView(current));

	TabParams* params = h_viewParams[current];
	MapHandlerGen* m = params->selectedMap;
	if(m != NULL)
	{
		PerMapTabParams p = params->perMap[m->getName()];
		p.positionVBO = NULL;
		p.vectorVBO = NULL;
		m_dockTab->combo_positionVBO->clear();
		m_dockTab->combo_vectorVBO->clear();
		QList<Utils::VBO*> vbos = m->getVBOList();
		for(int j = 0; j < vbos.count(); ++j)
		{
			m_dockTab->combo_positionVBO->addItem(QString::fromStdString(vbos[j]->name()));
			m_dockTab->combo_vectorVBO->addItem(QString::fromStdString(vbos[j]->name()));
		}
		if(vbos.count() > 0)
		{
			p.positionVBO = vbos[0];
			m_dockTab->combo_positionVBO->setCurrentIndex(0);
			p.vectorVBO = vbos[0];
			m_dockTab->combo_vectorVBO->setCurrentIndex(0);
		}
	}
}

void RenderVectorPlugin::cb_vectorsScaleFactorChanged(int i)
{
	View* current = m_window->getCurrentView();
	assert(isLinkedToView(current));

	TabParams* params = h_viewParams[current];
	MapHandlerGen* m = params->selectedMap;
	params->perMap[m->getName()].vectorsScaleFactor = i / 50.0;
	current->updateGL();
}

/**
 * If we want to compile this plugin in debug mode,
 * we also define a DEBUG macro at the compilation
 */
#ifndef DEBUG
// essential Qt function:
// arguments are
//  - the compiled name of the plugin
//  - the main class of our plugin
Q_EXPORT_PLUGIN2(RenderVectorPlugin, RenderVectorPlugin)
#else
Q_EXPORT_PLUGIN2(RenderVectorPluginD, RenderVectorPlugin)
#endif
