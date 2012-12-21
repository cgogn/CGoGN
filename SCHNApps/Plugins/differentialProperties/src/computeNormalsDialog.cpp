#include "computeNormalsDialog.h"

#include "differentialProperties.h"
#include "window.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

ComputeNormalsDialog::ComputeNormalsDialog(Window* w) : m_window(w)
{
	setupUi(this);
	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));
}

void ComputeNormalsDialog::init()
{
	mapList->clear();
	combo_positionAttribute->clear();
	attributeName->setText("normal");
	const QList<MapHandlerGen*>& maps = m_window->getMapsList();
	foreach(MapHandlerGen* map, maps)
		mapList->addItem(map->getName());
}

void ComputeNormalsDialog::cb_selectedMapChanged()
{
	QList<QListWidgetItem*> currentItems = mapList->selectedItems();
	if(!currentItems.empty())
	{
		combo_positionAttribute->clear();
		const QString& mapname = currentItems[0]->text();
		MapHandlerGen* mh = m_window->getMap(mapname);
		GenericMap* map = mh->getGenericMap();
		AttributeContainer& cont = map->getAttributeContainer<VERTEX>();

		std::vector<std::string> names;
		std::vector<std::string> types;
		cont.getAttributesNames(names);
		cont.getAttributesTypes(types);
		std::string vec3TypeName = VEC3::CGoGNnameOfType();
		for(unsigned int i = 0; i < names.size(); ++i)
		{
			if(types[i] == vec3TypeName)
				combo_positionAttribute->addItem(QString::fromStdString(names[i]));
		}
	}
}

} // namespace SCHNApps

} // namespace CGoGN
