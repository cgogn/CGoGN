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

//    m_renderExplod = new Algo::Render::GL2::ExplodeVolumeRender(true);
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
	connect(m_dockTab->button_refreshVBOs, SIGNAL(clicked()), this, SLOT(cb_refreshVBOs()));
	connect(m_dockTab->check_renderEdges, SIGNAL(toggled(bool)), this, SLOT(cb_renderEdgesChanged(bool)));
	connect(m_dockTab->check_renderFaces, SIGNAL(toggled(bool)), this, SLOT(cb_renderFacesChanged(bool)));
	connect(m_dockTab->slider_facesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(cb_facesScaleFactorChanged(int)));
	connect(m_dockTab->slider_volumesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(cb_volumesScaleFactorChanged(int)));

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

void RenderExplodPlugin::viewLinked(View* view)
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

void RenderExplodPlugin::viewUnlinked(View* view)
{
	h_viewParams.remove(view);

	View* current = m_window->getCurrentView();
	if(isLinkedToView(current))
		m_dockTab->refreshUI(h_viewParams[current]);
}

void RenderExplodPlugin::currentViewChanged(View* view)
{
	assert(isLinkedToView(view));
	m_dockTab->refreshUI(h_viewParams[view]);
}

void RenderExplodPlugin::mapLinked(View* view, MapHandlerGen* m)
{
	assert(isLinkedToView(view));
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet p(m);
	params->perMap.insert(m->getName(), p);
	if(params->perMap.count() == 1)
		params->selectedMap = m;

	m_dockTab->refreshUI(params);
}

void RenderExplodPlugin::mapUnlinked(View* view, MapHandlerGen* m)
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

void RenderExplodPlugin::cb_selectedMapChanged()
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

void RenderExplodPlugin::cb_positionVBOChanged(int index)
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

void RenderExplodPlugin::cb_colorVBOChanged(int index)
{
	if(!b_refreshingUI)
	{

	}
}

void RenderExplodPlugin::cb_refreshVBOs()
{
	View* current = m_window->getCurrentView();
	if(isLinkedToView(current))
		m_dockTab->refreshUI(h_viewParams[current]);
}

void RenderExplodPlugin::cb_renderEdgesChanged(bool b)
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

void RenderExplodPlugin::cb_renderFacesChanged(bool b)
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

void RenderExplodPlugin::cb_facesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* m = params->selectedMap;
		params->perMap[m->getName()].facesScaleFactor = i / 50.0;
		current->updateGL();
	}
}

void RenderExplodPlugin::cb_volumesScaleFactorChanged(int i)
{
	if(!b_refreshingUI)
	{
		View* current = m_window->getCurrentView();
		ParameterSet* params = h_viewParams[current];
		MapHandlerGen* m = params->selectedMap;
		params->perMap[m->getName()].volumesScaleFactor = i / 50.0;
		current->updateGL();
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
//
//			QList<Utils::VBO*> vbos = map->getVBOList();
//			for(int i = 0; i < vbos.count(); ++i)
//			{
//				combo_positionVBO->addItem(QString::fromStdString(vbos[i]->name()));
//				if(p.positionVBO == NULL)
//				{										// if nothing is specified in the parameter set
//					if(vbos[i]->name() == "position")	// try to select a VBO named "position"
//					{
//						p.positionVBO = vbos[i];
//						combo_positionVBO->setCurrentIndex(i);
//					}
//				}
//				else if(vbos[i] == p.positionVBO)
//					combo_positionVBO->setCurrentIndex(i);
//
//				combo_normalVBO->addItem(QString::fromStdString(vbos[i]->name()));
//				if(p.normalVBO == NULL)
//				{									// if nothing is specified in the parameter set
//					if(vbos[i]->name() == "normal")	// try to select a VBO named "normal"
//					{
//						p.normalVBO = vbos[i];
//						combo_normalVBO->setCurrentIndex(i);
//					}
//				}
//				else if(vbos[i] == p.normalVBO)
//					combo_normalVBO->setCurrentIndex(i);
//			}
//
//			if(p.positionVBO == NULL && vbos.count() > 0)
//			{
//				p.positionVBO = vbos[0];
//				combo_positionVBO->setCurrentIndex(0);
//			}
//			if(p.normalVBO == NULL && vbos.count() > 0)
//			{
//				p.normalVBO = vbos[0];
//				combo_normalVBO->setCurrentIndex(0);
//			}

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
