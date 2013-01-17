#include "importSurface.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"

#include <QFileDialog>
#include <QFileInfo>

bool ImportSurfacePlugin::enable()
{
	importAction = new QAction("import", this);
	addMenuAction("Surface;Import", importAction);
	connect(importAction, SIGNAL(triggered()), this, SLOT(cb_import()));
	return true;
}

void ImportSurfacePlugin::cb_import()
{
	QString fileName = QFileDialog::getOpenFileName(m_window, "Import file", m_window->getAppPath(), "Mesh Files (*.ply *.off *.trian)");
	QFileInfo fi(fileName);

	if(fi.exists())
	{
//		typename PFP2::MAP* m = new typename PFP2::MAP();
		GenericMap* m = m_window->createMap(2);
		typename PFP2::MAP* map = static_cast<typename PFP2::MAP*>(m);
		MapHandler<PFP2>* h = new MapHandler<PFP2>(fi.baseName(), m_window, map);

		std::vector<std::string> attrNames ;
		Algo::Surface::Import::importMesh<PFP2>(*map, fileName.toUtf8().constData(), attrNames);

		// get vertex position attribute
		VertexAttribute<typename PFP2::VEC3> position = map->getAttribute<typename PFP2::VEC3, CGoGN::VERTEX>(attrNames[0]);

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
Q_EXPORT_PLUGIN2(ImportSurfacePlugin, ImportSurfacePlugin)
#else
Q_EXPORT_PLUGIN2(ImportSurfacePluginD, ImportSurfacePlugin)
#endif
