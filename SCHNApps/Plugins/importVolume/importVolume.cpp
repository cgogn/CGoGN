#include "importVolume.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"

#include <QFileDialog>
#include <QFileInfo>

bool ImportVolumePlugin::enable()
{
	importAction = new QAction("import", this);
	addMenuAction("Volume;Import", importAction);
	connect(importAction, SIGNAL(triggered()), this, SLOT(cb_import()));
	return true;
}

void ImportVolumePlugin::cb_import()
{
	QString fileName = QFileDialog::getOpenFileName(m_window, "Import file", m_window->getAppPath(), "Mesh Files (*.node *.ts *.off *.tet)");
	QFileInfo fi(fileName);

	if(fi.exists())
	{
		GenericMap* m = m_window->createMap(3);
		PFP3::MAP* map = static_cast<PFP3::MAP*>(m);
		MapHandler<PFP3>* h = new MapHandler<PFP3>(fi.baseName(), m_window, map);

		std::vector<std::string> attrNames ;
		Algo::Volume::Import::importMesh<PFP3>(*map, fileName.toUtf8().constData(), attrNames);

		// get vertex position attribute
		VertexAttribute<PFP2::VEC3> position = map->getAttribute<PFP2::VEC3, CGoGN::VERTEX>(attrNames[0]);

		// create VBO for vertex position attribute
		h->createVBO(position);

		// compute map bounding box
		h->updateBB(position);

		// compute primitive connectivity VBOs
		h->updatePrimitives(Algo::Render::GL2::POINTS);
		h->updatePrimitives(Algo::Render::GL2::LINES);
		h->updatePrimitives(Algo::Render::GL2::TRIANGLES);

		m_window->addMap(h);
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(ImportVolumePlugin, ImportVolumePlugin)
#else
Q_EXPORT_PLUGIN2(ImportVolumePluginD, ImportVolumePlugin)
#endif
