#include "surface_differentialProperties.h"

#include "mapHandler.h"

#include "Algo/Geometry/normal.h"
#include "Algo/Geometry/curvature.h"

namespace CGoGN
{

namespace SCHNApps
{

bool Surface_DifferentialProperties_Plugin::enable()
{
	//	magic line that init static variables of GenericMap in the plugins
		GenericMap::copyAllStatics(m_schnapps->getStaticPointers());

	m_computeNormalDialog = new Dialog_ComputeNormal(m_schnapps);
	m_computeCurvatureDialog = new Dialog_ComputeCurvature(m_schnapps);

	m_computeNormalAction = new QAction("Compute Normal", this);
	m_computeCurvatureAction = new QAction("Compute Curvature", this);

	m_schnapps->addMenuAction(this, "Surface;Differential Properties;Compute Normal", m_computeNormalAction);
	m_schnapps->addMenuAction(this, "Surface;Differential Properties;Compute Curvature", m_computeCurvatureAction);

	connect(m_computeNormalAction, SIGNAL(triggered()), this, SLOT(openComputeNormalDialog()));
	connect(m_computeCurvatureAction, SIGNAL(triggered()), this, SLOT(openComputeCurvatureDialog()));

	connect(m_computeNormalDialog, SIGNAL(accepted()), this, SLOT(computeNormalFromDialog()));
	connect(m_computeNormalDialog->button_apply, SIGNAL(clicked()), this, SLOT(computeNormalFromDialog()));

	connect(m_computeCurvatureDialog, SIGNAL(accepted()), this, SLOT(computeCurvatureFromDialog()));
	connect(m_computeCurvatureDialog->button_apply, SIGNAL(clicked()), this, SLOT(computeCurvatureFromDialog()));

	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	connect(m_schnapps, SIGNAL(schnappsClosing()), this, SLOT(schnappsClosing()));


	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapAdded(map);

	return true;
}

void Surface_DifferentialProperties_Plugin::disable()
{
	disconnect(m_computeNormalAction, SIGNAL(triggered()), this, SLOT(openComputeNormalDialog()));
	disconnect(m_computeCurvatureAction, SIGNAL(triggered()), this, SLOT(openComputeCurvatureDialog()));

	disconnect(m_computeNormalDialog, SIGNAL(accepted()), this, SLOT(computeNormalFromDialog()));
	disconnect(m_computeNormalDialog->button_apply, SIGNAL(clicked()), this, SLOT(computeNormalFromDialog()));

	disconnect(m_computeCurvatureDialog, SIGNAL(accepted()), this, SLOT(computeCurvatureFromDialog()));
	disconnect(m_computeCurvatureDialog->button_apply, SIGNAL(clicked()), this, SLOT(computeCurvatureFromDialog()));

	disconnect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	disconnect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

	disconnect(m_schnapps, SIGNAL(schnappsClosing()), this, SLOT(schnappsClosing()));
}

void Surface_DifferentialProperties_Plugin::mapAdded(MapHandlerGen *map)
{
	connect(map, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
}

void Surface_DifferentialProperties_Plugin::mapRemoved(MapHandlerGen *map)
{
	disconnect(map, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
}

void Surface_DifferentialProperties_Plugin::attributeModified(unsigned int orbit, QString nameAttr)
{
	if(orbit == VERTEX)
	{
		MapHandlerGen* map = static_cast<MapHandlerGen*>(QObject::sender());
		if(computeNormalLastParameters.contains(map->getName()))
		{
			ComputeNormalParameters& params = computeNormalLastParameters[map->getName()];
			if(params.autoUpdate && params.positionName == nameAttr)
				computeNormal(map->getName(), params.positionName, params.normalName, true);
		}
		if(computeCurvatureLastParameters.contains(map->getName()))
		{
			ComputeCurvatureParameters& params = computeCurvatureLastParameters[map->getName()];
			if(params.autoUpdate && (params.positionName == nameAttr || params.normalName == nameAttr))
				computeCurvature(
					map->getName(),
					params.positionName, params.normalName,
					params.KmaxName, params.kmaxName, params.KminName, params.kminName, params.KnormalName,
					true
				);
		}
	}
}

void Surface_DifferentialProperties_Plugin::openComputeNormalDialog()
{
	m_computeNormalDialog->show();
}

void Surface_DifferentialProperties_Plugin::openComputeCurvatureDialog()
{
	m_computeCurvatureDialog->show();
}

void Surface_DifferentialProperties_Plugin::computeNormalFromDialog()
{
	QList<QListWidgetItem*> currentItems = m_computeNormalDialog->list_maps->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapName = currentItems[0]->text();

		QString positionName = m_computeNormalDialog->combo_positionAttribute->currentText();

		QString normalName;
		if(m_computeNormalDialog->normalAttributeName->text().isEmpty())
			normalName = m_computeNormalDialog->combo_normalAttribute->currentText();
		else
			normalName = m_computeNormalDialog->normalAttributeName->text();

		bool autoUpdate = (currentItems[0]->checkState() == Qt::Checked);

		computeNormal(mapName, positionName, normalName, autoUpdate);

		// create VBO if asked
		if (m_computeNormalDialog->enableVBO->isChecked())
		{
			MapHandlerGen* mhg = getSCHNApps()->getMap(mapName);
			if (mhg != NULL)
				mhg->createVBO(normalName);
		}
	}
}

void Surface_DifferentialProperties_Plugin::computeCurvatureFromDialog()
{
	QList<QListWidgetItem*> currentItems = m_computeCurvatureDialog->list_maps->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapName = currentItems[0]->text();

		QString positionName = m_computeCurvatureDialog->combo_positionAttribute->currentText();
		QString normalName = m_computeCurvatureDialog->combo_normalAttribute->currentText();

		QString KmaxName;
		if(m_computeCurvatureDialog->KmaxAttributeName->text().isEmpty())
			KmaxName = m_computeCurvatureDialog->combo_KmaxAttribute->currentText();
		else
			KmaxName = m_computeCurvatureDialog->KmaxAttributeName->text();

		QString kmaxName;
		if(m_computeCurvatureDialog->kmaxAttributeName->text().isEmpty())
			kmaxName = m_computeCurvatureDialog->combo_kmaxAttribute->currentText();
		else
			kmaxName = m_computeCurvatureDialog->kmaxAttributeName->text();

		QString KminName;
		if(m_computeCurvatureDialog->KminAttributeName->text().isEmpty())
			KminName = m_computeCurvatureDialog->combo_KminAttribute->currentText();
		else
			KminName = m_computeCurvatureDialog->KminAttributeName->text();

		QString kminName;
		if(m_computeCurvatureDialog->kminAttributeName->text().isEmpty())
			kminName = m_computeCurvatureDialog->combo_kminAttribute->currentText();
		else
			kminName = m_computeCurvatureDialog->kminAttributeName->text();

		QString KnormalName;
		if(m_computeCurvatureDialog->KnormalAttributeName->text().isEmpty())
			KnormalName = m_computeCurvatureDialog->combo_KnormalAttribute->currentText();
		else
			KnormalName = m_computeCurvatureDialog->KnormalAttributeName->text();

		bool compute_kmean = (m_computeCurvatureDialog->check_computeKmean->checkState() == Qt::Checked);
		bool compute_kgaussian = (m_computeCurvatureDialog->check_computeKgaussian->checkState() == Qt::Checked);
		bool autoUpdate = (currentItems[0]->checkState() == Qt::Checked);

		computeCurvature(
			mapName,
			positionName, normalName,
			KmaxName, kmaxName, KminName, kminName, KnormalName,
			compute_kmean, compute_kgaussian,
			autoUpdate
		);
	}
}

void Surface_DifferentialProperties_Plugin::computeNormal(
	const QString& mapName,
	const QString& positionAttributeName,
	const QString& normalAttributeName,
	bool autoUpdate)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));
	if(mh == NULL)
		return;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName);
	if(!position.isValid())
		return;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> normal = mh->getAttribute<PFP2::VEC3, VERTEX>(normalAttributeName);
	if(!normal.isValid())
		normal = mh->addAttribute<PFP2::VEC3, VERTEX>(normalAttributeName);

	PFP2::MAP* map = mh->getMap();
	Algo::Surface::Geometry::computeNormalVertices<PFP2>(*map, position, normal);

	computeNormalLastParameters[mapName] =
		ComputeNormalParameters(positionAttributeName, normalAttributeName, autoUpdate);

	mh->notifyAttributeModification(normal);

	//this->pythonRecording("computeNormal", "", mh->getName(), QString(position.name().c_str()), QString(normal.name().c_str()), autoUpdate);
	this->pythonRecording("computeNormal", "", mh->getName(), positionAttributeName, normalAttributeName, autoUpdate);

}


