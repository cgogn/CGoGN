#include "surface_selection.h"

#include "schnapps.h"
#include "view.h"
#include "mapHandler.h"

#include "Algo/Selection/raySelector.h"
#include "Algo/Selection/collector.h"

#include <QKeyEvent>
#include <QMouseEvent>

namespace CGoGN
{

namespace SCHNApps
{

Surface_Selection_Plugin::Surface_Selection_Plugin() :
	m_selecting(false)
{}

bool Surface_Selection_Plugin::enable()
{
	m_dockTab = new Surface_Selection_DockTab(m_schnapps, this);
	m_schnapps->addPluginDockTab(this, m_dockTab, "Surface_Selection");

	m_drawer = new Utils::Drawer();
	m_selectionSphereVBO = new Utils::VBO();
	m_pointSprite = new CGoGN::Utils::PointSprite();

	registerShader(m_drawer->getShader());
	registerShader(m_pointSprite);

	connect(m_schnapps, SIGNAL(selectedViewChanged(View*, View*)), this, SLOT(selectedViewChanged(View*, View*)));
	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapAdded(map);

	m_dockTab->updateMapParameters();

	return true;
}

void Surface_Selection_Plugin::disable()
{
	delete m_selectionSphereVBO;
	delete m_pointSprite;

	disconnect(m_schnapps, SIGNAL(selectedViewChanged(View*, View*)), this, SLOT(selectedViewChanged(View*, View*)));
	disconnect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));
}

void Surface_Selection_Plugin::draw(View *view)
{
}

void Surface_Selection_Plugin::drawMap(View* view, MapHandlerGen* map)
{
	if(map == m_schnapps->getSelectedMap())
	{
		const MapParameters& p = h_viewParameterSet[view][map];
		if(p.positionAttribute.isValid())
		{
			unsigned int orbit = m_schnapps->getCurrentOrbit();
			CellSelectorGen* selector = m_schnapps->getSelectedSelector(orbit);
			if(selector)
			{
				const std::vector<Dart>& selectedCells = selector->getSelectedCells();

				m_drawer->newList(GL_COMPILE_AND_EXECUTE);
				m_drawer->pointSize(3.0f);
				m_drawer->color3f(0.0f, 0.0f, 1.0f);
				m_drawer->begin(GL_POINTS);
				for(std::vector<Dart>::const_iterator it = selectedCells.begin(); it != selectedCells.end(); ++it)
					m_drawer->vertex(p.positionAttribute[*it]);
				m_drawer->end();
				m_drawer->endList();
			}
		}

		if(m_selecting)
		{
			std::vector<PFP2::VEC3> selectionPoint;
			selectionPoint.push_back(m_selectionCenter);
			m_selectionSphereVBO->updateData(selectionPoint);

			m_pointSprite->setAttributePosition(m_selectionSphereVBO);
			m_pointSprite->setSize(m_selectionRadius);
			m_pointSprite->setColor(CGoGN::Geom::Vec4f(0.0f, 0.0f, 1.0f, 0.5f));
			m_pointSprite->setLightPosition(CGoGN::Geom::Vec3f(0.0f, 0.0f, 1.0f));

			m_pointSprite->enableVertexAttribs();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDrawArrays(GL_POINTS, 0, 1);
			glDisable(GL_BLEND);
			m_pointSprite->disableVertexAttribs();
		}
	}
}

void Surface_Selection_Plugin::keyPress(View* view, QKeyEvent* event)
{
	if(event->key() == Qt::Key_Shift)
	{
		view->setMouseTracking(true);
		m_selecting = true;
//		view->updateGL();
	}
}

void Surface_Selection_Plugin::keyRelease(View* view, QKeyEvent* event)
{
	if(event->key() == Qt::Key_Shift)
	{
		view->setMouseTracking(false);
		m_selecting = false;
//		view->updateGL();
	}
}

