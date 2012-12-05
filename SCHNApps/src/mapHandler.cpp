#include "mapHandler.h"

#include "system.h"

MapHandlerGen::MapHandlerGen(const QString& name, Window* window) :
	m_name(name),
	m_window(window)
{}

MapHandlerGen::~MapHandlerGen()
{
	foreach(CGoGN::Utils::VBO* vbo, h_vbo)
		delete vbo;
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

//CGoGN::Utils::VBO* MapHandlerGen::addVBO(const QString& name)
//{
//	if (h_vbo.contains(name))
//	{
//		System::Error::code = System::Error::VBO_EXISTS;
//		return NULL;
//	}
//
//	CGoGN::Utils::VBO* vbo = new CGoGN::Utils::VBO();
//	h_vbo.insert(name, vbo);
//	return vbo;
//}
//
//void MapHandlerGen::removeVBO(const QString& name)
//{
//	if (h_vbo.contains(name))
//	{
//		CGoGN::Utils::VBO* vbo = h_vbo[name];
//		h_vbo.remove(name);
//		delete vbo;
//	}
//}
//
//CGoGN::Utils::VBO* MapHandlerGen::getVBO(const QString& name)
//{
//	if (h_vbo.contains(name))
//		return h_vbo[name];
//	else
//		return addVBO(name);
//}
//
//CGoGN::Utils::VBO* MapHandlerGen::findFirstVBOMatching(const QRegExp& regexp)
//{
//	QHash<QString, CGoGN::Utils::VBO*>::iterator it;
//	for (it = h_vbo.begin(); it != h_vbo.end(); ++it)
//	{
//		if (it.key().contains(regexp))
//			return it.value();
//	}
//	return NULL;
//}
//
//QList<CGoGN::Utils::VBO*> MapHandlerGen::findVBOsMatching(const QRegExp& regexp)
//{
//	QList<CGoGN::Utils::VBO*> rlist;
//	QHash<QString, CGoGN::Utils::VBO*>::iterator it;
//	for (it = h_vbo.begin(); it != h_vbo.end(); ++it)
//	{
//		if (it.key().contains(regexp))
//			rlist.push_back(it.value());
//	}
//	return rlist;
//}

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
	{
		view->updateGL();
	}
}
