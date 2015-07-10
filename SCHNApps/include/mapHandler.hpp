#include "Utils/Shaders/shaderColorPerVertex.h"
#include "Utils/Shaders/shaderSimpleColor.h"

#include "Geometry/bounding_box.h"

namespace CGoGN
{

namespace SCHNApps
{



inline void MapHandlerGen::registerAttribute(const AttributeHandlerGen& ah)
{
	m_attribs[ah.getOrbit()].insert(QString::fromStdString(ah.name()), QString::fromStdString(ah.typeName()));
}

inline void MapHandlerGen::registerAttribute(unsigned int orbit, const QString& name, const QString& typeName)
{
	m_attribs[orbit].insert(name, typeName);
}

inline QString MapHandlerGen::getAttributeTypeName(unsigned int orbit, const QString& nameAttr) const
{
	if(m_attribs[orbit].contains(nameAttr))
		return m_attribs[orbit][nameAttr];
	else
		return "";
}





template <typename PFP>
MapHandler<PFP>::~MapHandler()
{
	// clean  the cell selector
	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		foreach(CellSelectorGen* cs, m_cellSelectors[orbit])
		{
			if (cs)
			{
				emit(cellSelectorRemoved(orbit, cs->getName()));
				disconnect(cs, SIGNAL(selectedCellsChanged()), this, SLOT(selectedCellsChanged()));
				delete cs;
			}
		}
		m_cellSelectors[orbit].clear();
	}

	if (m_map)
		delete m_map;
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
		DEBUG_EMIT("attributeAdded");
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

