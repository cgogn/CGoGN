#ifndef _SURFACE_RENDERTOPO_PLUGIN_H_
#define _SURFACE_RENDERTOPO_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_renderTopo_dockTab.h"

#include "Algo/Render/GL2/topoRender.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/Shaders/shaderColorPerVertex.h"

namespace CGoGN
{

namespace SCHNApps
{


class Surface_RenderTopo_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

	friend class Surface_RenderTopo_DockTab;

public:

	struct ViewMapParam
	{
		ViewMapParam() :
			drawDarts(true),
			dartsColor("white"),
			drawPhi1(true),
			phi1Color("cyan"),
			drawPhi2(true),
			phi2Color("red")
		{}

		bool drawDarts;
		QColor dartsColor;
		bool drawPhi1;
		QColor phi1Color;
		bool drawPhi2;
		QColor phi2Color;
	};


	struct MapParam
	{
		MapParam() :
			edgesScaleFactor(0.95f),
			facesScaleFactor(0.95f),
			needUpdate(true)
		{}

		QString posAttName;
		float edgesScaleFactor;
		float facesScaleFactor;
		bool needUpdate;
	};


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
	void attributeAdded(unsigned int orbit, const QString& name);

public slots:
	// slots for Python calls
//	void changeDrawDarts(const QString& view, const QString& map, bool b);
//	void changeDartsColor(const QString& view, const QString& map, QColor c);
//	void changeDrawPhi1(const QString& view, const QString& map, bool b);
//	void changePhi1Color(const QString& view, const QString& map, QColor c);
//	void changeDrawPhi2(const QString& view, const QString& map, bool b);
//	void changePhi2Color(const QString& view, const QString& map, QColor c);

//	void changePositionAttribute(const QString& map, const QString& attrName);
//	void changeEdgesScaleFactor(const QString& map, int i);
//	void changeFacesScaleFactor(const QString& map, int i);

//signals:
//	void dartSelected(Dart d);

protected:
	Surface_RenderTopo_DockTab* m_dockTab;

	/// parameters that depend of view and map
	QHash<View*, QHash<MapHandlerGen*, ViewMapParam> > h_viewParameterSet;

	/// parameters that depend of map only
	QHash<MapHandlerGen*, MapParam> h_parameterSet;

	/// shader used in TopoRender
	CGoGN::Utils::ShaderSimpleColor* m_shaderTopo1;

};

} // namespace SCHNApps

} // namespace CGoGN

#endif
