#ifndef _VIEW_H_
#define _VIEW_H_

#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>
#include <QGLViewer/manipulatedFrame.h>

#include "dialogList.h"

#include "schnapps.h"
#include "Utils/gl_matrices.h"
#include "Utils/GLSLShader.h"
#include "Utils/Shaders/shaderWallPaper.h"
#include "Utils/drawer.h"


#include "camera.h"
#include "viewButtonArea.h"
#include "plugin_interaction.h"

#include "dll.h"

namespace CGoGN
{

namespace SCHNApps
{

/**
* @brief View class inherit from QGLViewer(http://libqglviewer.com/refManual/classQGLViewer.html) allow rendering of plugins.
* It can be link with several:
* - plugins
* - maps
* - cameras (only one is used)
* One view of SCHNApps is selected (green framed).
*
* Python callable slots are tagged with [PYTHON]
*/
class SCHNAPPS_API View : public QGLViewer
{
	Q_OBJECT

	friend class SCHNApps;

public:
	static unsigned int viewCount;

	View(const QString& name, SCHNApps* s, const QGLWidget* shareWidget);
	~View();

	const QString& getName() const { return m_name; }

	/**
	 * @brief get the min and max points of the bounding box of the scene
	 * @param bbMin min to be filled
	 * @param bbMax max to be filled
	 */
	void getBB(qglviewer::Vec& bbMin, qglviewer::Vec& bbMax) const { bbMin = m_bbMin; bbMax = m_bbMax; }

	/// hide all dialogs of the view
	void hideDialogs();

public slots:
	/**
	 * @brief get the name of view
	 * @return name
	 */
	QString getName() { return m_name; }

	/// get schnnapps ptr
	SCHNApps* getSCHNApps() const { return m_schnapps; }

	/**
	 * @brief [PYTHON] test if the view is the selected one
	 * @return
	 */
	bool isSelectedView() const { return m_schnapps->getSelectedView() == this; }

	/**
	 * @brief set the current camera of the view
	 * @param c the camera ptr
	 */
	void setCurrentCamera(Camera* c);

	/**
	* @brief [PYTHON] set the current camera of the view
	* @param name the name of camera
	*/
	void setCurrentCamera(const QString& name);

	/**
	* @brief [PYTHON] get the current camera of the view
	* @return the camera object
	*/
	Camera* getCurrentCamera() const { return m_currentCamera; }

	/**
	 * @brief test if this view use a camera
	 * @param c camera ptr
	 * @return
	 */
	bool usesCamera(Camera* c) const { return m_currentCamera == c; }

	/**
	* @brief [PYTHON] test if a camera is the current camera
	* @param name the name of camera
	*/
	bool usesCamera(const QString& name) const;

	void linkPlugin(PluginInteraction* plugin);

	/**
	* @brief [PYTHON] link a plugin with the view
	* @param name the name of plugin
	*/
	void linkPlugin(const QString& name);

	void unlinkPlugin(PluginInteraction* plugin);

	/**
	* @brief [PYTHON] unlink a plugin of the view
	* @param name the name of plugin
	*/
	void unlinkPlugin(const QString& name);

	const QList<PluginInteraction*>& getLinkedPlugins() const { return l_plugins; }
	bool isLinkedToPlugin(PluginInteraction* plugin) const { return l_plugins.contains(plugin); }

	/**
	* @brief [PYTHON] test if the view is linked to a plugin
	* @param name the name of plugin
	*/
	bool isLinkedToPlugin(const QString& name) const;

	void linkMap(MapHandlerGen* map);

	/**
	* @brief [PYTHON] link a map with the view
	* @param name the name of map
	*/
	void linkMap(const QString& name);
	void unlinkMap(MapHandlerGen* map);

	/**
	* @brief [PYTHON] unlink a map of the view
	* @param name the name of map
	*/
	void unlinkMap(const QString& name);

	const QList<MapHandlerGen*>& getLinkedMaps() const { return l_maps; }
	bool isLinkedToMap(MapHandlerGen* map) const { return l_maps.contains(map); }

	/**
	* @brief [PYTHON] test if the view is linked to a lao
	* @param name the name of map
	*/
	bool isLinkedToMap(const QString& name) const;

private:
	bool b_updatingUI;

	virtual void init();
	virtual void preDraw();
	virtual void draw();
	virtual void postDraw();
	virtual void resizeGL(int width, int height);

	void drawButtons();
	void drawFrame();

	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);

	glm::mat4 getCurrentModelViewMatrix() const;
	glm::mat4 getCurrentProjectionMatrix() const;
	glm::mat4 getCurrentModelViewProjectionMatrix() const;

private slots:
	void closeDialogs();

	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);

	void ui_verticalSplitView(int x, int y, int globalX, int globalY);
	void ui_horizontalSplitView(int x, int y, int globalX, int globalY);
	void ui_closeView(int x, int y, int globalX, int globalY);

	void ui_mapsListView(int x, int y, int globalX, int globalY);
	void ui_pluginsListView(int x, int y, int globalX, int globalY);
	void ui_camerasListView(int x, int y, int globalX, int globalY);

	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);
	void mapCheckStateChanged(QListWidgetItem* item);

	void pluginEnabled(Plugin *plugin);
	void pluginDisabled(Plugin *plugin);
	void pluginCheckStateChanged(QListWidgetItem* item);

	void cameraAdded(Camera* camera);
	void cameraRemoved(Camera* camera);
	void cameraCheckStateChanged(QListWidgetItem* item);

	void updateBoundingBox();

signals:
	void currentCameraChanged(Camera*, Camera*);

	void mapLinked(MapHandlerGen*);
	void mapUnlinked(MapHandlerGen*);

	void pluginLinked(PluginInteraction*);
	void pluginUnlinked(PluginInteraction*);

	void boundingBoxChanged();

protected:
	QString m_name;
	SCHNApps* m_schnapps;

	Camera* m_currentCamera;
	QList<PluginInteraction*> l_plugins;
	QList<MapHandlerGen*> l_maps;

	qglviewer::Vec m_bbMin;
	qglviewer::Vec m_bbMax;

	ViewButtonArea* m_buttonArea;

	ViewButton* m_closeButton;
	ViewButton* m_VsplitButton;
	ViewButton* m_HsplitButton;

	ViewButtonArea* m_buttonAreaLeft;
	ViewButton* m_mapsButton;
	ViewButton* m_pluginsButton;
	ViewButton* m_camerasButton;

	QString m_textInfo;

	ListPopUp* m_dialogMaps;
	ListPopUp* m_dialogPlugins;
	ListPopUp* m_dialogCameras;

	Utils::Drawer* m_frameDrawer;

	Utils::Texture<2, Geom::Vec3uc>* m_textureWallpaper;
	Utils::ShaderWallPaper* m_shaderWallpaper;

	bool b_saveSnapshots;

	inline int pixelRatio() const
	{
		#if (QT_VERSION>>16) == 5
			return this->devicePixelRatio();
		#else
			return 1;
		#endif
	}
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
