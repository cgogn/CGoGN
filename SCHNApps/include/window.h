#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "ui_window.h"

#include "types.h"

class QVBoxLayout;
class QSplitter;

namespace CGoGN
{

namespace SCHNApps
{

class CamerasDialog;
class PluginsDialog;
class MapsDialog;

struct Texture;

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
	Window(const QString& appPath, QWidget* parent = NULL);

	/**
	 * \fn ~Window()
	 * \brief the class destructor
	 */
	~Window();

	const QString& getAppPath() { return m_appPath; }

	/*********************************************************
	 * MANAGE DOCK
	 *********************************************************/

	/**
	 * \fn QTabWidget* getDockTabWidget()
	 * \brief Accessor to the QTabWidget of this interface
	 *
	 * \return a pointer to the TabWidget, NULL if not allocated yet
	 */
	QTabWidget* getDockTabWidget() const { return m_dockTabWidget; }

	/**
	 * \fn void addTabInDock(QWidget* tabWidget, const QString& tabText)
	 * \brief Adds the widget as a new tab in the interface's dock
	 *
	 * \param tabWidget the created and allocated pointer to the QWidget to add in the dock
	 * \param tabText The text that will appears in the tab label
	 *
	 * \see removeTabInDock()
	 */
	void addTabInDock(QWidget* tabWidget, const QString& tabText, bool enable);

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

	void enablePluginTabWidgets(Plugin* plugin);
	void disablePluginTabWidgets(Plugin* plugin);

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
	 * MANAGE CAMERAS
	 *********************************************************/

	Camera* addCamera(const QString& name);
	Camera* addCamera();
	void removeCamera(const QString& name);
	Camera* getCamera(const QString& name) const;
	QList<Camera*> getCamerasList() const { return h_cameras.values(); }
	const CameraHash& getCamerasHash() const { return h_cameras; }

	/*********************************************************
	 * MANAGE VIEWS
	 *********************************************************/

	View* addView(const QString& name);
	View* addView();
	void removeView(const QString& name);
	View* getView(const QString& name) const;
	QList<View*> getViewsList() const { return h_views.values(); }
	const ViewHash& getViewsHash() const { return h_views; }

	View* getCurrentView() const { return m_currentView; }
	void setCurrentView(View* view);

	void splitView(const QString& name, Qt::Orientation orientation);

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
	Plugin* loadPlugin(const QString& pluginFilePath);

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

	Plugin* getPlugin(const QString& name) const;
	QList<Plugin*> getPluginsList() const { return h_plugins.values(); }
	const PluginHash& getPluginsHash() const { return h_plugins; }

	/*********************************************************
	 * MANAGE MAPS
	 *********************************************************/

	bool addMap(MapHandlerGen* map);
	void removeMap(const QString& name);
	MapHandlerGen* getMap(const QString& name) const;
	QList<MapHandlerGen*> getMapsList() const { return h_maps.values(); }
	const MapHash& getMapsHash() const { return h_maps; }

	/*********************************************************
	 * MANAGE TEXTURES
	 *********************************************************/

	Texture* getTexture(const QString& image);
	void releaseTexture(const QString& image);

protected:
	QString m_appPath;

	bool m_initialization;

	QVBoxLayout* m_centralLayout;
	QSplitter* m_rootSplitter;
	bool b_rootSplitterInitialized;

	View* m_firstView;
	View* m_currentView;

	QDockWidget* m_dock;
	QTabWidget* m_dockTabWidget;

	PluginHash h_plugins;
	ViewHash h_views;
	CameraHash h_cameras;
	MapHash h_maps;

	TextureHash h_textures;

	CamerasDialog* m_camerasDialog;
	PluginsDialog* m_pluginsDialog;
	MapsDialog* m_mapsDialog;

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
	
	void cb_showHideDock();

	/**
	 * \fn void cb_manageCameras()
	 * \brief method called when the "Cameras" action is triggered.
	 * Show the cameras management dialog:
	 */
	void cb_manageCameras();

	/**
	 * \fn void cb_managePlugins()
	 * \brief method called when the "Plugins" action is triggered.
	 * Show the plugins management dialog
	 */
	void cb_managePlugins();

	void cb_manageMaps();

signals:
	void cameraAdded(Camera* camera);
	void cameraRemoved(Camera* camera);

	void viewAdded(View* view);
	void viewRemoved(View* view);

	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);

	void pluginAdded(Plugin* plugin);
	void pluginRemoved(Plugin* plugin);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
