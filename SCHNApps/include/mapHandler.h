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

	inline const QString& getName() const { return m_name; }

public slots:
	/**
	 * @brief get the name
	 * @return name
	 */
	QString getName();

	SCHNApps* getSCHNApps() const;

	/**
	 * @brief test is map is the current selected map
	 * @return selected / not selected
	 */
	bool isSelectedMap() const;

	GenericMap* getGenericMap() const;

	/*********************************************************
	 * MANAGE FRAME
	 *********************************************************/

public slots:
	qglviewer::ManipulatedFrame* getFrame() const;
	glm::mat4 getFrameMatrix() const;

private slots:
	void frameModified();

	/*********************************************************
	 * MANAGE BOUNDING BOX
	 *********************************************************/

public slots:

	/**
	* @brief Fix if bounding box has to be drawn
	* @param b yes or not
	*/
	void showBB(bool b);

	/**
	* @brief is bounding-box of map drawn
	* @return is bounding-box of map drawn
	*/
	bool isBBshown() const;

	/**
	* @brief choose the vertex attribute used to compute the bounding-box
	* @param name name of attribute
	*/
	void setBBVertexAttribute(const QString& name);

	AttributeMultiVectorGen* getBBVertexAttribute() const;

	/**
	* @brief get the vertex attribute used to compute the bounding-box
	* @return name of attribute
	*/
	QString getBBVertexAttributeName() const;

	/**
	* @brief get the length of diagonal of bounding-box of map
	*/
	float getBBdiagSize() const;

	Utils::Drawer* getBBDrawer() const;

	virtual bool transformedBB(qglviewer::Vec& bbMin, qglviewer::Vec& bbMax) = 0;

protected:
	bool m_showBB;
	virtual void updateBB() = 0;

	/*********************************************************
	 * MANAGE DRAWING
	 *********************************************************/

public:
	virtual void draw(Utils::GLSLShader* shader, int primitive) = 0;
	virtual void drawBB() = 0;

	inline void setPrimitiveDirty(int primitive) {	m_render->setPrimitiveDirty(primitive);	}

	/*********************************************************
	 * MANAGE TOPOLOGICAL QUERIES
	 *********************************************************/

	virtual unsigned int getNbDarts() = 0;
	virtual unsigned int getNbOrbits(unsigned int orbit) = 0;

	/*********************************************************
	 * MANAGE ATTRIBUTES
	 *********************************************************/

	inline void registerAttribute(const AttributeHandlerGen& ah);
	inline void registerAttribute(unsigned int orbit, const QString& name, const QString& typeName);

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

	/**
	* @brief create a VBO from vertex attribute (with same name)
	* @param name name of attribute
	*/
	Utils::VBO* createVBO(const QString& name);

	void updateVBO(const AttributeMultiVectorGen* attr);
	void updateVBO(const AttributeHandlerGen& attr);

	/**
	* @brief update VBO from attribute data
	* @param name name of VBO (and corresponding attribute)
	*/
	void updateVBO(const QString& name);

	/**
	* @brief Get VBO object from its name
	* @param name of VBO
	*/
	Utils::VBO* getVBO(const QString& name) const;
	const VBOSet& getVBOSet() const { return m_vbo; }

	/**
	* @brief Delete a VBO
	* @param name name of VBO
	*/
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
	/**
	 * @brief link a view to the map
	 * @param view
	 */
	void linkView(View* view);

	/**
	 * @brief unlink a view from the map
	 * @param view
	 */
	void unlinkView(View* view);

public slots:
	const QList<View*>& getLinkedViews() const { return l_views; }
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

	/*********************************************************
	 * MANAGE TOPO_RENDERING
	 *********************************************************/

public:
	virtual void createTopoRender(std::vector<CGoGN::Utils::GLSLShader*> s) = 0;
	void deleteTopoRender();
	virtual void updateTopoRender(const QString& positionAttributeName) = 0;
	virtual void drawTopoRender(int code) = 0;

	inline Algo::Render::GL2::TopoRender* getTopoRender() { return m_topoRender; }


	/*********************************************************
	* MANAGE TRANSFO
	*********************************************************/

	inline const glm::mat4& getTransfoMatrix() const { return m_transfoMatrix; }
	inline const glm::mat4& getInverseTransfoMatrix() const { return m_transfoMatrixInv; }

public slots:
	/**
	* @brief Set the scaling that apply on rendering the map
	* @param sx scaling x factor
	* @param sy scaling y factor
	* @param sz scaling z factor
	*/
	void setScaling(float sx, float sy, float sz);
	

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

	void boundingBoxModified();

protected:
	QString m_name;
	SCHNApps* m_schnapps;

	GenericMap* m_map;

	qglviewer::ManipulatedFrame* m_frame;
	glm::mat4 m_transfoMatrix;
	glm::mat4 m_transfoMatrixInv;

	AttributeMultiVectorGen* m_bbVertexAttribute;
	float m_bbDiagSize;
	Utils::Drawer* m_bbDrawer;

	Algo::Render::GL2::MapRender* m_render;
	Algo::Render::GL2::TopoRender* m_topoRender;

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

	void updateBB();
	void updateBBDrawer();

	bool transformedBB(qglviewer::Vec& bbMin, qglviewer::Vec& bbMax);

	/*********************************************************
	 * MANAGE TOPO DRAWING
	 *********************************************************/

	void createTopoRender(std::vector<CGoGN::Utils::GLSLShader*> s);
	void updateTopoRender(const QString& positionAttributeName);
	void drawTopoRender(int code);

	/*********************************************************
	 * MANAGE CELL SELECTORS
	 *********************************************************/

	virtual CellSelectorGen* addCellSelector(unsigned int orbit, const QString& name);

	template <unsigned int ORBIT>
	CellSelector<MAP, ORBIT>* getCellSelector(const QString& name) const;

protected:
	Geom::BoundingBox<VEC3> m_bb;

public:
	inline const Geom::BoundingBox<VEC3>& getBB() const { return m_bb; }
};

} // namespace SCHNApps

} // namespace CGoGN

#include "mapHandler.hpp"

#endif
