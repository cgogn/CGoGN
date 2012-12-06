#include "mapHandler.h"

#include "system.h"

MapHandlerGen::MapHandlerGen(const QString& name, Window* window) :
	m_name(name),
	m_window(window)
{
	m_render = new CGoGN::Algo::Render::GL2::MapRender();
}

MapHandlerGen::~MapHandlerGen()
{
	foreach(CGoGN::Utils::VBO* vbo, h_vbo)
		delete vbo;
}

void MapHandlerGen::draw(CGoGN::Utils::GLSLShader* shader, int primitive)
{
	m_render->draw(shader, primitive);
}

CGoGN::Utils::VBO* MapHandlerGen::getVBO(const std::string& name)
{
	if (h_vbo.contains(name))
		return h_vbo[name];
	else
	{
		CGoGN::Utils::VBO* vbo = new CGoGN::Utils::VBO();
		h_vbo.insert(name, vbo);
		return vbo;
	}
}

void MapHandlerGen::deleteVBO(const std::string& name)
{
	if (h_vbo.contains(name))
	{
		CGoGN::Utils::VBO* vbo = h_vbo[name];
		h_vbo.remove(name);
		delete vbo;
	}
}

/*********************************************************
 * MANAGE LINKED VIEWS
 *********************************************************/

bool MapHandlerGen::linkView(View* view)
{
	if(view && !l_views.contains(view))
	{
		l_views.push_back(view);
		view->updateGL();
		return true;
	}
	else
		return false;
}

void MapHandlerGen::unlinkView(View* view)
{
	if(l_views.removeOne(view))
		view->updateGL();
}
