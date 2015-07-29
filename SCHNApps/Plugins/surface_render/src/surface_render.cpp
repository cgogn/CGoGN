#include "surface_render.h"

#include "mapHandler.h"
#include "slot_debug.h"

namespace CGoGN
{

namespace SCHNApps
{

bool Surface_Render_Plugin::enable()
{
	//	magic line that init static variables of GenericMap in the plugins
	GenericMap::copyAllStatics(m_schnapps->getStaticPointers());

	m_dockTab = new Surface_Render_DockTab(m_schnapps, this);
	m_schnapps->addPluginDockTab(this, m_dockTab, "Surface_Render");

	m_flatShader = new CGoGN::Utils::ShaderFlat();
	m_flatShader->setAmbiant(CGoGN::Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f));
	m_flatShader->setExplode(1.0f);

	m_phongShader = new CGoGN::Utils::ShaderPhong();
	m_phongShader->setAmbiant(CGoGN::Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f));
	m_phongShader->setSpecular(CGoGN::Geom::Vec4f(0.9f, 0.9f, 0.9f, 1.0f));
	m_phongShader->setShininess(80.0f);

	m_colorPerVertexShader = new CGoGN::Utils::ShaderColorPerVertex();

	m_simpleColorShader = new CGoGN::Utils::ShaderSimpleColor();

	m_pointSprite = new CGoGN::Utils::PointSprite();

	registerShader(m_flatShader);
	registerShader(m_phongShader);
	registerShader(m_colorPerVertexShader);
	registerShader(m_simpleColorShader);
	registerShader(m_pointSprite);

	connect(m_schnapps, SIGNAL(selectedViewChanged(View*, View*)), this, SLOT(selectedViewChanged(View*, View*)));
	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(schnappsClosing()), this, SLOT(schnappsClosing()));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapAdded(map);

	m_dockTab->updateMapParameters();

	return true;
}

void Surface_Render_Plugin::disable()
{
	delete m_flatShader;
	delete m_phongShader;
	delete m_simpleColorShader;
	delete m_pointSprite;

	disconnect(m_schnapps, SIGNAL(selectedViewChanged(View*, View*)), this, SLOT(selectedViewChanged(View*, View*)));
	disconnect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(schnappsClosing()), this, SLOT(schnappsClosing()));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapRemoved(map);
}

void Surface_Render_Plugin::drawMap(View* view, MapHandlerGen* map)
{
	const MapParameters& p = h_viewParameterSet[view][map];
	if(p.positionVBO)
	{
        if(p.renderFaces)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_LIGHTING);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0f, 1.0f);
            switch(p.faceStyle)
            {
                case MapParameters::FLAT :
					if(p.colorVBO)
					{
						m_colorPerVertexShader->setAttributePosition(p.positionVBO);
						m_colorPerVertexShader->setAttributeColor(p.colorVBO);
						map->draw(m_colorPerVertexShader, CGoGN::Algo::Render::GL2::TRIANGLES);
					}
					else
					{
						m_flatShader->setBackColor(p.backColor);
						m_flatShader->setAttributePosition(p.positionVBO);
						m_flatShader->setDiffuse(p.diffuseColor);
						m_flatShader->setDoubleSided(p.renderBackfaces);
						map->draw(m_flatShader, CGoGN::Algo::Render::GL2::TRIANGLES);
					}
                    break;
                case MapParameters::PHONG :
                    if(p.normalVBO != NULL)
                    {
						m_phongShader->setBackColor(p.backColor);
                        m_phongShader->setAttributePosition(p.positionVBO);
                        m_phongShader->setAttributeNormal(p.normalVBO);
                        m_phongShader->setDiffuse(p.diffuseColor);
						m_phongShader->setDoubleSided(p.renderBackfaces);
                        map->draw(m_phongShader, CGoGN::Algo::Render::GL2::TRIANGLES);
                    }
                    break;
            }
            glDisable(GL_POLYGON_OFFSET_FILL);
        }

		if(p.renderVertices)
		{
			m_pointSprite->setSize(p.basePSradius * p.verticesScaleFactor); 
			m_pointSprite->setAttributePosition(p.positionVBO);
			m_pointSprite->setColor(p.vertexColor);
			map->draw(m_pointSprite, CGoGN::Algo::Render::GL2::POINTS);
		}

		if(p.renderEdges)
		{
			glLineWidth(1.0f);
			m_simpleColorShader->setAttributePosition(p.positionVBO);
			m_simpleColorShader->setColor(p.simpleColor);
			map->draw(m_simpleColorShader, CGoGN::Algo::Render::GL2::LINES);
		}

		if(p.renderBoundary)
		{
			glLineWidth(5.0f);
			CGoGN::Geom::Vec4f c(0.8f, 0.8f, 0.1f, 1.0f);
			m_simpleColorShader->setColor(c);
			m_simpleColorShader->setAttributePosition(p.positionVBO);
			map->draw(m_simpleColorShader, CGoGN::Algo::Render::GL2::BOUNDARY);
		}
	}
}





