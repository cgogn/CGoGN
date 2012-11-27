#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "ui_window.h"

#include <QVBoxLayout>

#include "types.h"
#include "system.h"
#include "splitArea.h"

class Plugin;
class Scene;
class View;
class Camera;
//class Context;
class MapHandler;

class Window : public QMainWindow, Ui::Window
{
	Q_OBJECT

public:
	/**
	 * \fn Window(QWidget* parent = NULL)
	 * \brief Default (and unique) constructor
	 *
	 * \param parent the parent of the window
	 */
	Window(QWidget* parent = NULL);

	/**
	 * \fn ~Window()
	 * \brief the class destructor
	 */
	~Window();

	QGLContext* getContext() { return m_context; }

	/*********************************************************
	 * MANAGE DOCK
	 *********************************************************/

	/**
	 * \fn QTabWidget* getDockTabWidget()
	 * \brief Accessor to the QTabWidget of this interface
	 *
	 * \return a pointer to the TabWidget, NULL if not allocated yet
	 */
	QTabWidget* getDockTabWidget();

	/**
	 * \fn void addTabInDock(QWidget* tabWidget, const QString& tabText)
	 * \brief Adds the widget as a new tab in the interface's dock
	 *
	 * \param tabWidget the created and allocated pointer to the QWidget to add in the dock
	 * \param tabText The text that will appears in the tab label
	 *
	 * \see removeTabInDock()
	 */
	void addTabInDock(QWidget* tabWidget, const QString& tabText);

	/**
	 * \fn void removeTabInDock(QWidget* tabWidget)
	 * \brief removes a given widget from the tab
	 *
	 * \param tabWidget the reference to the widget you want to remove
	 *
	 * If the widget does belong to the tab, it will be destroyed.
	 *
	 * \see addWidgetInDockTab()
	 */
	void removeTabInDock(QWidget* tabWidget);

	/*********************************************************
	 * MANAGE MENU ACTIONS
	 *********************************************************/

	/**
	 * \fn bool addMenuAction(const QString& menuPath, QAction* action)
	 * \brief adds an action in the program menu bar
	 *
	 * \param menuPath the menu path (see details below) to specify a location for the action in the menu.
	 * \param action a pointer to the allocated action to add in the menu. All the details on that action (such as
	 *          QObject::connect() calls) are not dealt in this function.
	 *
	 *  The menu path is a string used to specify the location of the new action in the menu bar.
	 *  In such path every menu step of the path should be separate by ';'. The action is the last substring after
	 *  the last ';', and will be added in the menu with that name. All non-existing menu specified in the path
	 *  shall be created.
	 *
	 *  <b>Example 1:</b> if you want to add an entry <em>"action"</em> to the existing menu <em>"Plugins"</em> the
	 *                  menu path shall be: <em>Plugins;action</em>
	 *
	 *  <b>Example 2:</b> if you want to add an entry <em>"action"</em> in a new menu <em>"config"</em> that is also a
	 *                  submenu of a new menu <em>"Settings"</em>, the menu path
	 *                  shall be: <em>Settings;config;action</em>
	 *
	 *  A new action should at least belong to one menu (already existing or not). Otherwise the method will fail.
	 *
	 *  This method is called by Plugin methods
	 *
	 *  \return a boolean whether the method succeeded or not.
	 *
	 * If the function failed, the error code ( Error::code ) is affected with a value
	 *    depending on the error. This error can be shown with Error::showError
	 *
	 * \see deleteMenuAction()
	 * \see Plugin::addMenuAction()
	 */
	bool addMenuAction(const QString& menuPath, QAction* action);

	/**
	 * \fn void deleteMenuAction(QAction* action)
	 * \brief delete an action from the menu bar
	 *
	 * \param action pointer to the allocated action to delete.
	 *
	 *  If this action was the only action remaining in a menu, this menu will also be deleted.
	 *
	 *  \warning DO NOT use this method with an action that hasn't been added with addMenuAction()
	 *
	 *  This method is called by Plugin methods
	 *
	 *  \see addMenuAction()
	 *  \see Plugin::removeMenuAction()
	 */
	void removeMenuAction(QAction* action);

	/*********************************************************
	 * MANAGE TOOLBAR ACTIONS
	 *********************************************************/

	bool addToolbarAction(QAction* action);

	void removeToolbarAction(QAction* action);

	/*********************************************************
	 * MANAGE PLUGINS
	 *********************************************************/

	/**
	 * \fn bool loadPlugin(QString pluginPath)
	 * \brief Loads and references a Plugin
	 *
	 * \param pluginPath the absolute path of the Plugin location
	 *
	 * The Plugin is loaded and referenced under a name that is the Plugin file name
	 * where the extension ('.so', '.dylib') and the prefix 'lib' were removed.
	 *
	 *  <b>Example:</b> <em>/path/libExample.so</em> will be referenced as <em>Example</em>
	 *
	 * You have to make sure that a Plugin file with a similar name hasn't been loaded yet, otherwise
	 * the loading will fail.
	 *
	 * This method calls the Plugin::enable() method of the concerned Plugin. That is why
	 * when Plugin are written, this method is overriden and used as an initialization method.
	 *
	 * \warning In the program this method is called under specific and controlled circumstances, you should probably not call it.
	 *
	 * \return a boolean whether the loading succeeded or not.
	 *
	 * If the function failed, the error code ( Error::code ) is affected with a value
	 * depending on the error. This error can be shown with Error::showError
	 *
	 * \see unloadPlugin()
	 * \see getPlugins()
	 * \see Plugin::enable()
	 */
	Plugin* loadPlugin(const QString& pluginPath);

