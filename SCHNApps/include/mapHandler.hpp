#include "Utils/Shaders/shaderColorPerVertex.h"
#include "Utils/Shaders/shaderSimpleColor.h"

namespace CGoGN
{

namespace SCHNApps
{

inline void MapHandlerGen::registerAttribute(const AttributeHandlerGen& ah)
{
	m_attribs[ah.getOrbit()].insert(QString::fromStdString(ah.name()), QString::fromStdString(ah.typeName()));
}

inline QString MapHandlerGen::getAttributeTypeName(unsigned int orbit, const QString& nameAttr) const
{
	if(m_attribs[orbit].contains(nameAttr))
		return m_attribs[orbit][nameAttr];
	else
		return "";
}




template <typename PFP>
unsigned int MapHandler<PFP>::getNbDarts()
{
	return static_cast<MAP*>(m_map)->getNbDarts();
}

template <typename PFP>
unsigned int MapHandler<PFP>::getNbOrbits(unsigned int orbit)
{
	return Algo::Topo::getNbOrbits(*static_cast<MAP*>(m_map), orbit);
}

template <typename PFP>
template <typename T, unsigned int ORBIT>
AttributeHandler<T, ORBIT, typename PFP::MAP> MapHandler<PFP>::getAttribute(const QString& nameAttr, bool onlyRegistered) const
{
	if(onlyRegistered)
	{
		if(m_attribs[ORBIT].contains(nameAttr))
			return static_cast<MAP*>(m_map)->template getAttribute<T, ORBIT, MAP>(nameAttr.toStdString());
		else
			return AttributeHandler<T, ORBIT, MAP>();
	}
	else
		return static_cast<MAP*>(m_map)->template getAttribute<T, ORBIT, MAP>(nameAttr.toStdString());
}

template <typename PFP>
template <typename T, unsigned int ORBIT>
AttributeHandler<T, ORBIT, typename PFP::MAP> MapHandler<PFP>::addAttribute(const QString& nameAttr, bool registerAttr)
{
	AttributeHandler<T, ORBIT, MAP> ah = static_cast<MAP*>(m_map)->template addAttribute<T, ORBIT, MAP>(nameAttr.toStdString());
	if(ah.isValid() && registerAttr)
	{
		registerAttribute(ah);
		emit(attributeAdded(ORBIT, nameAttr));
	}
	return ah;
}

template <typename PFP>
void MapHandler<PFP>::draw(Utils::GLSLShader* shader, int primitive)
{
	if(!m_render)
		m_render = new Algo::Render::GL2::MapRender();

	if(!m_render->isPrimitiveUpToDate(primitive))
		m_render->initPrimitives<PFP>(*(static_cast<MAP*>(m_map)), primitive) ;

	glPushMatrix();
	glMultMatrixd(m_frame->matrix());
	m_render->draw(shader, primitive);
	glPopMatrix();
}

template <typename PFP>
void MapHandler<PFP>::drawBB()
{
	if(!m_bbDrawer)
	{
		m_bbDrawer = new Utils::Drawer();
		updateBBDrawer();
	}

	glPushMatrix();
	glMultMatrixd(m_frame->matrix());
//	QGLViewer::drawAxis();
	m_bbDrawer->callList();
	glPopMatrix();
}

template <typename PFP>
void MapHandler<PFP>::updateBB(const VertexAttribute<VEC3, MAP>& position)
{
	m_bb = CGoGN::Algo::Geometry::computeBoundingBox<PFP>(*(static_cast<MAP*>(m_map)), position);
	m_bbMin = qglviewer::Vec(m_bb.min()[0], m_bb.min()[1], m_bb.min()[2]);
	m_bbMax = qglviewer::Vec(m_bb.max()[0], m_bb.max()[1], m_bb.max()[2]);
	m_bbDiagSize = (m_bbMax - m_bbMin).norm();

	updateBBDrawer();
}

template <typename PFP>
void MapHandler<PFP>::updateBBDrawer()
{
	if(!m_bbDrawer)
		m_bbDrawer = new Utils::Drawer();

	const Geom::Vec3f& bbmin = m_bb.min();
	const Geom::Vec3f& bbmax = m_bb.max();

	m_bbDrawer->newList(GL_COMPILE);
	m_bbDrawer->color3f(0.0f,1.0f,0.0f);
	m_bbDrawer->lineWidth(1.0f);
	m_bbDrawer->begin(GL_LINE_LOOP);
	m_bbDrawer->vertex(bbmin);
	m_bbDrawer->vertex3f(bbmin[0], bbmax[1], bbmin[2]);
	m_bbDrawer->vertex3f(bbmax[0], bbmax[1], bbmin[2]);
	m_bbDrawer->vertex3f(bbmax[0], bbmin[1], bbmin[2]);
	m_bbDrawer->vertex(bbmin);
	m_bbDrawer->end();
	m_bbDrawer->begin(GL_LINE_LOOP);
	m_bbDrawer->vertex(bbmax);
	m_bbDrawer->vertex3f(bbmax[0], bbmin[1], bbmax[2]);
	m_bbDrawer->vertex3f(bbmin[0], bbmin[1], bbmax[2]);
	m_bbDrawer->vertex3f(bbmin[0], bbmax[1], bbmax[2]);
	m_bbDrawer->vertex(bbmax);
	m_bbDrawer->end();
	m_bbDrawer->begin(GL_LINES);
	m_bbDrawer->vertex(bbmin);
	m_bbDrawer->vertex3f(bbmin[0], bbmin[1], bbmax[2]);
	m_bbDrawer->vertex3f(bbmin[0], bbmax[1], bbmin[2]);
	m_bbDrawer->vertex3f(bbmin[0], bbmax[1], bbmax[2]);
	m_bbDrawer->vertex3f(bbmax[0], bbmax[1], bbmin[2]);
	m_bbDrawer->vertex(bbmax);
	m_bbDrawer->vertex3f(bbmax[0], bbmin[1], bbmin[2]);
	m_bbDrawer->vertex3f(bbmax[0], bbmin[1], bbmax[2]);
	m_bbDrawer->end();
	m_bbDrawer->endList();
}

template <typename PFP>
CellSelectorGen* MapHandler<PFP>::addCellSelector(unsigned int orbit, const QString& name)
{
	if(m_cellSelectors[orbit].contains(name))
		return NULL;

	CellSelectorGen* cs = NULL;
	MAP *m = static_cast<MAP*>(m_map);

	switch(orbit)
	{
		case DART: cs = new CellSelector<MAP, DART>(*m, name); break;
		case VERTEX: cs = new CellSelector<MAP, VERTEX>(*m, name); break;
		case EDGE: cs = new CellSelector<MAP, EDGE>(*m, name); break;
		case FACE: cs = new CellSelector<MAP, FACE>(*m, name); break;
		case VOLUME: cs = new CellSelector<MAP, VOLUME>(*m, name); break;
	}

	if(!cs)
		return NULL;

	m_cellSelectors[orbit].insert(name, cs);
	emit(cellSelectorAdded(orbit, name));

	connect(cs, SIGNAL(selectedCellsChanged()), this, SLOT(selectedCellsChanged()));

	return cs;
}

template <typename PFP>
template <unsigned int ORBIT>
CellSelector<typename PFP::MAP, ORBIT>* MapHandler<PFP>::getCellSelector(const QString& name) const
{
	if (m_cellSelectors[ORBIT].contains(name))
		return static_cast<CellSelector<MAP, ORBIT>*>(m_cellSelectors[ORBIT][name]);
	else
		return NULL;
}

template <typename PFP>
void MapHandler<PFP>::createTopoRender(CGoGN::Utils::GLSLShader* sh1)
{
//	std::cout << "MH:createTopo"<< std::endl;
	if (m_topoRender)
		return;

	if (m_map->dimension() == 2)
	{
		CGoGN::Utils::ShaderSimpleColor* ssc =static_cast<CGoGN::Utils::ShaderSimpleColor*>(sh1);

		m_topoRender = new Algo::Render::GL2::TopoRender(ssc);
		m_topoRender->setInitialDartsColor(0.25f, 0.25f, 0.25f) ;
	}
	else
		std::cerr << "TOPO3 NOT SUPPORTED"<< std::endl;
}

template <typename PFP>
void MapHandler<PFP>::updateTopoRender(const QString& positionName)
{
	if (m_topoRender==NULL)
		return;

	typename PFP::MAP* map = this->getMap();

	VertexAttribute<typename PFP::VEC3, typename PFP::MAP> position = map->getAttribute<VEC3, VERTEX, MAP>(positionName.toStdString()) ;
	if(position.isValid())
	{
		m_topoRender->updateData<PFP>(*map,position,false);

	}
}

template <typename PFP>
void MapHandler<PFP>::drawTopoRender(int code)
{
	if (m_topoRender == NULL)
		return;
	m_topoRender->drawTopo(code);
}




} // namespace SCHNApps

} // namespace CGoGN
