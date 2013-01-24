#include "computeCurvatureDialog.h"

#include "differentialProperties.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

ComputeCurvatureDialog::ComputeCurvatureDialog(Window* w) : m_window(w)
{
	setupUi(this);
	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));
}

void ComputeCurvatureDialog::init()
{
	mapList->clear();
	combo_positionAttribute->clear();
	combo_normalAttribute->clear();
	combo_KmaxAttribute->clear();
	combo_kmaxAttribute->clear();
	combo_KminAttribute->clear();
	combo_kminAttribute->clear();
	combo_KnormalAttribute->clear();
	KmaxAttributeName->setText("Kmax");
	kmaxAttributeName->setText("kmax");
	KminAttributeName->setText("Kmin");
	kminAttributeName->setText("kmin");
	KnormalAttributeName->setText("Knormal");
	const QList<MapHandlerGen*>& maps = m_window->getMapsList();
	foreach(MapHandlerGen* map, maps)
		mapList->addItem(map->getName());
}

void ComputeCurvatureDialog::cb_selectedMapChanged()
{
	QList<QListWidgetItem*> currentItems = mapList->selectedItems();
	if(!currentItems.empty())
	{
		combo_positionAttribute->clear();
		combo_normalAttribute->clear();
		const QString& mapname = currentItems[0]->text();
		MapHandlerGen* mh = m_window->getMap(mapname);
		GenericMap* map = mh->getGenericMap();
		AttributeContainer& cont = map->getAttributeContainer<VERTEX>();

		std::vector<std::string> names;
		std::vector<std::string> types;
		cont.getAttributesNames(names);
		cont.getAttributesTypes(types);
		std::string vec3TypeName = nameOfType(PFP2::VEC3());
		std::string realTypeName = nameOfType(PFP2::REAL());
		unsigned int j = 0;
		unsigned int k = 0;
		for(unsigned int i = 0; i < names.size(); ++i)
		{
			if(types[i] == vec3TypeName)
			{
				combo_positionAttribute->addItem(QString::fromStdString(names[i]));
				if(names[i] == "position") // try to select a position attribute named "position"
					combo_positionAttribute->setCurrentIndex(j);

				combo_normalAttribute->addItem(QString::fromStdString(names[i]));
				if(names[i] == "normal") // try to select a normal attribute named "normal"
					combo_normalAttribute->setCurrentIndex(j);

				combo_KmaxAttribute->addItem(QString::fromStdString(names[i]));
				if(names[i] == "Kmax") // try to select a normal attribute named "Kmax"
					combo_KmaxAttribute->setCurrentIndex(j);

				combo_KminAttribute->addItem(QString::fromStdString(names[i]));
				if(names[i] == "Kmin") // try to select a normal attribute named "Kmin"
					combo_KminAttribute->setCurrentIndex(j);

				combo_KnormalAttribute->addItem(QString::fromStdString(names[i]));
				if(names[i] == "Knormal") // try to select a normal attribute named "Knormal"
					combo_KnormalAttribute->setCurrentIndex(j);

				++j;
			}
			else if(types[i] == realTypeName)
			{
				combo_kmaxAttribute->addItem(QString::fromStdString(names[i]));
				if(names[i] == "kmax") // try to select a normal attribute named "kmax"
					combo_kmaxAttribute->setCurrentIndex(k);

				combo_kminAttribute->addItem(QString::fromStdString(names[i]));
				if(names[i] == "kmin") // try to select a normal attribute named "kmin"
					combo_kminAttribute->setCurrentIndex(k);

				++k;
			}
		}
	}
}

} // namespace SCHNApps

} // namespace CGoGN
