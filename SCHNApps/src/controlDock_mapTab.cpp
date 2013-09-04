#include "controlDock_mapTab.h"

#include "schnapps.h"
#include "mapHandler.h"
#include "view.h"

namespace CGoGN
{

namespace SCHNApps
{

ControlDock_MapTab::ControlDock_MapTab(SCHNApps* s) :
	m_schnapps(s),
	m_selectedMap(NULL),
	b_updatingUI(false)
{
	setupUi(this);

	connect(list_maps, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(mapCheckStateChanged(QListWidgetItem*)));
	connect(list_vertexAttributes, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(vertexAttributeCheckStateChanged(QListWidgetItem*)));
	connect(list_maps, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));

	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(selectedViewChanged(View*,View*)), this, SLOT(selectedViewChanged(View*,View*)));
}





void ControlDock_MapTab::selectedMapChanged()
{
	if(!b_updatingUI)
	{
		MapHandlerGen* old = m_selectedMap;

		if(m_selectedMap)
		{
			disconnect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(selectedMapAttributeAdded(unsigned int, const QString&)));
			disconnect(m_selectedMap, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(selectedMapVBOAdded(Utils::VBO*)));
			disconnect(m_selectedMap, SIGNAL(vboRemoved(Utils::VBO*)), this, SLOT(selectedMapVBORemoved(Utils::VBO*)));
		}

		QList<QListWidgetItem*> items = list_maps->selectedItems();
		if(!items.empty())
		{
			QString selectedMapName = items[0]->text();
			m_selectedMap = m_schnapps->getMap(selectedMapName);

			updateSelectedMapInfo();

			m_schnapps->notifySelectedMapChanged(old, m_selectedMap);

			connect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(selectedMapAttributeAdded(unsigned int, const QString&)));
			connect(m_selectedMap, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(selectedMapVBOAdded(Utils::VBO*)));
			connect(m_selectedMap, SIGNAL(vboRemoved(Utils::VBO*)), this, SLOT(selectedMapVBORemoved(Utils::VBO*)));
		}
	}
}

void ControlDock_MapTab::mapCheckStateChanged(QListWidgetItem *item)
{
	if(!b_updatingUI)
	{
		View* selectedView = m_schnapps->getSelectedView();
		MapHandlerGen* m = m_schnapps->getMap(item->text());
		if(m)
		{
			if(item->checkState() == Qt::Checked)
				selectedView->linkMap(m);
			else
				selectedView->unlinkMap(m);
		}
	}
}

void ControlDock_MapTab::vertexAttributeCheckStateChanged(QListWidgetItem *item)
{
	if(!b_updatingUI)
	{
		if(item->checkState() == Qt::Checked)
		{
			Utils::VBO* vbo = m_selectedMap->createVBO(item->text());
			if(!vbo)
			{
				b_updatingUI = true;
				item->setCheckState(Qt::Unchecked);
				b_updatingUI = false;
			}
		}
		else
			m_selectedMap->deleteVBO(item->text());
	}
}





void ControlDock_MapTab::mapAdded(MapHandlerGen* m)
{
	b_updatingUI = true;
	QListWidgetItem* item = new QListWidgetItem(m->getName(), list_maps);
	item->setCheckState(Qt::Unchecked);
	b_updatingUI = false;
}

void ControlDock_MapTab::mapRemoved(MapHandlerGen* m)
{
	b_updatingUI = true;
	QList<QListWidgetItem*> items = list_maps->selectedItems();
	if(!items.empty())
		delete items[0];
	b_updatingUI = false;
}

void ControlDock_MapTab::selectedViewChanged(View* prev, View* cur)
{
	if(prev)
	{
		foreach(MapHandlerGen* map, prev->getLinkedMaps())
		{
			QList<QListWidgetItem*> prevItems = list_maps->findItems(map->getName(), Qt::MatchExactly);
			if(!prevItems.empty())
			{
				b_updatingUI = true;
				prevItems[0]->setCheckState(Qt::Unchecked);
				b_updatingUI = false;
			}
		}

		disconnect(prev, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(selectedViewMapLinked(MapHandlerGen*)));
		disconnect(prev, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(selectedViewMapUnlinked(MapHandlerGen*)));
	}
	if(cur)
	{
		foreach(MapHandlerGen* map, cur->getLinkedMaps())
		{
			QList<QListWidgetItem*> curItems = list_maps->findItems(map->getName(), Qt::MatchExactly);
			if(!curItems.empty())
			{
				b_updatingUI = true;
				curItems[0]->setCheckState(Qt::Checked);
				b_updatingUI = false;
			}
		}

		connect(cur, SIGNAL(mapLinked(MapHandlerGen*)), this, SLOT(selectedViewMapLinked(MapHandlerGen*)));
		connect(cur, SIGNAL(mapUnlinked(MapHandlerGen*)), this, SLOT(selectedViewMapUnlinked(MapHandlerGen*)));
	}
}





