#include "subdivideSurfaceDialog.h"

#include "subdivideSurface.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

SubdivideSurfaceDialog::SubdivideSurfaceDialog(Window* w) :
	m_window(w),
	m_selectedMap(NULL)
{
	setupUi(this);

	connect(m_window, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(addMapToList(MapHandlerGen*)));
	connect(m_window, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(removeMapFromList(MapHandlerGen*)));

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));

	const QList<MapHandlerGen*>& maps = m_window->getMapsList();
	foreach(MapHandlerGen* map, maps)
		mapList->addItem(map->getName());
}

void SubdivideSurfaceDialog::refreshUI()
{
	if(m_selectedMap)
		disconnect(m_selectedMap, SIGNAL(attributeAdded()), this, SLOT(refreshUI()));

	QList<QListWidgetItem*> currentItems = mapList->selectedItems();
	if(!currentItems.empty())
	{
		combo_positionAttribute->clear();
		const QString& mapname = currentItems[0]->text();
		MapHandlerGen* mh = m_window->getMap(mapname);
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
				if(names[i] == "position") // try to select a position attribute named "position"
					combo_positionAttribute->setCurrentIndex(j);

				++j;
			}
		}

		m_selectedMap = mh;
		connect(m_selectedMap, SIGNAL(attributeAdded()), this, SLOT(refreshUI()));
	}
	else
		m_selectedMap = NULL;
}

void SubdivideSurfaceDialog::addMapToList(MapHandlerGen* m)
{
	mapList->addItem(m->getName());
}

void SubdivideSurfaceDialog::removeMapFromList(MapHandlerGen* m)
{
	QList<QListWidgetItem*> items = mapList->findItems(m->getName(), Qt::MatchExactly);
	if(!items.empty())
		delete items[0];

	if(m_selectedMap == m)
	{
		disconnect(m_selectedMap, SIGNAL(attributeAdded()), this, SLOT(refreshUI()));
		m_selectedMap = NULL;
	}
}

} // namespace SCHNApps

} // namespace CGoGN
