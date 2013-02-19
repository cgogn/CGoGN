#include "computeNormalDialog.h"

#include "differentialProperties.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

ComputeNormalDialog::ComputeNormalDialog(Window* w) :
	m_window(w),
	m_selectedMap(NULL)
{
	setupUi(this);

	normalAttributeName->setText("normal");

	connect(m_window, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(addMapToList(MapHandlerGen*)));
	connect(m_window, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(removeMapFromList(MapHandlerGen*)));

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));

	const QList<MapHandlerGen*>& maps = m_window->getMapsList();
	foreach(MapHandlerGen* map, maps)
	{
		QListWidgetItem* item = new QListWidgetItem(map->getName(), mapList);
		item->setCheckState(Qt::Unchecked);
		connect(map, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
	}
}

void ComputeNormalDialog::selectedMapChanged()
{
	if(m_selectedMap)
		disconnect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList(unsigned int, const QString&)));

	QList<QListWidgetItem*> currentItems = mapList->selectedItems();
	if(!currentItems.empty())
	{
		combo_positionAttribute->clear();
		combo_normalAttribute->clear();

		const QString& mapname = currentItems[0]->text();
		MapHandlerGen* mh = m_window->getMap(mapname);

		QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));

		unsigned int j = 0;
		const AttributeHash& attribs = mh->getAttributesList(VERTEX);
		for(AttributeHash::const_iterator i = attribs.constBegin(); i != attribs.constEnd(); ++i)
		{
			if(i.value() == vec3TypeName)
			{
				combo_positionAttribute->addItem(i.key());
				if(i.key() == "position") // try to select a position attribute named "position"
					combo_positionAttribute->setCurrentIndex(j);

				combo_normalAttribute->addItem(i.key());
				if(i.key() == "normal") // try to select a normal attribute named "normal"
					combo_normalAttribute->setCurrentIndex(j);

				++j;
			}
		}

		m_selectedMap = mh;
		connect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList(unsigned int, const QString&)));
	}
	else
		m_selectedMap = NULL;
}

void ComputeNormalDialog::addMapToList(MapHandlerGen* m)
{
	QListWidgetItem* item = new QListWidgetItem(m->getName(), mapList);
	item->setCheckState(Qt::Unchecked);
}

void ComputeNormalDialog::removeMapFromList(MapHandlerGen* m)
{
	QList<QListWidgetItem*> items = mapList->findItems(m->getName(), Qt::MatchExactly);
	if(!items.empty())
		delete items[0];

	if(m_selectedMap == m)
	{
		disconnect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList(unsigned int, const QString&)));
		m_selectedMap = NULL;
	}
}

void ComputeNormalDialog::addAttributeToList(unsigned int orbit, const QString& nameAttr)
{
	QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));

	const QString& typeAttr = m_selectedMap->getAttributeTypeName(orbit, nameAttr);

	if(typeAttr == vec3TypeName)
	{
		combo_positionAttribute->addItem(nameAttr);
		combo_normalAttribute->addItem(nameAttr);
	}
}

} // namespace SCHNApps

} // namespace CGoGN
