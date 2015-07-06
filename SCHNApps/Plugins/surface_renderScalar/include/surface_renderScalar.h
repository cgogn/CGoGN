#ifndef _SURFACE_RENDERSCALAR_PLUGIN_H_
#define _SURFACE_RENDERSCALAR_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_renderScalar_dockTab.h"

#include "Utils/Shaders/shaderScalarField.h"

namespace CGoGN
{

namespace SCHNApps
{

enum ColorMap {
	BlueWhiteRed = 0,
	CyanWhiteRed = 1,
	BlueCyanGreenYellowRed = 2,
	BlueGreenRed = 3
};

struct MapParameters
{
	MapParameters() :
		positionVBO(NULL),
		scalarVBO(NULL),
		scalarMin(0.0f),
		scalarMax(0.0f),
		colorMap(0),
		expansion(0)
	{}

	Utils::VBO* positionVBO;
	Utils::VBO* scalarVBO;
	float scalarMin, scalarMax;
	int colorMap;
	int expansion;
};


/**
* @brief Plugin for rendering a scalar attribute over a surface with colormap.
* The scalar attribute to render must have its generated VBO.
* A position VBO must be provided for surface rendering.
* All parameters are per view / per map.
*/
class Surface_RenderScalar_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
	friend class Surface_RenderScalar_DockTab;

public:
	Surface_RenderScalar_Plugin()
	{}

	~Surface_RenderScalar_Plugin()
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
	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);
	void attributeModified(unsigned int orbit, QString nameAttr);

public slots:
	// slots for Python calls
	/**
	 * @brief [PYTHON] change the position VBO use for rendering (for specific view and map)
	 * @param view the view name
	 * @param map the map name
	 * @param vbo the position vbo name
	 */
	void changePositionVBO(const QString& view, const QString& map, const QString& vbo);

	/**
	 * @brief [PYTHON] change the scalar VBO use for rendering (for specific view and map)
	 * @param view the view name
	 * @param map the map name
	 * @param vbo the scalar vbo name
	 */
	void changeScalarVBO(const QString& view, const QString& map, const QString& vbo);

	/**
	 * @brief [PYTHON] change the kind of colormap.
	 * @param view the view name
	 * @param map the map name
	 * @param c 0:BWR 1:CWR 2:BCGYR 3:BGR
	 */
	void changeColorMap(const QString& view, const QString& map, int c);

	/**
	 * @brief [PYTHON] change the expansion of color-map
	 * @param view the view name
	 * @param map the map name
	 * @param i ????
	 */
	void changeExpansion(const QString& view, const QString& map, int i);

protected:
	Surface_RenderScalar_DockTab* m_dockTab;
	QHash<View*, QHash<MapHandlerGen*, MapParameters> > h_viewParameterSet;

	CGoGN::Utils::ShaderScalarField* m_scalarShader;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
