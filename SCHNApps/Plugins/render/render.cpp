#include "render.h"

#include "mapHandler.h"

#include "Algo/Import/import.h"

bool RenderPlugin::enable()
{
	m_dockTab = new RenderDockTab();
	addTabInDock(m_dockTab, "Render");

	m_flatShader = new CGoGN::Utils::ShaderFlat();
	m_flatShader->setAmbiant(CGoGN::Geom::Vec4f(0.2f, 0.2f, 0.2f, 0.1f));
	m_flatShader->setDiffuse(CGoGN::Geom::Vec4f(0.8f, 0.9f, 0.7f, 1.0f));
	m_flatShader->setExplode(1.0f);

	m_simpleColorShader = new CGoGN::Utils::ShaderSimpleColor();
	CGoGN::Geom::Vec4f c(0.1f, 0.1f, 0.1f, 1.0f);
	m_simpleColorShader->setColor(c);

	m_pointSprite = new CGoGN::Utils::PointSprite();

	connect(m_dockTab->check_renderVertices, SIGNAL(toggled(bool)), this, SLOT(cb_renderVerticesChanged(bool)));
	connect(m_dockTab->slider_verticesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(cb_verticesScaleFactorChanged(int)));
	connect(m_dockTab->check_renderEdges, SIGNAL(toggled(bool)), this, SLOT(cb_renderEdgesChanged(bool)));
	connect(m_dockTab->check_renderFaces, SIGNAL(toggled(bool)), this, SLOT(cb_renderFacesChanged(bool)));

	return true;
}

void RenderPlugin::disable()
{
	delete m_flatShader;
	delete m_simpleColorShader;
	delete m_pointSprite;
}

void RenderPlugin::redraw(View* view)
{
	TabParams* params = h_viewParams[view];

	const QList<MapHandlerGen*>& maps = view->getLinkedMaps();
	foreach(MapHandlerGen* m, maps)
	{
		CGoGN::Utils::VBO* positionVBO = m->getVBO("position");
		if(params->renderVertices)
		{
			m_pointSprite->setSize(m->getBBdiagSize() / 100.0f * params->verticesScaleFactor);
			m_pointSprite->setAttributePosition(positionVBO);
			m_pointSprite->predraw(CGoGN::Geom::Vec3f(0.0f, 0.0f, 1.0f));
			m->draw(m_pointSprite, CGoGN::Algo::Render::GL2::POINTS);
			m_pointSprite->postdraw();
		}
		if(params->renderEdges)
		{
			glLineWidth(1.0f);
			m_simpleColorShader->setAttributePosition(positionVBO);
			m->draw(m_simpleColorShader, CGoGN::Algo::Render::GL2::LINES);
		}
		if(params->renderFaces)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_LIGHTING);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 1.0f);
			m_flatShader->setAttributePosition(positionVBO);
			m->draw(m_flatShader, CGoGN::Algo::Render::GL2::TRIANGLES);
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}
}

void RenderPlugin::viewLinked(View* view)
{
	assert(!h_viewParams.contains(view));
	h_viewParams.insert(view, new TabParams(1.0, false, false, true));

	CGoGN::Utils::GLSLShader::registerShader(view, m_flatShader);
	CGoGN::Utils::GLSLShader::registerShader(view, m_simpleColorShader);
	CGoGN::Utils::GLSLShader::registerShader(view, m_pointSprite);
}

void RenderPlugin::viewUnlinked(View* view)
{
	assert(h_viewParams.contains(view));
	h_viewParams.remove(view);

	CGoGN::Utils::GLSLShader::unregisterShader(view, m_flatShader);
	CGoGN::Utils::GLSLShader::unregisterShader(view, m_simpleColorShader);
	CGoGN::Utils::GLSLShader::unregisterShader(view, m_pointSprite);
}

void RenderPlugin::currentViewChanged(View* view)
{
	TabParams* params = h_viewParams[view];
	m_dockTab->check_renderVertices->setChecked(params->renderVertices);
	m_dockTab->slider_verticesScaleFactor->setSliderPosition(params->verticesScaleFactor * 50.0);
	m_dockTab->check_renderEdges->setChecked(params->renderEdges);
	m_dockTab->check_renderFaces->setChecked(params->renderFaces);
}

void RenderPlugin::cb_renderVerticesChanged(bool b)
{
	View* current = m_window->getCurrentView();
	assert(isLinkedToView(current));

	TabParams* params = h_viewParams[current];
	params->renderVertices = b;
	current->updateGL();
}

void RenderPlugin::cb_verticesScaleFactorChanged(int i)
{
	View* current = m_window->getCurrentView();
	assert(isLinkedToView(current));

	TabParams* params = h_viewParams[current];
	params->verticesScaleFactor = i / 50.0;
	current->updateGL();
}

void RenderPlugin::cb_renderEdgesChanged(bool b)
{
	View* current = m_window->getCurrentView();
	assert(isLinkedToView(current));

	TabParams* params = h_viewParams[current];
	params->renderEdges = b;
	current->updateGL();
}

void RenderPlugin::cb_renderFacesChanged(bool b)
{
	View* current = m_window->getCurrentView();
	assert(isLinkedToView(current));

	TabParams* params = h_viewParams[current];
	params->renderFaces = b;
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
Q_EXPORT_PLUGIN2(RenderPlugin, RenderPlugin)
#else
Q_EXPORT_PLUGIN2(RenderPluginD, RenderPlugin)
#endif
