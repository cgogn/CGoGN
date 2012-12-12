#include "renderVector.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"

bool RenderVectorPlugin::enable()
{
	m_dockTab = new RenderVectorDockTab();
	addTabInDock(m_dockTab, "RenderVector");

	m_vectorShader = new Utils::ShaderVectorPerVertex() ;
	m_vectorShader->setColor(Geom::Vec4f(1.0f, 0.0f, 0.0f, 1.0f)) ;

	registerShader(m_vectorShader);

	connect(m_dockTab->slider_vectorsScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(cb_vectorsScaleFactorChanged(int)));

	return true;
}

void RenderVectorPlugin::disable()
{
	delete m_vectorShader;
}

void RenderVectorPlugin::redraw(View* view)
{
	TabParams* params = h_viewParams[view];

	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	foreach(MapHandlerGen* m, maps)
	{
		CGoGN::Utils::VBO* positionVBO = m->getVBO("position");
		CGoGN::Utils::VBO* normalVBO = m->getVBO("normal");
		m_vectorShader->setScale(m->getBBdiagSize() / 100.0f * params->vectorsScaleFactor) ;
		m_vectorShader->setAttributePosition(positionVBO) ;
		m_vectorShader->setAttributeVector(normalVBO) ;
		glLineWidth(1.0f) ;
		m->draw(m_vectorShader, Algo::Render::GL2::POINTS) ;
	}
}

void RenderVectorPlugin::viewLinked(View* view)
{
	h_viewParams.insert(view, new TabParams(1.0));
}

void RenderVectorPlugin::viewUnlinked(View* view)
{
	h_viewParams.remove(view);
}

void RenderVectorPlugin::currentViewChanged(View* view)
{
	TabParams* params = h_viewParams[view];
	m_dockTab->slider_vectorsScaleFactor->setSliderPosition(params->vectorsScaleFactor * 50.0);
}

void RenderVectorPlugin::cb_vectorsScaleFactorChanged(int i)
{
	View* current = m_window->getCurrentView();
	assert(isLinkedToView(current));

	TabParams* params = h_viewParams[current];
	params->vectorsScaleFactor = i / 50.0;
	current->updateGL();
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
Q_EXPORT_PLUGIN2(RenderVectorPlugin, RenderVectorPlugin)
#else
Q_EXPORT_PLUGIN2(RenderVectorPluginD, RenderVectorPlugin)
#endif
