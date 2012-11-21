#ifndef _SIMPLEPLUGIN_H_
#define _SIMPLEPLUGIN_H_

#include "plugin.h"

class SimplePlugin : public QObject, public Plugin
{
	Q_OBJECT

public:
	SimplePlugin();
	~SimplePlugin();

	void cb_updateMatrix(View* view) {}
	void cb_redraw(Scene* scene) {}
	void cb_initGL(Scene* scene) {}

	void updateGL()	{}
	void updateGL(Scene* scene)	{}

	bool cb_keyPress(Scene* scene, int event) { return false; }
	bool cb_keyRelease(Scene* scene, int event) { return false; }
	bool cb_mousePress(Scene* scene, int button, int x, int y) { return false; }
	bool cb_mouseRelease(Scene* scene, int button, int x, int y) { return false; }
	bool cb_mouseClick(Scene* scene, int button, int x, int y) { return false; }
	bool cb_mouseMove(Scene* scene, int buttons, int x, int y) { return false; }
	bool cb_wheelEvent(Scene* scene, int delta, int x, int y) { return false; }

	void cb_recievedMap(MapHandler* map) {}
	void cb_removingMap(MapHandler* map) {}

	bool isWaitingForScene() { return false; }
	void recieveScene(Scene* scene) {}
	void deleteLinkWithScene(Scene* scene);
	bool hasManualLinkWithScene(Scene* scene) { return false; }

protected:
	template<typename T>
	T* getDependencie(QString dependencie){ return( (T*)(this->addDependencie(dependencie)) ); }

	bool addWidgetInDockTab(QWidget* newTabWidget, QString tabText);
	void removeTabInDock(QWidget* tabWidget);

	bool addReferencedMap(QString map_name, CGoGN::GenericMap* map);
	template<typename T>
	bool addNewReferencedMap(QString map_name, T* &map){
		if(m_window){
			bool r= m_window->addNewReferencedMap<T>(map_name, map);
			if(r) l_map.push_back(map);
			return r;
		}
		else{
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
			return false;
		}
	}
	template<typename T>
	T* getReferencedMap(QString map_name){
		if(m_window){
			return m_window->getReferencedMap<T>(map_name);
		}
		else{
			System::Error::code= System::Error::BAD_LINK_PLUGIN_WINDOW_f(m_name);
			return NULL;
		}
	}


	QList<QAction*> l_action;

	QAction* addMenuAction(QString menuPath, const char* method);
	void deleteMenuActions();

};
