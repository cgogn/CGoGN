#include "controlDock_mapTab.h"

#include "schnapps.h"
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
	for(unsigned int i = 0; i < NB_ORBITS; ++i)
		m_selectedSelector[i] = NULL;

	setupUi(this);

	connect(list_maps, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));
	connect(list_maps, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(mapCheckStateChanged(QListWidgetItem*)));
	connect(list_vertexAttributes, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(vertexAttributeCheckStateChanged(QListWidgetItem*)));

	connect(tabWidget_mapInfo, SIGNAL(currentChanged(int)), this, SLOT(selectedSelectorChanged()));

	connect(list_dartSelectors, SIGNAL(itemSelectionChanged()), this, SLOT(selectedSelectorChanged()));
	connect(list_dartSelectors, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectorCheckStateChanged(QListWidgetItem*)));
	connect(button_dartAddSelector, SIGNAL(clicked()), this, SLOT(addSelector()));
	connect(button_dartRemoveSelector, SIGNAL(clicked()), this, SLOT(removeSelector()));

	connect(list_vertexSelectors, SIGNAL(itemSelectionChanged()), this, SLOT(selectedSelectorChanged()));
	connect(list_vertexSelectors, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectorCheckStateChanged(QListWidgetItem*)));
	connect(button_vertexAddSelector, SIGNAL(clicked()), this, SLOT(addSelector()));
	connect(button_vertexRemoveSelector, SIGNAL(clicked()), this, SLOT(removeSelector()));

	connect(list_edgeSelectors, SIGNAL(itemSelectionChanged()), this, SLOT(selectedSelectorChanged()));
	connect(list_edgeSelectors, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectorCheckStateChanged(QListWidgetItem*)));
	connect(button_edgeAddSelector, SIGNAL(clicked()), this, SLOT(addSelector()));
	connect(button_edgeRemoveSelector, SIGNAL(clicked()), this, SLOT(removeSelector()));

	connect(list_faceSelectors, SIGNAL(itemSelectionChanged()), this, SLOT(selectedSelectorChanged()));
	connect(list_faceSelectors, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectorCheckStateChanged(QListWidgetItem*)));
	connect(button_faceAddSelector, SIGNAL(clicked()), this, SLOT(addSelector()));
	connect(button_faceRemoveSelector, SIGNAL(clicked()), this, SLOT(removeSelector()));

	connect(list_volumeSelectors, SIGNAL(itemSelectionChanged()), this, SLOT(selectedSelectorChanged()));
	connect(list_volumeSelectors, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(selectorCheckStateChanged(QListWidgetItem*)));
	connect(button_volumeAddSelector, SIGNAL(clicked()), this, SLOT(addSelector()));
	connect(button_volumeRemoveSelector, SIGNAL(clicked()), this, SLOT(removeSelector()));

	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(selectedViewChanged(View*,View*)), this, SLOT(selectedViewChanged(View*,View*)));
}

unsigned int ControlDock_MapTab::getCurrentOrbit()
{
	int current = tabWidget_mapInfo->currentIndex();
	switch(current)
	{
		case 0 : return DART; break;
		case 1 : return VERTEX; break;
		case 2 : return EDGE; break;
		case 3 : return FACE; break;
		case 4 : return VOLUME; break;
	}
	return DART;
}





void ControlDock_MapTab::selectedMapChanged()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
		{
			disconnect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(selectedMapAttributeAdded(unsigned int, const QString&)));
			disconnect(m_selectedMap, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(selectedMapVBOAdded(Utils::VBO*)));
			disconnect(m_selectedMap, SIGNAL(vboRemoved(Utils::VBO*)), this, SLOT(selectedMapVBORemoved(Utils::VBO*)));
			disconnect(m_selectedMap, SIGNAL(cellSelectorAdded(unsigned int, const QString&)), this, SLOT(selectedMapCellSelectorAdded(unsigned int, const QString&)));
			disconnect(m_selectedMap, SIGNAL(cellSelectorRemoved(unsigned int, const QString&)), this, SLOT(selectedMapCellSelectorRemoved(unsigned int, const QString&)));
		}

		QList<QListWidgetItem*> items = list_maps->selectedItems();
		if(!items.empty())
		{
			MapHandlerGen* old = m_selectedMap;

			QString selectedMapName = items[0]->text();
			m_selectedMap = m_schnapps->getMap(selectedMapName);

			updateSelectedMapInfo();

			m_schnapps->notifySelectedMapChanged(old, m_selectedMap);

			connect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(selectedMapAttributeAdded(unsigned int, const QString&)));
			connect(m_selectedMap, SIGNAL(vboAdded(Utils::VBO*)), this, SLOT(selectedMapVBOAdded(Utils::VBO*)));
			connect(m_selectedMap, SIGNAL(vboRemoved(Utils::VBO*)), this, SLOT(selectedMapVBORemoved(Utils::VBO*)));
			connect(m_selectedMap, SIGNAL(cellSelectorAdded(unsigned int, const QString&)), this, SLOT(selectedMapCellSelectorAdded(unsigned int, const QString&)));
			connect(m_selectedMap, SIGNAL(cellSelectorRemoved(unsigned int, const QString&)), this, SLOT(selectedMapCellSelectorRemoved(unsigned int, const QString&)));
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

