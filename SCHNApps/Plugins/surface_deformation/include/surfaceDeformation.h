#ifndef _SURFACEDEFORMATION_PLUGIN_H_
#define _SURFACEDEFORMATION_PLUGIN_H_

#include "plugin.h"
#include "surfaceDeformationDockTab.h"

#include "mapHandler.h"

#include "Utils/pointSprite.h"

#include "Container/fakeAttribute.h"

#include "NL/nl.h"
#include "Algo/LinearSolving/basic.h"
#include "Eigen/Dense"

namespace CGoGN
{

namespace SCHNApps
{


enum SelectionMode
{
	LOCKED,
	HANDLE
};


typedef NoNameIOAttribute<Eigen::Matrix3f> Eigen_Matrix3f ;

struct PerMapParameterSet
{
	PerMapParameterSet(MapHandlerGen* mh);
	~PerMapParameterSet();

	void initParameters();

	MapHandlerGen* mh;

	VertexAttribute<PFP2::VEC3> positionAttribute;
	CellMarker<VERTEX>* lockingMarker;
	CellMarker<VERTEX>* handleMarker;
	SelectionMode verticesSelectionMode;
	std::vector<PFP2::VEC3> lockedVertices;
	std::vector<PFP2::VEC3> handleVertices;
	std::vector<unsigned int> handleVerticesId;
	Utils::VBO* lockedVerticesVBO;
	Utils::VBO* handleVerticesVBO;

	VertexAttribute<PFP2::VEC3> positionInit;
	VertexAttribute<PFP2::VEC3> vertexNormal;
	EdgeAttribute<PFP2::REAL> edgeAngle;
	EdgeAttribute<PFP2::REAL> edgeWeight;
	VertexAttribute<PFP2::REAL> vertexArea;
	VertexAttribute<PFP2::VEC3> diffCoord;
	VertexAttribute<Eigen_Matrix3f> vertexRotationMatrix;
	VertexAttribute<PFP2::VEC3> rotatedDiffCoord;

	VertexAttribute<unsigned int> vIndex;
	unsigned int nb_vertices;
	NLContext nlContext;
};

struct ParameterSet
{
	ParameterSet() : selectedMap(NULL)
	{}

	PerMapParameterSet* getCurrentMapParameterSet()
	{
		if(selectedMap)
			return perMap[selectedMap->getName()];
		else
			return NULL;
	}

	QHash<QString, PerMapParameterSet*> perMap;
	MapHandlerGen* selectedMap;
};


class SurfaceDeformationPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	SurfaceDeformationPlugin() :
		selecting(false),
		dragging(false)
	{
		setProvidesRendering(true);
	}

	~SurfaceDeformationPlugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void redraw(View *view);

	virtual void keyPress(View* view, QKeyEvent* event);
	virtual void keyRelease(View* view, QKeyEvent* event);
	virtual void mousePress(View* view, QMouseEvent* event);
	virtual void mouseRelease(View* view, QMouseEvent* event);
	virtual void mouseMove(View* view, QMouseEvent* event);
	virtual void wheelEvent(View* view, QWheelEvent* event);

protected:
	SurfaceDeformationDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	Utils::PointSprite* m_pointSprite;

	Utils::VBO* selectionSphereVBO;

	bool selecting;
	PFP2::VEC3 selectionCenter;
	PFP2::REAL selectionRadius;
	bool dragging;
	PFP2::REAL dragZ;
	qglviewer::Vec dragPrevious;

public slots:
	void viewLinked(View* view, Plugin* plugin);
	void viewUnlinked(View* view, Plugin* plugin);
	void currentViewChanged(View* view);

	void mapLinked(MapHandlerGen* m);
	void mapUnlinked(MapHandlerGen* m);

protected:
	void addManagedMap(View *v, MapHandlerGen* m);
	void removeManagedMap(View *v, MapHandlerGen* m);

public slots:
	void changeSelectedMap(View* view, MapHandlerGen* map);

	void changePositionAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP2::VEC3> attribute, bool fromUI = false);
	void changeVerticesSelectionMode(View* view, MapHandlerGen* map, SelectionMode m, bool fromUI = false);

protected:
	void matchDiffCoord(View* view, MapHandlerGen* map);
	void asRigidAsPossible(View* view, MapHandlerGen* map);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
