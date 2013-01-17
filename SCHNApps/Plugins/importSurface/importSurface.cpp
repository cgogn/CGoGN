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
	QString fileName = QFileDialog::getOpenFileName(m_window, "Import file", m_window->getAppPath(), "Mesh Files (*.ply *.off)");
	QFileInfo fi(fileName);

	if(fi.exists())
	{
		MAP* m = new MAP();
		MapHandler<PFP>* h = new MapHandler<PFP>(fi.baseName(), m_window, m);

		std::vector<std::string> attrNames ;
		Algo::Surface::Import::importMesh<PFP>(*m, fileName.toUtf8().constData(), attrNames);

		// get vertex position attribute
		VertexAttribute<VEC3> position = m->getAttribute<VEC3, CGoGN::VERTEX>(attrNames[0]);

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
