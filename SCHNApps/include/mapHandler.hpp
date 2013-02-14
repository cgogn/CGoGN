
namespace CGoGN
{

namespace SCHNApps
{

template <typename T, unsigned int ORBIT>
AttributeHandler<T, ORBIT> MapHandlerGen::getAttribute(const QString& nameAttr, bool onlyRegistered) const
{
	if(onlyRegistered)
	{
		if(h_attribs[ORBIT].contains(nameAttr))
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

template <typename T, unsigned int ORBIT>
inline void MapHandlerGen::registerAttribute(const AttributeHandler<T, ORBIT>& ah)
{
	h_attribs[ORBIT].insert(QString::fromStdString(ah.name()), QString::fromStdString(nameOfType(T())));
}

inline QString MapHandlerGen::getAttributeTypeName(unsigned int orbit, const QString& nameAttr) const
{
	if(h_attribs[orbit].contains(nameAttr))
		return h_attribs[orbit][nameAttr];
	else
		return "";
}

template <typename ATTR_HANDLER>
Utils::VBO* MapHandlerGen::createVBO(const ATTR_HANDLER& attr)
{
	QString name = QString::fromStdString(attr.name());
	Utils::VBO* vbo = getVBO(name);
	if(!vbo)
	{
		vbo = new Utils::VBO();
		h_vbo.insert(name, vbo);
		emit(vboAdded(vbo));
	}
	return vbo;
}

template <typename ATTR_HANDLER>
inline void MapHandlerGen::updateVBO(const ATTR_HANDLER& attr)
{
	Utils::VBO* vbo = getVBO(QString::fromStdString(attr.name()));
	if(vbo)
		vbo->updateData(attr);
}



template <typename PFP>
void MapHandler<PFP>::draw(Utils::GLSLShader* shader, int primitive)
{
	if(!m_render->isPrimitiveUpToDate(primitive))
		m_render->initPrimitives<PFP>(*(static_cast<typename PFP::MAP*>(m_map)), allDarts, primitive) ;
	m_render->draw(shader, primitive);
}

template <typename PFP>
void MapHandler<PFP>::updateBB(const VertexAttribute<typename PFP::VEC3>& position)
{
	CGoGN::Geom::BoundingBox<typename PFP::VEC3> bb = CGoGN::Algo::Geometry::computeBoundingBox<PFP>(*(static_cast<typename PFP::MAP*>(m_map)), position);
	m_bbMin = qglviewer::Vec(bb.min()[0], bb.min()[1], bb.min()[2]);
	m_bbMax = qglviewer::Vec(bb.max()[0], bb.max()[1], bb.max()[2]);
	m_bbDiagSize = (m_bbMax - m_bbMin).norm();
}

} // namespace SCHNApps

} // namespace CGoGN
