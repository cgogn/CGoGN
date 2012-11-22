#ifndef _VISUALPLUGIN_H_
#define _VISUALPLUGIN_H_

#include "plugin.h"

class VisualPlugin : public QObject, public Plugin
{
	Q_OBJECT

public:
	VisualPlugin();
	~VisualPlugin();

	virtual void cb_updateMatrix(View* view);
	virtual void cb_redraw(Scene* scene) = 0;
	virtual void cb_initGL(Scene* scene) = 0;

	void updateGL();
	void updateGL(Scene* scene);

	bool cb_keyPress(Scene* scene, int event) { return false; }
	bool cb_keyRelease(Scene* scene, int event) { return false; }
	bool cb_mousePress(Scene* scene, int button, int x, int y) { return false; }
	bool cb_mouseRelease(Scene* scene, int button, int x, int y) { return false; }
	bool cb_mouseClick(Scene* scene, int button, int x, int y) { return false; }
	bool cb_mouseMove(Scene* scene, int buttons, int x, int y) { return false; }
	bool cb_wheelEvent(Scene* scene, int delta, int x, int y) { return false; }

	virtual void cb_recievedMap(MapHandler* map) {}
	virtual void cb_removingMap(MapHandler* map) {}

	bool isWaitingForScene() { return (m_waitedScene > 0 || m_waitedScene == UNLIMITED_NUMBER_OF_SCENES); }
	void recieveScene(Scene* scene);
	void deleteLinkWithScene(Scene* scene);
	bool hasManualLinkWithScene(Scene* scene) { return l_recievedScene.contains(scene); }

protected:
	std::list<Scene*> l_scene;

	bool associateScene(QString glv_name, Scene* &scene, bool cb_initGL=false);

	bool addNewScene(QString glv_name, Scene* &scene);
	bool addNewSceneDialog(QString name, Scene* &scene);

	bool addWidgetInDockTab(QWidget* newTabWidget, QString tabText);
	void removeTabInDock(QWidget* tabWidget);

	bool addReferencedMap(QString map_name, MapHandler* map);
	template<typename T>
	MapHandler* addNewReferencedMap(QString map_name, T* &map){
		if(m_window){
			MapHandler* vh= m_window->addNewReferencedMap<T>(map_name, map);
			if(vh) l_map.push_back(vh);
			return vh;
		}
		else{
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
			return NULL;
		}
	}

	MapHandler*  getReferencedMap(QString map_name);


	QList<QAction*> l_action;

	QList<QWidget*> l_customWidget;

	QAction* addMenuAction(QString menuPath, const char* method);
	bool addMenuAction(QString menuPath, QAction* action);
	void deleteActions();
	QAction* addToolbarAction(const char* method, QIcon icon= QIcon());
	bool addToolbarAction(QAction* action);


	int m_waitedScene;
	QList<Scene*> l_recievedScene;

	bool m_allowSameViewSeveralTime;
	void allowSamerViewSeveralTime(bool b){m_allowSameViewSeveralTime=b;};

	void waitingForScene(int nb=UNLIMITED_NUMBER_OF_SCENES);

	ViewButton* addSceneNewViewButton(Scene* scene, QString imagePath);
	bool addSceneViewButton(Scene* scene, ViewButton* viewButton);
	void takeSceneViewButton(Scene* scene, ViewButton* viewButton);

	QList<ViewButton*> l_viewButtons;

signals:
	void recievedScene(Scene*);
};

#endif
