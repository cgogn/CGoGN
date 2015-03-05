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
		selectionMethod(SingleCell)
	{}

	VertexAttribute<PFP2::VEC3, PFP2::MAP> positionAttribute;
	VertexAttribute<PFP2::VEC3, PFP2::MAP> normalAttribute;
	SelectionMethod selectionMethod;
};

class Surface_Selection_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

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

	// slots called from MapHandler signals
	void selectedMapAttributeAdded(unsigned int orbit, const QString& name);
	void selectedMapAttributeModified(unsigned int orbit, const QString& name);
	void selectedMapConnectivityModified();
	void selectedMapBoundingBoxModified();

public slots:
	// slots for Python calls
	void changePositionAttribute(const QString& map, const QString& name);
	void changeNormalAttribute(const QString& map, const QString& name);
	void changeSelectionMethod(const QString& map, unsigned int method);

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