void Surface_Render_Plugin::selectedViewChanged(View *prev, View *cur)
{
	DEBUG_SLOT();
	m_dockTab->updateMapParameters();
}

void Surface_Render_Plugin::selectedMapChanged(MapHandlerGen *prev, MapHandlerGen *cur)
{
	DEBUG_SLOT();
	m_dockTab->updateMapParameters();
}

void Surface_Render_Plugin::mapAdded(MapHandlerGen *map)
{
	DEBUG_SLOT();
	connect(map, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(vboAdded(Utils::VBO*)));
	connect(map, SIGNAL(vboRemoved(Utils::VBO*)), this, SLOT(vboRemoved(Utils::VBO*)));
	connect(map, SIGNAL(boundingBoxModified()), this, SLOT(boundingBoxModified()));
}

void Surface_Render_Plugin::mapRemoved(MapHandlerGen *map)
{
	DEBUG_SLOT();
	disconnect(map, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(vboAdded(Utils::VBO*)));
	disconnect(map, SIGNAL(vboRemoved(Utils::VBO*)), this, SLOT(vboRemoved(Utils::VBO*)));
	disconnect(map, SIGNAL(boundingBoxModified()), this, SLOT(boundingBoxModified()));

	if(map == m_schnapps->getSelectedMap())
		m_dockTab->updateMapParameters();
}





void Surface_Render_Plugin::vboAdded(Utils::VBO *vbo)
{
	DEBUG_SLOT();
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	if(map == m_schnapps->getSelectedMap())
	{
		if(vbo->dataSize() == 3)
		{
			m_dockTab->addPositionVBO(QString::fromStdString(vbo->name()));
			m_dockTab->addNormalVBO(QString::fromStdString(vbo->name()));
			m_dockTab->addColorVBO(QString::fromStdString(vbo->name()));
		}
	}
}

void Surface_Render_Plugin::vboRemoved(Utils::VBO *vbo)
{
	DEBUG_SLOT();
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	if(map == m_schnapps->getSelectedMap())
	{
		if(vbo->dataSize() == 3)
		{
			m_dockTab->removePositionVBO(QString::fromStdString(vbo->name()));
			m_dockTab->removeNormalVBO(QString::fromStdString(vbo->name()));
			m_dockTab->removeColorVBO(QString::fromStdString(vbo->name()));
		}
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
		if(mapParam.normalVBO == vbo)
		{
			mapParam.normalVBO = NULL;
			if(view->isLinkedToMap(map)) viewsToUpdate.insert(view);
		}
		if(mapParam.colorVBO == vbo)
		{
			mapParam.colorVBO = NULL;
			if(view->isLinkedToMap(map)) viewsToUpdate.insert(view);
		}
	}

	foreach(View* v, viewsToUpdate)
		v->updateGL();
}

