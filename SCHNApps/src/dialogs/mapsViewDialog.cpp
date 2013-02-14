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
	m_view(view),
	b_refreshingUI(false)
{
	this->setupUi(this);
	this->setWindowTitle(m_view->getName() + QString(" : maps"));

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapsChanged()));

	connect(m_window, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(addMapToList(MapHandlerGen*)));
	connect(m_window, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(removeMapFromList(MapHandlerGen*)));

	connect(m_window, SIGNAL(viewAndMapLinked(View*, MapHandlerGen*)), this, SLOT(selectMap(View*, MapHandlerGen*)));
	connect(m_window, SIGNAL(viewAndMapUnlinked(View*, MapHandlerGen*)), this, SLOT(deselectMap(View*, MapHandlerGen*)));

	QList<MapHandlerGen*> maps = m_window->getMapsList();
	foreach(MapHandlerGen* m, maps)
		mapList->addItem(m->getName());
}

MapsViewDialog::~MapsViewDialog()
{}

void MapsViewDialog::selectedMapsChanged()
{
	if(!b_refreshingUI)
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
	}
}

void MapsViewDialog::selectMap(View* view, MapHandlerGen* plugin)
{
	if(view == m_view)
	{
		QList<QListWidgetItem*> items = mapList->findItems(plugin->getName(), Qt::MatchExactly);
		if(!items.empty())
		{
			b_refreshingUI = true;
			items[0]->setSelected(true);
			b_refreshingUI = false;
		}
	}
}

void MapsViewDialog::deselectMap(View* view, MapHandlerGen* plugin)
{
	if(view == m_view)
	{
		QList<QListWidgetItem*> items = mapList->findItems(plugin->getName(), Qt::MatchExactly);
		if(!items.empty())
		{
			b_refreshingUI = true;
			items[0]->setSelected(false);
			b_refreshingUI = false;
		}
	}
}

void MapsViewDialog::addMapToList(MapHandlerGen* m)
{
	mapList->addItem(m->getName());
}

void MapsViewDialog::removeMapFromList(MapHandlerGen* m)
{
	QList<QListWidgetItem*> items = mapList->findItems(m->getName(), Qt::MatchExactly);
	if(!items.empty())
		delete items[0];
}

} // namespace SCHNApps

} // namespace CGoGN
