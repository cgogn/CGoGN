#ifndef _SURFACE_SELECTION_PLUGIN_H_
#define _SURFACE_SELECTION_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_selection_dockTab.h"

#include "Utils/pointSprite.h"
#include "Utils/drawer.h"

namespace CGoGN
{

namespace SCHNApps
{

enum SelectionMethod
{
	SingleCell = 0,
	WithinSphere = 1,
	NormalAngle = 2
};

struct MapParameters
{
	MapParameters() :
		selectionMethod(SingleCell),
		color(255,0,0)
	{}

	VertexAttribute<PFP2::VEC3, PFP2::MAP> positionAttribute;
	VertexAttribute<PFP2::VEC3, PFP2::MAP> normalAttribute;
	SelectionMethod selectionMethod;
	float verticesScaleFactor;
	float basePSradius;
	QColor color;
};

/**
* @brief Plugin for cell selection (in conjonction with CellSelectors of MapHandlers)
* Selection of cells can be done:
* - individually
* - in a sphere
* - with a give variation of the normal.
*/
class Surface_Selection_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
	friend class Surface_Selection_DockTab;

public:
	Surface_Selection_Plugin();

	~Surface_Selection_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void draw(View *view) {}
	virtual void drawMap(View* view, MapHandlerGen* map);

	virtual void keyPress(View* view, QKeyEvent* event);
	virtual void keyRelease(View* view, QKeyEvent* event);
	virtual void mousePress(View* view, QMouseEvent* event);
	virtual void mouseRelease(View* , QMouseEvent* ) {}
	virtual void mouseMove(View* view, QMouseEvent* event);
	virtual void wheelEvent(View* view, QWheelEvent* event);

	virtual void viewLinked(View *view) {}
	virtual void viewUnlinked(View *view) {}

private slots:
	// slots called from SCHNApps signals
	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);
	void updateSelectedCellsRendering();
	
	void updateRemovedSelector(unsigned int orbit, const QString& name);

	// slots called from MapHandler signals
	void selectedMapAttributeAdded(unsigned int orbit, const QString& name);
	void selectedMapAttributeModified(unsigned int orbit, const QString& name);
	void selectedMapConnectivityModified();
	void selectedMapBoundingBoxModified();

public slots:
	/**
	 * @brief [PYTHON] change the position attribute
	 * @param map map name
	 * @param name position attribute name
	 */
	void changePositionAttribute(const QString& map, const QString& name);

	/**
	 * @brief [PYTHON] change the normal attribute
	 * @param map map name
	 * @param name normal attribute name
	 */
	void changeNormalAttribute(const QString& map, const QString& name);

	/**
	 * @brief [PYTHON] change the selection
	 * @param map map name
	 * @param method selection method 0:single Cell / 1: WithinSphere / 2: Normal angle
	 */
	void changeSelectionMethod(const QString& map, unsigned int method);

	/**
	 * @brief [PYTHON] change the vertices scale factor
	 * @param map  map name
	 * @param f scaling factor 0 - 2
	 */
	void changeVerticesScaleFactor(const QString& map, float f);

	/**
	 * @brief [PYTHON] change the vertices rendering size (when scaling is 1)
	 * @param map  map name
	 * @param f vertex size ( in object frame)
	 */
	void changeVerticesBaseSize(const QString& map, float f);

	/**
	 * @brief [PYTHON] change the color for rendering selected cells
	 * @param map map name
	 * @param col color in Qt string format "#rrggbb". Each component is given in hexa, for example red is #ff0000
	 */
	void changeSelectedColor(const QString& map, const QString& col);

	/**
	* @brief [PYTHON] clear the selection
	* @param map map name
	* @param orbit orbit selector 0:DART 1:VERTEX 2:EDGE 3:FACE
	* @param selectorName name of selector
	*/
	void clearSelection(const QString& map, unsigned int orbit, const QString& selectorName);

protected:
	Surface_Selection_DockTab* m_dockTab;
	QHash<MapHandlerGen*, MapParameters> h_parameterSet;

	bool m_selecting;

	// cell under the mouse when selecting
	Vertex m_selectingVertex;
	Edge m_selectingEdge;
	Face m_selectingFace;

	// selecting & selected cells drawing
	Utils::PointSprite* m_pointSprite;
	Utils::VBO* m_selectedVerticesVBO;

	Utils::Drawer* m_selectedEdgesDrawer;
	Utils::Drawer* m_selectedFacesDrawer;

	Utils::Drawer* m_selectingCellDrawer;

	bool m_selectedVertices_dirty;
	bool m_selectedEdges_dirty;
	bool m_selectedFaces_dirty;

	// WithinSphere parameters
	Utils::VBO* m_selectionSphereVBO;
	PFP2::REAL m_selectionRadiusBase;
	PFP2::REAL m_selectionRadiusCoeff;

	// NormalAngle parameters
	PFP2::REAL m_normalAngleThreshold;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
