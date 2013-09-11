#ifndef _SURFACE_RENDERTOPO_PLUGIN_H_
#define _SURFACE_RENDERTOPO_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_renderTopo_dockTab.h"

#include "Algo/Render/GL2/topoRender.h"

namespace CGoGN
{

namespace SCHNApps
{

struct MapParameters
{
	MapParameters() :
		drawDarts(true),
		dartsColor("white"),
		drawPhi1(true),
		phi1Color("cyan"),
		drawPhi2(true),
		phi2Color("red"),
		edgesScaleFactor(1.0f),
		facesScaleFactor(1.0f)
	{}

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

class Surface_RenderTopo_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

	friend class Surface_RenderTopo_DockTab;

public:
	Surface_RenderTopo_Plugin()
	{}

	~Surface_RenderTopo_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void draw(View *view) {}
	virtual void drawMap(View* view, MapHandlerGen* map);

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
	void selectedViewChanged(View* prev, View* cur);
	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);
	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);

	// slots called from MapHandler signals
	void attributeAdded(unsigned int orbit, const QString& nameAttr);

public slots:
	// slots for Python calls
//	void changePositionAttribute(const QString& view, const QString& map, const QString& attrName);
//	void changeDrawDarts(const QString& view, const QString& map, bool b);
//	void changeDartsColor(const QString& view, const QString& map, QColor c);
//	void changeDrawPhi1(const QString& view, const QString& map, bool b);
//	void changePhi1Color(const QString& view, const QString& map, QColor c);
//	void changeDrawPhi2(const QString& view, const QString& map, bool b);
//	void changePhi2Color(const QString& view, const QString& map, QColor c);
//	void changeEdgesScaleFactor(const QString& view, const QString& map, int i);
//	void changeFacesScaleFactor(const QString& view, const QString& map, int i);

//signals:
//	void dartSelected(Dart d);

protected:
	Surface_RenderTopo_DockTab* m_dockTab;
	QHash<View*, QHash<MapHandlerGen*, MapParameters> > h_viewParameterSet;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