void Surface_Render_Plugin::boundingBoxModified()
{
	DEBUG_SLOT();
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	QList<View*> views = map->getLinkedViews();
	foreach(View* v, views)
	{
		if (h_viewParameterSet.contains(v))
			h_viewParameterSet[v][map].basePSradius = map->getBBdiagSize() / (2 * std::sqrt(map->getNbOrbits(EDGE)));
	}
}

void Surface_Render_Plugin::changePositionVBO(const QString& view, const QString& map, const QString& vbo)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		Utils::VBO* vbuf = m->getVBO(vbo);
		h_viewParameterSet[v][m].positionVBO = vbuf;
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeNormalVBO(const QString& view, const QString& map, const QString& vbo)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		Utils::VBO* vbuf = m->getVBO(vbo);
		h_viewParameterSet[v][m].normalVBO = vbuf;
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeColorVBO(const QString& view, const QString& map, const QString& vbo)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		Utils::VBO* vbuf = m->getVBO(vbo);
		h_viewParameterSet[v][m].colorVBO = vbuf;
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeRenderVertices(const QString& view, const QString& map, bool b)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_viewParameterSet[v][m].renderVertices = b;
		if (b)
			h_viewParameterSet[v][m].basePSradius = m->getBBdiagSize() / (2 * std::sqrt(m->getNbOrbits(EDGE)));
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeVerticesScaleFactor(const QString& view, const QString& map, float f)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_viewParameterSet[v][m].verticesScaleFactor = f;
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeRenderEdges(const QString& view, const QString& map, bool b)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_viewParameterSet[v][m].renderEdges = b;
		if (b)
			h_viewParameterSet[v][m].basePSradius = m->getBBdiagSize() / (16 * std::sqrt(m->getNbOrbits(EDGE)));
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeRenderFaces(const QString& view, const QString& map, bool b)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_viewParameterSet[v][m].renderFaces = b;
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeFacesStyle(const QString& view, const QString& map, int iStyle)
{
	MapParameters::FaceShadingStyle style = MapParameters::FaceShadingStyle(iStyle);
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_viewParameterSet[v][m].faceStyle = style;
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeRenderBoundary(const QString& view, const QString& map, bool b)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_viewParameterSet[v][m].renderBoundary = b;
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeFaceColor(const QString& view, const QString& map, float r, float g, float b)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_viewParameterSet[v][m].diffuseColor = Geom::Vec4f(r,g,b,0);
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeEdgeColor(const QString& view, const QString& map, float r, float g, float b)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_viewParameterSet[v][m].simpleColor = Geom::Vec4f(r,g,b,0);
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeVertexColor(const QString& view, const QString& map, float r, float g, float b)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_viewParameterSet[v][m].vertexColor = Geom::Vec4f(r,g,b,0);
		if(v->isSelectedView())
		{
			if(v->isLinkedToMap(m))	v->updateGL();
			if(m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}


void Surface_Render_Plugin::changeBackColor(const QString& view, const QString& map, float r, float g, float b)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if (v && m)
	{
		h_viewParameterSet[v][m].backColor = Geom::Vec4f(r, g, b, 0);
		if (v->isSelectedView())
		{
			if (v->isLinkedToMap(m))	v->updateGL();
			if (m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Render_Plugin::changeRenderBackfaces(const QString& view, const QString& map, bool b)
{
	DEBUG_SLOT();
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if (v && m)
	{
		h_viewParameterSet[v][m].renderBackfaces = b;
		if (v->isSelectedView())
		{
			if (v->isLinkedToMap(m))	v->updateGL();
			if (m->isSelectedMap()) m_dockTab->updateMapParameters();
		}
	}
}



void Surface_Render_Plugin::schnappsClosing()
{
	m_dockTab->m_colorDial->close();
}
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#else
	Q_EXPORT_PLUGIN2(Surface_Render_Plugin, Surface_Render_Plugin)
#endif


} // namespace SCHNApps

} // namespace CGoGN
