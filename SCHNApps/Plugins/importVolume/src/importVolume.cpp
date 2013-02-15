#include "importVolume.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"

#include <QFileDialog>
#include <QFileInfo>

bool ImportVolumePlugin::enable()
{
	importAction = new QAction("import", this);
	addMenuAction("Volume;Import", importAction);
	connect(importAction, SIGNAL(triggered()), this, SLOT(importFromFileDialog()));
	return true;
}

MapHandlerGen* ImportVolumePlugin::importFromFile(const QString& fileName)
{
	QFileInfo fi(fileName);
	if(fi.exists())
	{
		MapHandlerGen* mhg = m_window->addMap(fi.baseName(), 3);
		if(mhg)
		{
			MapHandler<PFP3>* mh = static_cast<MapHandler<PFP3>*>(mhg);
			PFP3::MAP* map = mh->getMap();

			std::vector<std::string> attrNames ;
			Algo::Volume::Import::importMesh<PFP3>(*map, fileName.toStdString(), attrNames);

			// get vertex position attribute
			VertexAttribute<PFP3::VEC3> position = map->getAttribute<PFP3::VEC3, CGoGN::VERTEX>(attrNames[0]);
			mh->registerAttribute<PFP2::VEC3, VERTEX>(position);

			// create VBO for vertex position attribute
			mh->createVBO(position);

			// compute map bounding box
			mh->updateBB(position);
		}
		return mhg;
	}
	else
		return NULL;
}

void ImportVolumePlugin::importFromFileDialog()
{
	QString fileName = QFileDialog::getOpenFileName(m_window, "Import file", m_window->getAppPath(), "Mesh Files (*.node *.ts *.off *.tet)");
	importFromFile(fileName);
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(ImportVolumePlugin, ImportVolumePlugin)
#else
Q_EXPORT_PLUGIN2(ImportVolumePluginD, ImportVolumePlugin)
#endif
