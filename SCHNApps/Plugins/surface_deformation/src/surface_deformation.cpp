#include "surface_deformation.h"

#include "Algo/Geometry/normal.h"
#include "Algo/Geometry/laplacian.h"
#include "Algo/LinearSolving/basic.h"

#include "Algo/Topo/basic.h"

#include "camera.h"

#include <QKeyEvent>
#include <QMouseEvent>

namespace CGoGN
{

namespace SCHNApps
{

MapParameters::MapParameters() :
	handleSelector(NULL),
	freeSelector(NULL),
	initialized(false),
	nlContext(NULL)
{}

MapParameters::~MapParameters()
{
	if(nlContext)
		nlDeleteContext(nlContext);
}

void MapParameters::start(MapHandlerGen* mhg)
{
	if(!initialized)
	{
		if(positionAttribute.isValid() && handleSelector && freeSelector)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);

			positionInit = mh->getAttribute<PFP2::VEC3, VERTEX>("positionInit");
			if(!positionInit.isValid())
				positionInit = mh->addAttribute<PFP2::VEC3, VERTEX>("positionInit");

			diffCoord = mh->getAttribute<PFP2::VEC3, VERTEX>("diffCoord");
			if(!diffCoord.isValid())
				diffCoord = mh->addAttribute<PFP2::VEC3, VERTEX>("diffCoord");

			vertexRotationMatrix = mh->getAttribute<Eigen_Matrix3f, VERTEX>("vertexRotationMatrix");
			if(!vertexRotationMatrix.isValid())
				vertexRotationMatrix = mh->addAttribute<Eigen_Matrix3f, VERTEX>("vertexRotationMatrix");

			rotatedDiffCoord = mh->getAttribute<PFP2::VEC3, VERTEX>("rotatedDiffCoord");
			if(!rotatedDiffCoord.isValid())
				rotatedDiffCoord = mh->addAttribute<PFP2::VEC3, VERTEX>("rotatedDiffCoord");

			vIndex = mh->getAttribute<unsigned int, VERTEX>("vIndex");
			if(!vIndex.isValid())
				vIndex = mh->addAttribute<unsigned int, VERTEX>("vIndex");

			PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(mh)->getMap();

			map->copyAttribute(positionInit, positionAttribute);

			Algo::Surface::Geometry::computeLaplacianTopoVertices<PFP2>(*map, positionAttribute, diffCoord);

			for(unsigned int i = vertexRotationMatrix.begin(); i != vertexRotationMatrix.end(); vertexRotationMatrix.next(i))
				vertexRotationMatrix[i] = Eigen::Matrix3f::Identity();

			nb_vertices = Algo::Topo::computeIndexCells<VERTEX>(*map, vIndex);

			if(nlContext)
				nlDeleteContext(nlContext);
			nlContext = nlNewContext();
			nlSolverParameteri(NL_NB_VARIABLES, nb_vertices);
			nlSolverParameteri(NL_LEAST_SQUARES, NL_TRUE);
			nlSolverParameteri(NL_SOLVER, NL_CHOLMOD_EXT);

			initialized = true;
		}
	}
}

void MapParameters::stop(MapHandlerGen* mh)
{
	if(initialized)
	{
//		if(positionInit.isValid())
//			mh->removeAttribute(positionInit);

//		if(diffCoord.isValid())
//			mh->removeAttribute(diffCoord);

//		if(vertexRotationMatrix.isValid())
//			mh->removeAttribute(vertexRotationMatrix);

//		if(rotatedDiffCoord.isValid())
//			mh->removeAttribute(rotatedDiffCoord);

//		if(vIndex.isValid())
//			mh->removeAttribute(vIndex);

		if(nlContext)
			nlDeleteContext(nlContext);

		initialized = false;
	}
}





bool Surface_Deformation_Plugin::enable()
{
	//	magic line that init static variables of GenericMap in the plugins
	GenericMap::copyAllStatics(m_schnapps->getStaticPointers());

	m_dockTab = new Surface_Deformation_DockTab(m_schnapps, this);
	m_schnapps->addPluginDockTab(this, m_dockTab, "Surface_Deformation");

	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapAdded(map);

	m_dockTab->updateMapParameters();

	return true;
}

void Surface_Deformation_Plugin::disable()
{
	disconnect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
}

void Surface_Deformation_Plugin::draw(View *view)
{
}

