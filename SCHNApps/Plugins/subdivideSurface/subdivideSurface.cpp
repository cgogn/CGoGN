#include "subdivideSurface.h"

#include "mapHandler.h"

#include "Algo/Modelisation/subdivision.h"

bool SubdivideSurfacePlugin::enable()
{
	m_dockTab = new SubdivideSurfaceDockTab();
	addTabInDock(m_dockTab, "SubdivideSurface");

	connect(m_window, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(cb_addMapToList(MapHandlerGen*)));
	connect(m_window, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(cb_removeMapFromList(MapHandlerGen*)));

	connect(m_dockTab->mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));

	connect(m_dockTab->button_trianguleFaces, SIGNAL(clicked()), this, SLOT(cb_trianguleFaces()));
	connect(m_dockTab->button_loopSubdivision, SIGNAL(clicked()), this, SLOT(cb_loopSubdivision()));
	connect(m_dockTab->button_CCSubdivision, SIGNAL(clicked()), this, SLOT(cb_CCSubdivision()));

	QList<MapHandlerGen*> maps = m_window->getMapsList();
	foreach(MapHandlerGen* m, maps)
		m_dockTab->mapList->addItem(m->getName());

	return true;
}

void SubdivideSurfacePlugin::cb_addMapToList(MapHandlerGen* m)
{
	m_dockTab->mapList->addItem(m->getName());
}

void SubdivideSurfacePlugin::cb_removeMapFromList(MapHandlerGen* m)
{
	for(int i = 0; i < m_dockTab->mapList->count(); ++i)
	{
		if(m_dockTab->mapList->item(i)->text() == m->getName())
		{
			delete m_dockTab->mapList->item(i);
			return;
		}
	}
}

void SubdivideSurfacePlugin::cb_selectedMapChanged()
{
	QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
	if(!currentItems.empty())
	{
		m_dockTab->combo_positionAttribute->clear();
		const QString& mapname = currentItems[0]->text();
		MapHandlerGen* mh = m_window->getMap(mapname);
		GenericMap* map = mh->getGenericMap();
		AttributeContainer& cont = map->getAttributeContainer<VERTEX>();
		std::vector<std::string> names;
		std::vector<std::string> types;
		cont.getAttributesNames(names);
		cont.getAttributesTypes(types);
		std::string vec3TypeName = VEC3::CGoGNnameOfType();
		unsigned int j = 0;
		for(unsigned int i = 0; i < names.size(); ++i)
		{
			if(types[i] == vec3TypeName)
			{
				m_dockTab->combo_positionAttribute->addItem(QString::fromStdString(names[i]));
				if(names[i] == "position") // try to select an attribute named "position"
					m_dockTab->combo_positionAttribute->setCurrentIndex(j);
				++j;
			}
		}
	}
}

void SubdivideSurfacePlugin::cb_loopSubdivision()
{
	QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandler<PFP>* mh = reinterpret_cast<MapHandler<PFP>*>(m_window->getMap(mapname));
		MAP* map = mh->getMap();
		std::string positionName = m_dockTab->combo_positionAttribute->currentText().toUtf8().constData();
		VertexAttribute<VEC3> position = map->getAttribute<VEC3, VERTEX>(positionName);

		Algo::Modelisation::LoopSubdivision<PFP>(*map, position);

		mh->updatePrimitives(Algo::Render::GL2::POINTS);
		mh->updatePrimitives(Algo::Render::GL2::LINES);
		mh->updatePrimitives(Algo::Render::GL2::TRIANGLES);
		mh->updateVBO(position);

		QList<View*> views = m_window->getViewsList();
		foreach(View* view, views)
		{
			if(view->isLinkedToMap(mh))
				view->updateGL();
		}
	}
}

void SubdivideSurfacePlugin::cb_CCSubdivision()
{
	QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandler<PFP>* mh = reinterpret_cast<MapHandler<PFP>*>(m_window->getMap(mapname));
		MAP* map = mh->getMap();
		std::string positionName = m_dockTab->combo_positionAttribute->currentText().toUtf8().constData();
		VertexAttribute<VEC3> position = map->getAttribute<VEC3, VERTEX>(positionName);

		Algo::Modelisation::CatmullClarkSubdivision<PFP>(*map, position);

		mh->updatePrimitives(Algo::Render::GL2::POINTS);
		mh->updatePrimitives(Algo::Render::GL2::LINES);
		mh->updatePrimitives(Algo::Render::GL2::TRIANGLES);
		mh->updateVBO(position);

		QList<View*> views = m_window->getViewsList();
		foreach(View* view, views)
		{
			if(view->isLinkedToMap(mh))
				view->updateGL();
		}
	}
}

void SubdivideSurfacePlugin::cb_trianguleFaces()
{
	QList<QListWidgetItem*> currentItems = m_dockTab->mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapname = currentItems[0]->text();
		MapHandler<PFP>* mh = reinterpret_cast<MapHandler<PFP>*>(m_window->getMap(mapname));
		MAP* map = mh->getMap();
		std::string positionName = m_dockTab->combo_positionAttribute->currentText().toUtf8().constData();
		VertexAttribute<VEC3> position = map->getAttribute<VEC3, VERTEX>(positionName);

		Algo::Modelisation::trianguleFaces<PFP>(*map, position);

		mh->updatePrimitives(Algo::Render::GL2::POINTS);
		mh->updatePrimitives(Algo::Render::GL2::LINES);
		mh->updatePrimitives(Algo::Render::GL2::TRIANGLES);
		mh->updateVBO(position);

		QList<View*> views = m_window->getViewsList();
		foreach(View* view, views)
		{
			if(view->isLinkedToMap(mh))
				view->updateGL();
		}
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(SubdivideSurfacePlugin, SubdivideSurfacePlugin)
#else
Q_EXPORT_PLUGIN2(SubdivideSurfacePluginD, SubdivideSurfacePlugin)
#endif
