#include "subdivideSurface.h"

#include "mapHandler.h"

#include "Algo/Modelisation/subdivision.h"

bool SubdivideSurfacePlugin::enable()
{
	m_subdivideSurfaceDialog = new SubdivideSurfaceDialog(m_window);

	m_subdivideSurfaceAction = new QAction("Subdivide surface", this);

	addMenuAction("Surface;Subdivide", m_subdivideSurfaceAction);

	connect(m_subdivideSurfaceAction, SIGNAL(triggered()), this, SLOT(openSubdivideSurfaceDialog()));

	connect(m_subdivideSurfaceDialog, SIGNAL(accepted()), this, SLOT(subdivideSurface()));
	connect(m_subdivideSurfaceDialog->button_apply, SIGNAL(clicked()), this, SLOT(subdivideSurface()));

	return true;
}

void SubdivideSurfacePlugin::openSubdivideSurfaceDialog()
{
	m_subdivideSurfaceDialog->show();
}

void SubdivideSurfacePlugin::subdivideSurface()
{
	QList<QListWidgetItem*> currentItems = m_subdivideSurfaceDialog->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_window->getMap(mapname));
		PFP2::MAP* map = mh->getMap();

		std::string positionName = m_subdivideSurfaceDialog->combo_positionAttribute->currentText().toUtf8().constData();
		VertexAttribute<PFP2::VEC3> position = map->getAttribute<PFP2::VEC3, VERTEX>(positionName);

		if(m_subdivideSurfaceDialog->radio_Loop->isChecked())
			loopSubdivision(map, position);
		else if(m_subdivideSurfaceDialog->radio_CC->isChecked())
			CCSubdivision(map, position);
		else if(m_subdivideSurfaceDialog->radio_trianguleFaces->isChecked())
			trianguleFaces(map, position);

		mh->updatePrimitives(Algo::Render::GL2::POINTS);
		mh->updatePrimitives(Algo::Render::GL2::LINES);
		mh->updatePrimitives(Algo::Render::GL2::TRIANGLES);
		mh->updateVBO(position);

		QList<View*> views = mh->getLinkedViews();
		foreach(View* view, views)
			view->updateGL();
	}
}

void SubdivideSurfacePlugin::loopSubdivision(PFP2::MAP* map, VertexAttribute<PFP2::VEC3>& position)
{
	Algo::Surface::Modelisation::LoopSubdivision<PFP2>(*map, position);
}

void SubdivideSurfacePlugin::CCSubdivision(PFP2::MAP* map, VertexAttribute<PFP2::VEC3>& position)
{
	Algo::Surface::Modelisation::CatmullClarkSubdivision<PFP2>(*map, position);
}

void SubdivideSurfacePlugin::trianguleFaces(PFP2::MAP* map, VertexAttribute<PFP2::VEC3>& position)
{
	Algo::Surface::Modelisation::trianguleFaces<PFP2>(*map, position);
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(SubdivideSurfacePlugin, SubdivideSurfacePlugin)
#else
Q_EXPORT_PLUGIN2(SubdivideSurfacePluginD, SubdivideSurfacePlugin)
#endif