void Surface_Deformation_Plugin::keyPress(View* view, QKeyEvent* event)
{
	switch(event->key())
	{
		case Qt::Key_D : {
			MapHandlerGen* mh = m_schnapps->getSelectedMap();
			if(mh)
			{
				const MapParameters& p = h_parameterSet[mh];
				if(!m_dragging)
				{
					if(p.handleSelector && !p.handleSelector->getSelectedCells().empty())
					{
						m_dragging = true;
						m_draginit = false;
						view->setMouseTracking(true);
						view->getCurrentCamera()->disableViewsBoundingBoxFitting();
					}
				}
				else
				{
					m_dragging = false;
					m_draginit = false;
					view->setMouseTracking(false);
					view->getCurrentCamera()->enableViewsBoundingBoxFitting();
				}
			}
			break;
		}

		case Qt::Key_R : {
			MapHandlerGen* mh = m_schnapps->getSelectedMap();
			if(mh)
			{
				const MapParameters& p = h_parameterSet[mh];
				if(p.initialized)
				{
					asRigidAsPossible(mh);
					mh->notifyAttributeModification(p.positionAttribute);
					view->updateGL();
				}
			}
			break;
		}
	}
}

void Surface_Deformation_Plugin::mouseMove(View* view, QMouseEvent* event)
{
	if (m_dragging)
	{
		MapHandlerGen* mh = m_schnapps->getSelectedMap();

		MapParameters& p = h_parameterSet[mh];
		const std::vector<Vertex>& handle = p.handleSelector->getSelectedCells();

		if (!m_draginit)
		{
			m_dragZ = 0;
			for (std::vector<Vertex>::const_iterator it = handle.begin(); it != handle.end(); ++it)
			{
				const PFP2::VEC3& pp = p.positionAttribute[*it];
				qglviewer::Vec q = view->camera()->projectedCoordinatesOf(qglviewer::Vec(pp[0],pp[1],pp[2]));
				m_dragZ += q.z;
			}
			m_dragZ /= handle.size();

			qglviewer::Vec pp(event->x(), event->y(), m_dragZ);
			m_dragPrevious = view->camera()->unprojectedCoordinatesOf(pp);

			m_draginit = true;
		}
		else
		{
			qglviewer::Vec pp(event->x(), event->y(), m_dragZ);
			qglviewer::Vec qq = view->camera()->unprojectedCoordinatesOf(pp);

			qglviewer::Vec vec = qq - m_dragPrevious;
			PFP2::VEC3 t(vec.x, vec.y, vec.z);
			for (std::vector<Vertex>::const_iterator it = handle.begin(); it != handle.end(); ++it)
				p.positionAttribute[*it] += t;

			m_dragPrevious = qq;

//			matchDiffCoord(map);
			if (p.initialized)
			{
				asRigidAsPossible(mh);
				mh->notifyAttributeModification(p.positionAttribute);
			}
		}

		view->updateGL();
	}
}





void Surface_Deformation_Plugin::selectedMapChanged(MapHandlerGen *prev, MapHandlerGen *cur)
{
	m_dockTab->updateMapParameters();
	if (cur==NULL)
		m_dockTab->setDisabled(true);
	else
		m_dockTab->setDisabled(false);
}

void Surface_Deformation_Plugin::mapAdded(MapHandlerGen* map)
{
	connect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
	connect(map, SIGNAL(cellSelectorAdded(unsigned int, const QString&)), this, SLOT(cellSelectorAdded(unsigned int, const QString&)));
	connect(map, SIGNAL(cellSelectorRemoved(unsigned int, const QString&)), this, SLOT(cellSelectorRemoved(unsigned int, const QString&)));
	connect(map, SIGNAL(selectedCellsChanged(CellSelectorGen*)), this, SLOT(selectedCellsChanged(CellSelectorGen*)));
}

void Surface_Deformation_Plugin::mapRemoved(MapHandlerGen* map)
{
	disconnect(map, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(attributeAdded(unsigned int, const QString&)));
	disconnect(map, SIGNAL(cellSelectorAdded(unsigned int, const QString&)), this, SLOT(cellSelectorAdded(unsigned int, const QString&)));
	disconnect(map, SIGNAL(cellSelectorRemoved(unsigned int, const QString&)), this, SLOT(cellSelectorRemoved(unsigned int, const QString&)));
	disconnect(map, SIGNAL(selectedCellsChanged(CellSelectorGen*)), this, SLOT(selectedCellsChanged(CellSelectorGen*)));
}





void Surface_Deformation_Plugin::attributeAdded(unsigned int orbit, const QString& name)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	if(orbit == VERTEX && map->isSelectedMap())
		m_dockTab->addVertexAttribute(name);
}

void Surface_Deformation_Plugin::cellSelectorAdded(unsigned int orbit, const QString& name)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	if(orbit == VERTEX && map->isSelectedMap())
		m_dockTab->addVertexSelector(name);
}

