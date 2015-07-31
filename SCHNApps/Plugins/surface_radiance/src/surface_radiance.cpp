#include "surface_radiance.h"

#include "meshTableSurfaceRadiance.h"
#include "meshTableSurfaceRadiance_P.h"

#include "halfEdgeSelectorRadiance.h"
#include "edgeSelectorRadiance.h"

#include "mapHandler.h"
#include "camera.h"

#include "Algo/Geometry/distances.h"
#include "Algo/Geometry/plane.h"

#include "SphericalFunctionIntegratorCartesian.h"

#include <QFileDialog>
#include <QFileInfo>

namespace CGoGN
{

namespace SCHNApps
{

bool Surface_Radiance_Plugin::enable()
{
	//	magic line that init static variables of GenericMap in the plugins
	GenericMap::copyAllStatics(m_schnapps->getStaticPointers());

	m_dockTab = new Surface_Radiance_DockTab(m_schnapps, this);
	m_schnapps->addPluginDockTab(this, m_dockTab, "Surface_Radiance");

	m_importSHAction = new QAction("importSH", this);
	m_schnapps->addMenuAction(this, "Radiance;Import (SH)", m_importSHAction);
	connect(m_importSHAction, SIGNAL(triggered()), this, SLOT(importFromFileDialog_SH()));

	m_importPAction = new QAction("importP", this);
	m_schnapps->addMenuAction(this, "Radiance;Import (P)", m_importPAction);
	connect(m_importPAction, SIGNAL(triggered()), this, SLOT(importFromFileDialog_P()));

	m_computeRadianceDistanceDialog = new Dialog_ComputeRadianceDistance(m_schnapps);

	m_computeRadianceDistanceAction = new QAction("Compute Radiance Distance", this);
	m_schnapps->addMenuAction(this, "Radiance;Compute Distance", m_computeRadianceDistanceAction);
	connect(m_computeRadianceDistanceAction, SIGNAL(triggered()), this, SLOT(openComputeRadianceDistanceDialog()));

	connect(m_computeRadianceDistanceDialog, SIGNAL(accepted()), this, SLOT(computeRadianceDistanceFromDialog()));
	connect(m_computeRadianceDistanceDialog->button_apply, SIGNAL(clicked()), this, SLOT(computeRadianceDistanceFromDialog()));

	connect(m_schnapps, SIGNAL(selectedViewChanged(View*, View*)), this, SLOT(selectedViewChanged(View*, View*)));
	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapAdded(map);

	m_dockTab->updateMapParameters();

	connect(m_schnapps, SIGNAL(schnappsClosing()), this, SLOT(schnappsClosing()));

	return true;
}

void Surface_Radiance_Plugin::disable()
{
	disconnect(m_schnapps, SIGNAL(selectedViewChanged(View*, View*)), this, SLOT(selectedViewChanged(View*, View*)));
	disconnect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*, MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapRemoved(map);
}

void Surface_Radiance_Plugin::drawMap(View* view, MapHandlerGen* map)
{
	const MapParameters& p = h_mapParameterSet[map];

	if(p.radiancePerVertexShader && p.positionVBO && p.normalVBO)
	{
		p.radiancePerVertexShader->setAttributePosition(p.positionVBO);
		p.radiancePerVertexShader->setAttributeNormal(p.normalVBO);
		p.radiancePerVertexShader->setAttributeRadiance(p.paramVBO, p.radianceTexture, GL_TEXTURE1);

		qglviewer::Vec c = view->getCurrentCamera()->position();
		PFP2::VEC3 camera(c.x, c.y, c.z);
		p.radiancePerVertexShader->setCamera(Geom::Vec3f(camera[0], camera[1], camera[2])); // convert to Vec3f because PFP2 can hold Vec3d !

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		map->draw(p.radiancePerVertexShader, Algo::Render::GL2::TRIANGLES);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	if(p.radiancePerVertexPShader && p.positionVBO && p.normalVBO && p.tangentVBO && p.binormalVBO)
	{
		p.radiancePerVertexPShader->setAttributePosition(p.positionVBO);
		p.radiancePerVertexPShader->setAttributeTangent(p.tangentVBO);
		p.radiancePerVertexPShader->setAttributeNormal(p.normalVBO);
		p.radiancePerVertexPShader->setAttributeBiNormal(p.binormalVBO);
		p.radiancePerVertexPShader->setAttributeRadiance(p.paramVBO, p.radianceTexture, GL_TEXTURE1);

		qglviewer::Vec c = view->getCurrentCamera()->position();
		PFP2::VEC3 camera(c.x, c.y, c.z);
		p.radiancePerVertexPShader->setCamera(Geom::Vec3f(camera[0], camera[1], camera[2])); // convert to Vec3f because PFP2 can hold Vec3d !

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		map->draw(p.radiancePerVertexPShader, Algo::Render::GL2::TRIANGLES);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}





void Surface_Radiance_Plugin::selectedViewChanged(View *prev, View *cur)
{
	m_dockTab->updateMapParameters();
}

void Surface_Radiance_Plugin::selectedMapChanged(MapHandlerGen *prev, MapHandlerGen *cur)
{
	m_dockTab->updateMapParameters();
	if (cur == NULL)
		m_dockTab->setDisabled(true);
	else
		m_dockTab->setDisabled(false);
}

void Surface_Radiance_Plugin::mapAdded(MapHandlerGen* map)
{
	connect(map, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(vboAdded(Utils::VBO*)));
	connect(map, SIGNAL(vboRemoved(Utils::VBO*)), this, SLOT(vboRemoved(Utils::VBO*)));
	connect(map, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
}

void Surface_Radiance_Plugin::mapRemoved(MapHandlerGen* map)
{
	disconnect(map, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(vboAdded(Utils::VBO*)));
	disconnect(map, SIGNAL(vboRemoved(Utils::VBO*)), this, SLOT(vboRemoved(Utils::VBO*)));
	disconnect(map, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
}

void Surface_Radiance_Plugin::schnappsClosing()
{
//	m_computeRadianceDistanceDialog->close();
}





void Surface_Radiance_Plugin::vboAdded(Utils::VBO *vbo)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	if(map == m_schnapps->getSelectedMap())
	{
		if(vbo->dataSize() == 3)
		{
			m_dockTab->addPositionVBO(QString::fromStdString(vbo->name()));
			m_dockTab->addNormalVBO(QString::fromStdString(vbo->name()));
			m_dockTab->addTangentVBO(QString::fromStdString(vbo->name()));
			m_dockTab->addBiNormalVBO(QString::fromStdString(vbo->name()));
		}
	}
}

void Surface_Radiance_Plugin::vboRemoved(Utils::VBO *vbo)
{
	MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());

	if(map == m_schnapps->getSelectedMap())
	{
		if(vbo->dataSize() == 3)
		{
			m_dockTab->removePositionVBO(QString::fromStdString(vbo->name()));
			m_dockTab->removeNormalVBO(QString::fromStdString(vbo->name()));
			m_dockTab->removeTangentVBO(QString::fromStdString(vbo->name()));
			m_dockTab->removeBiNormalVBO(QString::fromStdString(vbo->name()));
		}
	}

	MapParameters& mapParam = h_mapParameterSet[map];
	if(mapParam.positionVBO == vbo)
	{
		mapParam.positionVBO = NULL;
	}
	if(mapParam.normalVBO == vbo)
	{
		mapParam.normalVBO = NULL;
	}
	if(mapParam.tangentVBO == vbo)
	{
		mapParam.tangentVBO = NULL;
	}
	if(mapParam.binormalVBO == vbo)
	{
		mapParam.binormalVBO = NULL;
	}
}

void Surface_Radiance_Plugin::attributeModified(unsigned int orbit, QString nameAttr)
{
	if(orbit == VERTEX)
	{

	}
}

void Surface_Radiance_Plugin::importFromFileDialog_SH()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(m_schnapps, "Import surface with radiance (SH)", m_schnapps->getAppPath(), "Surface with radiance Files (*.ply)");
	QStringList::Iterator it = fileNames.begin();
	while(it != fileNames.end()) {
		importFromFile_SH(*it);
		++it;
	}
}

void Surface_Radiance_Plugin::importFromFileDialog_P()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(m_schnapps, "Import surface with radiance (P)", m_schnapps->getAppPath(), "Surface with radiance Files (*.ply)");
	QStringList::Iterator it = fileNames.begin();
	while(it != fileNames.end()) {
		importFromFile_P(*it);
		++it;
	}
}

void Surface_Radiance_Plugin::openComputeRadianceDistanceDialog()
{
	m_computeRadianceDistanceDialog->show();
}

void Surface_Radiance_Plugin::computeRadianceDistanceFromDialog()
{
	QList<QListWidgetItem*> currentItems1 = m_computeRadianceDistanceDialog->list_maps_1->selectedItems();
	QList<QListWidgetItem*> currentItems2 = m_computeRadianceDistanceDialog->list_maps_2->selectedItems();

	if(!currentItems1.empty() && !currentItems2.empty())
	{
		const QString& mapName1 = currentItems1[0]->text();
		const QString& mapName2 = currentItems2[0]->text();

		QString positionName1 = m_computeRadianceDistanceDialog->combo_positionAttribute_1->currentText();
		QString positionName2 = m_computeRadianceDistanceDialog->combo_positionAttribute_2->currentText();

		QString normalName1 = m_computeRadianceDistanceDialog->combo_normalAttribute_1->currentText();
		QString normalName2 = m_computeRadianceDistanceDialog->combo_normalAttribute_2->currentText();

		QString distanceName1;
		if(m_computeRadianceDistanceDialog->distanceAttributeName_1->text().isEmpty())
			distanceName1 = m_computeRadianceDistanceDialog->combo_distanceAttribute_1->currentText();
		else
			distanceName1 = m_computeRadianceDistanceDialog->distanceAttributeName_1->text();

		QString distanceName2;
		if(m_computeRadianceDistanceDialog->distanceAttributeName_2->text().isEmpty())
			distanceName2 = m_computeRadianceDistanceDialog->combo_distanceAttribute_2->currentText();
		else
			distanceName2 = m_computeRadianceDistanceDialog->distanceAttributeName_2->text();

		// create VBO if asked
		if (m_computeRadianceDistanceDialog->enableVBO->isChecked())
		{
			MapHandlerGen* mhg1 = getSCHNApps()->getMap(mapName1);
			if (mhg1)
				mhg1->createVBO(distanceName1);

			MapHandlerGen* mhg2 = getSCHNApps()->getMap(mapName2);
			if (mhg2)
				mhg2->createVBO(distanceName2);
		}

		computeRadianceDistance(mapName1, positionName1, normalName1, distanceName1, mapName2, positionName2, normalName2, distanceName2);
	}
}





void Surface_Radiance_Plugin::changePositionVBO(const QString& map, const QString& vbo)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		Utils::VBO* vbuf = m->getVBO(vbo);
		h_mapParameterSet[m].positionVBO = vbuf;
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}

void Surface_Radiance_Plugin::changeNormalVBO(const QString& map, const QString& vbo)
{
	MapHandlerGen* m = m_schnapps->getMap(map);
	if(m)
	{
		Utils::VBO* vbuf = m->getVBO(vbo);
		h_mapParameterSet[m].normalVBO = vbuf;
		if(m->isSelectedMap())
			m_dockTab->updateMapParameters();
	}
}

MapHandlerGen* Surface_Radiance_Plugin::importFromFile_SH(const QString& fileName)
{
	QFileInfo fi(fileName);
	if(fi.exists())
	{
		MapHandlerGen* mhg = m_schnapps->addMap(fi.baseName(), 2);
		if(mhg)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
			PFP2::MAP* map = mh->getMap();

			MeshTablesSurface_Radiance importer(*map);
			if (!importer.importPLY<Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3> >(fileName.toStdString()))
			{
				std::cout << "could not import " << fileName.toStdString() << std::endl;
				return NULL;
			}
			CGoGN::Algo::Surface::Import::importMesh<PFP2>(*map, importer);

			// get vertex position attribute
			VertexAttribute<PFP2::VEC3, PFP2::MAP> position = map->getAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>("position") ;
			VertexAttribute<PFP2::VEC3, PFP2::MAP> normal = map->getAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>("normal");
			mh->registerAttribute(position);
			mh->registerAttribute(normal);

			MapParameters& mapParams = h_mapParameterSet[mhg];

			mapParams.nbVertices = Algo::Topo::getNbOrbits<VERTEX>(*map);

			mapParams.radiance = map->getAttribute<Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3>, VERTEX, PFP2::MAP>("radiance") ;
			mapParams.radianceTexture = new Utils::Texture<2, Geom::Vec3f>(GL_FLOAT);
			mapParams.param = map->checkAttribute<Geom::Vec2i, VERTEX, PFP2::MAP>("param");

			// create texture
			unsigned int nbv_nbc = Algo::Topo::getNbOrbits<VERTEX>(*map) * Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3>::get_nb_coefs();
			unsigned int size = 1;
			while (size * size < nbv_nbc)
				size <<= 1;

			mapParams.radianceTexture->create(Geom::Vec2i(size, size));

			// fill texture
			unsigned int count = 0;
			foreach_cell<VERTEX>(*map, [&] (Vertex v)
			{
				unsigned int i = count / size;
				unsigned int j = count % size;
				mapParams.param[v] = Geom::Vec2i(i, j) ; // first index for current vertex
				for (int l = 0 ; l <= Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3>::get_resolution() ; ++l)
				{
					for (int m = -l ; m <= l ; ++m)
					{
						i = count / size;
						j = count % size;
						(*(mapParams.radianceTexture))(i,j) = mapParams.radiance[v].get_coef(l, m);
						++count;
					}
				}
			}) ;
			// resulting texture : SH00_vx0, SH1-1_vx0, ..., SHlm_vx0, SH00_vx1, SH1-1_vx1, ..., SHlm_vx1, etc.
			// resulting param : param[vxI] points to SH00_vxI
			// the size of the texture is needed to know where to do the divisions and modulos.

			mapParams.radianceTexture->update();

			// uncomment this line to be able to load multiple objects with different SH basis
			// (decimation will be unavailable)
//			map->removeAttribute(mapParams.radiance);

			mapParams.paramVBO = new Utils::VBO();
			mapParams.paramVBO->updateData(mapParams.param);

			mapParams.radiancePerVertexShader = new Utils::ShaderRadiancePerVertex(Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3>::get_resolution());
			registerShader(mapParams.radiancePerVertexShader);
		}

		this->pythonRecording("importFile", mhg->getName(), fi.baseName());

		return mhg;
	}
	else
		return NULL;
}

