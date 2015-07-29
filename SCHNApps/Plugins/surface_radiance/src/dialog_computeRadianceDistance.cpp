#include "dialog_computeRadianceDistance.h"

#include "surface_radiance.h"
#include "schnapps.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

Dialog_ComputeRadianceDistance::Dialog_ComputeRadianceDistance(SCHNApps* s) :
	m_schnapps(s),
	m_selectedMap1(NULL),
	m_selectedMap2(NULL)
{
	setupUi(this);

	distanceAttributeName_1->setText("distance");
	distanceAttributeName_2->setText("distance");

	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(addMapToList(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(removeMapFromList(MapHandlerGen*)));

	connect(list_maps_1, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMap1Changed()));
	connect(list_maps_2, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMap2Changed()));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
	{
		list_maps_1->addItem(map->getName());
		list_maps_2->addItem(map->getName());
	}
}

void Dialog_ComputeRadianceDistance::selectedMap1Changed()
{
	if(m_selectedMap1)
		disconnect(m_selectedMap1, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList1(unsigned int, const QString&)));

	QList<QListWidgetItem*> currentItems = list_maps_1->selectedItems();
	if(!currentItems.empty())
	{
		combo_positionAttribute_1->clear();
		combo_distanceAttribute_1->clear();

		const QString& mapname = currentItems[0]->text();
		MapHandlerGen* mh = m_schnapps->getMap(mapname);

		QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));
		QString realTypeName = QString::fromStdString(nameOfType(PFP2::REAL()));

		const AttributeSet& attribs = mh->getAttributeSet(VERTEX);
		for(AttributeSet::const_iterator i = attribs.constBegin(); i != attribs.constEnd(); ++i)
		{
			if(i.value() == vec3TypeName)
			{
				combo_positionAttribute_1->addItem(i.key());
				combo_normalAttribute_1->addItem(i.key());
			}
			if(i.value() == realTypeName)
			{
				combo_distanceAttribute_1->addItem(i.key());
			}
		}

		m_selectedMap1 = mh;
		connect(m_selectedMap1, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList1(unsigned int, const QString&)));
	}
	else
		m_selectedMap1 = NULL;
}

void Dialog_ComputeRadianceDistance::selectedMap2Changed()
{
	if(m_selectedMap2)
		disconnect(m_selectedMap2, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList2(unsigned int, const QString&)));

	QList<QListWidgetItem*> currentItems = list_maps_2->selectedItems();
	if(!currentItems.empty())
	{
		combo_positionAttribute_2->clear();
		combo_distanceAttribute_2->clear();

		const QString& mapname = currentItems[0]->text();
		MapHandlerGen* mh = m_schnapps->getMap(mapname);

		QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));
		QString realTypeName = QString::fromStdString(nameOfType(PFP2::REAL()));

		const AttributeSet& attribs = mh->getAttributeSet(VERTEX);
		for(AttributeSet::const_iterator i = attribs.constBegin(); i != attribs.constEnd(); ++i)
		{
			if(i.value() == vec3TypeName)
			{
				combo_positionAttribute_2->addItem(i.key());
				combo_normalAttribute_2->addItem(i.key());
			}
			if(i.value() == realTypeName)
			{
				combo_distanceAttribute_2->addItem(i.key());
			}
		}

		m_selectedMap2 = mh;
		connect(m_selectedMap2, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList2(unsigned int, const QString&)));
	}
	else
		m_selectedMap2 = NULL;
}

void Dialog_ComputeRadianceDistance::addMapToList(MapHandlerGen* map)
{
	list_maps_1->addItem(map->getName());
	list_maps_2->addItem(map->getName());
}

void Dialog_ComputeRadianceDistance::removeMapFromList(MapHandlerGen* map)
{
	QList<QListWidgetItem*> items1 = list_maps_1->findItems(map->getName(), Qt::MatchExactly);
	if(!items1.empty())
		delete items1[0];

	QList<QListWidgetItem*> items2 = list_maps_2->findItems(map->getName(), Qt::MatchExactly);
	if(!items2.empty())
		delete items2[0];

	if(m_selectedMap1 == map)
	{
		disconnect(m_selectedMap1, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList1(unsigned int, const QString&)));
		m_selectedMap1 = NULL;
	}

	if(m_selectedMap2 == map)
	{
		disconnect(m_selectedMap2, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList2(unsigned int, const QString&)));
		m_selectedMap2 = NULL;
	}
}

void Dialog_ComputeRadianceDistance::addAttributeToList1(unsigned int orbit, const QString& nameAttr)
{
	if(orbit == VERTEX)
	{
		QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));
		QString realTypeName = QString::fromStdString(nameOfType(PFP2::REAL()));

		const QString& typeAttr = m_selectedMap1->getAttributeTypeName(orbit, nameAttr);

		if(typeAttr == vec3TypeName)
		{
			combo_positionAttribute_1->addItem(nameAttr);
			combo_normalAttribute_1->addItem(nameAttr);
		}
		if(typeAttr == realTypeName)
		{
			combo_distanceAttribute_1->addItem(nameAttr);
		}
	}
}

void Dialog_ComputeRadianceDistance::addAttributeToList2(unsigned int orbit, const QString& nameAttr)
{
	if(orbit == VERTEX)
	{
		QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));
		QString realTypeName = QString::fromStdString(nameOfType(PFP2::REAL()));

		const QString& typeAttr = m_selectedMap2->getAttributeTypeName(orbit, nameAttr);

		if(typeAttr == vec3TypeName)
		{
			combo_positionAttribute_2->addItem(nameAttr);
			combo_normalAttribute_2->addItem(nameAttr);
		}
		if(typeAttr == realTypeName)
		{
			combo_distanceAttribute_2->addItem(nameAttr);
		}
	}
}

} // namespace SCHNApps

} // namespace CGoGN