void Surface_DifferentialProperties_Plugin::computeCurvature(
	const QString& mapName,
	const QString& positionAttributeName,
	const QString& normalAttributeName,
	const QString& KmaxAttributeName,
	const QString& kmaxAttributeName,
	const QString& KminAttributeName,
	const QString& kminAttributeName,
	const QString& KnormalAttributeName,
	bool compute_kmean,
	bool compute_kgaussian,
	bool autoUpdate)
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

	VertexAttribute<PFP2::VEC3, PFP2::MAP> Kmax = mh->getAttribute<PFP2::VEC3, VERTEX>(KmaxAttributeName);
	if(!Kmax.isValid())
		Kmax = mh->addAttribute<PFP2::VEC3, VERTEX>(KmaxAttributeName);

	VertexAttribute<PFP2::REAL, PFP2::MAP> kmax = mh->getAttribute<PFP2::REAL, VERTEX>(kmaxAttributeName);
	if(!kmax.isValid())
		kmax = mh->addAttribute<PFP2::REAL, VERTEX>(kmaxAttributeName);

	VertexAttribute<PFP2::VEC3, PFP2::MAP> Kmin = mh->getAttribute<PFP2::VEC3, VERTEX>(KminAttributeName);
	if(!Kmin.isValid())
		Kmin = mh->addAttribute<PFP2::VEC3, VERTEX>(KminAttributeName);

	VertexAttribute<PFP2::REAL, PFP2::MAP> kmin = mh->getAttribute<PFP2::REAL, VERTEX>(kminAttributeName);
	if(!kmin.isValid())
		kmin = mh->addAttribute<PFP2::REAL, VERTEX>(kminAttributeName);

	VertexAttribute<PFP2::VEC3, PFP2::MAP> Knormal = mh->getAttribute<PFP2::VEC3, VERTEX>(KnormalAttributeName);
	if(!Knormal.isValid())
		Knormal = mh->addAttribute<PFP2::VEC3, VERTEX>(KnormalAttributeName);

	EdgeAttribute<PFP2::REAL, PFP2::MAP> edgeAngle = mh->getAttribute<PFP2::REAL, EDGE>("edgeAngle");
	if(!edgeAngle.isValid())
		edgeAngle = mh->addAttribute<PFP2::REAL, EDGE>("edgeAngle");

	EdgeAttribute<PFP2::REAL, PFP2::MAP> edgeArea = mh->getAttribute<PFP2::REAL, EDGE>("edgeArea");
	if(!edgeArea.isValid())
		edgeArea = mh->addAttribute<PFP2::REAL, EDGE>("edgeArea");

	PFP2::MAP* map = mh->getMap();

	Algo::Surface::Geometry::computeAnglesBetweenNormalsOnEdges<PFP2>(*map, position, edgeAngle);
	Algo::Surface::Geometry::computeAreaEdges<PFP2>(*map, position, edgeArea);

	PFP2::REAL meanEdgeLength = Algo::Geometry::meanEdgeLength<PFP2>(*map, position);

	float radius = 2.0f * meanEdgeLength;
	Algo::Surface::Geometry::computeCurvatureVertices_NormalCycles_Projected<PFP2>(*map, radius, position, normal, edgeAngle, edgeArea, kmax, kmin, Kmax, Kmin, Knormal);

	computeCurvatureLastParameters[mapName] =
		ComputeCurvatureParameters(
			positionAttributeName, normalAttributeName,
			KmaxAttributeName, kmaxAttributeName, KminAttributeName, kminAttributeName, KnormalAttributeName,
			compute_kmean, compute_kgaussian, autoUpdate);

	mh->notifyAttributeModification(Kmax);
	mh->notifyAttributeModification(kmax);
	mh->notifyAttributeModification(Kmin);
	mh->notifyAttributeModification(kmin);
	mh->notifyAttributeModification(Knormal);

	if(compute_kmean)
	{
		VertexAttribute<PFP2::REAL, PFP2::MAP> kmean = mh->getAttribute<PFP2::REAL, VERTEX>("kmean");
		if(!kmean.isValid())
			kmean = mh->addAttribute<PFP2::REAL, VERTEX>("kmean");

		for(unsigned int i = kmin.begin(); i != kmin.end(); kmin.next(i))
			kmean[i] = (kmin[i] + kmax[i]) / 2.0;

		mh->notifyAttributeModification(kmean);
	}

	if(compute_kgaussian)
	{
		VertexAttribute<PFP2::REAL, PFP2::MAP> kgaussian = mh->getAttribute<PFP2::REAL, VERTEX>("kgaussian");
		if(!kgaussian.isValid())
			kgaussian = mh->addAttribute<PFP2::REAL, VERTEX>("kgaussian");

		for(unsigned int i = kmin.begin(); i != kmin.end(); kmin.next(i))
			kgaussian[i] = kmin[i] * kmax[i];

		mh->notifyAttributeModification(kgaussian);
	}

	this->pythonRecording("computeCurvature", "", mh->getName(),
		positionAttributeName, normalAttributeName, KmaxAttributeName, kmaxAttributeName, KminAttributeName, kminAttributeName, KnormalAttributeName, compute_kmean, compute_kgaussian, autoUpdate);
}

void Surface_DifferentialProperties_Plugin::schnappsClosing()
{
	m_computeNormalDialog->close();
	m_computeCurvatureDialog->close();
}

#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#else
	Q_EXPORT_PLUGIN2(Surface_DifferentialProperties_Plugin, Surface_DifferentialProperties_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
