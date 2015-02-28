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


#include "dll.h"


namespace CGoGN
{

namespace SCHNApps
{

class Camera;
class ViewButtonArea;
class ViewButton;
class PluginInteraction;


class SCHNAPPS_API View : public QGLViewer
{
	Q_OBJECT

	friend class SCHNApps;

public:
	static unsigned int viewCount;

	View(const QString& name, SCHNApps* s, QGLFormat& format);
	View(const QString& name, SCHNApps* s,  QGLFormat& format, const QGLWidget* shareWidget);
	~View();

	const QString& getName() const { return m_name; }

	MapHandlerGen* lastSelectedMap() { return m_lastSelectedMap;}

	void closeDialogs();

public slots:
	QString getName() { return m_name; }
	SCHNApps* getSCHNApps() const { return m_schnapps; }

	bool isSelectedView() const { return m_schnapps->getSelectedView() == this; }

	void setCurrentCamera(Camera* c);
	void setCurrentCamera(const QString& name);

	Camera* getCurrentCamera() const { return m_currentCamera; }
	bool usesCamera(const QString& cameraName) const;

	void linkPlugin(PluginInteraction* plugin);
	void linkPlugin(const QString& name);
	void unlinkPlugin(PluginInteraction* plugin);
	void unlinkPlugin(const QString& name);

	const QList<PluginInteraction*>& getLinkedPlugins() const { return l_plugins; }
	bool isLinkedToPlugin(PluginInteraction* plugin) const { return l_plugins.contains(plugin); }
	bool isLinkedToPlugin(const QString& name) const;

	void linkMap(MapHandlerGen* map);
	void linkMap(const QString& name);
	void unlinkMap(MapHandlerGen* map);
	void unlinkMap(const QString& name);

	const QList<MapHandlerGen*>& getLinkedMaps() const { return l_maps; }
	bool isLinkedToMap(MapHandlerGen* map) const { return l_maps.contains(map); }
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

//	void updateCurrentCameraBB();

private slots:
	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);

	void ui_verticalSplitView(int x, int y, int globalX, int globalY);
	void ui_horizontalSplitView(int x, int y, int globalX, int globalY);
	void ui_closeView(int x, int y, int globalX, int globalY);

	void ui_mapsListView(int x, int y, int globalX, int globalY);
	void ui_pluginsListView(int x, int y, int globalX, int globalY);
	void ui_camerasListView(int x, int y, int globalX, int globalY);

	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);
	void pluginEnabled(Plugin *plugin);
	void pluginDisabled(Plugin *plugin);
	void cameraAdded(Camera* camera);
	void cameraRemoved(Camera* camera);

	void mapCheckStateChanged(QListWidgetItem* item);
	void pluginCheckStateChanged(QListWidgetItem* item);
	void cameraCheckStateChanged(QListWidgetItem* item);

signals:
	void currentCameraChanged(Camera*, Camera*);

	void mapLinked(MapHandlerGen*);
	void mapUnlinked(MapHandlerGen*);

	void pluginLinked(PluginInteraction*);
	void pluginUnlinked(PluginInteraction*);

protected:
	QString m_name;
	SCHNApps* m_schnapps;

	Camera* m_currentCamera;
	QList<PluginInteraction*> l_plugins;
	QList<MapHandlerGen*> l_maps;
	MapHandlerGen* m_lastSelectedMap;

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
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
