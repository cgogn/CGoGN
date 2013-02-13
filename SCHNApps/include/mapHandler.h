#ifndef _MAPHANDLER_H_
#define _MAPHANDLER_H_

#include <QString>

#include "types.h"
#include "view.h"
#include "plugin.h"

#include "Topology/generic/genericmap.h"
#include "Topology/generic/attribmap.h"
#include "Topology/generic/functor.h"
#include "Topology/generic/attributeHandler.h"
#include "Utils/vbo.h"
#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Geometry/boundingbox.h"

namespace CGoGN
{

namespace SCHNApps
{

class MapHandlerGen : public QObject
{
	Q_OBJECT

public:
	MapHandlerGen(const QString& name, Window* window, GenericMap* map);
	virtual ~MapHandlerGen();

	const QString& getName() const { return m_name; }

public slots:
	QString getName() { return m_name; }
	void setName(const QString& name) { m_name = name; }

	Window* getWindow() const { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	GenericMap* getGenericMap() const { return m_map; }

	const qglviewer::Vec& getBBmin() const { return m_bbMin; }
	const qglviewer::Vec& getBBmax() const { return m_bbMax; }
	float getBBdiagSize() const { return m_bbDiagSize; }

	bool isUsed() const { return !l_views.empty(); }

public:
	virtual void draw(Utils::GLSLShader* shader, int primitive) = 0;

	void setPrimitiveDirty(int primitive)
	{
		m_render->setPrimitiveDirty(primitive);
	}

	/*********************************************************
	 * MANAGE ATTRIBUTES
	 *********************************************************/

	template <typename T, unsigned int ORBIT>
	AttributeHandler<T, ORBIT> getAttribute(const QString& nameAttr, bool onlyRegistered = true) const
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
	AttributeHandler<T, ORBIT> addAttribute(const QString& nameAttr, bool registerAttr = true)
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
	void registerAttribute(const AttributeHandler<T, ORBIT>& ah)
	{
		h_attribs[ORBIT].insert(QString::fromStdString(ah.name()), QString::fromStdString(nameOfType(T())));
	}

	QString getAttributeTypeName(unsigned int orbit, const QString& nameAttr) const
	{
		if(h_attribs[orbit].contains(nameAttr))
			return h_attribs[orbit][nameAttr];
		else
			return "";
	}

	const AttributeHash& getAttributesList(unsigned int orbit) const { return h_attribs[orbit];	}

	/*********************************************************
	 * MANAGE VBOs
	 *********************************************************/

	template <typename ATTR_HANDLER>
	Utils::VBO* createVBO(const ATTR_HANDLER& attr)
	{
		QString name = QString::fromStdString(attr.name());
		Utils::VBO* vbo = getVBO(name);
		if(!vbo)
		{
			vbo = new Utils::VBO();
			h_vbo.insert(name, vbo);
		}
		vbo->updateData(attr);
		emit(vboAdded(vbo));
		return vbo;
	}

	template <typename ATTR_HANDLER>
	void updateVBO(const ATTR_HANDLER& attr)
	{
		Utils::VBO* vbo = getVBO(QString::fromStdString(attr.name()));
		if(vbo)
			vbo->updateData(attr);
	}

	Utils::VBO* getVBO(const QString& name) const;
	QList<Utils::VBO*> getVBOList() const { return h_vbo.values(); }
	QList<Utils::VBO*> getVBOList(const std::string& typeName) const;
	void deleteVBO(const QString& name);

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

	void linkView(View* view);
	void unlinkView(View* view);
	const QList<View*>& getLinkedViews() const { return l_views; }
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

protected:
	QString m_name;
	Window* m_window;
	GenericMap* m_map;

	Algo::Render::GL2::MapRender* m_render;

	qglviewer::Vec m_bbMin;
	qglviewer::Vec m_bbMax;
	float m_bbDiagSize;

	QList<View*> l_views;

	VBOHash h_vbo;
	AttributeHash h_attribs[NB_ORBITS];

signals:
	void attributeAdded(unsigned int orbit, const QString& name);
	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);
};

template <typename PFP>
class MapHandler : public MapHandlerGen
{
public:
	MapHandler(const QString& name, Window* window, typename PFP::MAP* map) :
		MapHandlerGen(name, window, map)
	{}

	~MapHandler()
	{
		if (m_map)
			delete m_map;
	}

	virtual void draw(Utils::GLSLShader* shader, int primitive)
	{
		if(!m_render->isPrimitiveUpToDate(primitive))
			m_render->initPrimitives<PFP>(*(static_cast<typename PFP::MAP*>(m_map)), allDarts, primitive) ;
		m_render->draw(shader, primitive);
	}

	typename PFP::MAP* getMap() { return static_cast<typename PFP::MAP*>(m_map); }

	void updateBB(const VertexAttribute<typename PFP::VEC3>& position)
	{
		CGoGN::Geom::BoundingBox<typename PFP::VEC3> bb = CGoGN::Algo::Geometry::computeBoundingBox<PFP>(*(static_cast<typename PFP::MAP*>(m_map)), position);
		m_bbMin = qglviewer::Vec(bb.min()[0], bb.min()[1], bb.min()[2]);
		m_bbMax = qglviewer::Vec(bb.max()[0], bb.max()[1], bb.max()[2]);
		m_bbDiagSize = (m_bbMax - m_bbMin).norm();
	}
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
