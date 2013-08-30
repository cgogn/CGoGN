#include "subdivideSurface.h"

#include "mapHandler.h"

#include "Algo/Modelisation/subdivision.h"

namespace CGoGN
{

namespace SCHNApps
{

bool SubdivideSurfacePlugin::enable()
{
	m_subdivideSurfaceDialog = new SubdivideSurfaceDialog(m_window);

	m_subdivideSurfaceAction = new QAction("Subdivide surface", this);

	addMenuAction("Surface;Subdivide", m_subdivideSurfaceAction);

	connect(m_subdivideSurfaceAction, SIGNAL(triggered()), this, SLOT(openSubdivideSurfaceDialog()));

	connect(m_subdivideSurfaceDialog, SIGNAL(accepted()), this, SLOT(subdivideSurfaceFromDialog()));
	connect(m_subdivideSurfaceDialog->button_apply, SIGNAL(clicked()), this, SLOT(subdivideSurfaceFromDialog()));

	return true;
}

void SubdivideSurfacePlugin::openSubdivideSurfaceDialog()
{
	m_subdivideSurfaceDialog->show();
}

void SubdivideSurfacePlugin::subdivideSurfaceFromDialog()
{
	QList<QListWidgetItem*> currentItems = m_subdivideSurfaceDialog->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapName = currentItems[0]->text();
		const QString& positionName = m_subdivideSurfaceDialog->combo_positionAttribute->currentText();

		if(m_subdivideSurfaceDialog->radio_Loop->isChecked())
			loopSubdivision(mapName, positionName);
		else if(m_subdivideSurfaceDialog->radio_CC->isChecked())
			CCSubdivision(mapName, positionName);
		else if(m_subdivideSurfaceDialog->radio_trianguleFaces->isChecked())
			trianguleFaces(mapName, positionName);
	}
}

void SubdivideSurfacePlugin::loopSubdivision(
	const QString& mapName,
	const QString& positionAttributeName)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_window->getMap(mapName));
	if(mh == NULL)
		return;

	VertexAttribute<PFP2::VEC3> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName);
	if(!position.isValid())
		return;

	PFP2::MAP* map = mh->getMap();
	Algo::Surface::Modelisation::LoopSubdivision<PFP2>(*map, position);

	mh->notifyAttributeModification(position);
	mh->notifyConnectivityModification();

	QList<View*> views = mh->getLinkedViews();
	foreach(View* view, views)
		view->updateGL();
}

void SubdivideSurfacePlugin::CCSubdivision(
	const QString& mapName,
	const QString& positionAttributeName)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_window->getMap(mapName));
	if(mh == NULL)
		return;

	VertexAttribute<PFP2::VEC3> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName);
	if(!position.isValid())
		return;

	PFP2::MAP* map = mh->getMap();
	Algo::Surface::Modelisation::CatmullClarkSubdivision<PFP2>(*map, position);

	mh->notifyAttributeModification(position);
	mh->notifyConnectivityModification();

	QList<View*> views = mh->getLinkedViews();
	foreach(View* view, views)
		view->updateGL();
}

void SubdivideSurfacePlugin::trianguleFaces(
	const QString& mapName,
	const QString& positionAttributeName)
{
	MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_window->getMap(mapName));
	if(mh == NULL)
		return;

	VertexAttribute<PFP2::VEC3> position = mh->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName);
	if(!position.isValid())
		return;

	PFP2::MAP* map = mh->getMap();
	Algo::Surface::Modelisation::trianguleFaces<PFP2>(*map, position);

	mh->notifyAttributeModification(position);
	mh->notifyConnectivityModification();

	QList<View*> views = mh->getLinkedViews();
	foreach(View* view, views)
		view->updateGL();
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(SubdivideSurfacePlugin, SubdivideSurfacePlugin)
#else
Q_EXPORT_PLUGIN2(SubdivideSurfacePluginD, SubdivideSurfacePlugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
