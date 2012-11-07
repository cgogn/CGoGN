#ifndef _WINDOW_H_
#define _WINDOW_H_


#include "ui_window.h"

#include <QVBoxLayout>

#include "mapHandler.h"

#include "Topology/generic/genericmap.h"

#include "types.h"

#include "splitArea.h"

#include "system.h"


class Scene;
class View;
class Camera;
class Plugin;
class Context;
class MapHandler;


class Window : public QMainWindow, Ui::Window{
	Q_OBJECT
public:
	/**
	 * \fn Window(QWidget* parent=0)
	 * \brief Default (and unique) constructor
	 *
	 * \param parent the parent of the window
	 */
	Window(QWidget* parent=0);
	/**
	 * \fn ~Window()
	 * \brief the class destructor
	 */
	~Window();

	bool addNewEmptyScene(QString name, Scene* &scene, bool dialog, Camera* sharedCamera=NULL);
	bool addNewSceneView(Scene* scene, View* view);

	QList<Scene*> scenes(){return h_scene.values();}

	bool associateSceneWithPlugin(QString glviewer, Plugin* plugin, Scene* &scene, bool cb_initGL=false);

	bool addNewSceneFromPlugin(QString name, Plugin* plugin, Scene* &scene);
	bool addNewSceneFromPluginDialog(QString name, Plugin* plugin, Scene* &scene);

	void removeScene(QString name);


	/**
	 * \fn void addEmptyDock()
	 * \brief add an empty dock to the window, if one doesn't already exists
	 *
	 * \see addWidgetInDock()
	 * \see VisualPlugin::addWidgetInDockTab()
	 */
	void addEmptyDock();
	/**
	 * \fn QTabWidget* getDockTabWidget()
	 * \brief Accessor to the QTabWidget of this interface
	 *
	 * An early developed function. Aparantly their's no need for such feature
	 *
	 * \return a pointer to the TabWidget, NULL if not allocated yet
	 */
	QTabWidget* getDockTabWidget();
	/**
	 * \fn void addWidgetInDockTab(QWidget* newTabWidget, QString tabText)
	 * \brief Adds the widget as a new tab in the interface's dock
	 *
	 * \param newTabWidget the created and allocated pointer to the QWidget to add in the dock
	 * \param tabText The text that will appears in the tab label
	 *
	 * \see removeTabInDock()
	 */
	void addWidgetInDockTab(QWidget* newTabWidget, QString tabText);
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

	/**
	 * \fn bool addMenuAction(QString menuPath, QAction* act)
	 * \brief adds an action in the program menu bar
	 *
	 * \param menuPath the menu path (see details below) to specify a location for the action in the menu.
	 * \param act a pointer to the allocated action to add in the menu. All the details ont that action (such as
	 * 			QObject::connect() calls) are not dealt in this function.
	 *
	 *	The menu path is a string used to specify the location of the new action in the menu bar.
	 *	In such path every menu step of the path should be separate by ';'. The action is the last substring after
	 *	the last ';', and will be added in the menu with that name. All non-existing menu specified in the path
	 *	shall be created.
	 *
	 *	<b>Example 1:</b> if you want to add an entry <em>"action"</em> to the existing menu <em>"Plugins"</em> the
	 *					menu path shall be: <em>Plugins;action</em>
	 *
	 *	<b>Example 2:</b> if you want to add an entry <em>"action"</em> in a new menu <em>"config"</em> that is also a
	 *					submenu of a new menu <em>"Settings"</em>, the menu path
	 *					shall be: <em>Settings;config;action</em>
	 *
	 *
	 *	A new action should at least belong to one menu (already existing or not). Otherwise the method will fail.
	 *
	 *	This method is already called by reimplemented Plugin methods, there's apparently no reason for calling it.
	 *
	 *	\return a boolean whether the method succeeded or not.
	 *
	 * If the function failed, the error code ( Error::code ) is affected with a value
	 	 depending on the error. This error can be shown with Error::showError
	 *
	 * \see deleteMenuAction()
	 * \see VisualPlugin::addMenuAction()
	 */
	bool addMenuAction(QString menuPath, QAction* act);
	/**
	 * \fn void deleteMenuAction(QAction* act)
	 * \brief delete an action from the menu bar
	 *
	 * \param act pointer to the allocated action to delete.
	 *
	 *	If this action was the only action remaining in a menu, this menu will also be deleted.
	 *
	 *	\warning DO NOT use this method with an action that hasn't been added with addMenuAction()
	 *
	 *	This method is already called by reimplemented Plugin methods, there's apparently no reason for calling it.
	 *
	 *	\see addMenuAction()
	 *	\see VisualPlugin::deleteMenuActions()
	 */
	void deleteMenuAction(QAction* act);

