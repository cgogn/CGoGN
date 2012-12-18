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

void MapsDialog::clearInfo()
{
	dartAttributes->clear();
	vertexAttributes->clear();
	edgeAttributes->clear();
	faceAttributes->clear();
	volumeAttributes->clear();
	lineEdit_dart_orbits->setText("");
	lineEdit_dart_cells->setText("");
	lineEdit_vertex_orbits->setText("");
	lineEdit_vertex_cells->setText("");
	lineEdit_edge_orbits->setText("");
	lineEdit_edge_cells->setText("");
	lineEdit_face_orbits->setText("");
	lineEdit_face_cells->setText("");
}

void MapsDialog::cb_removeMap()
{
	QList<QListWidgetItem*> currentItems = mapList->selectedItems();
	if(!currentItems.empty())
	{
		const QString& name = currentItems[0]->text();
		MapHandlerGen* m = m_window->getMap(name);
		if(!m->isUsed())
		{
			m_window->removeMap(name);
			cb_selectedMapChanged();
		}
		else
			QMessageBox::warning(this, tr("Warning"), "Map is currently used");
	}
}

void MapsDialog::cb_selectedMapChanged()
{
	clearInfo();

	QList<QListWidgetItem*> currentItems = mapList->selectedItems();
	if(!currentItems.empty())
	{
		QListWidgetItem* current = currentItems[0];

		const QString& name = current->text();
		MapHandlerGen* mh = m_window->getMap(name);
		GenericMap* m = mh->getGenericMap();
		for(unsigned int orbit = DART; orbit <= VOLUME; ++orbit)
		{
			unsigned int nbc = m->getNbCells(orbit);
			switch(orbit)
			{
				case DART : {
					unsigned int nb = m->getNbDarts();
					lineEdit_dart_orbits->setText(QString::number(nb));
					lineEdit_dart_cells->setText(QString::number(nbc));
					break;
				}
				case VERTEX : {
					unsigned int nb = m->getNbOrbits<VERTEX>();
					lineEdit_vertex_orbits->setText(QString::number(nb));
					lineEdit_vertex_cells->setText(QString::number(nbc));
					break;
				}
				case EDGE : {
					unsigned int nb = m->getNbOrbits<EDGE>();
					lineEdit_edge_orbits->setText(QString::number(nb));
					lineEdit_edge_cells->setText(QString::number(nbc));
					break;
				}
				case FACE : {
					unsigned int nb = m->getNbOrbits<FACE>();
					lineEdit_face_orbits->setText(QString::number(nb));
					lineEdit_face_cells->setText(QString::number(nbc));
					break;
				}
				case VOLUME : {
					unsigned int nb = m->getNbOrbits<VOLUME>();
					lineEdit_volume_orbits->setText(QString::number(nb));
					lineEdit_volume_cells->setText(QString::number(nbc));
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
						case DART : dartAttributes->addItem(name + " (" + type + ")"); break;
						case VERTEX : vertexAttributes->addItem(name + " (" + type + ")"); break;
						case EDGE : edgeAttributes->addItem(name + " (" + type + ")"); break;
						case FACE : faceAttributes->addItem(name + " (" + type + ")"); break;
						case VOLUME : volumeAttributes->addItem(name + " (" + type + ")"); break;
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
