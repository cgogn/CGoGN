#include "surface_distance.h"

#include "mapHandler.h"

#include "Algo/Geometry/distances.h"

namespace CGoGN
{

namespace SCHNApps
{

bool Surface_Distance_Plugin::enable()
{
	//	magic line that init static variables of GenericMap in the plugins
	GenericMap::copyAllStatics(m_schnapps->getStaticPointers());

	m_computeDistanceDialog = new Dialog_ComputeDistance(m_schnapps);

	m_computeDistanceAction = new QAction("Compute Distance", this);

	m_schnapps->addMenuAction(this, "Surface;Compute Distance", m_computeDistanceAction);

	connect(m_computeDistanceAction, SIGNAL(triggered()), this, SLOT(openComputeDistanceDialog()));

	connect(m_computeDistanceDialog, SIGNAL(accepted()), this, SLOT(computeDistanceFromDialog()));
	connect(m_computeDistanceDialog->button_apply, SIGNAL(clicked()), this, SLOT(computeDistanceFromDialog()));

	connect(m_schnapps, SIGNAL(appsFinished()), this, SLOT(appsFinished()));

	return true;
}

void Surface_Distance_Plugin::disable()
{
	disconnect(m_computeDistanceAction, SIGNAL(triggered()), this, SLOT(openComputeNormalDialog()));

	disconnect(m_computeDistanceDialog, SIGNAL(accepted()), this, SLOT(computeDistanceFromDialog()));
	disconnect(m_computeDistanceDialog->button_apply, SIGNAL(clicked()), this, SLOT(computeDistanceFromDialog()));

	disconnect(m_schnapps, SIGNAL(appsFinished()), this, SLOT(appsFinished()));
}

void Surface_Distance_Plugin::openComputeDistanceDialog()
{
	m_computeDistanceDialog->show();
}

void Surface_Distance_Plugin::computeDistanceFromDialog()
{
	QList<QListWidgetItem*> currentItems1 = m_computeDistanceDialog->list_maps_1->selectedItems();
	QList<QListWidgetItem*> currentItems2 = m_computeDistanceDialog->list_maps_2->selectedItems();

	if(!currentItems1.empty() && !currentItems2.empty())
	{
		const QString& mapName1 = currentItems1[0]->text();
		const QString& mapName2 = currentItems2[0]->text();

		QString positionName1 = m_computeDistanceDialog->combo_positionAttribute_1->currentText();
		QString positionName2 = m_computeDistanceDialog->combo_positionAttribute_2->currentText();

		QString distanceName1;
		if(m_computeDistanceDialog->distanceAttributeName_1->text().isEmpty())
			distanceName1 = m_computeDistanceDialog->combo_distanceAttribute_1->currentText();
		else
			distanceName1 = m_computeDistanceDialog->distanceAttributeName_1->text();

		QString distanceName2;
		if(m_computeDistanceDialog->distanceAttributeName_2->text().isEmpty())
			distanceName2 = m_computeDistanceDialog->combo_distanceAttribute_2->currentText();
		else
			distanceName2 = m_computeDistanceDialog->distanceAttributeName_2->text();

		// create VBO if asked
		if (m_computeDistanceDialog->enableVBO->isChecked())
		{
			MapHandlerGen* mhg1 = getSCHNApps()->getMap(mapName1);
			if (mhg1)
				mhg1->createVBO(distanceName1);

			MapHandlerGen* mhg2 = getSCHNApps()->getMap(mapName2);
			if (mhg2)
				mhg2->createVBO(distanceName2);
		}

		computeDistance(mapName1, positionName1, distanceName1, mapName2, positionName2, distanceName2);
	}
}

void Surface_Distance_Plugin::computeDistance(
	const QString& mapName1,
	const QString& positionAttributeName1,
	const QString& distanceAttributeName1,
	const QString& mapName2,
	const QString& positionAttributeName2,
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

	VertexAttribute<PFP2::VEC3, PFP2::MAP> position2 = mh2->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName2);
	if(!position2.isValid())
		return;

	VertexAttribute<PFP2::REAL, PFP2::MAP> distance1 = mh1->getAttribute<PFP2::REAL, VERTEX>(distanceAttributeName1);
	if(!distance1.isValid())
		distance1 = mh1->addAttribute<PFP2::REAL, VERTEX>(distanceAttributeName1);

	VertexAttribute<PFP2::REAL, PFP2::MAP> distance2 = mh2->getAttribute<PFP2::REAL, VERTEX>(distanceAttributeName2);
	if(!distance2.isValid())
		distance2 = mh2->addAttribute<PFP2::REAL, VERTEX>(distanceAttributeName2);

	PFP2::MAP* map1 = mh1->getMap();
	PFP2::MAP* map2 = mh2->getMap();

	// compute distance between map1 and map2 here
	//   distance from map1 to map2 can be stored in map1 vertex attribute distance1
	//   distance from map2 to map1 can be stored in map2 vertex attribute distance2

	// Algo::Geometry::computeDistance<PFP2>(map1, position1, distance1, map2, position2);
	// Algo::Geometry::computeDistance<PFP2>(map2, position2, distance2, map1, position1);

	mh1->notifyAttributeModification(distance1);
	mh2->notifyAttributeModification(distance2);
}

void Surface_Distance_Plugin::appsFinished()
{
	m_computeDistanceDialog->close();
}

Q_EXPORT_PLUGIN2(Surface_Distance_Plugin, Surface_Distance_Plugin)

} // namespace SCHNApps

} // namespace CGoGN
