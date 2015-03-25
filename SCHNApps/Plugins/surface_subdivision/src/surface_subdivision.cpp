#include "surface_subdivision.h"

#include "mapHandler.h"

#include "Algo/Modelisation/subdivision.h"

namespace CGoGN
{

namespace SCHNApps
{

bool Surface_Subdivision_Plugin::enable()
{
	//	magic line that init static variables of GenericMap in the plugins
		GenericMap::copyAllStatics(m_schnapps->getStaticPointers());

	m_subdivisionDialog = new Dialog_Surface_Subdivision(m_schnapps);

	m_subdivisionAction = new QAction("Subdivide surface", this);

	m_schnapps->addMenuAction(this, "Surface;Subdivision", m_subdivisionAction);

	connect(m_subdivisionAction, SIGNAL(triggered()), this, SLOT(openSubdivisionDialog()));

	connect(m_subdivisionDialog, SIGNAL(accepted()), this, SLOT(subdivideFromDialog()));
	connect(m_subdivisionDialog->button_apply, SIGNAL(clicked()), this, SLOT(subdivideFromDialog()));

	connect(m_schnapps, SIGNAL(schnappsClosing()), this, SLOT(schnappsClosing()));

	return true;
}

void Surface_Subdivision_Plugin::disable()
{
	disconnect(m_subdivisionAction, SIGNAL(triggered()), this, SLOT(openSubdivisionDialog()));

	disconnect(m_subdivisionDialog, SIGNAL(accepted()), this, SLOT(subdivideFromDialog()));
	disconnect(m_subdivisionDialog->button_apply, SIGNAL(clicked()), this, SLOT(subdivideFromDialog()));

	disconnect(m_schnapps, SIGNAL(schnappsClosing()), this, SLOT(schnappsClosing()));
}

void Surface_Subdivision_Plugin::openSubdivisionDialog()
{
	m_subdivisionDialog->show();
}

void Surface_Subdivision_Plugin::subdivideFromDialog()
{
	QList<QListWidgetItem*> currentItems = m_subdivisionDialog->list_maps->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapName = currentItems[0]->text();
		const QString& positionName = m_subdivisionDialog->combo_positionAttribute->currentText();

		if(m_subdivisionDialog->radio_Loop->isChecked())
			loopSubdivision(mapName, positionName);
		else if(m_subdivisionDialog->radio_CC->isChecked())
			CCSubdivision(mapName, positionName);
		else if(m_subdivisionDialog->radio_trianguleFaces->isChecked())
			trianguleFaces(mapName, positionName);
	}
}

void Surface_Subdivision_Plugin::loopSubdivision(
	const QString& mapName,
	const QString& positionAttributeName)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));
	if(mh == NULL)
		return;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName);
	if(!position.isValid())
		return;

	pythonRecording("loopSubdivision", "", mapName, positionAttributeName);

	PFP2::MAP* map = mh->getMap();
	Algo::Surface::Modelisation::LoopSubdivision<PFP2>(*map, position);

	mh->notifyAttributeModification(position);
	mh->notifyConnectivityModification();

	foreach(View* view, mh->getLinkedViews())
		view->updateGL();
}

void Surface_Subdivision_Plugin::CCSubdivision(
	const QString& mapName,
	const QString& positionAttributeName)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));
	if(mh == NULL)
		return;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName);
	if(!position.isValid())
		return;

	pythonRecording("CCSubdivision", "", mapName, positionAttributeName);

	PFP2::MAP* map = mh->getMap();
	Algo::Surface::Modelisation::CatmullClarkSubdivision<PFP2>(*map, position);

	mh->notifyAttributeModification(position);
	mh->notifyConnectivityModification();

	foreach(View* view, mh->getLinkedViews())
		view->updateGL();
}

void Surface_Subdivision_Plugin::trianguleFaces(
	const QString& mapName,
	const QString& positionAttributeName)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));
	if(mh == NULL)
		return;

	VertexAttribute<PFP2::VEC3, PFP2::MAP> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName);
	if(!position.isValid())
		return;

	pythonRecording("trianguleFaces", "", mapName, positionAttributeName);

	PFP2::MAP* map = mh->getMap();
	Algo::Surface::Modelisation::trianguleFaces<PFP2>(*map, position);

	mh->notifyAttributeModification(position);
	mh->notifyConnectivityModification();

	foreach(View* view, mh->getLinkedViews())
		view->updateGL();
}

void Surface_Subdivision_Plugin::schnappsClosing()
{
	m_subdivisionDialog->close();
}

#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#else
	Q_EXPORT_PLUGIN2(Surface_Subdivision_Plugin, Surface_Subdivision_Plugin)
#endif


} // namespace SCHNApps

} // namespace CGoGN