	bool addToolbarAction(QAction* act);





	/**
	 * \fn bool loadPlugin(QString pluginPath)
	 * \brief Loads and references a Plugin
	 *
	 * \param pluginPath the absolute path of the Plugin location
	 *
	 * The Plugin is loaded and referenced under a name that is the Plugin file name
	 	 where the extension '.so' and the prefix 'lib' were removed.
	 *
	 *	<b>Example:</b> <em>/path/libExample.so</em> will be referenced as <em>Example</em>
	 *
	 * You have to make sure that a Plugin file with a similar name hasn't been loaded yet, otherwise
	 * the loading will fail.
	 *
	 * This method calls for the Plugin::activate() method on the concerned Plugin. That is why
	   when Plugin are written, this method is override and used as an initialization method.
	 *
	 *  \warning In the program this method is called under specific and controlled circumstances, you should probably not call it.
	 *
	 *	\return a boolean whether the loading succeeded or not.
	 *
	 * If the function failed, the error code ( Error::code ) is affected with a value
	 	 depending on the error. This error can be shown with Error::showError
	 *
	 * \see unloadPlugin()
	 * \see getPlugin()
	 * \see Plugin::activate()
	*/
	Plugin* loadPlugin(QString pluginPath);

	/**
	 * \fn void unloadPlugin(QString pluginName)
	 * \brief Unload the Plugin of a given name
	 *
	 * \param pluginName the name under which the Plugin is referenced
	 *
	 * The Plugin of the given name is dereferenced and deleted, if he exists and was previously
	 * referenced, if not, the method does nothing.
	 *
	 * This method calls for the Plugin::disable() method on the concerned Plugin. That is why,
	   when Plugin are written, this method is override and used as an destruction method.
	 *
	 * \warning In the program this method is called under specific and controlled circumstances, you should probably not call it.
	 *
	 * \see loadPlugin()
	 * \see getPlugin()
	 * \see Plugin::disable()
	 */
	void unloadPlugin(QString pluginName);
	/**
	 * \fn Plugin* checkPluginDependencie(QString name, Plugin* dependantPlugin)
	 * \brief checks for a dependencie Plugin, and set the dependencie link for the found Plugin
	 *
	 * \param name the name of the Plugin you have to check the existence in order to make a dependencie
	 * \param dependantPlugin a reference to the Plugin that asks for the dependencie that will be set as a
	 * 			dependant Plugin for the found Plugin
	 *
	 * \warning In the program this method is called under specific and controlled circumstances
	 * 			by the Plugins you should probably not call it.
	 *
	 * \return a pointer to the found dependencie Plugin, NULL if this Plugin wasn't referenced
	 *
	 * If the function failed, the error code ( Error::code ) is affected with a value
	 	 depending on the error. This error can be shown with Error::showError
	 *
	 * \see loadPlugin()
	 * \see VisualPlugin::addDependencie()
	 **/

	Plugin* checkPluginDependencie(QString name, Plugin* dependantPlugin);

	QList<Plugin*> activePlugins(){return h_plugin.values();}

	void linkDialog(Scene* scene);
	void unlinkDialog(Scene* scene, QList<Plugin*> dependingPlugins);


