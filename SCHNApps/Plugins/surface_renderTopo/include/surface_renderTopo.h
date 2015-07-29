#ifndef _SURFACE_RENDERTOPO_PLUGIN_H_
#define _SURFACE_RENDERTOPO_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_renderTopo_dockTab.h"

#include "Algo/Render/GL2/topoRender.h"
//#include "Utils/Shaders/shaderSimpleColor.h"
//#include "Utils/Shaders/shaderColorPerVertex.h"

namespace CGoGN
{

namespace SCHNApps
{

/**
* @brief Plugin for 2-Maps topology rendering.
* Some parameters are per view / per map:
* - drawing of relations and dart
* - colors
* Others are only per map (due to computation time):
* - position attribute
* - scale factors
*/
class Surface_RenderTopo_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
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

	virtual void keyPress(View* , QKeyEvent* ) {}
	virtual void keyRelease(View* , QKeyEvent* ) {}
	virtual void mousePress(View* , QMouseEvent* ) {}
	virtual void mouseRelease(View* , QMouseEvent* ) {}
	virtual void mouseMove(View* , QMouseEvent* ) {}
	virtual void wheelEvent(View* , QWheelEvent* ) {}

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
	/**
	 * @brief [PYTHON] choose to draw (or not) the darts
	 * @param view the view name
	 * @param map the map name
	 * @param b draw or not
	 */
	void changeDrawDarts(const QString& view, const QString& map, bool b);

	/**
	 * @brief [PYTHON] choose to draw (or not) the phi1 relations
	 * @param view the view name
	 * @param map the map name
	 * @param b draw or not
	 */
	void changeDrawPhi1(const QString& view, const QString& map, bool b);

	/**
	 * @brief [PYTHON] choose to draw (or not) the phi2 relations
	 * @param view the view name
	 * @param map the map name
	 * @param b draw or not
	 */
	void changeDrawPhi2(const QString& view, const QString& map, bool b);

	/**
	 * @brief [PYTHON] change the color use for dart
	 * @param view the view name
	 * @param map the map name
	 * @param color
	 */
	void changeDartsColor(const QString& view, const QString& map, const QString& color);

	/**
	 * @brief [PYTHON] change the color use for phi1
	 * @param view the view name
	 * @param map the map name
	 * @param color
	 */
	void changePhi1Color(const QString& view, const QString& map, const QString& color);

	/**
	 * @brief [PYTHON] change the color use for phi2
	 * @param view the view name
	 * @param map the map name
	 * @param color
	 */
	void changePhi2Color(const QString& view, const QString& map, const QString& color);

	/**
	 * @brief [PYTHON] change the position attribute of map for topo rendering
	 * @param map the map name
	 * @param attrName position attribute name
	 */
	void changePositionAttribute(const QString& map, const QString& attrName);

	/**
	 * @brief [PYTHON] change the edges scaling factor
	 * @param map the map name
	 * @param s scaling factor
	 */
	void changeEdgesScaleFactor(const QString& map, float s);

	/**
	 * @brief [PYTHON] change the faces scaling factor
	 * @param map the map name
	 * @param s scaling factor
	 */
	void changeFacesScaleFactor(const QString& map, float s);

//signals:
//	void dartSelected(Dart d);

protected:
	Surface_RenderTopo_DockTab* m_dockTab;

	/// parameters that depend of view and map
	QHash<View*, QHash<MapHandlerGen*, ViewMapParam> > h_viewParameterSet;

	/// parameters that depend of map only
	QHash<MapHandlerGen*, MapParam> h_parameterSet;

	/// shader used in TopoRender
//	CGoGN::Utils::ShaderSimpleColor* m_shaderTopo1;
	std::vector<CGoGN::Utils::GLSLShader*> m_shadersTopo;

};

} // namespace SCHNApps

} // namespace CGoGN

#endif
