#include "surfaceDeformation.h"

#include "Algo/Selection/raySelector.h"
#include "Algo/Selection/collector.h"


PerMapParameterSet::PerMapParameterSet(MapHandlerGen* mh) :
	setLockedVertices(true),
	selecting(false),
	selectionRadius(0.1f),
	dragging(false)
{
	GenericMap* map = mh->getGenericMap();
	AttributeContainer& cont = map->getAttributeContainer<VERTEX>();

	std::vector<std::string> names;
	std::vector<std::string> types;
	cont.getAttributesNames(names);
	cont.getAttributesTypes(types);
	std::string vec3TypeName = nameOfType(PFP2::VEC3());
	for(unsigned int i = 0; i < names.size(); ++i)
	{
		if(types[i] == vec3TypeName)
		{
			if(names[i] == "position") // try to select a position attribute named "position"
				positionAttribute = mh->getAttribute<PFP2::VEC3>(names[i]);
		}
	}

	if(!positionAttribute.isValid())
	{
		for(unsigned int i = 0; i < names.size(); ++i)
		{
			if(types[i] == vec3TypeName)
			{
				positionAttribute = mh->getAttribute<PFP2::VEC3>(names[i]);
				break;
			}
		}
	}
}


bool SurfaceDeformationPlugin::enable()
{
	m_dockTab = new SurfaceDeformationDockTab(this);
	addTabInDock(m_dockTab, "Surface Deformation");

	connect(m_dockTab->mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));
	connect(m_dockTab->combo_positionAttribute, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_positionAttributeChanged(int)));

	connect(m_window, SIGNAL(viewAndPluginLinked(View*, Plugin*)), this, SLOT(viewLinked(View*, Plugin*)));
	connect(m_window, SIGNAL(viewAndPluginUnlinked(View*, Plugin*)), this, SLOT(viewUnlinked(View*, Plugin*)));
	connect(m_window, SIGNAL(currentViewChanged(View*)), this, SLOT(currentViewChanged(View*)));

	return true;
}

void SurfaceDeformationPlugin::keyPress(View* view, QKeyEvent* event)
{
	if(event->key() == Qt::Key_Shift)
	{
		view->setMouseTracking(true);
		ParameterSet* params = h_viewParams[view];
		PerMapParameterSet& perMap = params->getCurrentMapParameterSet();
		perMap.selecting = true;
		view->updateGL();
	}
}

void SurfaceDeformationPlugin::keyRelease(View* view, QKeyEvent* event)
{
	if(event->key() == Qt::Key_Shift)
	{
		view->setMouseTracking(false);
		ParameterSet* params = h_viewParams[view];
		PerMapParameterSet& perMap = params->getCurrentMapParameterSet();
		perMap.selecting = false;
		view->updateGL();
	}
}

void SurfaceDeformationPlugin::mousePress(View* view, QMouseEvent* event)
{
	if(event->button() == Qt::RightButton && event->modifiers() & Qt::ShiftModifier)
	{
		view->setMouseTracking(false) ;

		ParameterSet* params = h_viewParams[view];
		PerMapParameterSet& perMap = params->getCurrentMapParameterSet();

		perMap.selecting = false ;
		perMap.dragging = true ;

		perMap.dragZ = 0;
		for(unsigned int i = 0; i < perMap.handle_vertices.size(); ++i)
		{
			PFP::VEC3& p = perMap.positionAttribute[handle_vertices[i]] ;
			qglviewer::Vec q = view->projectedCoordinatesOf(qglviewer::Vec(p[0],p[1],p[2]));
			dragZ += q.z ;
		}
		dragZ /= handle_vertices.size() ;

		qglviewer::Vec p(event->x(), event->y(), dragZ);
		perMap.dragPrevious = view->unprojectedCoordinatesOf(p);
	}
	if(button == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier)
	{
		ParameterSet* params = h_viewParams[view];
		PerMapParameterSet& perMap = params->getCurrentMapParameterSet();

		QPoint pixel(event->x(), event->y());
		qglviewer::Vec orig;
		qglviewer::Vec dir;
		view->convertClickToLine(pixel, orig, dir);

		PFP::VEC3 rayA(orig.x, orig.y, orig.z);
		PFP::VEC3 AB(dir.x, dir.y, dir.z);

		Dart d ;
		PFP2::MAP map = static_cast<MapHandler<PFP2>*>(params->selectedMap)->getMap();
		Algo::Selection::vertexRaySelection<PFP>(*map, position, rayA, AB, d) ;

		if(d != NIL)
		{
			Algo::Surface::Selection::Collector_WithinSphere<PFP> neigh(*map, perMap.positionAttribute, perMap.selectionRadius) ;
			neigh.collectAll(d) ;
			const std::vector<Dart>& insideV = neigh.getInsideVertices() ;

			if(perMap.setLockedVertices)
			{
				for(unsigned int i = 0; i < insideV.size(); ++i)
				{
					unsigned int v = map->getEmbedding<VERTEX>(insideV[i]) ;
					if (!lockingMarker.isMarked(v))
					{
						locked_vertices.push_back(v) ;
						lockingMarker.mark(v);
					}
				}
				LinearSolving::resetSolver(solver, false) ;
			}
			else
			{
				for(unsigned int i = 0; i < insideV.size(); ++i)
				{
					unsigned int v = myMap.getEmbedding<VERTEX>(insideV[i]) ;
					if(!handleMarker.isMarked(v))
					{
						handle_vertices.push_back(v) ;
						handleMarker.mark(v);
					}
					if(!lockingMarker.isMarked(v))
					{
						locked_vertices.push_back(v) ;
						lockingMarker.mark(v) ;
					}
				}
				LinearSolving::resetSolver(solver, false) ;
			}
			view->updateGL() ;
		}
	}
}

