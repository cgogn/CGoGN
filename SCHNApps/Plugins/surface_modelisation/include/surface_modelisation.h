#ifndef _SURFACE_MODELISATION_PLUGIN_H_
#define _SURFACE_MODELISATION_PLUGIN_H_

#include "dll.h"
#include "plugin_interaction.h"
#include "surface_modelisation_dockTab.h"

#include "mapHandler.h"
#include "Utils/drawer.h"
#include "Topology/map/map2.h"
#include <QVector>
#include "QGLViewer/camera.h"
#include "QGLViewer/vec.h"

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

	VertexAttribute<PFP2::VEC3, PFP2::MAP> positionAttribute;

	CellSelector<PFP2::MAP, VERTEX>* vertexSelector;
	CellSelector<PFP2::MAP, EDGE>* edgeSelector;
	CellSelector<PFP2::MAP, FACE>* faceSelector;
};

class SURFACE_MODELISATION_API Surface_Modelisation_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
	friend class Surface_Modelisation_DockTab;

public:
	Surface_Modelisation_Plugin();

	~Surface_Modelisation_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

    virtual void draw(View *view);
    virtual void drawMap(View* view, MapHandlerGen* map);

	virtual void keyPress(View* , QKeyEvent* ) {}
	virtual void keyRelease(View* , QKeyEvent* ) {}
    virtual void mousePress(View* view, QMouseEvent* event);
	virtual void mouseRelease(View* , QMouseEvent* ) {}
	virtual void mouseMove(View* , QMouseEvent* ) {}
	virtual void wheelEvent(View* , QWheelEvent* ) {}

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
	void createEmptyMap();
    void createNewFace(MapHandlerGen* mhg);
    void addCube(MapHandlerGen* mhg);
    void mergeVolumes(MapHandlerGen* mhg);
    void splitSurface(MapHandlerGen* mhg);
    void extrudeRegion(MapHandlerGen* mhg);

    void splitVertex(MapHandlerGen* mhg);
    void deleteVertex(MapHandlerGen* mhg);

    void cutEdge(MapHandlerGen* mhg);
    void uncutEdge(MapHandlerGen* mhg);
	void flipEdge(MapHandlerGen* mhg);
    void flipBackEdge(MapHandlerGen* mhg);
    void collapseEdge(MapHandlerGen* mhg);

    void splitFace(MapHandlerGen* mhg);
    void mergeFaces(MapHandlerGen* mhg);
    void deleteFace(MapHandlerGen* mhg);
    void sewFaces(MapHandlerGen* mhg);
    void unsewFaces(MapHandlerGen* mhg);
    void extrudeFace(MapHandlerGen* mhg);
    void pathExtrudeFace(MapHandlerGen *mhg);


protected:
	Surface_Modelisation_DockTab* m_dockTab;
	QHash<MapHandlerGen*, MapParameters> h_parameterSet;
    Utils::Drawer* m_drawer;

    std::vector<PFP2::VEC3> collectedVertices;
    bool collect;
    int mapNumber;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
