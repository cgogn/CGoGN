#ifndef _MAPHANDLER_H_
#define _MAPHANDLER_H_

#include <QString>

#include "types.h"
#include "view.h"

#include "Topology/generic/genericmap.h"
#include "Topology/generic/functor.h"
#include "Utils/vbo.h"
#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Geometry/boundingbox.h"

namespace CGoGN
{

namespace SCHNApps
{

class MapHandlerGen
{
public:
	MapHandlerGen(const QString& name, Window* window, GenericMap* map);
	virtual ~MapHandlerGen();

	const QString& getName() const { return m_name; }
	void setName(const QString& name) { m_name = name; }

	Window* getWindow() const { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	GenericMap* getGenericMap() { return m_map; }

	const qglviewer::Vec& getBBmin() const { return m_bbMin; }
	const qglviewer::Vec& getBBmax() const { return m_bbMax; }
	float getBBdiagSize() { return m_bbDiagSize; }

	bool isUsed() const { return !l_views.empty(); }

	void draw(Utils::GLSLShader* shader, int primitive) { m_render->draw(shader, primitive); }

	/*********************************************************
	 * MANAGE VBOs
	 *********************************************************/

	template <typename ATTR_HANDLER>
	Utils::VBO* createVBO(const ATTR_HANDLER& attr)
	{
		Utils::VBO* vbo = getVBO(QString::fromStdString(attr.name()));
		vbo->updateData(attr);
		return vbo;
	}

	Utils::VBO* getVBO(const QString& name);
	QList<Utils::VBO*> getVBOList() const { return h_vbo.values(); }
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

	typename PFP::MAP* getMap() { return reinterpret_cast<typename PFP::MAP*>(m_map); }

	void updateBB(const VertexAttribute<typename PFP::VEC3>& position)
	{
		CGoGN::Geom::BoundingBox<typename PFP::VEC3> bb = CGoGN::Algo::Geometry::computeBoundingBox<PFP>(*(reinterpret_cast<typename PFP::MAP*>(m_map)), position);
		m_bbMin = qglviewer::Vec(bb.min()[0], bb.min()[1], bb.min()[2]);
		m_bbMax = qglviewer::Vec(bb.max()[0], bb.max()[1], bb.max()[2]);
		m_bbDiagSize = (m_bbMax - m_bbMin).norm();
	}

	void updatePrimitives(int primitive, const FunctorSelect& good)
	{
		m_render->initPrimitives<PFP>(*(reinterpret_cast<typename PFP::MAP*>(m_map)), good, primitive) ;
	}
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