	/**
	 * \fn void unloadPlugin(QString pluginName)
	 * \brief Unload the Plugin of a given name
	 *
	 * \param pluginName the name under which the Plugin is referenced
	 *
	 * The Plugin of the given name is dereferenced and deleted, if it exists and was previously
	 * referenced, if not, the method does nothing.
	 *
	 * This method calls the Plugin::disable() method of the concerned Plugin. That is why,
	 * when Plugin are written, this method is overriden and used as a destruction method.
	 *
	 * \warning In the program this method is called under specific and controlled circumstances, you should probably not call it.
	 *
	 * \see loadPlugin()
	 * \see getPlugins()
	 * \see Plugin::disable()
	 */
	void unloadPlugin(const QString& pluginName);

	/**
	 * \fn Plugin* checkPluginDependencie(QString name, Plugin* dependantPlugin)
	 * \brief checks for a dependencie Plugin, and set the dependencie link for the found Plugin
	 *
	 * \param name the name of the Plugin you have to check the existence in order to make a dependencie
	 * \param dependantPlugin a reference to the Plugin that asks for the dependencie that will be set as a
	 *          dependant Plugin for the found Plugin
	 *
	 * \warning In the program this method is called under specific and controlled circumstances
	 *          by the Plugins you should probably not call it.
	 *
	 * \return a pointer to the found dependencie Plugin, NULL if this Plugin wasn't referenced
	 *
	 * If the function failed, the error code ( Error::code ) is affected with a value
	     depending on the error. This error can be shown with Error::showError
	 *
	 * \see loadPlugin()
	 * \see VisualPlugin::addDependencie()
	 */
//	Plugin *checkPluginDependencie(QString name, Plugin *dependantPlugin);

	Plugin* getPlugin(const QString& name);

	QList<Plugin*> getPlugins() { return h_plugins.values(); }

	/*********************************************************
	 * MANAGE SCENES
	 *********************************************************/

	Scene* addScene(const QString& name);
	void removeScene(const QString& name);
	Scene* getScene(const QString& name);
	QList<Scene*> getScenes() { return h_scenes.values(); }

//	bool addNewEmptyScene(QString name, Scene *&scene, bool dialog, Camera *sharedCamera = NULL);
//	bool addNewSceneView(Scene *scene, View *view);

//	bool associateSceneWithPlugin(QString glviewer, Plugin *plugin, Scene *&scene, bool cb_initGL = false);

//	bool addNewSceneFromPlugin(QString name, Plugin *plugin, Scene *&scene);
//	bool addNewSceneFromPluginDialog(QString name, Plugin *plugin, Scene *&scene);

//	void linkDialog(Scene *scene);
//	void unlinkDialog(Scene *scene, QList<Plugin *> dependingPlugins);

	/*********************************************************
	 * MANAGE VIEWS
	 *********************************************************/

	View* addView(const QString& name);
	void removeView(const QString& name);
	View* getView(const QString& name);
	QList<View*> getView() { return h_views.values(); }

	/*********************************************************
	 * MANAGE CAMERAS
	 *********************************************************/

	Camera* addCamera(const QString& name);
	void removeCamera(const QString& name);
	Camera* getCamera(const QString& name);
	QList<Camera*> getCameras() { return h_cameras.values(); }

	/*********************************************************
	 * MANAGE MAPS
	 *********************************************************/

	bool addMap(const QString& name, MapHandler* map);
	void removeMap(const QString& name);
	MapHandler* getMap(const QString& name);
	QList<MapHandler*> getMaps() { return h_maps.values(); }

//	template<typename T>
//	T* getReferencedMap(QString map_name){
//		MapHash::iterator it;
//		if((it=h_map.find(map_name))!=h_map.end()){
//			return ((T*)(*it));
//		}
//		else{
//			System::Error::code= System::Error::MAP_UNREFERENCED_f(map_name);
//			return NULL;
//		}
//	}

protected:
	bool m_initialization;

	QVBoxLayout* m_verticalLayout;
	SplitArea* m_splitArea;

	QGLContext* m_context;

	QDockWidget* m_dock;
	QTabWidget* m_dockTabWidget;

	PluginHash h_plugins;
	SceneHash h_scenes;
	ViewHash h_views;
	CameraHash h_cameras;
	MapHash h_maps;

	/**
	 * \var bool keys[3]
	 * \brief a static tab to store state of some keys (here: M, Shift and CTRL)
	 */
	bool keys[3];
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	/**
	 * \fn void moveView()
	 * \brief shows a reordering dialog for the Views
	 *
	 * Make a call with the right parameters to the dialog class
	 * GLVSelector and show this dialog.
	 *
	 * This method is meant to be called when the user press
	 * CTRL+Shift+M.
	 */
	void moveView();

public slots:
	/**
	 * \fn void cb_about_SCHNApps();
	 * \brief function that is called when the "about SCHNApps" menu action is triggered
	 */
	void cb_aboutSCHNApps();

	/**
	 * \fn void cb_about_CGoGN();
	 * \brief function that is called when the "about CGOGN" menu action is triggered
	 */
	void cb_aboutCGoGN();

	/**
	 * \fn void cb_managePlugins()
	 * \brief method called when the "Plugins" action is triggered.
	 * Show the plugins management dialog
	 */
	void cb_managePlugins();

	/**
	 * \fn void cb_manageScenes()
	 * \brief method called when the "Scenes" action is triggered.
	 * Show the scenes management dialog:
	 */
	void cb_manageScenes();

	/**
	 * \fn void cb_manageCameras()
	 * \brief method called when the "Cameras" action is triggered.
	 * Show the cameras management dialog:
	 */
	void cb_manageCameras();

	/**
	 * \fn void cb_manageMaps()
	 * \brief method called when the "Maps" action is triggered.
	 * Show the maps management dialog:
	 */
	void cb_manageMaps();
};

#endif