void Surface_Deformation_Plugin::cellSelectorRemoved(unsigned int orbit, const QString& name)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	if(orbit == VERTEX && map->isSelectedMap())
		m_dockTab->removeVertexSelector(name);

	MapParameters& p = h_parameterSet[map];
	if(p.handleSelector->getName() == name)
	{
		p.stop(map);
		if(!p.initialized && map->isSelectedMap())
			m_dockTab->mapParametersInitialized(false);
		p.handleSelector = NULL;
	}
	if(p.freeSelector->getName() == name)
	{
		p.stop(map);
		if(!p.initialized && map->isSelectedMap())
			m_dockTab->mapParametersInitialized(false);
		p.freeSelector = NULL;
	}
}

void Surface_Deformation_Plugin::selectedCellsChanged(CellSelectorGen* cs)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());
	MapParameters& p = h_parameterSet[map];
	if(p.initialized && (p.handleSelector == cs || p.freeSelector == cs))
	{
		nlMakeCurrent(p.nlContext) ;
		nlReset(NL_FALSE) ;
	}
}





void Surface_Deformation_Plugin::changePositionAttribute(const QString& map, const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		MapParameters& p = h_parameterSet[m];
		if(!p.initialized)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m);
			p.positionAttribute = mh->getAttribute<PFP2::VEC3, VERTEX>(name);
			if(m->isSelectedMap())
				m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Deformation_Plugin::changeHandleSelector(const QString& map, const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		MapParameters& p = h_parameterSet[m];
		if(!p.initialized)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m);
			p.handleSelector = mh->getCellSelector<VERTEX>(name);
			if(m->isSelectedMap())
				m_dockTab->updateMapParameters();
		}
	}
}

void Surface_Deformation_Plugin::changeFreeSelector(const QString& map, const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		MapParameters& p = h_parameterSet[m];
		if(!p.initialized)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m);
			p.freeSelector = mh->getCellSelector<VERTEX>(name);
			if(m->isSelectedMap())
				m_dockTab->updateMapParameters();
		}
	}
}





void Surface_Deformation_Plugin::toggleMapDeformation(MapHandlerGen* map)
{
	if(map)
	{
		MapParameters& p = h_parameterSet[map];
		if (!p.initialized)
		{
			p.start(map);
			if (p.initialized && map->isSelectedMap())
				m_dockTab->mapParametersInitialized(true);
		}
		else
		{
			p.stop(map);
			if (!p.initialized && map->isSelectedMap())
				m_dockTab->mapParametersInitialized(false);
		}
	}
}

void Surface_Deformation_Plugin::matchDiffCoord(MapHandlerGen* mh)
{
	PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(mh)->getMap();
	MapParameters& p = h_parameterSet[mh];

	nlMakeCurrent(p.nlContext);
	if (nlGetCurrentState() == NL_STATE_INITIAL)
		nlBegin(NL_SYSTEM) ;
	for (int coord = 0; coord < 3; ++coord)
	{
		Algo::LinearSolving::setupVariables<PFP2>(*map, p.vIndex, p.freeSelector->getMarker(), p.positionAttribute, coord);
		nlBegin(NL_MATRIX);
		Algo::LinearSolving::addRowsRHS_Laplacian_Topo<PFP2>(*map, p.vIndex, p.diffCoord, coord);
		nlEnd(NL_MATRIX);
		nlEnd(NL_SYSTEM);
		nlSolve();
		Algo::LinearSolving::getResult<PFP2>(*map, p.vIndex, p.positionAttribute, coord);
		nlReset(NL_TRUE);
	}
}

