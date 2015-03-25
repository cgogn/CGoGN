#include "surface_renderScalar.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

bool Surface_RenderScalar_Plugin::enable()
{
	//	magic line that init static variables of GenericMap in the plugins
		GenericMap::copyAllStatics(m_schnapps->getStaticPointers());

	m_dockTab = new Surface_RenderScalar_DockTab(m_schnapps, this);
	m_schnapps->addPluginDockTab(this, m_dockTab, "Surface_RenderScalar");

	m_scalarShader = new Utils::ShaderScalarField();

	registerShader(m_scalarShader);

	connect(m_schnapps, SIGNAL(selectedViewChanged(View*, View*)), this, SLOT(selectedViewChanged(View*, View*)));
	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapAdded(map);

	m_dockTab->updateMapParameters();

	return true;
}

void Surface_RenderScalar_Plugin::disable()
{
	delete m_scalarShader;

	disconnect(m_schnapps, SIGNAL(selectedViewChanged(View*, View*)), this, SLOT(selectedViewChanged(View*, View*)));
	disconnect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapRemoved(map);
}

void Surface_RenderScalar_Plugin::drawMap(View* view, MapHandlerGen* map)
{
	const MapParameters& p = h_viewParameterSet[view][map];

	if(p.positionVBO && p.scalarVBO)
	{
		m_scalarShader->setAttributePosition(p.positionVBO);
		m_scalarShader->setAttributeScalar(p.scalarVBO);
		m_scalarShader->setMinValue(p.scalarMin);
		m_scalarShader->setMaxValue(p.scalarMax);
		m_scalarShader->setColorMap(p.colorMap);
		m_scalarShader->setExpansion(p.expansion);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		map->draw(m_scalarShader, Algo::Render::GL2::TRIANGLES);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}





void Surface_RenderScalar_Plugin::selectedViewChanged(View *prev, View *cur)
{
	m_dockTab->updateMapParameters();
}

void Surface_RenderScalar_Plugin::selectedMapChanged(MapHandlerGen *prev, MapHandlerGen *cur)
{
	m_dockTab->updateMapParameters();
	if (cur==NULL)
		m_dockTab->setDisabled(true);
	else
		m_dockTab->setDisabled(false);
}

void Surface_RenderScalar_Plugin::mapAdded(MapHandlerGen* map)
{
	connect(map, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(vboAdded(Utils::VBO*)));
	connect(map, SIGNAL(vboRemoved(Utils::VBO*)), this, SLOT(vboRemoved(Utils::VBO*)));
	connect(map, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
}

void Surface_RenderScalar_Plugin::mapRemoved(MapHandlerGen* map)
{
	disconnect(map, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(vboAdded(Utils::VBO*)));
	disconnect(map, SIGNAL(vboRemoved(Utils::VBO*)), this, SLOT(vboRemoved(Utils::VBO*)));
	disconnect(map, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
}





void Surface_RenderScalar_Plugin::vboAdded(Utils::VBO *vbo)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	if(map == m_schnapps->getSelectedMap())
	{
		if(vbo->dataSize() == 3)
			m_dockTab->addPositionVBO(QString::fromStdString(vbo->name()));
		else if(vbo->dataSize() == 1)
			m_dockTab->addScalarVBO(QString::fromStdString(vbo->name()));
	}
}

void Surface_RenderScalar_Plugin::vboRemoved(Utils::VBO *vbo)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	if(map == m_schnapps->getSelectedMap())
	{
		if(vbo->dataSize() == 3)
			m_dockTab->removePositionVBO(QString::fromStdString(vbo->name()));
		else if(vbo->dataSize() == 1)
			m_dockTab->removeScalarVBO(QString::fromStdString(vbo->name()));
	}

	QSet<View*> viewsToUpdate;

	QHash<View*, QHash<MapHandlerGen*, MapParameters> >::iterator i;
	for (i = h_viewParameterSet.begin(); i != h_viewParameterSet.end(); ++i)
	{
		View* view = i.key();
		QHash<MapHandlerGen*, MapParameters>& viewParamSet = i.value();
		MapParameters& mapParam = viewParamSet[map];
		if(mapParam.positionVBO == vbo)
		{
			mapParam.positionVBO = NULL;
			if(view->isLinkedToMap(map)) viewsToUpdate.insert(view);
		}
		if(mapParam.scalarVBO == vbo)
		{
			mapParam.scalarVBO = NULL;
			if(view->isLinkedToMap(map)) viewsToUpdate.insert(view);
		}
	}

	foreach(View* v, viewsToUpdate)
		v->updateGL();
}

void Surface_RenderScalar_Plugin::attributeModified(unsigned int orbit, QString nameAttr)
{
	if(orbit == VERTEX)
	{
		MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

		QHash<View*, QHash<MapHandlerGen*, MapParameters> >::iterator i;
		for (i = h_viewParameterSet.begin(); i != h_viewParameterSet.end(); ++i)
		{
//			View* view = i.key();
			QHash<MapHandlerGen*, MapParameters>& viewParamSet = i.value();
			MapParameters& mapParam = viewParamSet[map];
			if(mapParam.scalarVBO && nameAttr == QString::fromStdString(mapParam.scalarVBO->name()))
			{
				MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(map);
				const VertexAttribute<PFP2::REAL, PFP2::MAP>& attr = mh->getAttribute<PFP2::REAL, VERTEX>(nameAttr);
				mapParam.scalarMin = std::numeric_limits<float>::max();
				mapParam.scalarMax = std::numeric_limits<float>::min();
				for(unsigned int i = attr.begin(); i != attr.end(); attr.next(i))
				{
					mapParam.scalarMin = attr[i] < mapParam.scalarMin ? attr[i] : mapParam.scalarMin;
					mapParam.scalarMax = attr[i] > mapParam.scalarMax ? attr[i] : mapParam.scalarMax;
				}
			}
		}
	}
}





void Surface_RenderScalar_Plugin::changePositionVBO(const QString& view, const QString& map, const QString& vbo)
{
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		Utils::VBO* vbuf = m->getVBO(vbo);
		h_viewParameterSet[v][m].positionVBO = vbuf;
		if(v->isSelectedView())
		{
			if (v->isLinkedToMap(m)) v->updateGL();
			if (m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_RenderScalar_Plugin::changeScalarVBO(const QString& view, const QString& map, const QString& vbo)
{
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		MapParameters& p = h_viewParameterSet[v][m];

		Utils::VBO* vbuf = m->getVBO(vbo);
		p.scalarVBO = vbuf;

		if(vbuf)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m);
			const VertexAttribute<PFP2::REAL, PFP2::MAP>& attr = mh->getAttribute<PFP2::REAL, VERTEX>(QString::fromStdString(vbuf->name()));
			p.scalarMin = std::numeric_limits<float>::max();
			p.scalarMax = std::numeric_limits<float>::min();
			for(unsigned int i = attr.begin(); i != attr.end(); attr.next(i))
			{
				p.scalarMin = attr[i] < p.scalarMin ? attr[i] : p.scalarMin;
				p.scalarMax = attr[i] > p.scalarMax ? attr[i] : p.scalarMax;
			}
		}

		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_RenderScalar_Plugin::changeColorMap(const QString& view, const QString& map, int c)
{
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_viewParameterSet[v][m].colorMap = c;
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_RenderScalar_Plugin::changeExpansion(const QString& view, const QString& map, int i)
{
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_viewParameterSet[v][m].expansion = i;
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#else
	Q_EXPORT_PLUGIN2(Surface_RenderScalar_Plugin, Surface_RenderScalar_Plugin)
#endif


} // namespace SCHNApps

} // namespace CGoGN
