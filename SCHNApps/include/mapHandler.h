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
#include "Utils/drawer.h"
#include "Algo/Geometry/boundingbox.h"

namespace CGoGN
{

namespace SCHNApps
{

class MapHandlerGen : public QObject
{
	Q_OBJECT

	friend class View;

public:
	MapHandlerGen(const QString& name, SCHNApps* s, GenericMap* map);
	virtual ~MapHandlerGen();

	const QString& getName() const { return m_name; }

public slots:
	QString getName() { return m_name; }
	SCHNApps* getSCHNApps() const { return m_schnapps; }

	bool isSelectedMap() const { return m_schnapps->getSelectedMap() == this; }

	GenericMap* getGenericMap() const { return m_map; }

	const qglviewer::Vec& getBBmin() const { return m_bbMin; }
	const qglviewer::Vec& getBBmax() const { return m_bbMax; }
	float getBBdiagSize() const { return m_bbDiagSize; }
	Utils::GLSLShader* getBBDrawerShader() const
	{
		if(m_bbDrawer)
			return m_bbDrawer->getShader();
		else
			return NULL;
	}

	const QList<View*>& getLinkedViews() const { return l_views; }
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

public:
	virtual void draw(Utils::GLSLShader* shader, int primitive) = 0;
	virtual void drawBB() = 0;

	void setPrimitiveDirty(int primitive) {	m_render->setPrimitiveDirty(primitive);	}

	/*********************************************************
	 * MANAGE ATTRIBUTES
	 *********************************************************/

	template <typename T, unsigned int ORBIT>
	AttributeHandler<T, ORBIT> getAttribute(const QString& nameAttr, bool onlyRegistered = true) const;

	template <typename T, unsigned int ORBIT>
	AttributeHandler<T, ORBIT> addAttribute(const QString& nameAttr, bool registerAttr = true);

	inline void registerAttribute(const AttributeHandlerGen& ah);

	inline QString getAttributeTypeName(unsigned int orbit, const QString& nameAttr) const;

	const AttributeSet& getAttributeSet(unsigned int orbit) const { return m_attribs[orbit]; }

	void notifyAttributeModification(const AttributeHandlerGen& attr)
	{
		QString nameAttr = QString::fromStdString(attr.name());
		if(m_vbo.contains(nameAttr))
			m_vbo[nameAttr]->updateData(attr);

		emit(attributeModified(attr.getOrbit(), nameAttr));

		foreach(View* view, l_views)
			view->updateGL();
	}

	void notifyConnectivityModification()
	{
		m_render->setPrimitiveDirty(Algo::Render::GL2::POINTS);
		m_render->setPrimitiveDirty(Algo::Render::GL2::LINES);
		m_render->setPrimitiveDirty(Algo::Render::GL2::TRIANGLES);

		emit(connectivityModified());

		foreach(View* view, l_views)
			view->updateGL();
	}

	/*********************************************************
	 * MANAGE VBOs
	 *********************************************************/

public slots:
	Utils::VBO* createVBO(const AttributeMultiVectorGen* attr);
	Utils::VBO* createVBO(const AttributeHandlerGen& attr);
	Utils::VBO* createVBO(const QString& name);

	void updateVBO(const AttributeMultiVectorGen* attr);
	void updateVBO(const AttributeHandlerGen& attr);
	void updateVBO(const QString& name);

	Utils::VBO* getVBO(const QString& name) const;
	const VBOSet& getVBOSet() const { return m_vbo; }

	void deleteVBO(const QString& name);

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

private:
	void linkView(View* view);
	void unlinkView(View* view);

	/*********************************************************
	 * SIGNALS
	 *********************************************************/

signals:
	void connectivityModified();

	void attributeAdded(unsigned int orbit, const QString& nameAttr);
	void attributeModified(unsigned int orbit, QString nameAttr);

	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);

protected:
	QString m_name;
	SCHNApps* m_schnapps;
	GenericMap* m_map;

	qglviewer::Vec m_bbMin;
	qglviewer::Vec m_bbMax;
	float m_bbDiagSize;

	Algo::Render::GL2::MapRender* m_render;
	Utils::Drawer* m_bbDrawer;

	QList<View*> l_views;

	VBOSet m_vbo;
	AttributeSet m_attribs[NB_ORBITS];
};


template <typename PFP>
class MapHandler : public MapHandlerGen
{
public:
	MapHandler(const QString& name, SCHNApps* s, typename PFP::MAP* map) :
		MapHandlerGen(name, s, map)
	{}

	~MapHandler()
	{
		if (m_map)
			delete m_map;
	}

	void draw(Utils::GLSLShader* shader, int primitive);
	void drawBB();

	inline typename PFP::MAP* getMap() { return static_cast<typename PFP::MAP*>(m_map); }

	void updateBB(const VertexAttribute<typename PFP::VEC3>& position);
	void updateBBDrawer();

protected:
	Geom::BoundingBox<typename PFP::VEC3> m_bb;
};

} // namespace SCHNApps

} // namespace CGoGN

#include "mapHandler.hpp"

#endif
