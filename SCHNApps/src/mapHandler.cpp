#include "mapHandler.h"

#include "system.h"
#include "Topology/generic/genericmap.h"
#include "Utils/vbo.h"

MapHandler::MapHandler(const QString& name, Window* window, CGoGN::GenericMap *map) :
	m_name(name),
	m_window(window),
	m_map(map)
{
//	connect(m_window, SIGNAL(viewRemoved(View*)), this, SLOT(cb_viewRemoved(View*)));
}

MapHandler::~MapHandler()
{
	foreach(CGoGN::Utils::VBO* vbo, h_vbo)
		delete vbo;

	if (m_map)
		delete m_map;
}

CGoGN::Utils::VBO* MapHandler::addVBO(const QString& name)
{
	if (h_vbo.contains(name))
	{
		System::Error::code = System::Error::VBO_EXISTS;
		return NULL;
	}

	CGoGN::Utils::VBO* vbo = new CGoGN::Utils::VBO();
	h_vbo.insert(name, vbo);
	return vbo;
}

void MapHandler::removeVBO(const QString& name)
{
	if (h_vbo.contains(name))
	{
		CGoGN::Utils::VBO* vbo = h_vbo[name];
		h_vbo.remove(name);
		delete vbo;
	}
}

CGoGN::Utils::VBO* MapHandler::getVBO(const QString& name)
{
	if (h_vbo.contains(name))
		return h_vbo[name];
	else
	{
		System::Error::code = System::Error::VBO_DOES_NOT_EXIST;
		return NULL;
	}
}

CGoGN::Utils::VBO* MapHandler::findFirstVBOMatching(const QRegExp& regexp)
{
	QHash<QString, CGoGN::Utils::VBO*>::iterator it;
	for (it = h_vbo.begin(); it != h_vbo.end(); ++it)
	{
		if (it.key().contains(regexp))
			return it.value();
	}
	return NULL;
}

QList<CGoGN::Utils::VBO*> MapHandler::findVBOsMatching(const QRegExp& regexp)
{
	QList<CGoGN::Utils::VBO*> rlist;
	QHash<QString, CGoGN::Utils::VBO*>::iterator it;
	for (it = h_vbo.begin(); it != h_vbo.end(); ++it)
	{
		if (it.key().contains(regexp))
			rlist.push_back(it.value());
	}
	return rlist;
}

/*********************************************************
 * MANAGE VIEWS
 *********************************************************/

bool MapHandler::linkView(View* view)
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

void MapHandler::unlinkView(View* view)
{
	if(l_views.removeOne(view))
	{
		view->updateGL();
	}
}

void MapHandler::cb_viewRemoved(View* view)
{
	unlinkView(view);
}
