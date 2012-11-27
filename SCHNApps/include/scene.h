#ifndef _SCENE_H_
#define _SCENE_H_

#include <QList>
#include <QString>
#include <QHash>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "types.h"

//forward declarations
class View;
class Window;
class Camera;
class Plugin;
class VBOHandler;
class Context;
class ViewButton;

class Scene : public QObject
{
	Q_OBJECT

public:
	Scene(const QString& name, Window* window);
	~Scene();

	const QString& getName() { return m_name; }
	void setName(const QString& name) { m_name = name; }

	Window* getWindow() { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	Context* getContext() { return m_context; }
	void setContext(Context* c) { m_context = c; }

	void initGL();
	void updateGL();
	void draw(View *v);

	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);

	/*********************************************************
	 * MANAGE VIEWS
	 *********************************************************/
	void linkView(View* view);
	void unlinkView(View* view);
	View* getLinkedView(unsigned int num);
	QList<View*> getLinkedViews() { return l_views; }
	int getNbLinkedViews() { return l_views.size(); }

	/*********************************************************
	 * MANAGE PLUGINS
	 *********************************************************/
	void linkPlugin(Plugin* plugin, bool callInitGL = true);
	void unlinkPlugin(Plugin* plugin);
	QList<Plugin*> getLinkedPlugins() { return l_plugins; }
	bool isLinkedToPlugins() { return !l_plugins.isEmpty(); }
	bool isLinkedToPlugin(Plugin* plugin) { return l_plugins.contains(plugin); }
//	void linkWithPlugin();
//	void unlinkPlugin();

//	VBOHandler* addNewVBO(QString name);
//	void addVBO(VBOHandler* vbo);
//	VBOHandler* findVBO(QString name);
//	VBOHandler* findFirstVBOMatching(QRegExp regexp);
//	QList<VBOHandler*> findVBOsMatching(QRegExp regexp);
//	VBOHandler* takeVBO(VBOHandler* vbo);

	void firstViewFitSphere(float x, float y, float z, float radius);

protected:
	QString m_name;
	Window* m_window;
	Context* m_context;

	QList<View*> l_views;
	QList<Plugin*> l_plugins;
//	QList<VBOHandler*> l_vbo;
//	QList<ViewButton*> l_viewButtons;
};

#endif
