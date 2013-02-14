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
	Window(const QString& appPath, PythonQtObjectPtr& pythonContext, PythonQtScriptingConsole& pythonConsole);
	~Window();

public slots:
	const QString& getAppPath() { return m_appPath; }
	CamerasDialog* getCamerasDialog() { return m_camerasDialog; }
	PluginsDialog* getPluginsDialog() { return m_pluginsDialog; }
	MapsDialog* getMapsDialog() { return m_mapsDialog; }

	/*********************************************************
	 * MANAGE DOCK
	 *********************************************************/

	QTabWidget* getDockTabWidget() const { return m_dockTabWidget; }

public:
	void addTabInDock(QWidget* tabWidget, const QString& tabText, bool enable);
	void removeTabInDock(QWidget* tabWidget);

public slots:
	void enablePluginTabWidgets(Plugin* plugin);
	void disablePluginTabWidgets(Plugin* plugin);

	/*********************************************************
	 * MANAGE MENU ACTIONS
	 *********************************************************/

public:
	bool addMenuAction(const QString& menuPath, QAction* action);
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

public slots:
	Camera* getCamera(const QString& name) const;

public:
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

	Plugin* loadPlugin(const QString& pluginFilePath);
	void unloadPlugin(const QString& pluginName);

	Plugin* getPlugin(const QString& name) const;
	QList<Plugin*> getPluginsList() const { return h_plugins.values(); }
	const PluginHash& getPluginsHash() const { return h_plugins; }

	const QMap<QString, QString>& getAvailablePlugins() const { return m_availablePlugins; }

	/*********************************************************
	 * MANAGE MAPS
	 *********************************************************/

public:
	MapHandlerGen* addMap(const QString& name, unsigned int dim);
	void removeMap(const QString& name);

public slots:
	MapHandlerGen* getMap(const QString& name) const;

public:
	QList<MapHandlerGen*> getMapsList() const { return h_maps.values(); }
	const MapHash& getMapsHash() const { return h_maps; }

	/*********************************************************
	 * MANAGE LINKS
	 *********************************************************/

public slots:
	void linkViewAndCamera(View* v, Camera* c);
	void linkViewAndCamera(const QString& viewName, const QString& cameraName);

	void linkViewAndMap(View* v, MapHandlerGen* m);
	void linkViewAndMap(const QString& viewName, const QString& mapName);
	void unlinkViewAndMap(View* v, MapHandlerGen* m);
	void unlinkViewAndMap(const QString& viewName, const QString& mapName);

	void linkViewAndPlugin(View* v, Plugin* p);
	void linkViewAndPlugin(const QString& viewName, const QString& pluginName);
	void unlinkViewAndPlugin(View* v, Plugin* p);
	void unlinkViewAndPlugin(const QString& viewName, const QString& pluginName);

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

	void cb_showCamerasDialog();
	void cb_showPluginsDialog();
	void cb_showMapsDialog();

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