void ControlDock_MapTab::selectedViewMapLinked(MapHandlerGen* map)
{
	QList<QListWidgetItem*> curItems = list_maps->findItems(map->getName(), Qt::MatchExactly);
	if(!curItems.empty())
	{
		b_updatingUI = true;
		curItems[0]->setCheckState(Qt::Checked);
		b_updatingUI = false;
	}
}

void ControlDock_MapTab::selectedViewMapUnlinked(MapHandlerGen* map)
{
	QList<QListWidgetItem*> prevItems = list_maps->findItems(map->getName(), Qt::MatchExactly);
	if(!prevItems.empty())
	{
		b_updatingUI = true;
		prevItems[0]->setCheckState(Qt::Unchecked);
		b_updatingUI = false;
	}
}





void ControlDock_MapTab::selectedMapAttributeAdded(unsigned int orbit, const QString &name)
{
	updateSelectedMapInfo();
}

void ControlDock_MapTab::selectedMapVBOAdded(Utils::VBO *vbo)
{
	updateSelectedMapInfo();
}

void ControlDock_MapTab::selectedMapVBORemoved(Utils::VBO *vbo)
{
	updateSelectedMapInfo();
}





void ControlDock_MapTab::updateSelectedMapInfo()
{
	b_updatingUI = true;

	list_dartAttributes->clear();
	list_vertexAttributes->clear();
	list_edgeAttributes->clear();
	list_faceAttributes->clear();
	list_volumeAttributes->clear();

	GenericMap* m = m_selectedMap->getGenericMap();
	for(unsigned int orbit = DART; orbit <= VOLUME; ++orbit)
	{
		unsigned int nbc = m->getNbCells(orbit);
		switch(orbit)
		{
			case DART : {
				unsigned int nb = m->getNbDarts();
				lineEdit_dartNbOrbits->setText(QString::number(nb));
				lineEdit_dartNbCells->setText(QString::number(nbc));
				break;
			}
			case VERTEX : {
				unsigned int nb = m->getNbOrbits<VERTEX>();
				lineEdit_vertexNbOrbits->setText(QString::number(nb));
				lineEdit_vertexNbCells->setText(QString::number(nbc));
				break;
			}
			case EDGE : {
				unsigned int nb = m->getNbOrbits<EDGE>();
				lineEdit_edgeNbOrbits->setText(QString::number(nb));
				lineEdit_edgeNbCells->setText(QString::number(nbc));
				break;
			}
			case FACE : {
				unsigned int nb = m->getNbOrbits<FACE>();
				lineEdit_faceNbOrbits->setText(QString::number(nb));
				lineEdit_faceNbCells->setText(QString::number(nbc));
				break;
			}
			case VOLUME : {
				unsigned int nb = m->getNbOrbits<VOLUME>();
				lineEdit_volumeNbOrbits->setText(QString::number(nb));
				lineEdit_volumeNbCells->setText(QString::number(nbc));
				break;
			}
		}

		if(m->isOrbitEmbedded(orbit))
		{
			AttributeContainer& cont = m->getAttributeContainer(orbit);
			std::vector<std::string> names;
			std::vector<std::string> types;
			cont.getAttributesNames(names);
			cont.getAttributesTypes(types);
			for(unsigned int i = 0; i < names.size(); ++i)
			{
				QString name = QString::fromStdString(names[i]);
				QString type = QString::fromStdString(types[i]);
				switch(orbit)
				{
					case DART : {
						list_dartAttributes->addItem(name + " (" + type + ")");
					} break;
					case VERTEX : {
						QListWidgetItem* item = new QListWidgetItem(name /*+ " (" + type + ")"*/, list_vertexAttributes);
						if(m_selectedMap->getVBO(name))
							item->setCheckState(Qt::Checked);
						else
							item->setCheckState(Qt::Unchecked);
					} break;
					case EDGE : {
						list_edgeAttributes->addItem(name + " (" + type + ")");
					} break;
					case FACE : {
						list_faceAttributes->addItem(name + " (" + type + ")");
					} break;
					case VOLUME : {
						list_volumeAttributes->addItem(name + " (" + type + ")");
					} break;
				}
			}
		}
	}

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
