#ifndef _RENDERTOPO_SURFACE_PLUGIN_H_
#define _RENDERTOPO_SURFACE_PLUGIN_H_

#include "plugin.h"
#include "renderTopoSurfaceDockTab.h"

#include "Algo/Render/GL2/topoRender.h"

namespace CGoGN
{

namespace SCHNApps
{

struct PerMapParameterSet
{
	PerMapParameterSet(MapHandlerGen* mh);
	~PerMapParameterSet();

	void updateRender();

	MapHandlerGen* mh;
	Algo::Render::GL2::TopoRender* m_renderTopo;
	VertexAttribute<PFP2::VEC3> positionAttribute;

	bool drawDarts;
	QColor dartsColor;
	bool drawPhi1;
	QColor phi1Color;
	bool drawPhi2;
	QColor phi2Color;
	float edgesScaleFactor;
	float facesScaleFactor;
};

struct ParameterSet
{
	ParameterSet() : selectedMap(NULL)
	{}

	QHash<QString, PerMapParameterSet*> perMap;
	MapHandlerGen* selectedMap;
};


class RenderTopoSurfacePlugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	RenderTopoSurfacePlugin()
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
	virtual void mousePress(View* view, QMouseEvent* event);
	virtual void mouseRelease(View* view, QMouseEvent* event) {}
	virtual void mouseMove(View* view, QMouseEvent* event) {}
	virtual void wheelEvent(View* view, QWheelEvent* event) {}

protected:
	RenderTopoSurfaceDockTab* m_dockTab;
	QHash<View*, ParameterSet*> h_viewParams;

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
	void changeDrawDarts(View* view, MapHandlerGen* map, bool b, bool fromUI = false);
	void changeDartsColor(View* view, MapHandlerGen* map, QColor c, bool fromUI = false);
	void changeDrawPhi1(View* view, MapHandlerGen* map, bool b, bool fromUI = false);
	void changePhi1Color(View* view, MapHandlerGen* map, QColor c, bool fromUI = false);
	void changeDrawPhi2(View* view, MapHandlerGen* map, bool b, bool fromUI = false);
	void changePhi2Color(View* view, MapHandlerGen* map, QColor c, bool fromUI = false);
	void changeEdgesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI = false);
	void changeFacesScaleFactor(View* view, MapHandlerGen* map, int i, bool fromUI = false);

	void attributeModified(unsigned int orbit, QString nameAttr);
	void connectivityModified();

signals:
	void dartSelected(Dart d);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
