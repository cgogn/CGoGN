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

/**
* @brief A MapHandlerGen is a generic class that encapsulate a map.
* It mainly allows the management of:
* - Bounding-Box
* - VBOs
*
* VBO are OpenGL Vertex Buffer Objects, they are created from vertex-attribute of the map.
*
* One map is selected in SCHNApps
* MapHandlerGen is derived by using a typed map (EmbeddedMap2 / EmbeddedMap3 ...)
*
* Python callable slots are tagged with [PYTHON]
*/
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
	 * @brief [PYTHON] get the name
	 * @return name
	 */
	QString getName();

	/// get schnapps object ptr
	SCHNApps* getSCHNApps() const;

	/**
	 * @brief [PYTHON] test is map is the current selected map
	 * @return selected / not selected
	 */
	bool isSelectedMap() const;

	GenericMap* getGenericMap() const;

	/*********************************************************
	 * MANAGE FRAME
	 *********************************************************/

public slots:
	/// get the frame associated to the map (corresponding to ctrl+mouse manipulation)
	qglviewer::ManipulatedFrame* getFrame() const;

	/// get the transfo matrix of frame associated to the map (corresponding to ctrl+mouse manipulation)
	glm::mat4 getFrameMatrix() const;

private slots:
	void frameModified();

	/*********************************************************
	 * MANAGE BOUNDING BOX
	 *********************************************************/

public slots:

	/**
	* @brief [PYTHON] Fix if bounding box has to be drawn
	* @param b yes or not
	*/
	void showBB(bool b);

	/**
	* @brief [PYTHON] is the bounding-box of map drawn ?
	* @return is bounding-box of map drawn
	*/
	bool isBBshown() const;

	/**
	* @brief [PYTHON] set color for drawing BB the bounding-box
	* @param color color name (red,green,...) or color format #rrggbb
	*/
	void setBBColor(const QString& color);

	/**
	* @brief [PYTHON] choose the vertex attribute used to compute the bounding-box
	* @param name name of attribute
	*/
	void setBBVertexAttribute(const QString& name);

	/// get the vertex attribute used to compute the bounding-box
	AttributeMultiVectorGen* getBBVertexAttribute() const;

	/**
	* @brief [PYTHON] get the vertex attribute used to compute the bounding-box
	* @return name of attribute
	*/
	QString getBBVertexAttributeName() const;

	/**
	* @brief [PYTHON] get the length of diagonal of bounding-box of map
	* @return  length of diagonal of bounding-box
	*/
	float getBBdiagSize() const;

	/// get the drawer used to draw the bounding box of map
	Utils::Drawer* getBBDrawer() const;

	///
	virtual bool transformedBB(qglviewer::Vec& bbMin, qglviewer::Vec& bbMax) = 0;

protected:
	/// draw the BB ?
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

	/**
	 * @brief register an attribute (visible and selectable in interface & plugins)
	 * @param ah the generic attribute handler to register (orbit and type deduce from object
	 */
	inline void registerAttribute(const AttributeHandlerGen& ah);

	/**
	 * @brief register an attribute (visible and selectable in interface & plugins)
	 * @param orbit orbit of attribute
	 * @param name name of attribute
	 * @param typeName nale of type of the attribute
	 */
	inline void registerAttribute(unsigned int orbit, const QString& name, const QString& typeName);

	/**
	 * @brief getAttributeTypeName
	 * @param orbit orbit of attribute
	 * @param nameAttr name of attribute
	 * @return name of type of the attribute
	 */
	inline QString getAttributeTypeName(unsigned int orbit, const QString& nameAttr) const;

	/// get the set of attribute for a given orbit.
	const AttributeSet& getAttributeSet(unsigned int orbit) const { return m_attribs[orbit]; }

	/// do necessary updates and send signals when an attribute has been modified
	void notifyAttributeModification(const AttributeHandlerGen& attr);

	/// do necessary updates and send signals when the topology of the map has been modified
	void notifyConnectivityModification();

	/**
	 * @brief clear the map
	 * @param removeAttrib remove all attributes whene cleaning the map.
	 */
	void clear(bool removeAttrib);

	/*********************************************************
	 * MANAGE VBOs
	 *********************************************************/

