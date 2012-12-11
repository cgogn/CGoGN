#ifndef _VIEW_H_
#define _VIEW_H_

#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>

#include "types.h"
#include "window.h"
#include "Utils/gl_matrices.h"

class ViewButtonArea;
class ViewButton;

class CameraViewDialog;
class PluginsViewDialog;
class MapsViewDialog;

class View : public QGLViewer
{
	Q_OBJECT

public:
	static unsigned int viewCount;

	View(const QString& name, Window* w, const QGLWidget* shareWidget = NULL);
	~View();

	const QString& getName() const { return m_name; }
	void setName(const QString& name) { m_name = name; }

	Window* getWindow() const { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	bool isCurrentView() const { return m_window->getCurrentView() == this; }

	virtual void init();
	virtual void preDraw();
	virtual void draw();
	virtual void postDraw();
	virtual void resizeGL(int width, int height);

	void drawText();
	void drawButtons();
	void drawFrame();

	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);

	/*********************************************************
	 * MANAGE LINKED CAMERA
	 *********************************************************/

	Camera* getCurrentCamera() const { return m_currentCamera; }
	void setCurrentCamera(Camera* c);

	/*********************************************************
	 * MANAGE LINKED PLUGINS
	 *********************************************************/

	void linkPlugin(Plugin* plugin);
	void unlinkPlugin(Plugin* plugin);
	const QList<Plugin*>& getLinkedPlugins() const { return l_plugins; }
	bool isLinkedToPlugin(Plugin* plugin) const { return l_plugins.contains(plugin); }

	/*********************************************************
	 * MANAGE LINKED MAPS
	 *********************************************************/

	void linkMap(MapHandlerGen* map);
	void unlinkMap(MapHandlerGen* map);
	const QList<MapHandlerGen*>& getLinkedMaps() const { return l_maps; }
	bool isLinkedToMap(MapHandlerGen* map) const { return l_maps.contains(map); }

	void updateViewBB();

	/*********************************************************
	 * MANAGE MATRICES
	 *********************************************************/

	glm::mat4 getCurrentModelViewMatrix() const;
	glm::mat4 getCurrentProjectionMatrix() const;
	glm::mat4 getCurrentModelViewProjectionMatrix() const;

//	void setCurrentModelViewMatrix(const glm::mat4& mvm);
//	void setCurrentProjectionMatrix(const glm::mat4& pm);

protected:
	QString m_name;
	Window* m_window;

	Camera* m_currentCamera;
	QList<Plugin*> l_plugins;
	QList<MapHandlerGen*> l_maps;

	ViewButtonArea* m_buttonArea;

	ViewButton* m_cameraButton;
	ViewButton* m_pluginsButton;
	ViewButton* m_mapsButton;
	ViewButton* m_closeButton;
	ViewButton* m_VsplitButton;
	ViewButton* m_HsplitButton;

	QString m_textInfo;

	CameraViewDialog* m_cameraViewDialog;
	PluginsViewDialog* m_pluginsViewDialog;
	MapsViewDialog* m_mapsViewDialog;

public slots:
	void cb_cameraView(int x, int y, int globalX, int globalY);
	void cb_pluginsView(int x, int y, int globalX, int globalY);
	void cb_mapsView(int x, int y, int globalX, int globalY);
	void cb_closeView(int x, int y, int globalX, int globalY);
	void cb_VsplitView(int x, int y, int globalX, int globalY);
	void cb_HsplitView(int x, int y, int globalX, int globalY);
};

#endif