	bool addReferencedMap(QString map_name, MapHandler* map);
	/**
	 * \fn bool addNewReferencedMap(QString map_name, T* &map)
	 * \brief adds and reference a new map
	 *
	 * This method insert in the map hash table a new map that will be referenced under
	 * the given name.
	 *
	 * \warning This function was meant to be used by the plugins and GLViewers, you should probably not call it.
	 *
	 *
	 * \tparam T the template parameter which the type of the map you want to add. T must be a CGoGN
	 * 		map type, that is to say an instance of a class that inherits CGoGN::GenericMap.
	 *
	 * \param[in] map_name The name under which will be referenced the map
	 * \param[out] map A pointer to a type T map, which will be set to the adress of the newly created map, or
	 * 		to the already existing map refererence by the same name
	 *
	 * \return true if the map is created, false if creation fails (returns false if a same named map already exists)
	 *
	 * \see getReferencedMap()
	 */
	template<typename T>
	MapHandler* addNewReferencedMap(QString map_name, T* &map){
		//if a map isn't already referenced under that name
		MapHash::iterator it;
		if((it=h_vizu.find(map_name))==h_vizu.end()){
			//the map is created, inserted, and reference under the given name
			map= new T();
			MapHandler* vh= new MapHandler((CGoGN::GenericMap*)map);
			h_vizu.insert(map_name,vh);

			return vh;
		}
		//if a map already has that name
		else{
			//failure + map affected with the address of the already existing map
			map= ((T*)((*it)->map()));
			System::Error::code= System::Error::MAP_EXISTS_f(map_name);
			return NULL;
		}
	}
	/**
	 * \fn T* getReferencedMap(QString map_name)
	 * \brief return the map referenced under the given name
	 *
	 * \warning This function was meant to be used by the plugins and GLViewers, you should probably not call it.
	 *
	 * \tparam T the type of the referenced map you want to fetch.
	 *
	 * \param map_name the name under which is referenced the map.
	 *
	 * \return a T type pointer to the map referenced under the given name, if faillure it returns NULL
	 *
	 * If the function failed, the error code ( Error::code ) is affected with a value
	 	 depending on the error. This error can be shown with Error::showError
	 *
	 * \see addNewReferencedMap()
	 */
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
	MapHandler* getReferencedMap(QString map_name);

	Context* context(){return m_context;}

	QList<Plugin*> plugins(){return h_plugin.values();}
	QList<MapHandler*> maps(){return h_vizu.values();}


protected:
	QVBoxLayout* verticalLayout;
	SplitArea* m_splitArea;

	/**
	 * \var QDockWidget* m_dock
	 * \brief The dock of the application
	 */
	QDockWidget* m_dock;
	/**
	 * \var QTabWidget* m_dockTabWidget
	 * \brief the tab widget that contains widgets provided by referenced Plugin
	 */
	QTabWidget* m_dockTabWidget;

	SceneHash h_scene;

	/**
	 * \var PluginHash h_plugin
	 * \brief Plugin referencing hash table
	 */
	PluginHash h_plugin;

	/**
	 * \var MapHash h_map
	 * \brief Map referencing hash table
	 */
//	MapHash h_map;
	MapHash h_vizu;

	/**
	 * \var bool initialization
	 * \brief a boolean to determine if the main window is in it's initialization phase or not
	 */
	bool m_initialization;

	/**
	 * \var bool keys[3]
	 * \brief a static tab to store state of some keys (here: M, Shift and CTRL)
	 */
	bool keys[3];


	Context* m_context;

	/**
	 * \fn void keyPressEvent( QKeyEvent * event )
	 * \brief handles the mouse button pressure
	 *
	 * Overload function from "QWidget" in order to handle the mouse
	 * button pressure. See the Qt documentation for more details
	 * ( http://qt-project.org/doc/qt-4.8/qwidget.html#mousePressEvent ).
	 */
	void keyPressEvent( QKeyEvent * event );

	/**
	 * \fn void keyReleaseEvent( QKeyEvent * event )
	 * \brief handles the mouse button release
	 *
	 * Overload function from "QWidget" in order to handle the mouse
	 * button release. See the Qt documentation for more details
	 * ( http://qt-project.org/doc/qt-4.8/qwidget.html#mouseReleaseEvent ).
	 */
	void keyReleaseEvent( QKeyEvent * event );

	/**
	 * \fn void moveView()
	 * \brief shows a reordering dialog for the Views
	 *
	 * Make a call with the rigth parameters to the dialog class
	 * GLVSelector and show this dialog.
	 *
	 * This method is meant to be called when the user press
	 * CTRL+Shift+M.
	 */
	void moveView();



public slots:
	/**
	 * \fn void cb_about_CGoGN();
	 * \brief function that is called when the "about CGOGN" menu action is triggered
	 */
	void cb_about_CGoGN();
	/**
	 * \fn void cb_pluginDialog()
	 * \brief method called when the "Plugins" menu action is triggered. Show the plugin managmement dialog:
	 * 				PluginDialog
	 */
	void cb_pluginDialog();

	/**
	 * \fn void cb_niewView()
	 * \brief method called when the "add niew view" button is pushed. Show the new empty view creation dialog:
	 * 				NewViewDialog
	 */
	void cb_niewScene();

	void cb_globalCamera();

	void cb_mapPlugin();
};

#endif
