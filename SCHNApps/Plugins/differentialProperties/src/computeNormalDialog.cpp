#include "computeNormalDialog.h"

#include "differentialProperties.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

ComputeNormalDialog::ComputeNormalDialog(Window* w) : m_window(w)
{
	setupUi(this);
	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));
}

void ComputeNormalDialog::init()
{
	mapList->clear();
	combo_positionAttribute->clear();
	combo_normalAttribute->clear();
	normalAttributeName->setText("normal");
	const QList<MapHandlerGen*>& maps = m_window->getMapsList();
	foreach(MapHandlerGen* map, maps)
		mapList->addItem(map->getName());
}

void ComputeNormalDialog::cb_selectedMapChanged()
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
		unsigned int j = 0;
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

				++j;
			}
		}
	}
}

} // namespace SCHNApps

} // namespace CGoGN
