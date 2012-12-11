#include "dialogs/mapsDialog.h"

#include "window.h"
#include "mapHandler.h"

#include <QMessageBox>
#include <QMouseEvent>

namespace CGoGN
{

namespace SCHNApps
{

MapsDialog::MapsDialog(Window* window) :
	QDialog(window),
	m_window(window)
{
	this->setupUi(this);
	this->setModal(false);

	connect(removeMapButton, SIGNAL(clicked()), this, SLOT(cb_removeMap()));

	connect(mapList, SIGNAL(itemSelectionChanged()), this, SLOT(cb_selectedMapChanged()));

	connect(m_window, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(cb_addMapToList(MapHandlerGen*)));
	connect(m_window, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(cb_removeMapFromList(MapHandlerGen*)));
}

MapsDialog::~MapsDialog()
{}

void MapsDialog::cb_removeMap()
{
	QList<QListWidgetItem*> currentItems = mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& name = currentItems[0]->text();
		MapHandlerGen* m = m_window->getMap(name);
		if(!m->isUsed())
			m_window->removeMap(name);
		else
			QMessageBox::warning(this, tr("Warning"), "Map is currently used");
	}
}

void MapsDialog::cb_selectedMapChanged()
{
	QList<QListWidgetItem*> currentItems = mapList->selectedItems();
	if(!currentItems.empty())
	{
		QListWidgetItem* current = currentItems[0];

		dartAttributes->clear();
		vertexAttributes->clear();
		edgeAttributes->clear();
		faceAttributes->clear();
		volumeAttributes->clear();

		const QString& name = current->text();
		MapHandlerGen* mh = m_window->getMap(name);
		CGoGN::GenericMap* m = mh->getGenericMap();
		for(unsigned int orbit = CGoGN::DART; orbit <= CGoGN::VOLUME; ++orbit)
		{
			if(m->isOrbitEmbedded(orbit))
			{
//				unsigned int nb = 0;
//				switch(orbit)
//				{
//					case CGoGN::DART :
//						nb = m->getNbOrbits<CGoGN::DART>();
//						break;
//					case CGoGN::VERTEX :
//						nb = m->getNbOrbits<CGoGN::VERTEX>();
//						break;
//					case CGoGN::EDGE :
//						nb = m->getNbOrbits<CGoGN::EDGE>();
//						break;
//					case CGoGN::FACE :
//						nb = m->getNbOrbits<CGoGN::FACE>();
//						break;
//					case CGoGN::VOLUME :
//						nb = m->getNbOrbits<CGoGN::VOLUME>();
//						break;
//				}

				CGoGN::AttributeContainer& cont = m->getAttributeContainer(orbit);
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
						case CGoGN::DART : dartAttributes->addItem(name + "(" + type + ")"); break;
						case CGoGN::VERTEX : vertexAttributes->addItem(name + "(" + type + ")"); break;
						case CGoGN::EDGE : edgeAttributes->addItem(name + "(" + type + ")"); break;
						case CGoGN::FACE : faceAttributes->addItem(name + "(" + type + ")"); break;
						case CGoGN::VOLUME : volumeAttributes->addItem(name + "(" + type + ")"); break;
					}
				}

			}
		}
	}
}

void MapsDialog::cb_addMapToList(MapHandlerGen* m)
{
	mapList->addItem(m->getName());
}

void MapsDialog::cb_removeMapFromList(MapHandlerGen* m)
{
	for(int i = 0; i < mapList->count(); ++i)
	{
		if(mapList->item(i)->text() == m->getName())
		{
			delete mapList->item(i);
			return;
		}
	}
}

} // namespace SCHNApps

} // namespace CGoGN
