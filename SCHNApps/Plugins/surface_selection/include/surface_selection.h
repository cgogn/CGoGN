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

struct MapParameters
{
	MapParameters()
	{}

	VertexAttribute<PFP2::VEC3> positionAttribute;
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
	virtual void mouseRelease(View* view, QMouseEvent* event);
	virtual void mouseMove(View* view, QMouseEvent* event);
	virtual void wheelEvent(View* view, QWheelEvent* event);

	virtual void viewLinked(View *view);
	virtual void viewUnlinked(View *view) {}

private slots:
	// slots called from SCHNApps signals
	void selectedViewChanged(View* prev, View* cur);
	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);
	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);

	// slots called from MapHandler signals
	void attributeAdded(unsigned int orbit, const QString& nameAttr);

public slots:
	// slots for Python calls

protected:
	Surface_Selection_DockTab* m_dockTab;
	QHash<View*, QHash<MapHandlerGen*, MapParameters> > h_viewParameterSet;

	Utils::Drawer* m_drawer;

	Utils::VBO* m_selectionSphereVBO;
	Utils::PointSprite* m_pointSprite;

	bool m_selecting;

	PFP2::VEC3 m_selectionCenter;
	PFP2::REAL m_selectionRadius;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
