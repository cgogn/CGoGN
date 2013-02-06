#ifndef _SURFACEDEFORMATION_PLUGIN_H_
#define _SURFACEDEFORMATION_PLUGIN_H_

#include "plugin.h"
#include "mapHandler.h"

#include "ui_surfaceDeformation.h"

#include "Container/fakeAttribute.h"

#include "OpenNL/linear_solver.h"
#include "Algo/LinearSolving/basic.h"


using namespace CGoGN;
using namespace SCHNApps;

namespace CGoGN { namespace Utils { class Drawer; } }


enum SelectionMode
{
	LOCKED,
	HANDLE
};


typedef NoNameIOAttribute<Eigen::Matrix3f> Eigen_Matrix3f ;

struct PerMapParameterSet
{
	PerMapParameterSet() {}
	PerMapParameterSet(MapHandlerGen* mh);
	~PerMapParameterSet();

	VertexAttribute<PFP2::VEC3> positionAttribute;
	CellMarker<VERTEX>* lockingMarker;
	CellMarker<VERTEX>* handleMarker;
	SelectionMode verticesSelectionMode;
	std::vector<unsigned int> locked_vertices;
	std::vector<unsigned int> handle_vertices;

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
	LinearSolver<PFP2::REAL>* solver;
};

struct ParameterSet
{
	ParameterSet() : selectedMap(NULL)
	{}

	PerMapParameterSet* getCurrentMapParameterSet()
	{
		return perMap[selectedMap->getName()];
	}

	QHash<QString, PerMapParameterSet*> perMap;
	MapHandlerGen* selectedMap;
};


class SurfaceDeformationPlugin;

class SurfaceDeformationDockTab : public QWidget, public Ui::SurfaceDeformationWidget
{
public:
	SurfaceDeformationDockTab(SurfaceDeformationPlugin* p) : plugin(p)
	{
		setupUi(this);
	}

	void refreshUI(ParameterSet* params);

private:
	SurfaceDeformationPlugin* plugin;
};


class SurfaceDeformationPlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	SurfaceDeformationPlugin() :
		b_refreshingUI(false),
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

	void setRefreshingUI(bool b) { b_refreshingUI = b; }

public slots:
	void viewLinked(View* view, Plugin* plugin);
	void viewUnlinked(View* view, Plugin* plugin);
	void currentViewChanged(View* view);

	void mapLinked(MapHandlerGen* m);
	void mapUnlinked(MapHandlerGen* m);

	void attributeAdded();

	void changeSelectedMap(View* view, MapHandlerGen* map);
	void changePositionAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP2::VEC3> attribute);
	void changeVerticesSelectionMode(View* view, MapHandlerGen* map, SelectionMode m);

	void cb_selectedMapChanged();
	void cb_positionAttributeChanged(int index);
	void cb_selectLockedVertices(bool b);
	void cb_selectHandleVertices(bool b);

	void matchDiffCoord(View* view, MapHandlerGen* map);
	void asRigidAsPossible(View* view, MapHandlerGen* map);

private:
	SurfaceDeformationDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	Utils::Drawer* m_drawer;

	bool b_refreshingUI;

	bool selecting;
	PFP2::VEC3 selectionCenter;
	PFP2::REAL selectionRadius;
	bool dragging;
	PFP2::REAL dragZ;
	qglviewer::Vec dragPrevious;
};

#endif
