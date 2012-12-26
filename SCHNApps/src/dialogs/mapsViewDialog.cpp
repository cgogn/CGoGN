#include "dialogs/mapsViewDialog.h"

#include "window.h"
#include "view.h"
#include "plugin.h"
#include "mapHandler.h"

#include <QMessageBox>
#include <QMouseEvent>

namespace CGoGN
{

namespace SCHNApps
{

MapsViewDialog::MapsViewDialog(Window* window, View* view) :
	QDialog(view),
	m_window(window),
	m_view(view)
{
	this->setupUi(this);
	this->setWindowTitle(m_view->getName() + QString(" : maps"));

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapsChanged()));

	connect(m_window, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(cb_addMapToList(MapHandlerGen*)));
	connect(m_window, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(cb_removeMapFromList(MapHandlerGen*)));

	QList<MapHandlerGen*> maps = m_window->getMapsList();
	foreach(MapHandlerGen* m, maps)
		mapList->addItem(m->getName());
}

MapsViewDialog::~MapsViewDialog()
{}

void MapsViewDialog::cb_selectedMapsChanged()
{
	for(int i = 0; i < mapList->count(); ++i)
	{
		QString mapName = mapList->item(i)->text();
		MapHandlerGen* map = m_window->getMap(mapName);
		if(mapList->item(i)->isSelected() && !m_view->isLinkedToMap(map))
		{
			assert(!map->isLinkedToView(m_view));
			m_view->linkMap(map);
			map->linkView(m_view);
		}
		else if(!mapList->item(i)->isSelected() && m_view->isLinkedToMap(map))
		{
			assert(map->isLinkedToView(m_view));
			m_view->unlinkMap(map);
			map->unlinkView(m_view);
		}
	}
	m_view->updateGL();
}

void MapsViewDialog::cb_addMapToList(MapHandlerGen* m)
{
	mapList->addItem(m->getName());
}

void MapsViewDialog::cb_removeMapFromList(MapHandlerGen* m)
{
	for(int i = 0; i < mapList->count(); ++i)
	{
		if(mapList->item(i)->text() == m->getName())
		{
			delete mapList->item(i);
			return;
		}
	}
}

} // namespace SCHNApps

} // namespace CGoGN
