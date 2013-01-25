#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "ui_window.h"

#include "types.h"
#include "PythonQt/PythonQt.h"
#include "PythonQt/gui/PythonQtScriptingConsole.h"

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
	 * \fn Window()
	 * \brief Constructor
	 */
	Window(const QString& appPath, PythonQtObjectPtr& pythonContext, PythonQtScriptingConsole& pythonConsole);

	/**
	 * \fn ~Window()
	 * \brief Destructor
	 */
	~Window();

	const QString& getAppPath() { return m_appPath; }

	/*********************************************************
	 * MANAGE DOCK
	 *********************************************************/

	QTabWidget* getDockTabWidget() const { return m_dockTabWidget; }

	/**
	 * \fn void addTabInDock(QWidget* tabWidget, const QString& tabText, bool enable)
	 * \brief Adds the widget as a new tab in the interface's dock
	 *
	 * \param tabWidget the created and allocated pointer to the QWidget to add in the dock
	 * \param tabText The text that will appears in the tab label
	 * \param enable is the new tab enabled ?
	 */
	void addTabInDock(QWidget* tabWidget, const QString& tabText, bool enable);

	/**
	 * \fn void removeTabInDock(QWidget* tabWidget)
	 * \brief removes a given widget from the tab
	 *
	 * \param tabWidget the reference to the widget you want to remove
	 *
	 * If the widget does belong to the tab, it will be destroyed.
	 */
	void removeTabInDock(QWidget* tabWidget);

public slots:
	void enablePluginTabWidgets(Plugin* plugin);
	void disablePluginTabWidgets(Plugin* plugin);

	/*********************************************************
	 * MANAGE MENU ACTIONS
	 *********************************************************/

public:
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

public slots:
	View* getView(const QString& name) const;

public:
	QList<View*> getViewsList() const { return h_views.values(); }
	const ViewHash& getViewsHash() const { return h_views; }

	View* getCurrentView() const { return m_currentView; }
	void setCurrentView(View* view);

	void splitView(const QString& name, Qt::Orientation orientation);

	/*********************************************************
	 * MANAGE PLUGINS
	 *********************************************************/

public slots:
	void registerPluginsDirectory(const QString& path);

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
	 * \return a boolean whether the loading succeeded or not.
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
	 */
	void unloadPlugin(const QString& pluginName);

public:
	Plugin* getPlugin(const QString& name) const;
	QList<Plugin*> getPluginsList() const { return h_plugins.values(); }
	const PluginHash& getPluginsHash() const { return h_plugins; }

	const QMap<QString, QString>& getAvailablePlugins() const { return m_availablePlugins; }

	/*********************************************************
	 * MANAGE MAPS
	 *********************************************************/

	MapHandlerGen* addMap(const QString& name, unsigned int dim);
	void removeMap(const QString& name);
	MapHandlerGen* getMap(const QString& name) const;
	QList<MapHandlerGen*> getMapsList() const { return h_maps.values(); }
	const MapHash& getMapsHash() const { return h_maps; }

	/*********************************************************
	 * MANAGE LINKS
	 *********************************************************/

public slots:
	void linkViewAndCamera(View* v, Camera* c);

	void linkViewAndMap(View* v, MapHandlerGen* m);
	void unlinkViewAndMap(View* v, MapHandlerGen* m);

	void linkViewAndPlugin(View* v, Plugin* p);
	void unlinkViewAndPlugin(View* v, Plugin* p);

	/*********************************************************
	 * MANAGE TEXTURES
	 *********************************************************/

public:
	Texture* getTexture(const QString& image);
	void releaseTexture(const QString& image);

protected:
	QString m_appPath;
	PythonQtObjectPtr& m_pythonContext;
	PythonQtScriptingConsole& m_pythonConsole;

	QVBoxLayout* m_centralLayout;
	QSplitter* m_rootSplitter;
	bool b_rootSplitterInitialized;

	View* m_firstView;
	View* m_currentView;

	QDockWidget* m_dock;
	QTabWidget* m_dockTabWidget;

	QDockWidget* m_pythonDock;

	QMap<QString, QString> m_availablePlugins;

	PluginHash h_plugins;
	ViewHash h_views;
	CameraHash h_cameras;
	MapHash h_maps;

	TextureHash h_textures;

	CamerasDialog* m_camerasDialog;
	PluginsDialog* m_pluginsDialog;
	MapsDialog* m_mapsDialog;

public slots:
	void cb_aboutSCHNApps();
	void cb_aboutCGoGN();
	
	void cb_showHideDock();
	void cb_showHidePythonDock();

	void cb_manageCameras();
	void cb_managePlugins();
	void cb_manageMaps();

signals:
	void cameraAdded(Camera* camera);
	void cameraRemoved(Camera* camera);

	void viewAdded(View* view);
	void viewRemoved(View* view);
	void currentViewChanged(View* view);

	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);

	void pluginLoaded(Plugin* plugin);
	void pluginUnloaded(Plugin* plugin);

	void viewAndCameraLinked(View* view, Camera* camera);
	void viewAndCameraUnlinked(View* view, Camera* camera);

	void viewAndMapLinked(View* view, MapHandlerGen* map);
	void viewAndMapUnlinked(View* view, MapHandlerGen* map);

	void viewAndPluginLinked(View* view, Plugin* plugin);
	void viewAndPluginUnlinked(View* view, Plugin* plugin);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
