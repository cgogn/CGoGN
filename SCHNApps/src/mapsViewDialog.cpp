#include "mapsViewDialog.h"

#include "window.h"
#include "view.h"
#include "plugin.h"

#include <QMessageBox>
#include <QMouseEvent>

MapsViewDialog::MapsViewDialog(Window* window, View* view) :
	QDialog(view),
	m_window(window),
	m_view(view)
{
	this->setupUi(this);
	this->setWindowTitle(m_view->getName() + QString(" : maps"));

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapsChanged()));

	connect(m_window, SIGNAL(mapAdded(MapHandler*)), this, SLOT(cb_addMapToList(MapHandler*)));
	connect(m_window, SIGNAL(mapRemoved(MapHandler*)), this, SLOT(cb_removeMapFromList(MapHandler*)));

	QList<MapHandler*> maps = m_window->getMapsList();
	foreach(MapHandler* m, maps)
		mapList->addItem(m->getName());
}

MapsViewDialog::~MapsViewDialog()
{}

void MapsViewDialog::selectCurrentMaps()
{
	QList<MapHandler*> currentMaps = m_view->getLinkedMaps();
	QList<QString> currentMapsNames;
	foreach(MapHandler* m, currentMaps)
	currentMapsNames.push_back(m->getName());

	for(int i = 0; i < mapList->count(); ++i)
	{
		if(currentMapsNames.contains(mapList->item(i)->text()))
			mapList->item(i)->setSelected(true);
		else
			mapList->item(i)->setSelected(false);
	}
}

void MapsViewDialog::cb_selectedMapsChanged()
{
	for(int i = 0; i < mapList->count(); ++i)
	{
		QString mapName = mapList->item(i)->text();
		MapHandler* map = m_window->getMap(mapName);
		if(mapList->item(i)->isSelected() && !m_view->isLinkedToMap(map))
		{
			m_view->linkMap(map);
			map->linkView(m_view);
		}
		else if(!mapList->item(i)->isSelected() && m_view->isLinkedToMap(map))
		{
			m_view->unlinkMap(map);
			map->unlinkView(m_view);
		}
	}
}

void MapsViewDialog::cb_addMapToList(MapHandler* m)
{
	mapList->addItem(m->getName());
}

void MapsViewDialog::cb_removeMapFromList(MapHandler* m)
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
