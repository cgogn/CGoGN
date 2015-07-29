#ifndef _SURFACE_RENDERVECTOR_PLUGIN_H_
#define _SURFACE_RENDERVECTOR_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_renderVector_dockTab.h"

#include "Utils/Shaders/shaderVectorPerVertex.h"

namespace CGoGN
{

namespace SCHNApps
{

struct MapParameters
{
	MapParameters() :
		positionVBO(NULL)
	{}

	Utils::VBO* positionVBO;
	QList<Utils::VBO*> vectorVBOs;
	QList<QColor> colors;
	QList<float> scaleFactors;
};

/**
* @brief Plugin for vector attributes rendering over a surface mesh
* Each vector attribute to render must be of dim 3, and have a VBO
* The length and color of each vector can be changed.
* All parameters are per view / per map.
*/
class Surface_RenderVector_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
	friend class Surface_RenderVector_DockTab;

public:
	Surface_RenderVector_Plugin()
	{}

	~Surface_RenderVector_Plugin()
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

public slots:
	// slots for Python calls
	/**
	 * @brief [PYTHON] change the position VBO use for rendering for specific view and map
	 * @param view the view name
	 * @param map the map name
	 * @param vbo
	 */
	void changePositionVBO(const QString& view, const QString& map, const QString& vbo);

	/**
	 * @brief [PYTHON] add a VBO use for rendering for specific view and map
	 * @param view the view name
	 * @param map the map name
	 * @param vbo
	 */
	void addVectorVBO(const QString& view, const QString& map, const QString& vbo);

	/**
	 * @brief [PYTHON] remove VBO of the corresponding vector
	 * @param view the view name
	 * @param map the map name
	 * @param vbo  the vbo name
	 */
	void removeVectorVBO(const QString& view, const QString& map, const QString& vbo);

	/**
	 * @brief [PYTHON] change the vector scale factor
	 * @param view the view name
	 * @param map the map name
	 * @param vbo vbo vector name on which apply the scale
	 * @param f the floating point scale value
	 */
	void changeVectorScaleFactor(const QString& view, const QString& map, const QString& vbo, float f);

	/**
	 * @brief [PYTHON] change thet vector color
	 * @param view the view name
	 * @param map the map name
	 * @param vbo the vbo vector name
	 * @param col the color in QString format ("red" or "#FF0000")
	 */
	void changeVectorColor(const QString& view, const QString& map, const QString& vbo, const QString& col);

protected:
	Surface_RenderVector_DockTab* m_dockTab;
	QHash<View*, QHash<MapHandlerGen*, MapParameters> > h_viewParameterSet;

	CGoGN::Utils::ShaderVectorPerVertex* m_vectorShader;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
