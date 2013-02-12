#include "surfaceDeformation.h"

#include "Algo/Selection/raySelector.h"
#include "Algo/Selection/collector.h"

#include "Algo/Geometry/normal.h"
#include "Algo/Geometry/laplacian.h"

#include "Utils/drawer.h"

#include <QKeyEvent>
#include <QMouseEvent>


PerMapParameterSet::PerMapParameterSet(MapHandlerGen* mh) :
	verticesSelectionMode(LOCKED)
{
	PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(mh)->getMap();

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
				positionAttribute = mh->getAttribute<PFP2::VEC3, VERTEX>(QString::fromStdString(names[i]));
		}
	}

	if(!positionAttribute.isValid())
	{
		for(unsigned int i = 0; i < names.size(); ++i)
		{
			if(types[i] == vec3TypeName)
			{
				positionAttribute = mh->getAttribute<PFP2::VEC3, VERTEX>(QString::fromStdString(names[i]));
				break;
			}
		}
	}

	lockingMarker = new CellMarker<VERTEX>(*map);
	handleMarker = new CellMarker<VERTEX>(*map);

	positionInit = mh->getAttribute<PFP2::VEC3, VERTEX>("positionInit") ;
	if(!positionInit.isValid())
		positionInit = mh->addAttribute<PFP2::VEC3, VERTEX>("positionInit") ;
	map->copyAttribute(positionInit, positionAttribute) ;

	vertexNormal = mh->getAttribute<PFP2::VEC3, VERTEX>("vertexNormal");
	if(!vertexNormal.isValid())
		vertexNormal = mh->addAttribute<PFP2::VEC3, VERTEX>("vertexNormal");

	edgeAngle = mh->getAttribute<PFP2::REAL, EDGE>("edgeAngle");
	if(!edgeAngle.isValid())
		edgeAngle = mh->addAttribute<PFP2::REAL, EDGE>("edgeAngle");

	edgeWeight = mh->getAttribute<PFP2::REAL, EDGE>("edgeWeight");
	if(!edgeWeight.isValid())
		edgeWeight = mh->addAttribute<PFP2::REAL, EDGE>("edgeWeight");

	vertexArea = mh->getAttribute<PFP2::REAL, VERTEX>("vertexArea");
	if(!vertexArea.isValid())
		vertexArea = mh->addAttribute<PFP2::REAL, VERTEX>("vertexArea");

	diffCoord = mh->getAttribute<PFP2::VEC3, VERTEX>("diffCoord");
	if(!diffCoord.isValid())
		diffCoord = mh->addAttribute<PFP2::VEC3, VERTEX>("diffCoord");

	vertexRotationMatrix = mh->getAttribute<Eigen_Matrix3f, VERTEX>("vertexRotationMatrix") ;
	if(!vertexRotationMatrix.isValid())
		vertexRotationMatrix = mh->addAttribute<Eigen_Matrix3f, VERTEX>("vertexRotationMatrix");

	rotatedDiffCoord = mh->getAttribute<PFP2::VEC3, VERTEX>("rotatedDiffCoord") ;
	if(!rotatedDiffCoord.isValid())
		rotatedDiffCoord = mh->addAttribute<PFP2::VEC3, VERTEX>("rotatedDiffCoord");

	Algo::Surface::Geometry::computeNormalVertices<PFP2>(*map, positionAttribute, vertexNormal) ;
	Algo::Surface::Geometry::computeAnglesBetweenNormalsOnEdges<PFP2>(*map, positionAttribute, edgeAngle) ;
	Algo::Surface::Geometry::computeCotanWeightEdges<PFP2>(*map, positionAttribute, edgeWeight) ;
	Algo::Surface::Geometry::computeVoronoiAreaVertices<PFP2>(*map, positionAttribute, vertexArea) ;
//	Algo::Surface::Geometry::computeLaplacianCotanVertices<PFP2>(*map, edgeWeight, vertexArea, positionAttribute, diffCoord) ;
	Algo::Surface::Geometry::computeLaplacianTopoVertices<PFP2>(*map, positionAttribute, diffCoord) ;

	for(unsigned int i = vertexRotationMatrix.begin(); i != vertexRotationMatrix.end() ; vertexRotationMatrix.next(i))
		vertexRotationMatrix[i] = Eigen::Matrix3f::Identity();

	vIndex = mh->getAttribute<unsigned int, VERTEX>("vIndex");
	if(!vIndex.isValid())
		vIndex = mh->addAttribute<unsigned int, VERTEX>("vIndex");
	nb_vertices = static_cast<AttribMap*>(map)->computeIndexCells<VERTEX>(vIndex);

	nlContext = nlNewContext();
	nlMakeCurrent(nlContext) ;
	nlSolverParameteri(NL_NB_VARIABLES, nb_vertices) ;
	nlSolverParameteri(NL_LEAST_SQUARES, NL_TRUE) ;
	nlSolverParameteri(NL_SOLVER, NL_CHOLMOD_EXT) ;
}

