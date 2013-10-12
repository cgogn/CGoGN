#ifndef _SURFACE_MODELISATION_PLUGIN_H_
#define _SURFACE_MODELISATION_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_modelisation_dockTab.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

struct MapParameters
{
	MapParameters() :
		vertexSelector(NULL),
		edgeSelector(NULL),
		faceSelector(NULL)
	{}

	VertexAttribute<PFP2::VEC3> positionAttribute;

	CellSelector<VERTEX>* vertexSelector;
	CellSelector<EDGE>* edgeSelector;
	CellSelector<FACE>* faceSelector;
};

class Surface_Modelisation_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

	friend class Surface_Modelisation_DockTab;

public:
	Surface_Modelisation_Plugin();

	~Surface_Modelisation_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void draw(View *view) {}
	virtual void drawMap(View* view, MapHandlerGen* map) {}

	virtual void keyPress(View* view, QKeyEvent* event) {}
	virtual void keyRelease(View* view, QKeyEvent* event) {}
	virtual void mousePress(View* view, QMouseEvent* event) {}
	virtual void mouseRelease(View* view, QMouseEvent* event) {}
	virtual void mouseMove(View* view, QMouseEvent* event) {}
	virtual void wheelEvent(View* view, QWheelEvent* event) {}

	virtual void viewLinked(View *view) {}
	virtual void viewUnlinked(View *view) {}

private slots:
	// slots called from SCHNApps signals
	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);
	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);

	// slots called from MapHandler signals
	void attributeAdded(unsigned int orbit, const QString& name);
	void cellSelectorAdded(unsigned int orbit, const QString& name);
	void cellSelectorRemoved(unsigned int orbit, const QString& name);

public slots:
	// slots for Python calls
	void changePositionAttribute(const QString& view, const QString& map, const QString& name);
	void changeVertexSelector(const QString& map, const QString& name);
	void changeEdgeSelector(const QString& map, const QString& name);
	void changeFaceSelector(const QString& map, const QString& name);

protected:
	void createCube(MapHandlerGen* mhg);
	void flipEdge(MapHandlerGen* mhg);

protected:
	Surface_Modelisation_DockTab* m_dockTab;
	QHash<MapHandlerGen*, MapParameters> h_parameterSet;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
