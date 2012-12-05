#include "firstPlugin.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Import/import.h"

bool FirstPlugin::enable()
{
	m_render = new Algo::Render::GL2::MapRender() ;
	m_positionVBO = new Utils::VBO() ;
	m_flatShader = new Utils::ShaderFlat() ;
	m_flatShader->setAttributePosition(m_positionVBO) ;
	m_flatShader->setAmbiant(Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f)) ;
	m_flatShader->setDiffuse(Geom::Vec4f(0.8f, 0.9f, 0.7f, 1.0f)) ;
	m_flatShader->setExplode(1.0f) ;
	CGoGN::Utils::GLSLShader::registerShader(NULL, m_flatShader) ;

	std::vector<std::string> attrNames ;
	CGoGN::Algo::Import::importMesh<PFP>(myMap, "/home/kraemer/Media/Data/surface/lowRes/duck_163.ply", attrNames);
	position = myMap.getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;

	m_render->initPrimitives<PFP>(myMap, allDarts, CGoGN::Algo::Render::GL2::POINTS) ;
	m_render->initPrimitives<PFP>(myMap, allDarts, CGoGN::Algo::Render::GL2::LINES) ;
	m_render->initPrimitives<PFP>(myMap, allDarts, CGoGN::Algo::Render::GL2::TRIANGLES) ;

	m_positionVBO->updateData(position) ;

	return true;
}

void FirstPlugin::disable()
{
}

void FirstPlugin::redraw(View* view)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) ;
	glEnable(GL_LIGHTING) ;
	m_render->draw(m_flatShader, Algo::Render::GL2::TRIANGLES) ;
}

void FirstPlugin::viewAdded(View* view)
{
	CGoGN::Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	qglviewer::Vec min(bb.min()[0], bb.min()[1], bb.min()[2]);
	qglviewer::Vec max(bb.max()[0], bb.max()[1], bb.max()[2]);
	view->setSceneBoundingBox(min, max);
}

/**
 * If we want to compile this plugin in debug mode,
 * we also define a DEBUG macro at the compilation
 */
#ifndef DEBUG
// essential Qt function:
// arguments are
//  - the complied name of the plugin
//  - the main class of our plugin
Q_EXPORT_PLUGIN2(FirstPlugin, FirstPlugin)
#else
Q_EXPORT_PLUGIN2(FirstPluginD, FirstPlugin)
#endif