void SurfaceDeformationPlugin::mouseRelease(View* view, QMouseEvent* event)
{
	if(event->button() == Qt::RightButton)
	{
		ParameterSet* params = h_viewParams[view];
		PerMapParameterSet& perMap = params->getCurrentMapParameterSet();
		perMap.dragging = false ;
	}
}

void SurfaceDeformationPlugin::mouseMove(View* view, QMouseEvent* event)
{
	ParameterSet* params = h_viewParams[view];
	PerMapParameterSet& perMap = params->getCurrentMapParameterSet();
	if(perMap.dragging)
	{
		qglviewer::Vec p(event->x(), event->y(), perMap.dragZ);
		qglviewer::Vec q = view->unprojectedCoordinatesOf(p);

		qglviewer::Vec vec = q - perMap.dragPrevious;
		PFP::VEC3 t(vec.x, vec.y, vec.z);
		for(unsigned int i = 0; i < handle_vertices.size(); ++i)
			perMap.positionAttribute[handle_vertices[i]] += t ;

		perMap.dragPrevious = q ;

//		matchDiffCoord() ;
//		for(unsigned int i = 0; i < 2; ++i)
//			asRigidAsPossible();

		m_positionVBO->updateData(position);

		view->updateGL();
	}
	else if(selecting)
	{
		QPoint pixel(event->x(), event->y());
		qglviewer::Vec orig;
		qglviewer::Vec dir;
		view->convertClickToLine(pixel, orig, dir);

		PFP::VEC3 rayA(orig.x, orig.y, orig.z);
		PFP::VEC3 AB(dir.x, dir.y, dir.z);

		Dart d ;
		PFP2::MAP map = static_cast<MapHandler<PFP2>*>(params->selectedMap)->getMap();
		Algo::Selection::vertexRaySelection<PFP2>(*map, position, rayA, AB, d) ;
		if(d != NIL)
		{
			perMap.selectionCenter = perMap.positionAttribute[d] ;
			view->updateGL() ;
		}
	}
}

void SurfaceDeformationPlugin::viewLinked(View* view, Plugin* plugin)
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

		if(view->isCurrentView())
			m_dockTab->refreshUI(params);
	}
}

void SurfaceDeformationPlugin::viewUnlinked(View* view, Plugin* plugin)
{
	if(plugin == this)
	{
		h_viewParams.remove(view);

		disconnect(view, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(mapLinked(MapHandlerGen*)));
		disconnect(view, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(mapUnlinked(MapHandlerGen*)));
	}
}

void SurfaceDeformationPlugin::currentViewChanged(View* view)
{
	if(isLinkedToView(view))
		m_dockTab->refreshUI(h_viewParams[view]);
}

void SurfaceDeformationPlugin::mapLinked(MapHandlerGen* m)
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

void SurfaceDeformationPlugin::mapUnlinked(MapHandlerGen* m)
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

void SurfaceDeformationPlugin::changeSelectedMap(View* view, MapHandlerGen* map)
{
	ParameterSet* params = h_viewParams[view];

	MapHandlerGen* prev = params->selectedMap;
	params->selectedMap = map;

	if(view->isCurrentView())
	{
		if(prev)
			disconnect(map, SIGNAL(attributeAdded()), this, SLOT(attributeAdded()));
		if(map)
			connect(map, SIGNAL(attributeAdded()), this, SLOT(attributeAdded()));

		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void SurfaceDeformationPlugin::changePositionAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP2::VEC3> attribute)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()].positionAttribute = attribute;

	if(view->isCurrentView())
	{
		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void SurfaceDeformationPlugin::cb_selectedMapChanged()
{
	if(!b_refreshingUI)
	{
		QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
		if(!currentItems.empty())
			changeSelectedMap(m_window->getCurrentView(), m_window->getMap(currentItems[0]->text()));
	}
}

void SurfaceDeformationPlugin::cb_positionAttributeChanged(int index)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changePositionAttribute(view, map, map->getAttribute<PFP2::VEC3>(m_dockTab->combo_positionVBO->currentText()));
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(SurfaceDeformationPlugin, SurfaceDeformationPlugin)
#else
Q_EXPORT_PLUGIN2(SurfaceDeformationPluginD, SurfaceDeformationPlugin)
#endif
