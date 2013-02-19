#ifndef _RENDER_PLUGIN_H_
#define _RENDER_PLUGIN_H_

#include "plugin.h"
#include "ui_renderTopoSurface.h"

#include "Algo/Render/GL2/topoRender.h"

using namespace CGoGN;
using namespace SCHNApps;


struct PerMapParameterSet
{
	PerMapParameterSet() :
		edgesScaleFactor(1.0f),
		facesScaleFactor(1.0f)
	{
		m_renderTopo = new Algo::Render::GL2::TopoRender();
	}

	~PerMapParameterSet()
	{
		delete m_renderTopo;
	}

	float edgesScaleFactor;
	float facesScaleFactor;

	Algo::Render::GL2::TopoRender* m_renderTopo;
};

struct ParameterSet
{
	ParameterSet() : selectedMap(NULL)
	{}

	QHash<QString, PerMapParameterSet> perMap;
	MapHandlerGen* selectedMap;
};


class RenderTopoSurfacePlugin;

class RenderTopoSurfaceDockTab : public QWidget, public Ui::RenderWidget
{
public:
	RenderTopoSurfaceDockTab(RenderTopoSurfacePlugin* p) : plugin(p)
	{
		setupUi(this);
	}

	void refreshUI(ParameterSet* params);

private:
	RenderTopoSurfacePlugin* plugin;
};


class RenderTopoSurfacePlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	RenderTopoSurfacePlugin() : b_refreshingUI(false)
	{
		setProvidesRendering(true);
	}

	~RenderTopoSurfacePlugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void redraw(View *view);

	virtual void keyPress(View* view, QKeyEvent* event) {}
	virtual void keyRelease(View* view, QKeyEvent* event) {}
	virtual void mousePress(View* view, QMouseEvent* event) {}
	virtual void mouseRelease(View* view, QMouseEvent* event) {}
	virtual void mouseMove(View* view, QMouseEvent* event) {}
	virtual void wheelEvent(View* view, QWheelEvent* event) {}

	void setRefreshingUI(bool b) { b_refreshingUI = b; }

protected:
	RenderDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

	bool b_refreshingUI;

public slots:
	void viewLinked(View* view, Plugin* plugin);
	void viewUnlinked(View* view, Plugin* plugin);
	void currentViewChanged(View* view);

	void mapLinked(MapHandlerGen* m);
	void mapUnlinked(MapHandlerGen* m);

	void changeSelectedMap(View* view, MapHandlerGen* map);
	void changeEdgesScaleFactor(View* view, MapHandlerGen* map, int i);
	void changeFacesScaleFactor(View* view, MapHandlerGen* map, int i);

	void cb_selectedMapChanged();
	void cb_edgesScaleFactorChanged(int i);
	void cb_facesScaleFactorChanged(int i);
};

#endif