PerMapParameterSet::~PerMapParameterSet()
{
	delete lockingMarker;
	delete handleMarker;
	nlDeleteContext(nlContext);
}


bool SurfaceDeformationPlugin::enable()
{
	m_dockTab = new SurfaceDeformationDockTab(this);
	addTabInDock(m_dockTab, "Surface Deformation");

	connect(m_dockTab->mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));
	connect(m_dockTab->combo_positionAttribute, SIGNAL(currentIndexChanged(int)), this, SLOT(cb_positionAttributeChanged(int)));
	connect(m_dockTab->radio_locked, SIGNAL(toggled(bool)), this, SLOT(cb_selectLockedVertices(bool)));
	connect(m_dockTab->radio_handle, SIGNAL(toggled(bool)), this, SLOT(cb_selectHandleVertices(bool)));

	connect(m_window, SIGNAL(viewAndPluginLinked(View*, Plugin*)), this, SLOT(viewLinked(View*, Plugin*)));
	connect(m_window, SIGNAL(viewAndPluginUnlinked(View*, Plugin*)), this, SLOT(viewUnlinked(View*, Plugin*)));
	connect(m_window, SIGNAL(currentViewChanged(View*)), this, SLOT(currentViewChanged(View*)));

	m_drawer = new Utils::Drawer();
	registerShader(m_drawer->getShader());

	return true;
}

void SurfaceDeformationPlugin::disable()
{
	delete m_drawer;
}

void SurfaceDeformationPlugin::redraw(View* view)
{
	if(selecting)
	{
		glDisable(GL_LIGHTING) ;
		m_drawer->newList(GL_COMPILE_AND_EXECUTE) ;
		m_drawer->lineWidth(2.0f) ;
		m_drawer->begin(GL_LINES) ;
		m_drawer->color3f(0.0f, 0.0f, 1.0f) ;
		m_drawer->vertex(selectionCenter) ;
		m_drawer->vertex(selectionCenter + selectionRadius * PFP2::VEC3(1,0,0)) ;
		m_drawer->vertex(selectionCenter) ;
		m_drawer->vertex(selectionCenter + selectionRadius * PFP2::VEC3(-1,0,0)) ;
		m_drawer->vertex(selectionCenter) ;
		m_drawer->vertex(selectionCenter + selectionRadius * PFP2::VEC3(0,1,0)) ;
		m_drawer->vertex(selectionCenter) ;
		m_drawer->vertex(selectionCenter + selectionRadius * PFP2::VEC3(0,-1,0)) ;
		m_drawer->vertex(selectionCenter) ;
		m_drawer->vertex(selectionCenter + selectionRadius * PFP2::VEC3(0,0,1)) ;
		m_drawer->vertex(selectionCenter) ;
		m_drawer->vertex(selectionCenter + selectionRadius * PFP2::VEC3(0,0,-1)) ;
		m_drawer->end() ;
		m_drawer->endList() ;
	}

	ParameterSet* params = h_viewParams[view];
	MapHandlerGen* mh = params->selectedMap;
	if(mh)
	{
		PerMapParameterSet* perMap = params->perMap[mh->getName()];

		if(!perMap->locked_vertices.empty() || !perMap->handle_vertices.empty())
		{
			glDisable(GL_LIGHTING) ;
			m_drawer->newList(GL_COMPILE_AND_EXECUTE) ;
			m_drawer->pointSize(4.0f) ;
			m_drawer->begin(GL_POINTS) ;
			m_drawer->color3f(1.0f, 0.0f, 0.0f) ;
			for(unsigned int i = 0; i < perMap->locked_vertices.size(); ++i)
			{
				if (!perMap->handleMarker->isMarked(perMap->locked_vertices[i]))
					m_drawer->vertex(perMap->positionAttribute[perMap->locked_vertices[i]]) ;
			}
			m_drawer->color3f(0.0f, 0.0f, 1.0f) ;
			for(unsigned int i = 0; i < perMap->handle_vertices.size(); ++i)
				m_drawer->vertex(perMap->positionAttribute[perMap->handle_vertices[i]]) ;
			m_drawer->end() ;
			m_drawer->endList() ;
		}
	}
}

