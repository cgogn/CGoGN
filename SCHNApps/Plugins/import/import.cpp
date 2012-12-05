#include "import.h"

#include "mapHandler.h"
#include "Algo/Import/import.h"

bool ImportPlugin::enable()
{
	importAction = new QAction("import", this);
	addMenuAction("Import;import", importAction);
	connect(importAction, SIGNAL(triggered()), this, SLOT(cb_import()));
	return true;
}

void ImportPlugin::disable()
{
}

void ImportPlugin::cb_import()
{
	MAP* m = new MAP();
	MapHandler<PFP>* h = new MapHandler<PFP>("duck", m_window, m);

	std::vector<std::string> attrNames ;
	CGoGN::Algo::Import::importMesh<PFP>(*m, "/home/kraemer/Media/Data/surface/lowRes/duck_163.ply", attrNames);

	CGoGN::VertexAttribute<PFP::VEC3> position = m->getAttribute<PFP::VEC3, CGoGN::VERTEX>(attrNames[0]);

	CGoGN::Geom::BoundingBox<PFP::VEC3> bb = CGoGN::Algo::Geometry::computeBoundingBox<PFP>(*m, position) ;
	qglviewer::Vec min(bb.min()[0], bb.min()[1], bb.min()[2]);
	qglviewer::Vec max(bb.max()[0], bb.max()[1], bb.max()[2]);

	h->setBBmin(min);
	h->setBBmax(max);

	h->updatePrimitives(CGoGN::Algo::Render::GL2::POINTS, CGoGN::allDarts) ;
	h->updatePrimitives(CGoGN::Algo::Render::GL2::LINES, CGoGN::allDarts) ;
	h->updatePrimitives(CGoGN::Algo::Render::GL2::TRIANGLES, CGoGN::allDarts) ;

	m_window->addMap(h);

	CGoGN::Utils::VBO* positionVBO = h->getVBO(position.name());
	positionVBO->updateData(position);
}

/**
 * If we want to compile this plugin in debug mode,
 * we also define a DEBUG macro at the compilation
 */
#ifndef DEBUG
// essential Qt function:
// arguments are
//  - the compiled name of the plugin
//  - the main class of our plugin
Q_EXPORT_PLUGIN2(ImportPlugin, ImportPlugin)
#else
Q_EXPORT_PLUGIN2(ImportPluginD, ImportPlugin)
#endif
