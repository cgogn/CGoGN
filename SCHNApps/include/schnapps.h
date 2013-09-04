#ifndef _SCHNAPPS_H_
#define _SCHNAPPS_H_

#include "ui_schnapps.h"

#include "types.h"
#include "PythonQt/PythonQt.h"
#include "PythonQt/gui/PythonQtScriptingConsole.h"

class QVBoxLayout;
class QSplitter;

namespace CGoGN
{

namespace SCHNApps
{

class ControlDock_CameraTab;
class ControlDock_MapTab;
class ControlDock_PluginTab;

class SCHNApps : public QMainWindow, Ui::SCHNApps
{
	Q_OBJECT

public:
	SCHNApps(const QString& appPath, PythonQtObjectPtr& pythonContext, PythonQtScriptingConsole& pythonConsole);
	~SCHNApps();

public slots:
	const QString& getAppPath() { return m_appPath; }

	/*********************************************************
	 * MANAGE CAMERAS
	 *********************************************************/

public slots:
	Camera* addCamera(const QString& name);
	Camera* addCamera();
	void removeCamera(const QString& name);

	Camera* getCamera(const QString& name) const;
	const CameraSet& getCameraSet() const { return m_cameras; }

	/*********************************************************
	 * MANAGE VIEWS
	 *********************************************************/

public slots:
	View* addView(const QString& name);
	View* addView();
	void removeView(const QString& name);

	View* getView(const QString& name) const;
	const ViewSet& getViewSet() const { return m_views; }

	View* getSelectedView() const { return m_selectedView; }
	void setSelectedView(View* view);

	void splitView(const QString& name, Qt::Orientation orientation);

	/*********************************************************
	 * MANAGE PLUGINS
	 *********************************************************/

public slots:
	void registerPluginsDirectory(const QString& path);

	Plugin* enablePlugin(const QString& pluginName);
	void disablePlugin(const QString& pluginName);

	Plugin* getPlugin(const QString& name) const;
	const PluginSet& getPluginSet() const { return m_plugins; }

	const QMap<QString, QString>& getAvailablePlugins() const { return m_availablePlugins; }

public:
	void addPluginDockTab(Plugin* plugin, QWidget* tabWidget, const QString& tabText);
	void removePluginDockTab(Plugin* plugin, QWidget* tabWidget);

private slots:
	void enablePluginTabWidgets(PluginInteraction* plugin);
	void disablePluginTabWidgets(PluginInteraction* plugin);

	/*********************************************************
	 * MANAGE MAPS
	 *********************************************************/

public slots:
	MapHandlerGen* addMap(const QString& name, unsigned int dim);
	void removeMap(const QString& name);

	MapHandlerGen* getMap(const QString& name) const;
	const MapSet& getMapSet() const { return m_maps; }

	void notifySelectedMapChanged(MapHandlerGen* old, MapHandlerGen* cur) { emit(selectedMapChanged(old, cur)); }
	MapHandlerGen* getSelectedMap() const;

	/*********************************************************
	 * MANAGE TEXTURES
	 *********************************************************/

public:
	Texture* getTexture(const QString& image);
	void releaseTexture(const QString& image);

	/*********************************************************
	 * MANAGE MENU ACTIONS
	 *********************************************************/

public:
	void addMenuAction(Plugin* plugin, const QString& menuPath, QAction* action);
	void removeMenuAction(Plugin* plugin, QAction* action);

public slots:
	void aboutSCHNApps();
	void aboutCGoGN();

	void showHideControlDock();
	void showHidePluginDock();
	void showHidePythonDock();

	void loadPythonScriptFromFile(const QString& fileName);

private slots:
	void loadPythonScriptFromFileDialog();

signals:
	void cameraAdded(Camera* camera);
	void cameraRemoved(Camera* camera);

	void viewAdded(View* view);
	void viewRemoved(View* view);
	void selectedViewChanged(View* old, View* cur);

	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);
	void selectedMapChanged(MapHandlerGen* old, MapHandlerGen* cur);

	void pluginAvailableAdded(QString name);
	void pluginEnabled(Plugin* plugin);
	void pluginDisabled(Plugin* plugin);

protected:
	QString m_appPath;
	PythonQtObjectPtr& m_pythonContext;
	PythonQtScriptingConsole& m_pythonConsole;

	QDockWidget* m_controlDock;
	QTabWidget* m_controlDockTabWidget;
	ControlDock_CameraTab* m_controlCameraTab;
	ControlDock_MapTab* m_controlMapTab;
	ControlDock_PluginTab* m_controlPluginTab;

	QDockWidget* m_pluginDock;
	QTabWidget* m_pluginDockTabWidget;

	QDockWidget* m_pythonDock;

	QVBoxLayout* m_centralLayout;
	QSplitter* m_rootSplitter;
	bool b_rootSplitterInitialized;

	View* m_firstView;
	View* m_selectedView;

	QMap<QString, QString> m_availablePlugins;
	QMap<Plugin*, QList<QWidget*> > m_pluginTabs;
	QMap<Plugin*, QList<QAction*> > m_pluginMenuActions;

	CameraSet m_cameras;
	ViewSet m_views;
	PluginSet m_plugins;
	MapSet m_maps;

	TextureSet m_textures;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