public slots:
	/**
	 * @brief create a VBO from vertex attribute
	 * @param attr generic multivector (low level storage)
	 * @return ptr on VBO
	 */
	Utils::VBO* createVBO(const AttributeMultiVectorGen* attr);

	/**
	 * @brief create a VBO from vertex attribute
	 * @param attr generic attribute handler (no data type)
	 * @return ptr on VBO
	 */
	Utils::VBO* createVBO(const AttributeHandlerGen& attr);

	/**
	* @brief [PYTHON] create a VBO from vertex attribute (with same name)
	* @param name name of attribute
	* @return ptr on VBO
	*/
	Utils::VBO* createVBO(const QString& name);

	/**
	 * @brief update the VBO from map attribute data
	 * @param attr generic multivector (low level storage)
	 */
	void updateVBO(const AttributeMultiVectorGen* attr);

	/**
	 * @brief update the VBO from map attribute data
	 * @param attr generic attribute handler (no data type)
	 */
	void updateVBO(const AttributeHandlerGen& attr);

	/**
	* @brief [PYTHON] update VBO from from map attribute data
	* @param name name of VBO (and corresponding attribute)
	*/
	void updateVBO(const QString& name);

	/**
	* @brief [PYTHON] Get VBO object from its name
	* @param name of VBO
	*/
	Utils::VBO* getVBO(const QString& name) const;

	/// get the set of VBO generated for this map
	const VBOSet& getVBOSet() const { return m_vbo; }

	/**
	* @brief [PYTHON] Delete a VBO
	* @param name name of VBO
	*/
	void deleteVBO(const QString& name);

	/*********************************************************
	 * MANAGE CELL SELECTORS
	 *********************************************************/

	/**
	 * @brief [PYTHON] add a CellSelector (for an orbit)
	 * @param orbit 0:DART 1:VERTEX 2:EDGE 3:FACE
	 * @param name name of selector
	 * @return
	 */
	virtual CellSelectorGen* addCellSelector(unsigned int orbit, const QString& name) = 0;

	/**
	 * @brief [PYTHON] remove a CellSelector of an orbit
	 * @param orbit 0:DART 1:VERTEX 2:EDGE 3:FACE
	 * @param name name of selector
	 */
	void removeCellSelector(unsigned int orbit, const QString& name);

	/**
	 * @brief get a CellSelector of an orbit
	 * @param orbit 0:DART 1:VERTEX 2:EDGE 3:FACE
	 * @param name name of selector
	 * @return ptr
	 */
	CellSelectorGen* getCellSelector(unsigned int orbit, const QString& name) const;

	/**
	 * @brief get the set of cell selector of given orbit
	 * @param orbit 0:DART 1:VERTEX 2:EDGE 3:FACE
	 * @return the set
	 */
	const CellSelectorSet& getCellSelectorSet(unsigned int orbit) const { return m_cellSelectors[orbit]; }


private slots:
	void selectedCellsChanged();

public:
	/// ???
	void updateMutuallyExclusiveSelectors(unsigned int orbit);


	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

private:

	void linkView(View* view);

	void unlinkView(View* view);

public slots:
	/// get list of views linked to the map
	const QList<View*>& getLinkedViews() const { return l_views; }

	/// test if a view is link to this map
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

	/*********************************************************
	 * MANAGE TOPO_RENDERING
	 *********************************************************/

public:
	/// create the topological render object if asked from the TopoRender plugin
	virtual void createTopoRender(std::vector<CGoGN::Utils::GLSLShader*> s) = 0;
	void deleteTopoRender();
	/// update the topo-render using an vertex attribute position
	virtual void updateTopoRender(const QString& positionAttributeName) = 0;
	/// draw the topo render
	virtual void drawTopoRender(int code) = 0;

	inline Algo::Render::GL2::TopoRender* getTopoRender() { return m_topoRender; }


	/*********************************************************
	* MANAGE TRANSFO
	*********************************************************/

	inline const glm::mat4& getTransfoMatrix() const { return m_transfoMatrix; }
	inline const glm::mat4& getInverseTransfoMatrix() const { return m_transfoMatrixInv; }

public slots:
	/**
	* @brief [PYTHON] Set the scaling that apply on rendering the map
	* @param sx scaling x factor
	* @param sy scaling y factor
	* @param sz scaling z factor
	*/
	void setScaling(float sx, float sy, float sz);

	/**
	* @brief [PYTHON] Save the frame into a string
	* @return the storage string
	*/
	QString frameToString();

	/**
	* @brief [PYTHON] restore frame from string storage
	* @param frame the string with frame info 
	*/
	void frameFromString(QString frame);

	/**
	 * @brief reset the frame manipulator to its initial position
	 */
	void frameReset();

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

	/// frame that old user object manipulation (ctrl mouse)
	qglviewer::ManipulatedFrame* m_frame;
	/// transformation matrix use for scaling
	glm::mat4 m_transfoMatrix;
	/// inverse of transfo matrix
	glm::mat4 m_transfoMatrixInv;

	AttributeMultiVectorGen* m_bbVertexAttribute;
	float m_bbDiagSize;
	Utils::Drawer* m_bbDrawer;
	glm::vec3 m_bbColor;

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

	~MapHandler();

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

	/// compute the BB (min&max) of map

	/**
	 * @brief compute the BB (min&max) of map after transformation (manipulated frame + transfo matrix)
	 * @param bbMin computed min point
	 * @param bbMax computed max point
	 * @return bb is initialized
	 */
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