void ControlDock_MapTab::selectedSelectorChanged()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
		{
			QList<QListWidgetItem*> items;
			unsigned int orbit = getCurrentOrbit();
			switch(orbit)
			{
				case DART: items = list_dartSelectors->selectedItems(); break;
				case VERTEX: items = list_vertexSelectors->selectedItems(); break;
				case EDGE: items = list_edgeSelectors->selectedItems(); break;
				case FACE: items = list_faceSelectors->selectedItems(); break;
				case VOLUME: items = list_volumeSelectors->selectedItems(); break;
			}
			if(!items.empty())
			{
				m_selectedSelector[orbit] = m_selectedMap->getCellSelector(orbit, items[0]->text());
				m_schnapps->notifySelectedCellSelectorChanged(m_selectedSelector[orbit]);
			}
		}
	}
}

void ControlDock_MapTab::selectorCheckStateChanged(QListWidgetItem* item)
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
		{
			unsigned int orbit = getCurrentOrbit();
			CellSelectorGen* cs = m_selectedMap->getCellSelector(orbit, item->text());
			cs->setMutuallyExclusive(item->checkState() == Qt::Checked);
			m_selectedMap->updateMutuallyExclusiveSelectors(orbit);
		}
	}
}

void ControlDock_MapTab::addSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
		{
			unsigned int orbit = getCurrentOrbit();
			m_selectedMap->addCellSelector(orbit, QString("selector_") + QString::number(CellSelectorGen::selectorCount));
		}
	}
}

void ControlDock_MapTab::removeSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
		{
			QList<QListWidgetItem*> items;
			unsigned int orbit = getCurrentOrbit();
			switch(orbit)
			{
				case DART: items = list_dartSelectors->selectedItems(); break;
				case VERTEX: items = list_vertexSelectors->selectedItems(); break;
				case EDGE: items = list_edgeSelectors->selectedItems(); break;
				case FACE: items = list_faceSelectors->selectedItems(); break;
				case VOLUME: items = list_volumeSelectors->selectedItems(); break;
			}
			if(!items.empty())
				m_selectedMap->removeCellSelector(orbit, items[0]->text());
		}
	}
}





void ControlDock_MapTab::mapAdded(MapHandlerGen* m)
{
	QListWidgetItem* item = new QListWidgetItem(m->getName(), list_maps);
	b_updatingUI = true;
	item->setCheckState(Qt::Unchecked);
	b_updatingUI = false;
}

void ControlDock_MapTab::mapRemoved(MapHandlerGen* m)
{
	QList<QListWidgetItem*> items = list_maps->findItems(m->getName(), Qt::MatchExactly);
	if(!items.empty())
	{
		b_updatingUI = true;
		delete items[0];
		b_updatingUI = false;
	}
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

void ControlDock_MapTab::selectedMapCellSelectorAdded(unsigned int orbit, const QString& name)
{
	updateSelectedMapInfo();
}

void ControlDock_MapTab::selectedMapCellSelectorRemoved(unsigned int orbit, const QString& name)
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

	list_dartSelectors->clear();
	list_vertexSelectors->clear();
	list_edgeSelectors->clear();
	list_faceSelectors->clear();
	list_volumeSelectors->clear();

	GenericMap* m = m_selectedMap->getGenericMap();
	for(unsigned int orbit = DART; orbit <= VOLUME; ++orbit)
	{
		unsigned int nbc = m->getNbCells(orbit);

		QListWidget* selectorList = NULL;

		switch(orbit)
		{
			case DART : {
				unsigned int nb = m_selectedMap->getNbDarts();
				label_dartNbOrbits->setText(QString::number(nb));
				label_dartNbCells->setText(QString::number(nbc));
				selectorList = list_dartSelectors;
				break;
			}
			case VERTEX : {
				unsigned int nb = m_selectedMap->getNbOrbits(VERTEX);
				label_vertexNbOrbits->setText(QString::number(nb));
				label_vertexNbCells->setText(QString::number(nbc));
				selectorList = list_vertexSelectors;
				break;
			}
			case EDGE : {
				unsigned int nb = m_selectedMap->getNbOrbits(EDGE);
				label_edgeNbOrbits->setText(QString::number(nb));
				label_edgeNbCells->setText(QString::number(nbc));
				selectorList = list_edgeSelectors;
				break;
			}
			case FACE : {
				unsigned int nb = m_selectedMap->getNbOrbits(FACE);
				label_faceNbOrbits->setText(QString::number(nb));
				label_faceNbCells->setText(QString::number(nbc));
				selectorList = list_faceSelectors;
				break;
			}
			case VOLUME : {
				unsigned int nb = m_selectedMap->getNbOrbits(VOLUME);
				label_volumeNbOrbits->setText(QString::number(nb));
				label_volumeNbCells->setText(QString::number(nbc));
				selectorList = list_volumeSelectors;
				break;
			}
		}

		foreach(CellSelectorGen* cs, m_selectedMap->getCellSelectorSet(orbit).values())
		{
			QListWidgetItem* item = new QListWidgetItem(cs->getName(), selectorList);
			item->setFlags(item->flags() | Qt::ItemIsEditable);
			if(m_selectedSelector[orbit] == cs)
				item->setSelected(true);
			if(cs->isMutuallyExclusive())
				item->setCheckState(Qt::Checked);
			else
				item->setCheckState(Qt::Unchecked);
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