MapHandlerGen* Surface_Radiance_Plugin::importFromFile_P(const QString& fileName)
{
	QFileInfo fi(fileName);
	if(fi.exists())
	{
		MapHandlerGen* mhg = m_schnapps->addMap(fi.baseName(), 2);
		if(mhg)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
			PFP2::MAP* map = mh->getMap();

			MeshTablesSurface_Radiance_P importer(*map);
			if (!importer.importPLY<Utils::BivariatePolynomials<PFP2::REAL, PFP2::VEC3> >(fileName.toStdString()))
			{
				std::cout << "could not import " << fileName.toStdString() << std::endl;
				return NULL;
			}
			CGoGN::Algo::Surface::Import::importMesh<PFP2>(*map, importer);

			// get vertex position attribute
			VertexAttribute<PFP2::VEC3, PFP2::MAP> position = map->getAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>("position") ;
			VertexAttribute<PFP2::VEC3, PFP2::MAP> tangent = map->getAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>("tangent");
			VertexAttribute<PFP2::VEC3, PFP2::MAP> normal = map->getAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>("normal");
			VertexAttribute<PFP2::VEC3, PFP2::MAP> binormal = map->getAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>("binormal");
			mh->registerAttribute(position);
			mh->registerAttribute(tangent);
			mh->registerAttribute(normal);
			mh->registerAttribute(binormal);

			MapParameters& mapParams = h_mapParameterSet[mhg];

			mapParams.nbVertices = Algo::Topo::getNbOrbits<VERTEX>(*map);

			mapParams.radiance_P = map->getAttribute<Utils::BivariatePolynomials<PFP2::REAL, PFP2::VEC3>, VERTEX, PFP2::MAP>("radiance") ;
			mapParams.radianceTexture = new Utils::Texture<2, Geom::Vec3f>(GL_FLOAT);
			mapParams.param = map->checkAttribute<Geom::Vec2i, VERTEX, PFP2::MAP>("param");

			unsigned int nbCoefs = mapParams.radiance_P[map->begin()].get_nb_coefs();

			// create texture
			unsigned int nbv_nbc = Algo::Topo::getNbOrbits<VERTEX>(*map) * nbCoefs;
			unsigned int size = 1;
			while (size * size < nbv_nbc)
				size <<= 1;

			mapParams.radianceTexture->create(Geom::Vec2i(size, size));

			// fill texture
			unsigned int count = 0;
			foreach_cell<VERTEX>(*map, [&] (Vertex v)
			{
				PFP2::VEC3* coefs = mapParams.radiance_P[v].get_coef_tab();
				unsigned int i = count / size;
				unsigned int j = count % size;
				mapParams.param[v] = Geom::Vec2i(i, j) ; // first index for current vertex
				for (unsigned int l = 0 ; l <= nbCoefs ; ++l)
				{
					i = count / size;
					j = count % size;
					(*(mapParams.radianceTexture))(i,j) = coefs[l];
					++count;
				}
			}) ;
			// resulting texture : PB0_v0, PB1_v0, ..., PB14_v0, PB0_v1, ...
			// resulting param : param[vx] points to PB0_vx
			// the size of the texture is needed to know where to do the divisions and modulos.

			mapParams.radianceTexture->update();

			mapParams.paramVBO = new Utils::VBO();
			mapParams.paramVBO->updateData(mapParams.param);

			mapParams.radiancePerVertexPShader = new Utils::ShaderRadiancePerVertex_P();
			registerShader(mapParams.radiancePerVertexPShader);
		}

		this->pythonRecording("importFile", mhg->getName(), fi.baseName());

		return mhg;
	}
	else
		return NULL;
}

