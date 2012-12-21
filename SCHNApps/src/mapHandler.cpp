#include "mapHandler.h"

#include "system.h"

namespace CGoGN
{

namespace SCHNApps
{

MapHandlerGen::MapHandlerGen(const QString& name, Window* window, GenericMap* map) :
	m_name(name),
	m_window(window),
	m_map(map)
{
	m_render = new Algo::Render::GL2::MapRender();
}

MapHandlerGen::~MapHandlerGen()
{
	foreach(CGoGN::Utils::VBO* vbo, h_vbo)
		delete vbo;
}

/*********************************************************
 * MANAGE VBOs
 *********************************************************/

Utils::VBO* MapHandlerGen::getVBO(const QString& name)
{
	if (h_vbo.contains(name))
		return h_vbo[name];
	else
	{
		Utils::VBO* vbo = new Utils::VBO();
		h_vbo.insert(name, vbo);
		return vbo;
	}
}

void MapHandlerGen::deleteVBO(const QString& name)
{
	if (h_vbo.contains(name))
	{
		Utils::VBO* vbo = h_vbo[name];
		h_vbo.remove(name);
		delete vbo;
	}
}

/*********************************************************
 * MANAGE LINKED VIEWS
 *********************************************************/

void MapHandlerGen::linkView(View* view)
{
	if(view && !l_views.contains(view))
		l_views.push_back(view);
}

void MapHandlerGen::unlinkView(View* view)
{
	l_views.removeOne(view);
}

} // namespace SCHNApps

} // namespace CGoGN