void Surface_Selection_Plugin::mousePress(View* view, QMouseEvent* event)
{
	if(m_selecting && (event->button() == Qt::LeftButton || event->button() == Qt::RightButton))
	{
		MapHandlerGen* mh = m_schnapps->getSelectedMap();
		const MapParameters& p = h_viewParameterSet[view][mh];
		if(p.positionAttribute.isValid())
		{
			QPoint pixel(event->x(), event->y());
			qglviewer::Vec orig;
			qglviewer::Vec dir;
			view->camera()->convertClickToLine(pixel, orig, dir);

			qglviewer::Vec orig_inv = mh->getFrame()->coordinatesOf(orig);
			qglviewer::Vec dir_inv = mh->getFrame()->transformOf(dir);

			PFP2::VEC3 rayA(orig_inv.x, orig_inv.y, orig_inv.z);
			PFP2::VEC3 AB(dir_inv.x, dir_inv.y, dir_inv.z);

			Dart d;
			PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(mh)->getMap();
			Algo::Selection::vertexRaySelection<PFP2>(*map, p.positionAttribute, rayA, AB, d);

			if(d != NIL)
			{
				Algo::Surface::Selection::Collector_WithinSphere<PFP2> neigh(*map, p.positionAttribute, m_selectionRadius);
				neigh.collectAll(d);

				unsigned int orbit = m_schnapps->getCurrentOrbit();
				CellSelectorGen* selector = m_schnapps->getSelectedSelector(orbit);

				if(selector)
				{
					if(event->button() == Qt::LeftButton)
					{
						switch(orbit)
						{
							case DART: break;
							case VERTEX: selector->select(neigh.getInsideVertices()); break;
							case EDGE: selector->select(neigh.getInsideEdges()); break;
							case FACE: selector->select(neigh.getInsideFaces()); break;
						}
					}
					else if(event->button() == Qt::RightButton)
					{
						switch(orbit)
						{
							case DART: break;
							case VERTEX: selector->unselect(neigh.getInsideVertices()); break;
							case EDGE: selector->unselect(neigh.getInsideEdges()); break;
							case FACE: selector->unselect(neigh.getInsideFaces()); break;
						}
					}
				}
			}
		}
	}
}

void Surface_Selection_Plugin::mouseRelease(View* view, QMouseEvent* event)
{

}

void Surface_Selection_Plugin::mouseMove(View* view, QMouseEvent* event)
{
	if(m_selecting)
	{
		MapHandlerGen* mh = m_schnapps->getSelectedMap();
		const MapParameters& p = h_viewParameterSet[view][mh];
		if(p.positionAttribute.isValid())
		{
			QPoint pixel(event->x(), event->y());
			qglviewer::Vec orig;
			qglviewer::Vec dir;
			view->camera()->convertClickToLine(pixel, orig, dir);

			qglviewer::Vec orig_inv = mh->getFrame()->coordinatesOf(orig);
			qglviewer::Vec dir_inv = mh->getFrame()->transformOf(dir);

			PFP2::VEC3 rayA(orig_inv.x, orig_inv.y, orig_inv.z);
			PFP2::VEC3 AB(dir_inv.x, dir_inv.y, dir_inv.z);

			Dart d;
			PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(mh)->getMap();
			Algo::Selection::vertexRaySelection<PFP2>(*map, p.positionAttribute, rayA, AB, d);
			if(d != NIL)
			{
				m_selectionCenter = p.positionAttribute[d];
				view->updateGL();
			}
		}
	}
}

void Surface_Selection_Plugin::wheelEvent(View* view, QWheelEvent* event)
{
	if(m_selecting)
	{
		if(event->delta() > 0)
			m_selectionRadius *= 0.9f;
		else
			m_selectionRadius *= 1.1f;
		view->updateGL();
	}
}

void Surface_Selection_Plugin::viewLinked(View *view)
{
	MapHandlerGen* map = m_schnapps->getSelectedMap();
	if(map)
		m_selectionRadius = map->getBBdiagSize() / 50.0f;
}





void Surface_Selection_Plugin::selectedViewChanged(View *prev, View *cur)
{
	m_dockTab->updateMapParameters();
}

void Surface_Selection_Plugin::selectedMapChanged(MapHandlerGen *prev, MapHandlerGen *cur)
{
	m_dockTab->updateMapParameters();
	if(cur)
		m_selectionRadius = cur->getBBdiagSize() / 50.0f;
}

void Surface_Selection_Plugin::mapAdded(MapHandlerGen* map)
{
	connect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
}

void Surface_Selection_Plugin::mapRemoved(MapHandlerGen* map)
{
	disconnect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
}





void Surface_Selection_Plugin::attributeAdded(unsigned int orbit, const QString& nameAttr)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());
	if(map == m_schnapps->getSelectedMap())
		m_dockTab->addAttributeToList(orbit, nameAttr);
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(Surface_Selection_Plugin, Surface_Selection_Plugin)
#else
Q_EXPORT_PLUGIN2(Surface_Selection_PluginD, Surface_Selection_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