void SurfaceDeformationPlugin::keyPress(View* view, QKeyEvent* event)
{
	if(event->key() == Qt::Key_Shift)
	{
		view->setMouseTracking(true);
		selecting = true;
		view->updateGL();
	}
	else if(event->key() == Qt::Key_M)
	{
		ParameterSet* params = h_viewParams[view];
		MapHandlerGen* map = params->selectedMap;
		matchDiffCoord(view, map);
	}
}

void SurfaceDeformationPlugin::keyRelease(View* view, QKeyEvent* event)
{
	if(event->key() == Qt::Key_Shift)
	{
		view->setMouseTracking(false);
		selecting = false;
		view->updateGL();
	}
}

void SurfaceDeformationPlugin::mousePress(View* view, QMouseEvent* event)
{
	if(event->button() == Qt::LeftButton && selecting)
	{
		ParameterSet* params = h_viewParams[view];
		PerMapParameterSet* perMap = params->getCurrentMapParameterSet();

		QPoint pixel(event->x(), event->y());
		qglviewer::Vec orig;
		qglviewer::Vec dir;
		view->camera()->convertClickToLine(pixel, orig, dir);

		PFP2::VEC3 rayA(orig.x, orig.y, orig.z);
		PFP2::VEC3 AB(dir.x, dir.y, dir.z);

		Dart d ;
		PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(params->selectedMap)->getMap();
		Algo::Selection::vertexRaySelection<PFP2>(*map, perMap->positionAttribute, rayA, AB, d) ;

		if(d != NIL)
		{
			Algo::Surface::Selection::Collector_WithinSphere<PFP2> neigh(*map, perMap->positionAttribute, selectionRadius) ;
			neigh.collectAll(d) ;
			const std::vector<Dart>& insideV = neigh.getInsideVertices() ;

			for(unsigned int i = 0; i < insideV.size(); ++i)
			{
				unsigned int v = map->getEmbedding<VERTEX>(insideV[i]) ;
				if (!perMap->lockingMarker->isMarked(v))
				{
					perMap->locked_vertices.push_back(v) ;
					perMap->lockingMarker->mark(v);
				}
				if(perMap->verticesSelectionMode == HANDLE)
				{
					if(!perMap->handleMarker->isMarked(v))
					{
						perMap->handle_vertices.push_back(v) ;
						perMap->handleMarker->mark(v);
					}
				}
			}

			nlMakeCurrent(perMap->nlContext) ;
			nlReset(NL_FALSE) ;

			view->updateGL() ;
		}
	}
	else if(event->button() == Qt::RightButton && event->modifiers() & Qt::ShiftModifier)
	{
		view->setMouseTracking(false) ;

		ParameterSet* params = h_viewParams[view];
		PerMapParameterSet* perMap = params->getCurrentMapParameterSet();

		selecting = false ;
		dragging = true ;

		dragZ = 0;
		for(unsigned int i = 0; i < perMap->handle_vertices.size(); ++i)
		{
			const PFP2::VEC3& p = perMap->positionAttribute[perMap->handle_vertices[i]] ;
			qglviewer::Vec q = view->camera()->projectedCoordinatesOf(qglviewer::Vec(p[0],p[1],p[2]));
			dragZ += q.z ;
		}
		dragZ /= perMap->handle_vertices.size() ;

		qglviewer::Vec p(event->x(), event->y(), dragZ);
		dragPrevious = view->camera()->unprojectedCoordinatesOf(p);
	}
}

void SurfaceDeformationPlugin::mouseRelease(View* view, QMouseEvent* event)
{
	if(event->button() == Qt::RightButton)
	{
		dragging = false ;
	}
}

