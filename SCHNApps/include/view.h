#ifndef _VIEW_H_
#define _VIEW_H_

#include <GL/glew.h>
#include <QGLViewer/qglviewer.h>

#include "schnapps.h"
#include "Utils/gl_matrices.h"

namespace CGoGN
{

namespace SCHNApps
{

class Camera;
class ViewButtonArea;
class ViewButton;
class PluginInteraction;

class View : public QGLViewer
{
	Q_OBJECT

	friend class SCHNApps;

public:
	static unsigned int viewCount;

	View(const QString& name, SCHNApps* s, const QGLWidget* shareWidget = NULL);
	~View();

	const QString& getName() const { return m_name; }

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

	void updateCurrentCameraBB();

private slots:
	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);

	void ui_verticalSplitView(int x, int y, int globalX, int globalY);
	void ui_horizontalSplitView(int x, int y, int globalX, int globalY);
	void ui_closeView(int x, int y, int globalX, int globalY);

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

	ViewButtonArea* m_buttonArea;

	ViewButton* m_closeButton;
	ViewButton* m_VsplitButton;
	ViewButton* m_HsplitButton;

	QString m_textInfo;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
