#ifndef _SURFACE_RENDER_PLUGIN_H_
#define _SURFACE_RENDER_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_render_dockTab.h"

#include "Utils/Shaders/shaderFlat.h"
#include "Utils/Shaders/shaderPhong.h"
#include "Utils/Shaders/shaderColorPerVertex.h"
#include "Utils/Shaders/shaderSimpleColor.h"
#include "Utils/pointSprite.h"

namespace CGoGN
{

namespace SCHNApps
{

class MapHandlerGen;

struct MapParameters
{
	enum FaceShadingStyle
	{
		FLAT = 0,
		PHONG = 1
	};

	MapParameters() :
		positionVBO(NULL),
		normalVBO(NULL),
		colorVBO(NULL),
		verticesScaleFactor(1.0f),
		renderVertices(false),
		renderEdges(false),
		renderFaces(true),
		faceStyle(FLAT),
		diffuseColor(0.85f,0.25f,0.19f,0.0f),
		simpleColor(0.0f,0.0f,0.0f,0.0f),
		vertexColor(0.0f,0.0f,1.0f,0.0f),
		backColor(0.85f, 0.25f, 0.19f, 0.0f)
	{}

	Utils::VBO* positionVBO;
	Utils::VBO* normalVBO;
	Utils::VBO* colorVBO;

	float verticesScaleFactor;
	float basePSradius;
	bool renderVertices;
	bool renderEdges;
	bool renderFaces;
	bool renderBoundary;
	bool renderBackfaces;
	FaceShadingStyle faceStyle;

	Geom::Vec4f diffuseColor;
	Geom::Vec4f simpleColor;
	Geom::Vec4f vertexColor;
	Geom::Vec4f backColor;
};

/**
* @brief Plugin for 2-Maps topology rendering.
* Some parameters are per view / per map:
* - drawing of relations and dart
* - colors
* Others are only per map:
* - position attribute
* - scale factors
*/
class Surface_Render_Plugin : public PluginInteraction
{
	Q_OBJECT

	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif

	friend class Surface_Render_DockTab;

public:
	Surface_Render_Plugin()
	{}

	~Surface_Render_Plugin()
	{}

private:
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
	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);
	void boundingBoxModified();

	void schnappsClosing();

public slots:
	// slots for Python calls
	/**
	 * @brief [PYTHON] change the position VBO use for rendering for specific view and map
	 * @param view the view name
	 * @param map the map name
	 * @param vbo the positionvbo name
	 */
	void changePositionVBO(const QString& view, const QString& map, const QString& vbo);

	/**
	 * @brief [PYTHON] change the normal VBO use for rendering (phong) for specific view and map
	 * @param view the view name
	 * @param map the map name
	 * @param vbo the normal vbo name
	 */
	void changeNormalVBO(const QString& view, const QString& map, const QString& vbo);

	/**
	 * @brief [PYTHON] change the color VBO use for rendering for specific view and map
	 * @param view the view name
	 * @param map the map name
	 * @param vbo the normal vbo name
	 */
	void changeColorVBO(const QString& view, const QString& map, const QString& vbo);

	/**
	 * @brief [PYTHON] choice if vertices should be rendered (for specific view and map)
	 * @param view the view name
	 * @param map the map name
	 * @param b rendered or not
	 */
	void changeRenderVertices(const QString& view, const QString& map, bool b);

	/**
	 * @brief [PYTHON] change the size of vertices VerticesScaleFactor
	 * @param view the view name
	 * @param map the map name
	 * @param f scale parameter [0,2]
	 */
	void changeVerticesScaleFactor(const QString&view, const QString& map, float f);

	/**
	 * @brief [PYTHON] choice if edges should be rendered (for specific view and map)
	 * @param view the view name
	 * @param map the map name
	 * @param b rendered or not
	 */
	void changeRenderEdges(const QString& view, const QString& map, bool b);

	/**
	 * @brief [PYTHON] choice if faces should be rendered (for specific view and map)
	 * @param view the view name
	 * @param map the map name
	 * @param b rendered or not
	 */
	void changeRenderFaces(const QString& view, const QString& map, bool b);

	/**
	 * @brief [PYTHON] change the style of rendering of faces (flat of phong)
	 * @param view the view name
	 * @param map the map name
	 * @param style 0:flat 1:phong
	 */
	void changeFacesStyle(const QString& view, const QString& map, int style);

	/**
	 * @brief [PYTHON] choose if boundary should be rendered or not.
	 * @param view the view name
	 * @param map the map name
	 * @param b rendered or not
	 */
	void changeRenderBoundary(const QString& view, const QString& map, bool b);

	/**
	 * @brief [PYTHON] change the color use for face rendering (when no volor VBO is used)
	 * @param view the view name
	 * @param map the map name
	 * @param r red value [0,1]
	 * @param g green value [0,1]
	 * @param b blue value [0,1]
	 */
	void changeFaceColor(const QString& view, const QString& map, float r, float g, float b);

	/**
	 * @brief [PYTHON] change the color use for edge line rendering.
	 * @param view the view name
	 * @param map the map name
	 * @param r red value [0,1]
	 * @param g green value [0,1]
	 * @param b blue value [0,1]
	 */
	void changeEdgeColor(const QString& view, const QString& map, float r, float g, float b);

	/**
	 * @brief [PYTHON] change the color use for vertices point sprite rendering.
	 * @param view the view name
	 * @param map the map name
	 * @param r red value [0,1]
	 * @param g green value [0,1]
	 * @param b blue value [0,1]
	 */
	void changeVertexColor(const QString& view, const QString& map, float r, float g, float b);

	/**
	 * @brief [PYTHON] change the color use for back-faces.
	 * @param view the view name
	 * @param map the map name
	 * @param r red value [0,1]
	 * @param g green value [0,1]
	 * @param b blue value [0,1]
	 */
	void changeBackColor(const QString& view, const QString& map, float r, float g, float b);

	/**
	 * @brief [PYTHON] choose if back-faces should be rendered.
	 * @param view the view name
	 * @param map the map name
	 * @param b
	 */
	void changeRenderBackfaces(const QString& view, const QString& map, bool b);


protected:
	Surface_Render_DockTab* m_dockTab;
	QHash<View*, QHash<MapHandlerGen*, MapParameters> > h_viewParameterSet;

	CGoGN::Utils::ShaderFlat* m_flatShader;
	CGoGN::Utils::ShaderPhong* m_phongShader;
	CGoGN::Utils::ShaderColorPerVertex* m_colorPerVertexShader;
	CGoGN::Utils::ShaderSimpleColor* m_simpleColorShader;
	CGoGN::Utils::PointSprite* m_pointSprite;

};

} // namespace SCHNApps

} // namespace CGoGN

#endif