void SurfaceDeformationPlugin::mouseMove(View* view, QMouseEvent* event)
{
	ParameterSet* params = h_viewParams[view];
	MapHandlerGen* map = params->selectedMap;
	PerMapParameterSet* perMap = params->perMap[map->getName()];

	if(dragging)
	{
		qglviewer::Vec p(event->x(), event->y(), dragZ);
		qglviewer::Vec q = view->camera()->unprojectedCoordinatesOf(p);

		qglviewer::Vec vec = q - dragPrevious;
		PFP2::VEC3 t(vec.x, vec.y, vec.z);
		for(unsigned int i = 0; i < perMap->handle_vertices.size(); ++i)
			perMap->positionAttribute[perMap->handle_vertices[i]] += t ;

		dragPrevious = q ;

//		matchDiffCoord(view, map);
		for(unsigned int i = 0; i < 2; ++i)
			asRigidAsPossible(view, map);

		params->selectedMap->updateVBO(perMap->positionAttribute);

		view->updateGL();
	}
	else if(selecting)
	{
		QPoint pixel(event->x(), event->y());
		qglviewer::Vec orig;
		qglviewer::Vec dir;
		view->camera()->convertClickToLine(pixel, orig, dir);

		PFP2::VEC3 rayA(orig.x, orig.y, orig.z);
		PFP2::VEC3 AB(dir.x, dir.y, dir.z);

		Dart d ;
		PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(params->selectedMap)->getMap();
		Algo::Selection::vertexRaySelection<PFP2>(*map, perMap->positionAttribute, rayA, AB, d) ;
		if(d != NIL)
		{
			selectionCenter = perMap->positionAttribute[d] ;
			view->updateGL() ;
		}
	}
}

void SurfaceDeformationPlugin::wheelEvent(View* view, QWheelEvent* event)
{
	if(selecting)
	{
		if(event->delta() > 0)
			selectionRadius *= 0.9f ;
		else
			selectionRadius *= 1.1f ;
		view->updateGL() ;
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
			PerMapParameterSet* p = new PerMapParameterSet(map);
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
		ParameterSet* params = h_viewParams[view];
		QHash<QString, PerMapParameterSet*>::const_iterator i = params->perMap.constBegin();
		while (i != params->perMap.constEnd())
		{
			delete i.value();
			++i;
		}
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
	PerMapParameterSet* p = new PerMapParameterSet(m);
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
	delete params->perMap[m->getName()];
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

void SurfaceDeformationPlugin::attributeAdded()
{
	ParameterSet* params = h_viewParams[m_window->getCurrentView()];
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
			disconnect(prev, SIGNAL(attributeAdded()), this, SLOT(attributeAdded()));
		if(map)
		{
			connect(map, SIGNAL(attributeAdded()), this, SLOT(attributeAdded()));
			selectionRadius = map->getBBdiagSize() / 50.0;
		}

		m_dockTab->refreshUI(params);
		view->updateGL();
	}
}

void SurfaceDeformationPlugin::changePositionAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP2::VEC3> attribute)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->positionAttribute = attribute;

	if(view->isCurrentView())
		m_dockTab->refreshUI(params);
}

void SurfaceDeformationPlugin::changeVerticesSelectionMode(View* view, MapHandlerGen* map, SelectionMode m)
{
	ParameterSet* params = h_viewParams[view];
	params->perMap[map->getName()]->verticesSelectionMode = m;

	if(view->isCurrentView())
		m_dockTab->refreshUI(params);
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
		changePositionAttribute(view, map, map->getAttribute<PFP2::VEC3, VERTEX>(m_dockTab->combo_positionAttribute->currentText()));
	}
}

void SurfaceDeformationPlugin::cb_selectLockedVertices(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeVerticesSelectionMode(view, map, LOCKED);
	}
}

void SurfaceDeformationPlugin::cb_selectHandleVertices(bool b)
{
	if(!b_refreshingUI)
	{
		View* view = m_window->getCurrentView();
		MapHandlerGen* map = h_viewParams[view]->selectedMap;
		changeVerticesSelectionMode(view, map, HANDLE);
	}
}

void SurfaceDeformationPlugin::matchDiffCoord(View* view, MapHandlerGen* mh)
{
	PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(mh)->getMap();
	PerMapParameterSet* perMap = h_viewParams[view]->perMap[mh->getName()];

	nlMakeCurrent(perMap->nlContext);
	if(nlGetCurrentState() == NL_STATE_INITIAL)
		nlBegin(NL_SYSTEM) ;
	for(int coord = 0; coord < 3; ++coord)
	{
		LinearSolving::setupVariables<PFP2>(*map, perMap->vIndex, *perMap->lockingMarker, perMap->positionAttribute, coord) ;
		nlBegin(NL_MATRIX) ;
		LinearSolving::addRowsRHS_Laplacian_Cotan<PFP2>(*map, perMap->vIndex, perMap->edgeWeight, perMap->vertexArea, perMap->diffCoord, coord) ;
		nlEnd(NL_MATRIX) ;
		nlEnd(NL_SYSTEM) ;
		nlSolve() ;
		LinearSolving::getResult<PFP2>(*map, perMap->vIndex, perMap->positionAttribute, coord) ;
		nlReset(NL_TRUE) ;
	}
}