void Surface_Radiance_Plugin::decimate(const QString& mapName, const QString& positionAttributeName, const QString& normalAttributeName, float decimationGoal, bool halfCollapse, bool exportMeshes)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));
	if(mh == NULL)
		return;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName);
	if(!position.isValid())
		return;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> normal = mh->getAttribute<PFP2::VEC3, VERTEX>(normalAttributeName);
	if(!normal.isValid())
		return;

	PFP2::MAP* map = mh->getMap();

	unsigned int nbVertices = Algo::Topo::getNbOrbits<VERTEX>(*map);

	MapParameters& mapParams = h_mapParameterSet[mh];

	if (halfCollapse)
	{
		if (mapParams.positionApproximator == NULL)
		{
			mapParams.positionApproximator = new Algo::Surface::Decimation::Approximator_HalfCollapse<PFP2, PFP2::VEC3>(*map, position);
		}

		if (mapParams.normalApproximator == NULL)
		{
			mapParams.normalApproximator = new Algo::Surface::Decimation::Approximator_HalfCollapse<PFP2, PFP2::VEC3>(*map, normal);
		}

		if (mapParams.radianceApproximator == NULL)
		{
			mapParams.radianceApproximator = new Algo::Surface::Decimation::Approximator_HalfCollapse<PFP2, Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3> >(*map, mapParams.radiance);
		}

		if (mapParams.selector == NULL)
		{
			mapParams.selector = new HalfEdgeSelector_Radiance<PFP2>(
				*map,
				position,
				normal,
				mapParams.radiance,
				*(Algo::Surface::Decimation::Approximator<PFP2, PFP2::VEC3, DART>*)(mapParams.positionApproximator),
				*(Algo::Surface::Decimation::Approximator<PFP2, PFP2::VEC3, DART>*)(mapParams.normalApproximator),
				*(Algo::Surface::Decimation::Approximator<PFP2, Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3>, DART>*)(mapParams.radianceApproximator)
			);
		}
	}
	else
	{
		if (mapParams.positionApproximator == NULL)
		{
			mapParams.positionApproximator = new Algo::Surface::Decimation::Approximator_QEM<PFP2>(*map, position);
		}

		if (mapParams.normalApproximator == NULL)
		{
			mapParams.normalApproximator =
				new Algo::Surface::Decimation::Approximator_InterpolateAlongEdge<PFP2, PFP2::VEC3>(
					*map,
					normal,
					position,
					((Algo::Surface::Decimation::Approximator<PFP2, PFP2::VEC3, EDGE>*)(mapParams.positionApproximator))->getApproximationResultAttribute()
				);
		}

		if (mapParams.radianceApproximator == NULL)
		{
			mapParams.radianceApproximator =
				new Algo::Surface::Decimation::Approximator_InterpolateAlongEdge<PFP2, Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3> >(
					*map,
					mapParams.radiance,
					position,
					((Algo::Surface::Decimation::Approximator<PFP2, PFP2::VEC3, EDGE>*)(mapParams.positionApproximator))->getApproximationResultAttribute()
				);
		}

		if (mapParams.selector == NULL)
		{
			mapParams.selector =
				new EdgeSelector_Radiance<PFP2>(
					*map,
					position,
					normal,
					mapParams.radiance,
					*(Algo::Surface::Decimation::Approximator<PFP2, PFP2::VEC3, EDGE>*)(mapParams.positionApproximator),
					*(Algo::Surface::Decimation::Approximator<PFP2, PFP2::VEC3, EDGE>*)(mapParams.normalApproximator),
					*(Algo::Surface::Decimation::Approximator<PFP2, Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3>, EDGE>*)(mapParams.radianceApproximator)
				);

			mapParams.selector =
				new Algo::Surface::Decimation::EdgeSelector_QEM<PFP2>(
					*map,
					position,
					*(Algo::Surface::Decimation::Approximator<PFP2, PFP2::VEC3, EDGE>*)(mapParams.positionApproximator)
				);
		}
	}

	std::vector<Algo::Surface::Decimation::ApproximatorGen<PFP2>*> approximators;
	approximators.push_back(mapParams.positionApproximator);
	approximators.push_back(mapParams.normalApproximator);
	approximators.push_back(mapParams.radianceApproximator);

	exportNbVert.clear();
	if (exportMeshes)
	{
		unsigned int goalNbV = decimationGoal * nbVertices;
		unsigned int curNbV = nbVertices / 2;
		while (curNbV > goalNbV)
		{
			exportNbVert.push_back(curNbV);
			curNbV /= 2;
		}
		exportNbVert.push_back(goalNbV);
		nextExportIndex = 0;
	}

	std::cout << "nb vert -> " << nbVertices << std::endl;
	for (unsigned int v : exportNbVert)
	{
		std::cout << v << std::endl;
	}

	m_currentlyDecimatedMap = mh;
	m_currentDecimationHalf = halfCollapse;
	Algo::Surface::Decimation::decimate<PFP2>(*map, mapParams.selector, approximators, decimationGoal * nbVertices, true, NULL, (void (*)(void*, const void*))(Surface_Radiance_Plugin::checkNbVerticesAndExport), (void*)(this));
	m_currentlyDecimatedMap = NULL;

	mh->notifyConnectivityModification();
	mh->notifyAttributeModification(position);
}

