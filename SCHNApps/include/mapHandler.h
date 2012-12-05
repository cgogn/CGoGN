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

	virtual void draw(CGoGN::Utils::GLSLShader* shader, int primitive) = 0;

	const QString& getName() { return m_name; }
	void setName(const QString& name) { m_name = name; }

	Window* getWindow() { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	const qglviewer::Vec& getBBmin() const { return m_bbMin; }
	void setBBmin(qglviewer::Vec& v) { m_bbMin = v; }

	const qglviewer::Vec& getBBmax() const { return m_bbMax; }
	void setBBmax(qglviewer::Vec& v) { m_bbMax = v; }

	/*********************************************************
	 * MANAGE VBOs
	 *********************************************************/

	CGoGN::Utils::VBO* getVBO(const std::string& name);
	void deleteVBO(const std::string& name);
//	CGoGN::Utils::VBO* addVBO(const QString& name);
//	CGoGN::Utils::VBO* findFirstVBOMatching(const QRegExp& regexp);
//	QList<CGoGN::Utils::VBO*> findVBOsMatching(const QRegExp& regexp);

	int getNbVBO() { return h_vbo.count(); }

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

	bool linkView(View* view);
	void unlinkView(View* view);
	bool isLinkedToView(View* view) { return l_views.contains(view); }
	QList<View*> getLinkedViews() { return l_views; }

protected:
	QString m_name;
	Window* m_window;

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
	{
		m_render = new CGoGN::Algo::Render::GL2::MapRender();
	}

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

	void draw(CGoGN::Utils::GLSLShader* shader, int primitive)
	{
		m_render->draw(shader, primitive);
	}

protected:
	typename PFP::MAP* m_map;
	CGoGN::Algo::Render::GL2::MapRender* m_render;
};

#endif