void SurfaceDeformationPlugin::asRigidAsPossible(View* view, MapHandlerGen* mh)
{
	PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(mh)->getMap();
	PerMapParameterSet* perMap = h_viewParams[view]->perMap[mh->getName()];

	CellMarkerNoUnmark<VERTEX> m(*map) ;

	for(Dart d = map->begin(); d != map->end(); map->next(d))
	{
		if(!m.isMarked(d))
		{
			m.mark(d) ;

			Eigen::Matrix3f cov = Eigen::Matrix3f::Zero() ;
			PFP2::VEC3 p = perMap->positionAttribute[d] ;
			PFP2::VEC3 pInit = perMap->positionInit[d] ;
//			PFP2::REAL area = perMap->vertexArea[d] ;
			Dart it = d ;
			do
			{
				Dart neigh = map->phi1(it) ;
				PFP2::VEC3 v = perMap->positionAttribute[neigh] - p ;
				PFP2::VEC3 vv = perMap->positionInit[neigh] - pInit ;
				for(unsigned int i = 0; i < 3; ++i)
					for(unsigned int j = 0; j < 3; ++j)
						cov(i,j) += v[i] * vv[j];// * perMap->edgeWeight[it] / area ;
				Dart dboundary = map->phi_1(it) ;
				if(map->phi2(dboundary) == dboundary)
				{
					v = perMap->positionAttribute[dboundary] - p ;
					vv = perMap->positionInit[dboundary] - p ;
					for(unsigned int i = 0; i < 3; ++i)
						for(unsigned int j = 0; j < 3; ++j)
							cov(i,j) += v[i] * vv[j];// * perMap->edgeWeight[dboundary] / area ;
				}
				it = map->alpha1(it) ;
			} while(it != d) ;

			Eigen::JacobiSVD<Eigen::Matrix3f> svd(cov, Eigen::ComputeFullU | Eigen::ComputeFullV) ;
			Eigen::Matrix3f R = svd.matrixU() * svd.matrixV().transpose() ;

			if(R.determinant() < 0)
			{
				Eigen::Matrix3f U = svd.matrixU() ;
				for(unsigned int i = 0; i < 3; ++i)
					U(i,2) *= -1 ;
				R = U * svd.matrixV().transpose() ;
			}

			perMap->vertexRotationMatrix[d] = R ;
		}
	}

	for(Dart d = map->begin(); d != map->end(); map->next(d))
	{
		if(m.isMarked(d))
		{
			m.unmark(d) ;

			unsigned int degree = 0 ;
			Eigen::Matrix3f r = Eigen::Matrix3f::Zero() ;
			Dart it = d ;
			do
			{
				r += perMap->vertexRotationMatrix[map->phi1(it)] ;
				++degree ;
				Dart dboundary = map->phi_1(it) ;
				if(map->phi2(dboundary) == dboundary)
				{
					r += perMap->vertexRotationMatrix[dboundary] ;
					++degree ;
				}
				it = map->alpha1(it) ;
			} while(it != d) ;
			r += perMap->vertexRotationMatrix[d] ;
			r /= degree + 1 ;
			PFP2::VEC3& dc = perMap->diffCoord[d] ;
			Eigen::Vector3f rdc(dc[0], dc[1], dc[2]) ;
			rdc = r * rdc ;
			perMap->rotatedDiffCoord[d] = PFP2::VEC3(rdc[0], rdc[1], rdc[2]) ;

//			Eigen::Vector3f rdc = Eigen::Vector3f::Zero() ;
//			Dart it = d ;
//			PFP::REAL vArea = perMap->vertexArea[d] ;
//			Eigen::Matrix3f& rotM = perMap->vertexRotationMatrix[d] ;
//			PFP::REAL val = 0 ;
//			do
//			{
//				Dart ddn = map->phi1(it) ;
//				PFP::REAL w = perMap->edgeWeight[it] / vArea ;
//				PFP::VEC3 vv = (perMap->positionInit[ddn] - perMap->positionInit[it]) * w ;
//				val += w ;
//				Eigen::Matrix3f r = 0.5f * (perMap->vertexRotationMatrix[ddn] + rotM) ;
//				Eigen::Vector3f vvr = r * Eigen::Vector3f(vv[0], vv[1], vv[2]) ;
//				rdc += vvr ;
//				Dart dboundary = map->phi_1(it) ;
//				if(map->phi2(dboundary) == dboundary)
//				{
//					w = perMap->edgeWeight[dboundary] / vArea ;
//					vv = (perMap->positionInit[dboundary] - perMap->positionInit[it]) * w ;
//					val += w ;
//					r = 0.5f * (perMap->vertexRotationMatrix[dboundary] + rotM) ;
//					vvr = r * Eigen::Vector3f(vv[0], vv[1], vv[2]) ;
//					rdc += vvr ;
//				}
//				it = map->alpha1(it) ;
//			} while(it != d) ;
//			rdc /= val ;
//			perMap->rotatedDiffCoord[d] = PFP::VEC3(rdc[0], rdc[1], rdc[2]) ;
		}
	}

	nlMakeCurrent(perMap->nlContext);
	if(nlGetCurrentState() == NL_STATE_INITIAL)
		nlBegin(NL_SYSTEM) ;
	for(int coord = 0; coord < 3; ++coord)
	{
		LinearSolving::setupVariables<PFP2>(*map, perMap->vIndex, *perMap->lockingMarker, perMap->positionAttribute, coord) ;
		nlBegin(NL_MATRIX) ;
//		LinearSolving::addRowsRHS_Laplacian_Cotan<PFP2>(*map, perMap->vIndex, perMap->edgeWeight, perMap->vertexArea, perMap->rotatedDiffCoord, coord) ;
		LinearSolving::addRowsRHS_Laplacian_Topo<PFP2>(*map, perMap->vIndex, perMap->rotatedDiffCoord, coord) ;
		nlEnd(NL_MATRIX) ;
		nlEnd(NL_SYSTEM) ;
		nlSolve() ;
		LinearSolving::getResult<PFP2>(*map, perMap->vIndex, perMap->positionAttribute, coord) ;
		nlReset(NL_TRUE) ;
	}
}