void Surface_Radiance_Plugin::computeRadianceDistance(
	const QString& mapName1,
	const QString& positionAttributeName1,
	const QString& normalAttributeName1,
	const QString& distanceAttributeName1,
	const QString& mapName2,
	const QString& positionAttributeName2,
	const QString& normalAttributeName2,
	const QString& distanceAttributeName2)
{
	MapHandler<PFP2>* mh1 = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName1));
	if(mh1 == NULL)
		return;

	MapHandler<PFP2>* mh2 = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName2));
	if(mh2 == NULL)
		return;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> position1 = mh1->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName1);
	if(!position1.isValid())
		return;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> normal1 = mh1->getAttribute<PFP2::VEC3, VERTEX>(normalAttributeName1);
	if(!normal1.isValid())
		return;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> position2 = mh2->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName2);
	if(!position2.isValid())
		return;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> normal2 = mh2->getAttribute<PFP2::VEC3, VERTEX>(normalAttributeName2);
	if(!normal2.isValid())
		return;

	VertexAttribute<PFP2::REAL, PFP2::MAP> distance1 = mh1->getAttribute<PFP2::REAL, VERTEX>(distanceAttributeName1);
	if(!distance1.isValid())
		distance1 = mh1->addAttribute<PFP2::REAL, VERTEX>(distanceAttributeName1);

	VertexAttribute<PFP2::REAL, PFP2::MAP> distance2 = mh2->getAttribute<PFP2::REAL, VERTEX>(distanceAttributeName2);
	if(!distance2.isValid())
		distance2 = mh2->addAttribute<PFP2::REAL, VERTEX>(distanceAttributeName2);

	MapParameters& mapParams1 = h_mapParameterSet[mh1];
	MapParameters& mapParams2 = h_mapParameterSet[mh2];

	SphericalFunctionIntegratorCartesian integrator;
	integrator.Init(29);

	PFP2::MAP* map1 = mh1->getMap();
	PFP2::MAP* map2 = mh2->getMap();

	// compute distance between map1 and map2 here
	//   distance from map1 to map2 is stored in map1 vertex attribute distance1
	//   distance from map2 to map1 is stored in map2 vertex attribute distance2

	// for each vertex of map1

	std::vector<PFP2::REAL> errors;
	errors.reserve(100000);

	map2->setExternalThreadsAuthorization(true);

	Parallel::foreach_cell<VERTEX>(*map1, [&] (Vertex v, unsigned int threadIndex)
	{
		const PFP2::VEC3& P = position1[v];
		PFP2::VEC3& N = normal1[v];

		// find closest point on map2

		PFP2::REAL minDist2 = std::numeric_limits<PFP2::REAL>::max();
		Face closestFace;

		for (Face f : allFacesOf(*map2))
		{
			PFP2::REAL dist = Algo::Geometry::squaredDistancePoint2Face<PFP2>(*map2, f, position2, P);
			if (dist < minDist2)
			{
				minDist2 = dist;
				closestFace = f;
			}
		}

//		PFP2::REAL minDist = sqrt(minDist2);

		double l1, l2, l3;
		Algo::Geometry::closestPointInTriangle<PFP2>(*map2, closestFace, position2, P, l1, l2, l3);

		// compute radiance error

		const PFP2::VEC3& P1 = position2[closestFace.dart];
		const PFP2::VEC3& P2 = position2[map2->phi1(closestFace.dart)];
		const PFP2::VEC3& P3 = position2[map2->phi_1(closestFace.dart)];
		PFP2::VEC3 CP = l1*P1 + l2*P2 + l3*P3;

		const PFP2::VEC3& N1 = normal2[closestFace.dart];
		const PFP2::VEC3& N2 = normal2[map2->phi1(closestFace.dart)];
		const PFP2::VEC3& N3 = normal2[map2->phi_1(closestFace.dart)];
		PFP2::VEC3 CPN = l1*N1 + l2*N2 + l3*N3;

		const Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3>& R1 = mapParams2.radiance[closestFace.dart];
		const Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3>& R2 = mapParams2.radiance[map2->phi1(closestFace.dart)];
		const Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3>& R3 = mapParams2.radiance[map2->phi_1(closestFace.dart)];
		Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3> CPR = R1*l1 + R2*l2 + R3*l3;

		Utils::SphericalHarmonics<PFP2::REAL, PFP2::VEC3> diffRad(mapParams1.radiance[v]);
		diffRad -= CPR;

		double integral;
		double area;
		integrator.Compute(&integral, &area, SHEvalCartesian_Error, &diffRad, isInHemisphere, N.data());

		PFP2::REAL radError = integral / area;

		distance1[v] = radError;

		errors.push_back(radError);
	}
	);

	map2->setExternalThreadsAuthorization(false);

	std::sort(errors.begin(), errors.end());
	PFP2::REAL Q1 = errors[int(errors.size() / 4)];
