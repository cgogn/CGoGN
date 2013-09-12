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

	connect(list_maps, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));
	connect(list_maps, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(mapCheckStateChanged(QListWidgetItem*)));
	connect(list_vertexAttributes, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(vertexAttributeCheckStateChanged(QListWidgetItem*)));

	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(selectedViewChanged(View*,View*)), this, SLOT(selectedViewChanged(View*,View*)));

	connect(button_dartAddSelector, SIGNAL(clicked()), this, SLOT(addDartSelector()));
	connect(button_dartRemoveSelector, SIGNAL(clicked()), this, SLOT(removeDartSelector()));
	connect(button_vertexAddSelector, SIGNAL(clicked()), this, SLOT(addVertexSelector()));
	connect(button_vertexRemoveSelector, SIGNAL(clicked()), this, SLOT(removeVertexSelector()));
	connect(button_edgeAddSelector, SIGNAL(clicked()), this, SLOT(addEdgeSelector()));
	connect(button_edgeRemoveSelector, SIGNAL(clicked()), this, SLOT(removeEdgeSelector()));
	connect(button_faceAddSelector, SIGNAL(clicked()), this, SLOT(addFaceSelector()));
	connect(button_faceRemoveSelector, SIGNAL(clicked()), this, SLOT(removeFaceSelector()));
	connect(button_volumeAddSelector, SIGNAL(clicked()), this, SLOT(addVolumeSelector()));
	connect(button_volumeRemoveSelector, SIGNAL(clicked()), this, SLOT(removeVolumeSelector()));
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

void ControlDock_MapTab::addDartSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
			m_selectedMap->addCellSelector(DART, QString("selector_") + QString::number(CellSelectorGen::selectorCount));
	}
}

void ControlDock_MapTab::removeDartSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
		{
			QList<QListWidgetItem*> items = list_dartSelectors->selectedItems();
			if(!items.empty())
				m_selectedMap->removeCellSelector(DART, items[0]->text());
		}
	}
}

void ControlDock_MapTab::addVertexSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
			m_selectedMap->addCellSelector(VERTEX, QString("selector_") + QString::number(CellSelectorGen::selectorCount));
	}
}

void ControlDock_MapTab::removeVertexSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
		{
			QList<QListWidgetItem*> items = list_vertexSelectors->selectedItems();
			if(!items.empty())
				m_selectedMap->removeCellSelector(VERTEX, items[0]->text());
		}
	}
}

void ControlDock_MapTab::addEdgeSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
			m_selectedMap->addCellSelector(EDGE, QString("selector_") + QString::number(CellSelectorGen::selectorCount));
	}
}

void ControlDock_MapTab::removeEdgeSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
		{
			QList<QListWidgetItem*> items = list_edgeSelectors->selectedItems();
			if(!items.empty())
				m_selectedMap->removeCellSelector(EDGE, items[0]->text());
		}
	}
}

void ControlDock_MapTab::addFaceSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
			m_selectedMap->addCellSelector(FACE, QString("selector_") + QString::number(CellSelectorGen::selectorCount));
	}
}

void ControlDock_MapTab::removeFaceSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
		{
			QList<QListWidgetItem*> items = list_faceSelectors->selectedItems();
			if(!items.empty())
				m_selectedMap->removeCellSelector(FACE, items[0]->text());
		}
	}
}

void ControlDock_MapTab::addVolumeSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
			m_selectedMap->addCellSelector(VOLUME, QString("selector_") + QString::number(CellSelectorGen::selectorCount));
	}
}

void ControlDock_MapTab::removeVolumeSelector()
{
	if(!b_updatingUI)
	{
		if(m_selectedMap)
		{
			QList<QListWidgetItem*> items = list_volumeSelectors->selectedItems();
			if(!items.empty())
				m_selectedMap->removeCellSelector(VOLUME, items[0]->text());
		}
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
		switch(orbit)
		{
			case DART : {
				unsigned int nb = m->getNbDarts();
				label_dartNbOrbits->setText(QString::number(nb));
				label_dartNbCells->setText(QString::number(nbc));
				foreach(CellSelectorGen* cs, m_selectedMap->getCellSelectorSet(orbit).values())
				{
					QListWidgetItem* item = new QListWidgetItem(cs->getName(), list_dartSelectors);
					item->setFlags(item->flags() | Qt::ItemIsEditable);
				}
				break;
			}
			case VERTEX : {
				unsigned int nb = m->getNbOrbits<VERTEX>();
				label_vertexNbOrbits->setText(QString::number(nb));
				label_vertexNbCells->setText(QString::number(nbc));
				foreach(CellSelectorGen* cs, m_selectedMap->getCellSelectorSet(orbit).values())
				{
					QListWidgetItem* item = new QListWidgetItem(cs->getName(), list_vertexSelectors);
					item->setFlags(item->flags() | Qt::ItemIsEditable);
				}
				break;
			}
			case EDGE : {
				unsigned int nb = m->getNbOrbits<EDGE>();
				label_edgeNbOrbits->setText(QString::number(nb));
				label_edgeNbCells->setText(QString::number(nbc));
				foreach(CellSelectorGen* cs, m_selectedMap->getCellSelectorSet(orbit).values())
				{
					QListWidgetItem* item = new QListWidgetItem(cs->getName(), list_edgeSelectors);
					item->setFlags(item->flags() | Qt::ItemIsEditable);
				}
				break;
			}
			case FACE : {
				unsigned int nb = m->getNbOrbits<FACE>();
				label_faceNbOrbits->setText(QString::number(nb));
				label_faceNbCells->setText(QString::number(nbc));
				foreach(CellSelectorGen* cs, m_selectedMap->getCellSelectorSet(orbit).values())
				{
					QListWidgetItem* item = new QListWidgetItem(cs->getName(), list_faceSelectors);
					item->setFlags(item->flags() | Qt::ItemIsEditable);
				}
				break;
			}
			case VOLUME : {
				unsigned int nb = m->getNbOrbits<VOLUME>();
				label_volumeNbOrbits->setText(QString::number(nb));
				label_volumeNbCells->setText(QString::number(nbc));
				foreach(CellSelectorGen* cs, m_selectedMap->getCellSelectorSet(orbit).values())
				{
					QListWidgetItem* item = new QListWidgetItem(cs->getName(), list_volumeSelectors);
					item->setFlags(item->flags() | Qt::ItemIsEditable);
				}
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
