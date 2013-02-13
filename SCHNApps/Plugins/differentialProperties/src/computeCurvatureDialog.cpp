#include "computeCurvatureDialog.h"

#include "differentialProperties.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

ComputeCurvatureDialog::ComputeCurvatureDialog(Window* w) :
	m_window(w),
	m_selectedMap(NULL)
{
	setupUi(this);

	KmaxAttributeName->setText("Kmax");
	kmaxAttributeName->setText("kmax");
	KminAttributeName->setText("Kmin");
	kminAttributeName->setText("kmin");
	KnormalAttributeName->setText("Knormal");

	connect(m_window, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(addMapToList(MapHandlerGen*)));
	connect(m_window, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(removeMapFromList(MapHandlerGen*)));

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(selectedMapChanged()));

	const QList<MapHandlerGen*>& maps = m_window->getMapsList();
	foreach(MapHandlerGen* map, maps)
		mapList->addItem(map->getName());
}

void ComputeCurvatureDialog::selectedMapChanged()
{
	if(m_selectedMap)
		disconnect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList(unsigned int, const QString&)));

	QList<QListWidgetItem*> currentItems = mapList->selectedItems();
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
		MapHandlerGen* mh = m_window->getMap(mapname);

		QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));
		QString realTypeName = QString::fromStdString(nameOfType(PFP2::REAL()));

		unsigned int j = 0;
		unsigned int k = 0;
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

				combo_KmaxAttribute->addItem(i.key());
				if(i.key() == "Kmax") // try to select a normal attribute named "Kmax"
					combo_KmaxAttribute->setCurrentIndex(j);

				combo_KminAttribute->addItem(i.key());
				if(i.key() == "Kmin") // try to select a normal attribute named "Kmin"
					combo_KminAttribute->setCurrentIndex(j);

				combo_KnormalAttribute->addItem(i.key());
				if(i.key() == "Knormal") // try to select a normal attribute named "Knormal"
					combo_KnormalAttribute->setCurrentIndex(j);

				++j;
			}
			else if(i.value() == realTypeName)
			{
				combo_kmaxAttribute->addItem(i.key());
				if(i.key() == "kmax") // try to select a normal attribute named "kmax"
					combo_kmaxAttribute->setCurrentIndex(k);

				combo_kminAttribute->addItem(i.key());
				if(i.key() == "kmin") // try to select a normal attribute named "kmin"
					combo_kminAttribute->setCurrentIndex(k);

				++k;
			}
		}

		m_selectedMap = mh;
		connect(m_selectedMap, SIGNAL(attributeAdded(unsigned int, const QString&)), this, SLOT(addAttributeToList(unsigned int, const QString&)));
	}
	else
		m_selectedMap = NULL;
}

void ComputeCurvatureDialog::addMapToList(MapHandlerGen* m)
{
	mapList->addItem(m->getName());
}

void ComputeCurvatureDialog::removeMapFromList(MapHandlerGen* m)
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

void ComputeCurvatureDialog::addAttributeToList(unsigned int orbit, const QString& nameAttr)
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
