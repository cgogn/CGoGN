#include "dialog_computeCurvature.h"

#include "surface_differentialProperties.h"
#include "schnapps.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

Dialog_ComputeCurvature::Dialog_ComputeCurvature(SCHNApps* s) :
	m_schnapps(s),
	m_selectedMap(NULL)
{
	setupUi(this);

	KmaxAttributeName->setText("Kmax");
	kmaxAttributeName->setText("kmax");
	KminAttributeName->setText("Kmin");
	kminAttributeName->setText("kmin");
	KnormalAttributeName->setText("Knormal");

	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(addMapToList(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(removeMapFromList(MapHandlerGen*)));

	connect(list_maps, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));

	foreach(MapHandlerGen* map,  m_schnapps->getMapSet().values())
	{
		QListWidgetItem* item = new QListWidgetItem(map->getName(), list_maps);
		item->setCheckState(Qt::Unchecked);
	}
}

void Dialog_ComputeCurvature::selectedMapChanged()
{
	if(m_selectedMap)
		disconnect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList(unsigned int, const QString&)));

	QList<QListWidgetItem*> currentItems = list_maps->selectedItems();
	if(!currentItems.empty())
	{
		combo_positionAttribute->clear();
		combo_normalAttribute->clear();
		combo_KmaxAttribute->clear();
		combo_KminAttribute->clear();
		combo_KnormalAttribute->clear();
		combo_kmaxAttribute->clear();
		combo_kminAttribute->clear();

		const QString& mapname = currentItems[0]->text();
		MapHandlerGen* mh = m_schnapps->getMap(mapname);

		QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));
		QString realTypeName = QString::fromStdString(nameOfType(PFP2::REAL()));

		unsigned int j = 0;
		unsigned int k = 0;
		const AttributeSet& attribs = mh->getAttributeSet(VERTEX);
		for(AttributeSet::const_iterator i = attribs.constBegin(); i != attribs.constEnd(); ++i)
		{
			if(i.value() == vec3TypeName)
			{
				combo_positionAttribute->addItem(i.key());
				combo_normalAttribute->addItem(i.key());
				combo_KmaxAttribute->addItem(i.key());
				combo_KminAttribute->addItem(i.key());
				combo_KnormalAttribute->addItem(i.key());

				++j;
			}
			else if(i.value() == realTypeName)
			{
				combo_kmaxAttribute->addItem(i.key());
				combo_kminAttribute->addItem(i.key());

				++k;
			}
		}

		m_selectedMap = mh;
		connect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList(unsigned int, const QString&)));
	}
	else
		m_selectedMap = NULL;
}

void Dialog_ComputeCurvature::addMapToList(MapHandlerGen* m)
{
	QListWidgetItem* item = new QListWidgetItem(m->getName(), list_maps);
	item->setCheckState(Qt::Unchecked);
}

void Dialog_ComputeCurvature::removeMapFromList(MapHandlerGen* m)
{
	QList<QListWidgetItem*> items = list_maps->findItems(m->getName(), Qt::MatchExactly);
	if(!items.empty())
		delete items[0];

	if(m_selectedMap == m)
	{
		disconnect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList(unsigned int, const QString&)));
		m_selectedMap = NULL;
	}
}

void Dialog_ComputeCurvature::addAttributeToList(unsigned int orbit, const QString& nameAttr)
{
	QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));
	QString realTypeName = QString::fromStdString(nameOfType(PFP2::REAL()));

	const QString& typeAttr = m_selectedMap->getAttributeTypeName(orbit, nameAttr);

	if(typeAttr == vec3TypeName)
	{
		combo_positionAttribute->addItem(nameAttr);
		combo_normalAttribute->addItem(nameAttr);
		combo_KmaxAttribute->addItem(nameAttr);
		combo_KminAttribute->addItem(nameAttr);
		combo_KnormalAttribute->addItem(nameAttr);
	}
	else if(typeAttr == realTypeName)
	{
		combo_kmaxAttribute->addItem(nameAttr);
		combo_kminAttribute->addItem(nameAttr);
	}
}

} // namespace SCHNApps

} // namespace CGoGN
