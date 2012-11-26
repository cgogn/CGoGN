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

	bool keyPressEvent(QKeyEvent* event);
	bool keyReleaseEvent(QKeyEvent *e);
	bool mousePressEvent(QMouseEvent* event);
	bool mouseReleaseEvent(QMouseEvent* event);
	bool mouseMoveEvent(QMouseEvent* event);
	bool wheelEvent(QWheelEvent* event);

	/*********************************************************
	 * MANAGE VIEWS
	 *********************************************************/
	View* addView(Camera* c = NULL);
	void removeView(View* view);
	View* getView(int num);
	QList<View*> getViews() { return l_views; }
	int getNbViews() { return l_views.size(); }

	void viewClickedButton(View* view, ViewButton* viewButton);

	/*********************************************************
	 * MANAGE PLUGINS
	 *********************************************************/
	void addPlugin(Plugin* plugin, bool callBackInitGL = true);
	void removePlugin(Plugin* plugin);
	QList<Plugin*> getPlugins() { return l_plugins; }
	bool hasPlugins() { return !l_plugins.isEmpty(); }
	bool hasPlugin(Plugin* plugin) { return l_plugins.contains(plugin); }
//	void linkWithPlugin();
//	void unlinkPlugin();

	/*********************************************************
	 * MANAGE VIEW BUTTONS
	 *********************************************************/
	bool addViewButton(ViewButton* viewButton);
	ViewButton* removeViewButton(ViewButton* viewButton);

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
	QList<ViewButton*> l_viewButtons;

signals:
	void viewButtonClicked(View*, ViewButton*);
};

#endif
