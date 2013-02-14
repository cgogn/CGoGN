#ifndef _SURFACEDEFORMATION_PLUGIN_H_
#define _SURFACEDEFORMATION_PLUGIN_H_

#include "plugin.h"


using namespace CGoGN;
using namespace SCHNApps;


struct PerMapParameterSet
{
	PerMapParameterSet() {}

	PerMapParameterSet(MapHandlerGen* mh);

	VertexAttribute<PFP2::VEC3> positionAttribute;
	CellMarker<VERTEX> lockingMarker;
	bool setLockedVertices;
	bool selecting;
	PFP2::VEC3 selectionCenter;
	PFP2::REAL selectionRadius;
	bool dragging;
	PFP2::REAL dragZ;
	qglviewer::Vec dragPrevious;
	std::vector<unsigned int> locked_vertices;
	std::vector<unsigned int> handle_vertices;
};

struct ParameterSet
{
	ParameterSet() : selectedMap(NULL)
	{}

	PerMapParameterSet& getCurrentMapParameterSet()
	{
		return perMap[selectedMap->getName()];
	}

	QHash<QString, PerMapParameterSet> perMap;
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
	SurfaceDeformationPlugin() : b_refreshingUI(false)
	{
		setProvidesRendering(true);
	}

	~SurfaceDeformationPlugin()
	{}

	virtual bool enable();
	virtual void disable() {}

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

	void changeSelectedMap(View* view, MapHandlerGen* map);
	void changePositionAttribute(View* view, MapHandlerGen* map, VertexAttribute<PFP2::VEC3> attribute);

	void cb_selectedMapChanged();
	void cb_positionAttributeChanged(int index);

private:
	RenderVectorDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	bool b_refreshingUI;
};

#endif
