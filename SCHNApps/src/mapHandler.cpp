#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

MapHandlerGen::MapHandlerGen(const QString& name, SCHNApps* s, GenericMap* map) :
	m_name(name),
	m_schnapps(s),
	m_map(map),
	m_bbDrawer(NULL),
	m_frame(NULL),
	m_render(NULL)
{
	m_frame = new qglviewer::ManipulatedFrame();
}

MapHandlerGen::~MapHandlerGen()
{
	if(m_bbDrawer)
		delete m_bbDrawer;
	if(m_render)
		delete m_render;
	foreach(CGoGN::Utils::VBO* vbo, m_vbo)
		delete vbo;
}

/*********************************************************
 * MANAGE VBOs
 *********************************************************/

Utils::VBO* MapHandlerGen::createVBO(const AttributeMultiVectorGen* attr)
{
	if(attr)
	{
		QString name = QString::fromStdString(attr->getName());
		Utils::VBO* vbo = getVBO(name);
		if(!vbo)
		{
			vbo = new Utils::VBO(attr->getName());
			vbo->updateData(attr);
			m_vbo.insert(name, vbo);
			emit(vboAdded(vbo));
		}
		else
		{
			vbo->updateData(attr);
		}
		return vbo;
	}
	else
		return NULL;
}

Utils::VBO* MapHandlerGen::createVBO(const AttributeHandlerGen& attr)
{
	return createVBO(attr.getDataVectorGen());
}

Utils::VBO* MapHandlerGen::createVBO(const QString& name)
{
	return createVBO(static_cast<AttribMap*>(m_map)->getAttributeVectorGen(VERTEX, name.toUtf8().constData()));
}

void MapHandlerGen::updateVBO(const AttributeMultiVectorGen* attr)
{
	if(attr)
	{
		Utils::VBO* vbo = getVBO(QString::fromStdString(attr->getName()));
		if(vbo)
			vbo->updateData(attr);
	}
}

void MapHandlerGen::updateVBO(const AttributeHandlerGen& attr)
{
	return updateVBO(attr.getDataVectorGen());
}

void MapHandlerGen::updateVBO(const QString& name)
{
	updateVBO(static_cast<AttribMap*>(m_map)->getAttributeVectorGen(VERTEX, name.toUtf8().constData()));
}

Utils::VBO* MapHandlerGen::getVBO(const QString& name) const
{
	if (m_vbo.contains(name))
		return m_vbo[name];
	else
		return NULL;
}

void MapHandlerGen::deleteVBO(const QString& name)
{
	if (m_vbo.contains(name))
	{
		Utils::VBO* vbo = m_vbo[name];
		m_vbo.remove(name);
		emit(vboRemoved(vbo));
		delete vbo;
	}
}

/*********************************************************
 * MANAGE CELL SELECTORS
 *********************************************************/

CellSelectorGen* MapHandlerGen::addCellSelector(unsigned int orbit, const QString& name)
{
	if(m_cellSelectors[orbit].contains(name))
		return NULL;

	CellSelectorGen* cs = NULL;

	switch(orbit)
	{
		case DART: cs = new CellSelector<DART>(*m_map, name); break;
		case VERTEX: cs = new CellSelector<VERTEX>(*m_map, name); break;
		case EDGE: cs = new CellSelector<EDGE>(*m_map, name); break;
		case FACE: cs = new CellSelector<FACE>(*m_map, name); break;
		case VOLUME: cs = new CellSelector<VOLUME>(*m_map, name); break;
	}

	if(!cs)
		return NULL;

	m_cellSelectors[orbit].insert(name, cs);
	emit(cellSelectorAdded(orbit, name));
	return cs;
}

void MapHandlerGen::removeCellSelector(unsigned int orbit, const QString& name)
{
	CellSelectorGen* cs = getCellSelector(orbit, name);
	if (cs)
	{
		m_cellSelectors[orbit].remove(name);
		emit(cellSelectorRemoved(orbit, name));
		delete cs;
	}
}

CellSelectorGen* MapHandlerGen::getCellSelector(unsigned int orbit, const QString& name) const
{
	if (m_cellSelectors[orbit].contains(name))
		return m_cellSelectors[orbit][name];
	else
		return NULL;
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
