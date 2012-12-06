#ifndef _MAPHANDLER_H_
#define _MAPHANDLER_H_

#include <QString>

#include "types.h"
#include "view.h"

#include "Topology/generic/functor.h"
#include "Utils/vbo.h"
#include "Algo/Render/GL2/mapRender.h"

class MapHandlerGen
{
public:
	MapHandlerGen(const QString& name, Window* window);
	virtual ~MapHandlerGen();

	const QString& getName() const { return m_name; }
	void setName(const QString& name) { m_name = name; }

	Window* getWindow() const { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	const qglviewer::Vec& getBBmin() const { return m_bbMin; }
	void setBBmin(qglviewer::Vec& v) { m_bbMin = v; }

	const qglviewer::Vec& getBBmax() const { return m_bbMax; }
	void setBBmax(qglviewer::Vec& v) { m_bbMax = v; }

	float getBBdiagSize() { return (m_bbMax - m_bbMin).norm(); }

	void draw(CGoGN::Utils::GLSLShader* shader, int primitive);

	/*********************************************************
	 * MANAGE VBOs
	 *********************************************************/

	CGoGN::Utils::VBO* getVBO(const std::string& name);
	void deleteVBO(const std::string& name);

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

	bool linkView(View* view);
	void unlinkView(View* view);
	const QList<View*>& getLinkedViews() const { return l_views; }
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

protected:
	QString m_name;
	Window* m_window;

	CGoGN::Algo::Render::GL2::MapRender* m_render;

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
		MapHandlerGen(name, window),
		m_map(map)
	{}

	~MapHandler()
	{
		if (m_map)
			delete m_map;
	}

	typename PFP::MAP* getMap() { return m_map; }

	void updatePrimitives(int primitive, const CGoGN::FunctorSelect& good)
	{
		m_render->initPrimitives<PFP>(*m_map, good, primitive) ;
	}

protected:
	typename PFP::MAP* m_map;
};

#endif
