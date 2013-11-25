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
	WithinSphere = 1
};

struct MapParameters
{
	MapParameters() :
		selectionMethod(SingleCell)
	{}

	VertexAttribute<PFP2::VEC3> positionAttribute;
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

	virtual void draw(View *view);
	virtual void drawMap(View* view, MapHandlerGen* map);

	virtual void keyPress(View* view, QKeyEvent* event);
	virtual void keyRelease(View* view, QKeyEvent* event);
	virtual void mousePress(View* view, QMouseEvent* event);
	virtual void mouseRelease(View* view, QMouseEvent* event) {}
	virtual void mouseMove(View* view, QMouseEvent* event);
	virtual void wheelEvent(View* view, QWheelEvent* event);

	virtual void viewLinked(View *view);
	virtual void viewUnlinked(View *view) {}

private slots:
	// slots called from SCHNApps signals
	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);
	void updateSelectedCellsRendering();

	// slots called from MapHandler signals
	void selectedMapAttributeAdded(unsigned int orbit, const QString& name);
	void selectedMapAttributeModified(unsigned int orbit, const QString& name);
	void selectedMapConnectivityModified();

public slots:
	// slots for Python calls
	void changePositionAttribute(const QString& view, const QString& map, const QString& name);
	void changeSelectionMethod(const QString& view, const QString& map, unsigned int method);

protected:
	Surface_Selection_DockTab* m_dockTab;
	QHash<MapHandlerGen*, MapParameters> h_parameterSet;

	bool m_selecting;

	// cell under the mouse when selecting
	Dart m_selectingVertex;
	Dart m_selectingEdge;
	Dart m_selectingFace;

	// selecting & selected cells drawing
	Utils::PointSprite* m_pointSprite;
	Utils::VBO* m_selectedVerticesVBO;

	Utils::Drawer* m_selectedEdgesDrawer;
	Utils::Drawer* m_selectedFacesDrawer;

	Utils::Drawer* m_selectingCellDrawer;

	// WithinSphere parameters
	Utils::VBO* m_selectionSphereVBO;
	PFP2::REAL m_selectionRadius;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