//	PFP2::REAL Q2 = errors[int(errors.size() / 2)];
	PFP2::REAL Q3 = errors[int(errors.size() * 3 / 4)];
	PFP2::REAL IQrange = Q3 - Q1;
	PFP2::REAL lowerBound = Q1 - 1.5*IQrange;
	PFP2::REAL upperBound = Q3 + 1.5*IQrange;
	for (PFP2::REAL& dist : distance1.iterable())
	{
		if (dist < lowerBound) { dist = lowerBound; }
		if (dist > upperBound) { dist = upperBound; }
	}

	integrator.Release();

	this->pythonRecording("computeRadianceDistance", "", mapName1, positionAttributeName1, distanceAttributeName1,
							mapName2, positionAttributeName2, distanceAttributeName2);

	mh1->notifyAttributeModification(distance1);
	mh2->notifyAttributeModification(distance2);
}

void Surface_Radiance_Plugin::checkNbVerticesAndExport(Surface_Radiance_Plugin* p, const unsigned int* nbVertices)
{
	if (!p->exportNbVert.empty())
	{
		MapHandlerGen* mhg = p->currentlyDecimatedMap();
		if (*nbVertices == p->exportNbVert[p->nextExportIndex])
		{
			std::stringstream exportName;
			exportName << p->currentlyDecimatedMap()->getName().toStdString() << "_" << (p->currentDecimationHalf() ? "half_" : "full_") << (*nbVertices) << ".ply";
			std::cout << "export : " << exportName.str() << std::endl;
			p->exportPLY(mhg->getName(), "position", "normal", QString::fromStdString(exportName.str()));
			p->nextExportIndex++;
		}
	}
}