void Surface_Deformation_Plugin::asRigidAsPossible(MapHandlerGen* mh)
{
	PFP2::MAP* map = static_cast<MapHandler<PFP2>*>(mh)->getMap();
	MapParameters& p = h_parameterSet[mh];

	if (p.initialized)
	{
		CellMarkerNoUnmark<PFP2::MAP, VERTEX> m(*map) ;

		for (Dart d = map->begin(); d != map->end(); map->next(d))
		{
			if (!m.isMarked(d))
			{
				m.mark(d) ;

				Eigen::Matrix3f cov = Eigen::Matrix3f::Zero() ;
				PFP2::VEC3 pp = p.positionAttribute[d] ;
				PFP2::VEC3 ppInit = p.positionInit[d] ;
	//			PFP2::REAL area = p.vertexArea[d] ;
				Dart it = d ;
				do
				{
					Dart neigh = map->phi1(it) ;
					PFP2::VEC3 v = p.positionAttribute[neigh] - pp ;
					PFP2::VEC3 vv = p.positionInit[neigh] - ppInit ;
					for (unsigned int i = 0; i < 3; ++i)
						for (unsigned int j = 0; j < 3; ++j)
							cov(i,j) += v[i] * vv[j];// * perMap->edgeWeight[it] / area ;
					Dart dboundary = map->phi_1(it) ;
					if (map->phi2(dboundary) == dboundary)
					{
						v = p.positionAttribute[dboundary] - pp ;
						vv = p.positionInit[dboundary] - pp ;
						for(unsigned int i = 0; i < 3; ++i)
							for(unsigned int j = 0; j < 3; ++j)
								cov(i,j) += v[i] * vv[j];// * perMap->edgeWeight[dboundary] / area ;
					}
					it = map->alpha1(it) ;
				} while (it != d) ;

				Eigen::JacobiSVD<Eigen::Matrix3f> svd(cov, Eigen::ComputeFullU | Eigen::ComputeFullV) ;
				Eigen::Matrix3f R = svd.matrixU() * svd.matrixV().transpose() ;

				if (R.determinant() < 0)
				{
					Eigen::Matrix3f U = svd.matrixU() ;
					for (unsigned int i = 0; i < 3; ++i)
						U(i,2) *= -1 ;
					R = U * svd.matrixV().transpose() ;
				}

				p.vertexRotationMatrix[d] = R ;
			}
		}

		for (Dart d = map->begin(); d != map->end(); map->next(d))
		{
			if (m.isMarked(d))
			{
				m.unmark(d) ;

				unsigned int degree = 0 ;
				Eigen::Matrix3f r = Eigen::Matrix3f::Zero() ;
				Dart it = d ;
				do
				{
					r += p.vertexRotationMatrix[map->phi1(it)] ;
					++degree ;
					Dart dboundary = map->phi_1(it) ;
					if(map->phi2(dboundary) == dboundary)
					{
						r += p.vertexRotationMatrix[dboundary] ;
						++degree ;
					}
					it = map->alpha1(it) ;
				} while (it != d) ;
				r += p.vertexRotationMatrix[d] ;
				r /= degree + 1 ;
				PFP2::VEC3& dc = p.diffCoord[d] ;
				Eigen::Vector3f rdc(dc[0], dc[1], dc[2]) ;
				rdc = r * rdc ;
				p.rotatedDiffCoord[d] = PFP2::VEC3(rdc[0], rdc[1], rdc[2]) ;

	//			Eigen::Vector3f rdc = Eigen::Vector3f::Zero() ;
	//			Dart it = d ;
	//			PFP::REAL vArea = p.vertexArea[d] ;
	//			Eigen::Matrix3f& rotM = p.vertexRotationMatrix[d] ;
	//			PFP::REAL val = 0 ;
	//			do
	//			{
	//				Dart ddn = map->phi1(it) ;
	//				PFP::REAL w = p.edgeWeight[it] / vArea ;
	//				PFP::VEC3 vv = (p.positionInit[ddn] - p.positionInit[it]) * w ;
	//				val += w ;
	//				Eigen::Matrix3f r = 0.5f * (p.vertexRotationMatrix[ddn] + rotM) ;
	//				Eigen::Vector3f vvr = r * Eigen::Vector3f(vv[0], vv[1], vv[2]) ;
	//				rdc += vvr ;
	//				Dart dboundary = map->phi_1(it) ;
	//				if(map->phi2(dboundary) == dboundary)
	//				{
	//					w = p.edgeWeight[dboundary] / vArea ;
	//					vv = (p.positionInit[dboundary] - p.positionInit[it]) * w ;
	//					val += w ;
	//					r = 0.5f * (p.vertexRotationMatrix[dboundary] + rotM) ;
	//					vvr = r * Eigen::Vector3f(vv[0], vv[1], vv[2]) ;
	//					rdc += vvr ;
	//				}
	//				it = map->alpha1(it) ;
	//			} while(it != d) ;
	//			rdc /= val ;
	//			p.rotatedDiffCoord[d] = PFP::VEC3(rdc[0], rdc[1], rdc[2]) ;
			}
		}

		nlMakeCurrent(p.nlContext);
		if (nlGetCurrentState() == NL_STATE_INITIAL)
			nlBegin(NL_SYSTEM);
		for (int coord = 0; coord < 3; ++coord)
		{
			Algo::LinearSolving::setupVariables<PFP2>(*map, p.vIndex, p.freeSelector->getMarker(), p.positionAttribute, coord);
			nlBegin(NL_MATRIX);
	//		LinearSolving::addRowsRHS_Laplacian_Cotan<PFP2>(*map, p.vIndex, p.edgeWeight, p.vertexArea, p.rotatedDiffCoord, coord);
			Algo::LinearSolving::addRowsRHS_Laplacian_Topo<PFP2>(*map, p.vIndex, p.rotatedDiffCoord, coord);
			nlEnd(NL_MATRIX);
			nlEnd(NL_SYSTEM);
			nlSolve();
			Algo::LinearSolving::getResult<PFP2>(*map, p.vIndex, p.positionAttribute, coord);
			nlReset(NL_TRUE);
		}
	}
}
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#else
	Q_EXPORT_PLUGIN2(Surface_Deformation_Plugin, Surface_Deformation_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
