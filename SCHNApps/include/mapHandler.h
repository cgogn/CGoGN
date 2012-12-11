#ifndef _MAPHANDLER_H_
#define _MAPHANDLER_H_

#include <QString>

#include "types.h"
#include "view.h"

#include "Topology/generic/genericmap.h"
#include "Topology/generic/functor.h"
#include "Utils/vbo.h"
#include "Algo/Render/GL2/mapRender.h"

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
	void setBBmin(qglviewer::Vec& v) { m_bbMin = v; }

	const qglviewer::Vec& getBBmax() const { return m_bbMax; }
	void setBBmax(qglviewer::Vec& v) { m_bbMax = v; }

	float getBBdiagSize() { return (m_bbMax - m_bbMin).norm(); }

	bool isUsed() const { return !l_views.empty(); }

	void draw(Utils::GLSLShader* shader, int primitive);

	/*********************************************************
	 * MANAGE VBOs
	 *********************************************************/

	Utils::VBO* getVBO(const std::string& name);
	void deleteVBO(const std::string& name);

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

	void updatePrimitives(int primitive, const FunctorSelect& good)
	{
		m_render->initPrimitives<PFP>(*(reinterpret_cast<typename PFP::MAP*>(m_map)), good, primitive) ;
	}
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
