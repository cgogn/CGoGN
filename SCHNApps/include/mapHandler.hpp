
namespace CGoGN
{

namespace SCHNApps
{

template <typename T, unsigned int ORBIT>
AttributeHandler<T, ORBIT> MapHandlerGen::getAttribute(const QString& nameAttr, bool onlyRegistered) const
{
	if(onlyRegistered)
	{
		if(m_attribs[ORBIT].contains(nameAttr))
			return static_cast<AttribMap*>(m_map)->getAttribute<T,ORBIT>(nameAttr.toStdString());
		else
			return AttributeHandler<T, ORBIT>();
	}
	else
		return static_cast<AttribMap*>(m_map)->getAttribute<T,ORBIT>(nameAttr.toStdString());
}

template <typename T, unsigned int ORBIT>
AttributeHandler<T, ORBIT> MapHandlerGen::addAttribute(const QString& nameAttr, bool registerAttr)
{
	AttributeHandler<T,ORBIT> ah = static_cast<AttribMap*>(m_map)->addAttribute<T,ORBIT>(nameAttr.toStdString());
	if(ah.isValid() && registerAttr)
	{
		registerAttribute(ah);
		emit(attributeAdded(ORBIT, nameAttr));
	}
	return ah;
}

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
void MapHandler<PFP>::draw(Utils::GLSLShader* shader, int primitive)
{
	if(!m_render)
		m_render = new Algo::Render::GL2::MapRender();

	if(!m_render->isPrimitiveUpToDate(primitive))
		m_render->initPrimitives<PFP>(*(static_cast<typename PFP::MAP*>(m_map)), primitive) ;

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
	QGLViewer::drawAxis();
	m_bbDrawer->callList();
	glPopMatrix();
}

template <typename PFP>
void MapHandler<PFP>::updateBB(const VertexAttribute<typename PFP::VEC3>& position)
{
	m_bb = CGoGN::Algo::Geometry::computeBoundingBox<PFP>(*(static_cast<typename PFP::MAP*>(m_map)), position);
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

} // namespace SCHNApps

} // namespace CGoGN
