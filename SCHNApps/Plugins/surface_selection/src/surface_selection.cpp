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

	m_pointSprite = new CGoGN::Utils::PointSprite();
	m_drawer = new Utils::Drawer();

	m_selectedVerticesVBO = new Utils::VBO();

	m_selectionSphereVBO = new Utils::VBO();

	registerShader(m_drawer->getShader());
	registerShader(m_pointSprite);

	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(selectedCellSelectorChanged(CellSelectorGen*)), this, SLOT(updateSelectedCellsRendering()));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapAdded(map);

	m_dockTab->updateMapParameters();

	return true;
}

void Surface_Selection_Plugin::disable()
{
	delete m_pointSprite;
	delete m_selectedVerticesVBO;
	delete m_selectionSphereVBO;

	disconnect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(selectedCellSelectorChanged(CellSelectorGen*)), this, SLOT(updateSelectedCellsRendering()));
}

void Surface_Selection_Plugin::draw(View *view)
{
}

void Surface_Selection_Plugin::drawMap(View* view, MapHandlerGen* map)
{
	if(map->isSelectedMap())
	{
		const MapParameters& p = h_parameterSet[map];
		if(p.positionAttribute.isValid())
		{
			unsigned int orbit = m_schnapps->getCurrentOrbit();
			CellSelectorGen* selector = m_schnapps->getSelectedSelector(orbit);
			if(selector)
			{
				const std::vector<Dart>& selectedCells = selector->getSelectedCells();
				unsigned int nbCells = map->getGenericMap()->getNbCells(orbit);
				switch(orbit)
				{
					case VERTEX : {
						m_pointSprite->setAttributePosition(m_selectedVerticesVBO);
						m_pointSprite->setSize(20 * map->getBBdiagSize() / nbCells);
						m_pointSprite->setColor(CGoGN::Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
						m_pointSprite->setLightPosition(CGoGN::Geom::Vec3f(0.0f, 0.0f, 1.0f));

						m_pointSprite->enableVertexAttribs();
						glDrawArrays(GL_POINTS, 0, selectedCells.size());
						m_pointSprite->disableVertexAttribs();

						if(m_selecting && m_selectingVertex != NIL)
						{
							std::vector<PFP2::VEC3> selectionPoint;
							selectionPoint.push_back(p.positionAttribute[m_selectingVertex]);
							m_selectionSphereVBO->updateData(selectionPoint);

							m_pointSprite->setAttributePosition(m_selectionSphereVBO);
							m_pointSprite->setColor(CGoGN::Geom::Vec4f(0.0f, 0.0f, 1.0f, 0.5f));
							m_pointSprite->setLightPosition(CGoGN::Geom::Vec3f(0.0f, 0.0f, 1.0f));

							switch(p.selectionMethod)
							{
								case SingleCell : {
									m_pointSprite->setSize(30 * map->getBBdiagSize() / nbCells);
									break;
								}
								case WithinSphere : {
									m_pointSprite->setSize(m_selectionRadius);
									break;
								}
							}

							m_pointSprite->enableVertexAttribs();
							glEnable(GL_BLEND);
							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
							glDrawArrays(GL_POINTS, 0, 1);
							glDisable(GL_BLEND);
							m_pointSprite->disableVertexAttribs();
						}
						break;
					}
					case EDGE : {
						PFP2::MAP* m = static_cast<MapHandler<PFP2>*>(map)->getMap();

						m_drawer->newList(GL_COMPILE_AND_EXECUTE);
						m_drawer->lineWidth(3.0f);
						m_drawer->color3f(1.0f, 0.0f, 0.0f);
						m_drawer->begin(GL_LINES);
						for(std::vector<Dart>::const_iterator it = selectedCells.begin(); it != selectedCells.end(); ++it)
						{
							m_drawer->vertex(p.positionAttribute[*it]);
							m_drawer->vertex(p.positionAttribute[m->phi1(*it)]);
						}
						m_drawer->end();
						m_drawer->endList();

						if(m_selecting && m_selectingEdge != NIL)
						{
							switch(p.selectionMethod)
							{
								case SingleCell : {
									m_drawer->newList(GL_COMPILE_AND_EXECUTE);
									m_drawer->lineWidth(6.0f);
									m_drawer->color3f(0.0f, 0.0f, 1.0f);
									m_drawer->begin(GL_LINES);
									m_drawer->vertex(p.positionAttribute[m_selectingEdge]);
									m_drawer->vertex(p.positionAttribute[m->phi1(m_selectingEdge)]);
									m_drawer->end();
									m_drawer->endList();
									break;
								}
								case WithinSphere : {
									break;
								}
							}
						}
						break;
					}
					case FACE : {
						PFP2::MAP* m = static_cast<MapHandler<PFP2>*>(map)->getMap();

						m_drawer->newList(GL_COMPILE_AND_EXECUTE);
						m_drawer->color3f(1.0f, 0.0f, 0.0f);
						m_drawer->begin(GL_TRIANGLES);
						for(std::vector<Dart>::const_iterator it = selectedCells.begin(); it != selectedCells.end(); ++it)
						{
							m_drawer->vertex(p.positionAttribute[*it]);
							m_drawer->vertex(p.positionAttribute[m->phi1(*it)]);
							m_drawer->vertex(p.positionAttribute[m->phi_1(*it)]);
						}
						m_drawer->end();
						m_drawer->endList();

						if(m_selecting && m_selectingFace != NIL)
						{
							switch(p.selectionMethod)
							{
								case SingleCell : {
									m_drawer->newList(GL_COMPILE_AND_EXECUTE);
									m_drawer->color3f(0.0f, 0.0f, 1.0f);
									m_drawer->begin(GL_TRIANGLES);
									m_drawer->vertex(p.positionAttribute[m_selectingFace]);
									m_drawer->vertex(p.positionAttribute[m->phi1(m_selectingFace)]);
									m_drawer->vertex(p.positionAttribute[m->phi_1(m_selectingFace)]);
									m_drawer->end();
									m_drawer->endList();
									break;
								}
								case WithinSphere : {
									break;
								}
							}
						}
						break;
					}
				}
			}
		}
	}
}

void Surface_Selection_Plugin::keyPress(View* view, QKeyEvent* event)
{
	if(event->key() == Qt::Key_Shift)
	{
		view->setMouseTracking(true);
		m_selecting = true;
		view->updateGL();
	}
}

void Surface_Selection_Plugin::keyRelease(View* view, QKeyEvent* event)
{
	if(event->key() == Qt::Key_Shift)
	{
		view->setMouseTracking(false);
		m_selecting = false;
		view->updateGL();
	}
}

void Surface_Selection_Plugin::mousePress(View* view, QMouseEvent* event)
{
	if(m_selecting && (event->button() == Qt::LeftButton || event->button() == Qt::RightButton))
	{
		MapHandlerGen* mh = m_schnapps->getSelectedMap();
		const MapParameters& p = h_parameterSet[mh];
		if(p.positionAttribute.isValid())
		{
			unsigned int orbit = m_schnapps->getCurrentOrbit();
			CellSelectorGen* selector = m_schnapps->getSelectedSelector(orbit);
			if(selector)
			{
				PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(mh)->getMap();

				switch(orbit)
				{
					case VERTEX : {
						if(m_selectingVertex != NIL)
						{
							switch(p.selectionMethod)
							{
								case SingleCell : {
									if(event->button() == Qt::LeftButton)
										selector->select(m_selectingVertex);
									else if(event->button() == Qt::RightButton)
										selector->unselect(m_selectingVertex);
									break;
								}
								case WithinSphere : {
									Algo::Surface::Selection::Collector_WithinSphere<PFP2> neigh(*map, p.positionAttribute, m_selectionRadius);
									neigh.collectAll(m_selectingVertex);
									if(event->button() == Qt::LeftButton)
										selector->select(neigh.getInsideVertices());
									else if(event->button() == Qt::RightButton)
										selector->unselect(neigh.getInsideVertices());
									break;
								}
							}
							updateSelectedCellsRendering();
						}
						break;
					}
					case EDGE : {
						if(m_selectingEdge != NIL)
						{
							switch(p.selectionMethod)
							{
								case SingleCell : {
									if(event->button() == Qt::LeftButton)
										selector->select(m_selectingEdge);
									else if(event->button() == Qt::RightButton)
										selector->unselect(m_selectingEdge);
									break;
								}
								case WithinSphere : {
									break;
								}
							}
						}
						break;
					}
					case FACE : {
						if(m_selectingFace != NIL)
						{
							switch(p.selectionMethod)
							{
								case SingleCell : {
									if(event->button() == Qt::LeftButton)
										selector->select(m_selectingFace);
									else if(event->button() == Qt::RightButton)
										selector->unselect(m_selectingFace);
									break;
								}
								case WithinSphere : {
									break;
								}
							}
						}
						break;
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
		const MapParameters& p = h_parameterSet[mh];
		if(p.positionAttribute.isValid())
		{
			unsigned int orbit = m_schnapps->getCurrentOrbit();
			CellSelectorGen* selector = m_schnapps->getSelectedSelector(orbit);
			if(selector)
			{
				QPoint pixel(event->x(), event->y());
				qglviewer::Vec orig;
				qglviewer::Vec dir;
				view->camera()->convertClickToLine(pixel, orig, dir);

				qglviewer::Vec orig_inv = mh->getFrame()->coordinatesOf(orig);
				qglviewer::Vec dir_inv = mh->getFrame()->transformOf(dir);

				PFP2::VEC3 rayA(orig_inv.x, orig_inv.y, orig_inv.z);
				PFP2::VEC3 AB(dir_inv.x, dir_inv.y, dir_inv.z);

				PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(mh)->getMap();

				switch(orbit)
				{
					case VERTEX : {
						Algo::Selection::vertexRaySelection<PFP2>(*map, p.positionAttribute, rayA, AB, m_selectingVertex);
						break;
					}
					case EDGE : {
						Algo::Selection::edgeRaySelection<PFP2>(*map, p.positionAttribute, rayA, AB, m_selectingEdge);
						break;
					}
					case FACE : {
						Algo::Selection::faceRaySelection<PFP2>(*map, p.positionAttribute, rayA, AB, m_selectingFace);
						break;
					}
				}

				view->updateGL();
			}
		}
	}
}

void Surface_Selection_Plugin::wheelEvent(View* view, QWheelEvent* event)
{
	if(m_selecting)
	{
		MapHandlerGen* mh = m_schnapps->getSelectedMap();
		const MapParameters& p = h_parameterSet[mh];

		if(p.selectionMethod == WithinSphere)
		{
			if(event->delta() > 0)
				m_selectionRadius *= 0.9f;
			else
				m_selectionRadius *= 1.1f;
			view->updateGL();
		}
	}
}

void Surface_Selection_Plugin::viewLinked(View *view)
{
	MapHandlerGen* map = m_schnapps->getSelectedMap();
	if(map)
		m_selectionRadius = map->getBBdiagSize() / 50.0f;
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

void Surface_Selection_Plugin::updateSelectedCellsRendering()
{
	MapHandlerGen* mh = m_schnapps->getSelectedMap();
	const MapParameters& p = h_parameterSet[mh];
	if(p.positionAttribute.isValid())
	{
		unsigned int orbit = m_schnapps->getCurrentOrbit();
		CellSelectorGen* selector = m_schnapps->getSelectedSelector(orbit);
		const std::vector<Dart>& selectedCells = selector->getSelectedCells();
		switch(orbit)
		{
			case VERTEX : {
				std::vector<PFP2::VEC3> selectedPoints;
				for(std::vector<Dart>::const_iterator it = selectedCells.begin(); it != selectedCells.end(); ++it)
					selectedPoints.push_back(p.positionAttribute[*it]);
				m_selectedVerticesVBO->updateData(selectedPoints);
				break;
			}
			case EDGE : {
				break;
			}
			case FACE : {
				break;
			}
		}
	}

	foreach(View* view, l_views)
	{
		if(view->isLinkedToMap(mh))
			view->updateGL();
	}
}





void Surface_Selection_Plugin::attributeAdded(unsigned int orbit, const QString& name)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());
	if(orbit == VERTEX && map->isSelectedMap())
		m_dockTab->addVertexAttribute(name);
}





void Surface_Selection_Plugin::changePositionAttribute(const QString& view, const QString& map, const QString& name)
{
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_parameterSet[m].positionAttribute = m->getAttribute<PFP2::VEC3, VERTEX>(name);
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}

void Surface_Selection_Plugin::changeSelectionMethod(const QString& view, const QString& map, unsigned int method)
{
	View* v = m_schnapps->getView(view);
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(v && m)
	{
		h_parameterSet[m].selectionMethod = SelectionMethod(method);
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(Surface_Selection_Plugin, Surface_Selection_Plugin)
#else
Q_EXPORT_PLUGIN2(Surface_Selection_PluginD, Surface_Selection_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
