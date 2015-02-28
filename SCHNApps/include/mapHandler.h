#ifndef _MAPHANDLER_H_
#define _MAPHANDLER_H_

#include <QString>

#include "types.h"
#include "view.h"
#include "plugin.h"
#include "cellSelector.h"

#include "Topology/generic/genericmap.h"
#include "Topology/generic/functor.h"
#include "Topology/generic/attributeHandler.h"

#include "Utils/drawer.h"

#include "Algo/Render/GL2/mapRender.h"
#include "Algo/Render/GL2/topoRender.h"
#include "Algo/Geometry/boundingbox.h"

#include "Algo/Topo/basic.h"

#include "Utils/vbo.h"

#include "dll.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNAPPS_API MapHandlerGen : public QObject
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

	const QList<View*>& getLinkedViews() const { return l_views; }
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

	float getBBdiagSize() const { return m_bbDiagSize; }
	Utils::GLSLShader* getBBDrawerShader() const
	{
		if(m_bbDrawer)
			return m_bbDrawer->getShader();
		else
			return NULL;
	}

	qglviewer::ManipulatedFrame* getFrame() const { return m_frame; }
	glm::mat4 getFrameMatrix() const
	{
		GLdouble m[16];
		m_frame->getMatrix(m);
		glm::mat4 matrix;
		for(unsigned int i = 0; i < 4; ++i)
		{
			for(unsigned int j = 0; j < 4; ++j)
				matrix[i][j] = (float)m[i*4+j];
		}
		return matrix;
	}

	void frameModified() { DEBUG_EMIT("frameModified");emit(BBisModified());}

public:
	virtual void draw(Utils::GLSLShader* shader, int primitive) = 0;
	virtual void drawBB() = 0;
	virtual void transformedBB(qglviewer::Vec& bbMin, qglviewer::Vec& bbMax) = 0;
	virtual void initBBDrawer() = 0;

	void setPrimitiveDirty(int primitive) {	m_render->setPrimitiveDirty(primitive);	}

	/*********************************************************
	 * MANAGE TOPOLOGICAL QUERIES
	 *********************************************************/

	virtual unsigned int getNbDarts() = 0;
	virtual unsigned int getNbOrbits(unsigned int orbit) = 0;

	/*********************************************************
	 * MANAGE ATTRIBUTES
	 *********************************************************/

	inline void registerAttribute(const AttributeHandlerGen& ah);

	inline QString getAttributeTypeName(unsigned int orbit, const QString& nameAttr) const;

	const AttributeSet& getAttributeSet(unsigned int orbit) const { return m_attribs[orbit]; }

	void notifyAttributeModification(const AttributeHandlerGen& attr);

	void notifyConnectivityModification();

	void clear(bool removeAttrib);

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
	 * MANAGE CELL SELECTORS
	 *********************************************************/

	virtual CellSelectorGen* addCellSelector(unsigned int orbit, const QString& name) = 0;
	void removeCellSelector(unsigned int orbit, const QString& name);

	CellSelectorGen* getCellSelector(unsigned int orbit, const QString& name) const;
	const CellSelectorSet& getCellSelectorSet(unsigned int orbit) const { return m_cellSelectors[orbit]; }

private slots:
	void selectedCellsChanged();

public:
	void updateMutuallyExclusiveSelectors(unsigned int orbit);

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

private:
	void linkView(View* view);
	void unlinkView(View* view);

	/*********************************************************
	 * MANAGE TOPO_RENDERING
	 *********************************************************/
public:

	virtual void createTopoRender(CGoGN::Utils::GLSLShader* sh1) = 0;
	void deleteTopoRender();
	virtual void updateTopoRender(const QString& name) = 0;
	virtual void drawTopoRender(int code) = 0;

	inline Algo::Render::GL2::TopoRender* getTopoRender() { return m_topoRender;}

protected:
	Algo::Render::GL2::TopoRender* m_topoRender;

	/*********************************************************
	 * SIGNALS
	 *********************************************************/

signals:
	void connectivityModified();

	void attributeAdded(unsigned int orbit, const QString& nameAttr);
	void attributeModified(unsigned int orbit, QString nameAttr);
	void attributeRemoved(unsigned int orbit, const QString& nameAttr);

	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);

	void cellSelectorAdded(unsigned int orbit, const QString& name);
	void cellSelectorRemoved(unsigned int orbit, const QString& name);
	void selectedCellsChanged(CellSelectorGen* cs);

	void BBisModified();

protected:
	QString m_name;
	SCHNApps* m_schnapps;

	GenericMap* m_map;

	qglviewer::ManipulatedFrame* m_frame;

	float m_bbDiagSize;
	Utils::Drawer* m_bbDrawer;

	Algo::Render::GL2::MapRender* m_render;

	QList<View*> l_views;

	VBOSet m_vbo;
	AttributeSet m_attribs[NB_ORBITS];

	CellSelectorSet m_cellSelectors[NB_ORBITS];
};


template <typename PFP>
class MapHandler : public MapHandlerGen
{
	typedef typename PFP::MAP MAP;
	typedef typename PFP::VEC3 VEC3;

public:
	MapHandler(const QString& name, SCHNApps* s, typename PFP::MAP* map) :
		MapHandlerGen(name, s, map)
	{}

	~MapHandler()
	{
		if (m_map)
			delete m_map;
	}

	inline MAP* getMap() { return static_cast<MAP*>(m_map); }

	/*********************************************************
	 * MANAGE TOPOLOGICAL QUERIES
	 *********************************************************/

	unsigned int getNbDarts();
	unsigned int getNbOrbits(unsigned int orbit);

	/*********************************************************
	 * MANAGE ATTRIBUTES
	 *********************************************************/

	template <typename T, unsigned int ORBIT>
	AttributeHandler<T, ORBIT, MAP> getAttribute(const QString& nameAttr, bool onlyRegistered = true) const;

	template <typename T, unsigned int ORBIT>
	AttributeHandler<T, ORBIT, MAP> addAttribute(const QString& nameAttr, bool registerAttr = true);

	/*********************************************************
	 * MANAGE DRAWING
	 *********************************************************/

	void draw(Utils::GLSLShader* shader, int primitive);
	void drawBB();

	void updateBB(const VertexAttribute<VEC3, MAP>& position);
	void initBBDrawer();
	void updateBBDrawer();

	void transformedBB(qglviewer::Vec& bbMin, qglviewer::Vec& bbMax);

	/*********************************************************
	 * MANAGE TOPO DRAWING
	 *********************************************************/

	void createTopoRender(CGoGN::Utils::GLSLShader* sh1);
	void updateTopoRender(const QString& positionName);
	void drawTopoRender(int code);

	/*********************************************************
	 * MANAGE CELL SELECTORS
	 *********************************************************/

	virtual CellSelectorGen* addCellSelector(unsigned int orbit, const QString& name);

	template <unsigned int ORBIT>
	CellSelector<MAP, ORBIT>* getCellSelector(const QString& name) const;

protected:
	Geom::BoundingBox<VEC3> m_bb;
};

} // namespace SCHNApps

} // namespace CGoGN


#include "mapHandler.hpp"

#endif