void SurfaceDeformationDockTab::refreshUI(ParameterSet* params)
{
	plugin->setRefreshingUI(true);

	mapList->clear();
	combo_positionAttribute->clear();

	MapHandlerGen* mh = params->selectedMap;

	QHash<QString, PerMapParameterSet*>::const_iterator i = params->perMap.constBegin();
	while (i != params->perMap.constEnd())
	{
		mapList->addItem(i.key());
		if(mh != NULL && i.key() == mh->getName())
		{
			QList<QListWidgetItem*> item = mapList->findItems(mh->getName(), Qt::MatchExactly);
			item[0]->setSelected(true);

			PerMapParameterSet* p = params->perMap[mh->getName()];

			GenericMap* map = mh->getGenericMap();

			AttributeContainer& cont = map->getAttributeContainer<VERTEX>();

			std::vector<std::string> names;
			std::vector<std::string> types;
			cont.getAttributesNames(names);
			cont.getAttributesTypes(types);
			std::string vec3TypeName = nameOfType(PFP2::VEC3());
			unsigned int j = 0;
			for(unsigned int i = 0; i < names.size(); ++i)
			{
				if(types[i] == vec3TypeName)
				{
					combo_positionAttribute->addItem(QString::fromStdString(names[i]));
					if(!p->positionAttribute.isValid())
					{								// if nothing is specified in the parameter set
						if(names[i] == "position")	// try to select a position attribute named "position"
						{
							p->positionAttribute = mh->getAttribute<PFP2::VEC3, VERTEX>(QString::fromStdString(names[i]));
							combo_positionAttribute->setCurrentIndex(j);
						}
					}
					++j;
				}
			}

			j = 0;
			if(!p->positionAttribute.isValid())
			{
				for(unsigned int i = 0; i < names.size(); ++i)
				{
					if(types[i] == vec3TypeName)
					{
						p->positionAttribute = mh->getAttribute<PFP2::VEC3, VERTEX>(QString::fromStdString(names[i]));
						combo_positionAttribute->setCurrentIndex(j);
						++j;
						break;
					}
				}
			}

			radio_locked->setChecked(p->verticesSelectionMode == LOCKED);
			radio_handle->setChecked(p->verticesSelectionMode == HANDLE);
		}
		++i;
	}

	plugin->setRefreshingUI(false);
}



#ifndef DEBUG
Q_EXPORT_PLUGIN2(SurfaceDeformationPlugin, SurfaceDeformationPlugin)
#else
Q_EXPORT_PLUGIN2(SurfaceDeformationPluginD, SurfaceDeformationPlugin)
#endif