void Surface_Radiance_Plugin::exportPLY(
	const QString& mapName,
	const QString& positionAttributeName,
	const QString& normalAttributeName,
	const QString& filename)
{
	typedef PFP2::MAP MAP;
	typedef PFP2::REAL REAL;
	typedef PFP2::VEC3 VEC3;

	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));
	if(mh == NULL)
		return;

	VertexAttribute<VEC3, MAP> position = mh->getAttribute<VEC3, VERTEX>(positionAttributeName);
	if(!position.isValid())
		return;

	VertexAttribute<VEC3, MAP> normal = mh->getAttribute<VEC3, VERTEX>(normalAttributeName);
	if(!normal.isValid())
		return;

	VertexAttribute<Utils::SphericalHarmonics<REAL, VEC3>, MAP> radiance = h_mapParameterSet[mh].radiance;
	if(!radiance.isValid())
		return;

	// open file
	std::ofstream out ;
	out.open(filename.toStdString(), std::ios::out | std::ios::binary) ;

	if (!out.good())
	{
		CGoGNerr << "Unable to open file " << CGoGNendl ;
		return ;
	}

	MAP* map = mh->getMap();

	unsigned int nbDarts = map->getNbDarts() ;
	std::vector<unsigned int> facesSize ;
	std::vector<std::vector<unsigned int> > facesIdx ;
	facesSize.reserve(nbDarts/3) ;
	facesIdx.reserve(nbDarts/3) ;
	std::map<unsigned int, unsigned int> vIndex ;
	unsigned int vCpt = 0 ;
	std::vector<unsigned int> vertices ;
	vertices.reserve(nbDarts/6) ;

	// Go over all faces
	CellMarker<MAP, VERTEX> markV(*map) ;
	TraversorF<MAP> t(*map) ;
	for(Dart d = t.begin(); d != t.end(); d = t.next())
	{
		std::vector<unsigned int> fidx ;
		fidx.reserve(8) ;
		unsigned int degree = 0 ;
		Traversor2FV<MAP> tfv(*map, d) ;
		for(Dart it = tfv.begin(); it != tfv.end(); it = tfv.next())
		{
			++degree ;
			unsigned int vNum = map->getEmbedding<VERTEX>(it) ;
			if(!markV.isMarked(it))
			{
				markV.mark(it) ;
				vIndex[vNum] = vCpt++ ;
				vertices.push_back(vNum) ;
			}
			fidx.push_back(vIndex[vNum]) ;
		}
		facesSize.push_back(degree) ;
		facesIdx.push_back(fidx) ;
	}

	// Start writing the file
	out << "ply" << std::endl ;

	// test endianness
	union
	{
		uint32_t i ;
		char c[4] ;
	} bint = {0x01020304} ;
	if (bint.c[0] == 1) // big endian
		out << "format binary_big_endian 1.0" << std::endl ;
	else
		out << "format binary_little_endian 1.0" << std::endl ;

	out << "comment File generated by the CGoGN library" << std::endl ;
	out << "comment See : http://cgogn.unistra.fr/" << std::endl ;
	out << "comment or contact : cgogn@unistra.fr" << std::endl ;
	// Vertex elements
	out << "element vertex " << vertices.size() << std::endl ;

	std::string nameOfTypePly_REAL(nameOfTypePly(position[0][0])) ;

	out << "property " << nameOfTypePly_REAL << " x" << std::endl ;
	out << "property " << nameOfTypePly_REAL << " y" << std::endl ;
	out << "property " << nameOfTypePly_REAL << " z" << std::endl ;


	out << "property " << nameOfTypePly_REAL << " nx" << std::endl ;
	out << "property " << nameOfTypePly_REAL << " ny" << std::endl ;
	out << "property " << nameOfTypePly_REAL << " nz" << std::endl ;

	int res = Utils::SphericalHarmonics<REAL, VEC3>::get_resolution() ;
	for (int l = 0 ; l <= res ; ++l)
	{
		for (int m = -l ; m <= l ; ++m)
		{
			out << "property " << nameOfTypePly_REAL << " SHcoef_" << l << "_" << m << "_r" << std::endl ;
			out << "property " << nameOfTypePly_REAL << " SHcoef_" << l << "_" << m << "_g" << std::endl ;
			out << "property " << nameOfTypePly_REAL << " SHcoef_" << l << "_" << m << "_b" << std::endl ;
		}
	}

	// Face element
	out << "element face " << facesSize.size() << std::endl ;
	out << "property list uint8 " << nameOfTypePly(facesIdx[0][0]) << " vertex_indices" << std::endl ;
	out << "end_header" << std::endl ;

	// binary vertices
	for(unsigned int i = 0; i < vertices.size(); ++i)
	{
		const VEC3& p = position[vertices[i]] ;
		out.write((char*)(&(p[0])), sizeof(p)) ;
		const VEC3& n = normal[vertices[i]] ;
		out.write((char*)(&(n[0])), sizeof(n)) ;

		for (int l=0 ; l <= res ; ++l)
		{
			for (int m=-l ; m <= l ; ++m)
			{
				const VEC3& r = radiance[vertices[i]].get_coef(l,m) ;
				out.write((char*)(&(r[0])), sizeof(r)) ;
			}
		}
	}

	// binary faces
	for(unsigned int i = 0; i < facesSize.size(); ++i)
	{
		uint8_t nbe = facesSize[i] ;
		out.write((char*)(&nbe), sizeof(uint8_t)) ;
		out.write((char*)(&(facesIdx[i][0])), facesSize[i] * sizeof(facesIdx[i][0])) ;
	}

	out.close() ;

	this->pythonRecording("exportPLY", "", mapName, positionAttributeName, normalAttributeName, filename);
}


#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#else
	Q_EXPORT_PLUGIN2(Surface_Radiance_Plugin, Surface_Radiance_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
