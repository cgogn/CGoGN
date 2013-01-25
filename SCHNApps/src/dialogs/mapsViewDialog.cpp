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

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapsChanged()));

	connect(m_window, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(addMapToList(MapHandlerGen*)));
	connect(m_window, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(removeMapFromList(MapHandlerGen*)));

	QList<MapHandlerGen*> maps = m_window->getMapsList();
	foreach(MapHandlerGen* m, maps)
		mapList->addItem(m->getName());
}

MapsViewDialog::~MapsViewDialog()
{}

void MapsViewDialog::selectedMapsChanged()
{
	for(int i = 0; i < mapList->count(); ++i)
	{
		QString mapName = mapList->item(i)->text();
		MapHandlerGen* map = m_window->getMap(mapName);

		if(mapList->item(i)->isSelected() && !m_view->isLinkedToMap(map))
			m_window->linkViewAndMap(m_view, map);

		else if(!mapList->item(i)->isSelected() && m_view->isLinkedToMap(map))
			m_window->unlinkViewAndMap(m_view, map);
	}
	m_view->updateGL();
}

void MapsViewDialog::addMapToList(MapHandlerGen* m)
{
	mapList->addItem(m->getName());
}

void MapsViewDialog::removeMapFromList(MapHandlerGen* m)
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
