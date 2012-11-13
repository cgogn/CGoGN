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

class Scene : public QObject{
	Q_OBJECT
public:
	Scene(QString name, Window* window, Camera* sharedCamera=NULL);
	Scene(QString name, Plugin* plugin, Window* window);
	~Scene();

	void initializeGL();
	void updateGL(View* view=NULL);
	void draw(View* view);
	void init();

	bool keyPressEvent(QKeyEvent* event);
	bool keyReleaseEvent(QKeyEvent *e);
	bool mousePressEvent(QMouseEvent* event);
	bool mouseReleaseEvent(QMouseEvent* event);
	bool mouseMoveEvent(QMouseEvent* event);
	bool wheelEvent(QWheelEvent* event);

	void setName(QString name){m_name=name;}
	QString getName(){return m_name;}

	View* getView(int num);
	QList<View*> views(){return l_view;}
	int countViews(){return l_view.size();}
	View* addNewView(Camera* c);
	void deleteView(View* view);

	void viewClickedButton(View* view, ViewButton* viewButton);

	void associateNewPlugin(Plugin* plugin,bool callBackInitGL=true);
	void suppressLinkWith(Plugin* plugin);
	void linkWithPlugin();
	void unlinkPlugin();

	bool isManual(){return (m_creator==NULL);}
	Plugin* creator(){return m_creator;}

	QList<Plugin*> linkedPlugins(){return l_plugin;}

	bool isLinked(){return !l_plugin.isEmpty();}
	bool isLinkedWith(Plugin* plugin){return l_plugin.contains(plugin);}

//	VBOHandler* addNewVBO(QString name);
//	void addVBO(VBOHandler* vbo);
//	VBOHandler* findVBO(QString name);
//	VBOHandler* findFirstVBOMatching(QRegExp regexp);
//	QList<VBOHandler*> findVBOsMatching(QRegExp regexp);
//	VBOHandler* takeVBO(VBOHandler* vbo);

	void firstViewFitSphere(float x, float y, float z, float radius);

	bool addCustomViewButton(ViewButton* viewButton);
	ViewButton* takeCustomViewButton(ViewButton* viewButton);


protected:
	Window* m_window;

	QList<View*> l_view;

	QString m_name;

	QList<Plugin*> l_plugin;

	Plugin* m_creator;

//	QList<VBOHandler*> l_vbo;

	Context* m_context;

	QList<ViewButton*> l_viewButton;

signals:
	void viewButtonClicked(View*, ViewButton*);
};


#endif
