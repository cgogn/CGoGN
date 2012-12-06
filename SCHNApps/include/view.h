#ifndef _VIEW_H_
#define _VIEW_H_

#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>

#include "types.h"
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

	View(const QString& name, Window* w, QWidget* parent, const QGLWidget* shareWidget = NULL);
	~View();

	const QString& getName() { return m_name; }
	void setName(const QString& name) { m_name = name; }

	Window* getWindow() { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	virtual void init();
	virtual void preDraw();
	virtual void draw();
	virtual void postDraw();
	virtual void resizeGL(int width, int height);

	void drawText();
	void drawButtons();

	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);

	void drawOverpaint(QPainter *painter);

//	virtual void paintGL() { update(); }
//	virtual void paintEvent(QPaintEvent *event);

	/*********************************************************
	 * MANAGE LINKED CAMERA
	 *********************************************************/

	Camera* getCurrentCamera() { return m_currentCamera; }
	void setCurrentCamera(Camera* c);

	/*********************************************************
	 * MANAGE LINKED PLUGINS
	 *********************************************************/

	void linkPlugin(Plugin* plugin);
	void unlinkPlugin(Plugin* plugin);
	QList<Plugin*> getLinkedPlugins() { return l_plugins; }
	bool isLinkedToPlugin(Plugin* plugin) { return l_plugins.contains(plugin); }

	/*********************************************************
	 * MANAGE LINKED MAPS
	 *********************************************************/

	void linkMap(MapHandler* map);
	void unlinkMap(MapHandler* map);
	QList<MapHandler*> getLinkedMaps() { return l_maps; }
	bool isLinkedToMap(MapHandler* map) { return l_maps.contains(map); }




	void updateTextInfo();

	glm::mat4 getCurrentModelViewMatrix();
	glm::mat4 getCurrentProjectionMatrix();
	glm::mat4 getCurrentModelViewProjectionMatrix();

	void setCurrentModelViewMatrix(const glm::mat4& mvm);
	void setCurrentProjectionMatrix(const glm::mat4& pm);

protected:
	QString m_name;
	Window* m_window;

	Camera* m_currentCamera;
	QList<Plugin*> l_plugins;
	QList<MapHandler*> l_maps;

	ViewButtonArea* m_buttonArea;

	ViewButton* m_cameraButton;
	ViewButton* m_pluginsButton;
	ViewButton* m_mapsButton;
	ViewButton* m_closeButton;

	QString m_textInfo;

	qglviewer::Vec bbmin;
	qglviewer::Vec bbmax;

	CameraViewDialog* m_cameraViewDialog;
	PluginsViewDialog* m_pluginsViewDialog;
	MapsViewDialog* m_mapsViewDialog;

public slots:
	void cb_cameraView(int x, int y, int globalX, int globalY);
	void cb_pluginsView(int x, int y, int globalX, int globalY);
	void cb_mapsView(int x, int y, int globalX, int globalY);
	void cb_closeView(int x, int y, int globalX, int globalY);

//signals:
//	void currentCameraChanged(Camera* camera);
};

#endif
