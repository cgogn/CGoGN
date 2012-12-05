#include "demo.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"

bool DemoPlugin::enable()
{
	addTabInDock(new DemoDockTab(), "demoTab");

	m_flatShader = new CGoGN::Utils::ShaderFlat() ;
	m_flatShader->setAmbiant(CGoGN::Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f)) ;
	m_flatShader->setDiffuse(CGoGN::Geom::Vec4f(0.8f, 0.9f, 0.7f, 1.0f)) ;
	m_flatShader->setExplode(1.0f) ;

	CGoGN::Utils::GLSLShader::registerShader(NULL, m_flatShader) ;

	return true;
}

void DemoPlugin::disable()
{
	delete m_flatShader;
}

void DemoPlugin::redraw(View* view)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) ;
	glEnable(GL_LIGHTING) ;

	QList<MapHandlerGen*> maps = view->getLinkedMaps();
	foreach(MapHandlerGen* m, maps)
	{
		CGoGN::Utils::VBO* positionVBO = m->getVBO("position");
		m_flatShader->setAttributePosition(positionVBO);
		m->draw(m_flatShader, CGoGN::Algo::Render::GL2::TRIANGLES) ;
	}
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
Q_EXPORT_PLUGIN2(DemoPlugin, DemoPlugin)
#else
Q_EXPORT_PLUGIN2(DemoPluginD, DemoPlugin)
#endif