	m_render->draw(shader, primitive);
}

template <typename PFP>
void MapHandler<PFP>::drawBB()
{
	if (m_showBB && m_bbDrawer)
		m_bbDrawer->callList();
}

template <typename PFP>
void MapHandler<PFP>::updateBB()
{
	if (m_bbVertexAttribute)
	{
		MAP* map = static_cast<MAP*>(m_map);
		VertexAttribute<VEC3, MAP> bbVertexAttribute(map, dynamic_cast<AttributeMultiVector<VEC3>*>(m_bbVertexAttribute));

		m_bb = CGoGN::Algo::Geometry::computeBoundingBox<PFP>(*map, bbVertexAttribute);
		if (m_bb.isInitialized())
			m_bbDiagSize = m_bb.diagSize();
		else
			m_bbDiagSize = 0;
	}
	else
	{
		m_bb.reset();
		m_bbDiagSize = 0;
	}

	updateBBDrawer();

	DEBUG_EMIT("boundingBoxModified");
	emit(boundingBoxModified());
}

template <typename PFP>
void MapHandler<PFP>::updateBBDrawer()
{
	if (!m_bbDrawer)
		m_bbDrawer = new Utils::Drawer(1);

	if (m_bb.isInitialized())
	{
		Geom::Vec3f bbmin = m_bb.min();
		Geom::Vec3f bbmax = m_bb.max();
		float shift = 0.005f * (bbmax - bbmin).norm();
		bbmin -= Geom::Vec3f(shift, shift, shift);
		bbmax += Geom::Vec3f(shift, shift, shift);

		m_bbDrawer->newList(GL_COMPILE);
		m_bbDrawer->color3f(m_bbColor[0], m_bbColor[1], m_bbColor[2]);
		m_bbDrawer->lineWidth(2.0f);
//		m_bbDrawer->lineWidth(shift);
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
	else
	{
		m_bbDrawer->newList();
		m_bbDrawer->endList();
	}
}

template <typename PFP>
bool MapHandler<PFP>::transformedBB(qglviewer::Vec& bbMin, qglviewer::Vec& bbMax)
{
	if (!m_bb.isInitialized())
		return false;

	const Geom::Vec3f& BBmin = m_bb.min();
	const Geom::Vec3f& BBmax = m_bb.max();

	CGoGN::Geom::BoundingBox<typename PFP::VEC3> bb;

//	qglviewer::Vec v  = qglviewer::Vec(BBmin[0], BBmin[1], BBmin[2]);
	glm::vec4 v4 = this->m_transfoMatrix* glm::vec4(BBmin[0], BBmin[1], BBmin[2], 1.0f);
	qglviewer::Vec v = qglviewer::Vec(v4[0], v4[1], v4[2]);
	qglviewer::Vec vt = m_frame->inverseCoordinatesOf(v);
	bb.addPoint(Geom::Vec3f(vt[0], vt[1], vt[2]));

//	v  = qglviewer::Vec(BBmax[0], BBmin[1], BBmin[2]);
	v4 = this->m_transfoMatrix* glm::vec4(BBmax[0], BBmin[1], BBmin[2], 1.0f);
	v = qglviewer::Vec(v4[0], v4[1], v4[2]);
	vt = m_frame->inverseCoordinatesOf(v);
	bb.addPoint(Geom::Vec3f(vt[0], vt[1], vt[2]));

//	v  = qglviewer::Vec(BBmin[0], BBmax[1], BBmin[2]);
	v4 = this->m_transfoMatrix* glm::vec4(BBmin[0], BBmax[1], BBmin[2], 1.0f);
	v = qglviewer::Vec(v4[0], v4[1], v4[2]);
	vt = m_frame->inverseCoordinatesOf(v);
	bb.addPoint(Geom::Vec3f(vt[0], vt[1], vt[2]));

//	v  = qglviewer::Vec(BBmin[0], BBmin[1], BBmax[2]);
	v4 = this->m_transfoMatrix* glm::vec4(BBmin[0], BBmin[1], BBmax[2], 1.0f);
	v = qglviewer::Vec(v4[0], v4[1], v4[2]);
	vt = m_frame->inverseCoordinatesOf(v);
	bb.addPoint(Geom::Vec3f(vt[0], vt[1], vt[2]));

//	v  = qglviewer::Vec(BBmax[0], BBmax[1], BBmin[2]);
	v4 = this->m_transfoMatrix* glm::vec4(BBmax[0], BBmax[1], BBmin[2], 1.0f);
	v = qglviewer::Vec(v4[0], v4[1], v4[2]);
	vt = m_frame->inverseCoordinatesOf(v);
	bb.addPoint(Geom::Vec3f(vt[0], vt[1], vt[2]));

//	v  = qglviewer::Vec(BBmax[0], BBmin[1], BBmax[2]);
	v4 = this->m_transfoMatrix* glm::vec4(BBmax[0], BBmin[1], BBmax[2], 1.0f);
	v = qglviewer::Vec(v4[0], v4[1], v4[2]);
	vt = m_frame->inverseCoordinatesOf(v);
	bb.addPoint(Geom::Vec3f(vt[0], vt[1], vt[2]));

//	v  = qglviewer::Vec(BBmin[0], BBmax[1], BBmax[2]);
	v4 = this->m_transfoMatrix* glm::vec4(BBmin[0], BBmax[1], BBmax[2], 1.0f);
	v = qglviewer::Vec(v4[0], v4[1], v4[2]);
	vt = m_frame->inverseCoordinatesOf(v);
	bb.addPoint(Geom::Vec3f(vt[0], vt[1], vt[2]));

//	v  = qglviewer::Vec(BBmax[0], BBmax[1], BBmax[2]);
	v4 = this->m_transfoMatrix* glm::vec4(BBmax[0], BBmax[1], BBmax[2], 1.0f);
	v = qglviewer::Vec(v4[0], v4[1], v4[2]);
	vt = m_frame->inverseCoordinatesOf(v);
	bb.addPoint(Geom::Vec3f(vt[0], vt[1], vt[2]));

	bbMin = qglviewer::Vec(bb.min()[0], bb.min()[1], bb.min()[2]);
	bbMax = qglviewer::Vec(bb.max()[0], bb.max()[1], bb.max()[2]);

	return true;
}

template <typename PFP>
void MapHandler<PFP>::createTopoRender(std::vector<CGoGN::Utils::GLSLShader*> s)
{
//	std::cout << "MH:createTopo"<< std::endl;
	if (m_topoRender)
		return;

	if (m_map->dimension() == 2)
	{
//		CGoGN::Utils::ShaderSimpleColor* ssc = static_cast<CGoGN::Utils::ShaderSimpleColor*>(s);
		m_topoRender = new Algo::Render::GL2::TopoRender(s);
		m_topoRender->setInitialDartsColor(0.25f, 0.25f, 0.25f) ;
	}
	else
		std::cerr << "TOPO3 NOT SUPPORTED"<< std::endl;
}

template <typename PFP>
void MapHandler<PFP>::updateTopoRender(const QString& positionAttributeName)
{
	if (!m_topoRender)
		return;

	typename PFP::MAP* map = this->getMap();

	VertexAttribute<typename PFP::VEC3, typename PFP::MAP> position = map->template getAttribute<VEC3, VERTEX, MAP>(positionAttributeName.toStdString());
	if(position.isValid())
		m_topoRender->updateData<PFP>(*map, position, false);
}

template <typename PFP>
void MapHandler<PFP>::drawTopoRender(int code)
{
	if (!m_topoRender)
		return;

	m_topoRender->drawTopo(code);
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
	DEBUG_EMIT("cellSelectorAdded");
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

} // namespace SCHNApps

} // namespace CGoGN
