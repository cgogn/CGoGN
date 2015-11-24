#ifndef _SCHNAPPS_H_
#define _SCHNAPPS_H_

#include "dll.h"

#include "ui_schnapps.h"

#include "types.h"

#include "PythonQt.h"
#include "gui/PythonQtScriptingConsole.h"
#include "slot_debug.h"
#include <QTextStream>

class QVBoxLayout;
class QSplitter;
class QFile;


namespace CGoGN
{

namespace SCHNApps
{

class ControlDock_CameraTab;
class ControlDock_MapTab;
class ControlDock_PluginTab;

/**
 * @brief The SCHNApps central object application
 * Manage:
 * - views
 * - cameras
 * - plugins
 * - maps
 *
 * Python callable slots are tagged with [PYTHON]
 */
class SCHNAPPS_API SCHNApps : public QMainWindow, Ui::SCHNApps
{
	Q_OBJECT

public:
	SCHNApps(const QString& appPath, PythonQtObjectPtr& pythonContext, PythonQtScriptingConsole& pythonConsole);
	~SCHNApps();

public slots:
	/**
	 * @brief  get the file path where application has been launched
	 * @return the path
	 */
	const QString& getAppPath() { return m_appPath; }

	/*********************************************************
	 * MANAGE CAMERAS
	 *********************************************************/

public slots:
	/**
	* @brief [PYTHON] add a camera with a given name
	* @param name name of camera
	*/
	Camera* addCamera(const QString& name);

	/**
	 * @brief [PYTHON] add a camera (name automatically)
	 * @return
	 */
	Camera* addCamera();

	/**
	* @brief [PYTHON] remove a camera
	* @param name name of camera to remove
	*/
	void removeCamera(const QString& name);

	/**
	* @brief [PYTHON] get camera object
	* @param name of camera
	*/
	Camera* getCamera(const QString& name) const;

	/// get the set of all cameras
	const CameraSet& getCameraSet() const { return m_cameras; }

	/*********************************************************
	 * MANAGE VIEWS
	 *********************************************************/
public:
	void redrawAllViews();


public slots:
	/**
	* @brief add a view with name
	* @param name name of view
	*/
	View* addView(const QString& name);

	/**
	* @brief add a view
	*/
	View* addView();

	/**
	* @brief [PYTHON] remove a view
	* @param name the name of the view
	*/
	void removeView(const QString& name);

	/**
	* @brief [PYTHON] get view object
	* @param name the name of view
	*/
	View* getView(const QString& name) const;

	/// get the set of all views
	const ViewSet& getViewSet() const { return m_views; }

	/**
	* @brief [PYTHON] get the selected view
	*/
	View* getSelectedView() const { return m_selectedView; }

	/**
	* @brief [PYTHON] set the selected view
	* @param view the view object
	*/
	void setSelectedView(View* view);

	/**
	* @brief [PYTHON] set the selected view
	* @param name the view name
	*/
	void setSelectedView(const QString& name);

	/**
	* @brief [PYTHON] split the view in the current orientation
	* @param orientation of split 0: Vertical Split 1:
	* @return the new View added by the split
	*/
	View* splitView(const QString& name, Qt::Orientation orientation);

	/**
	* @brief [PYTHON] save all split positions in a string
	* @return the storage string
	*/
	QString saveSplitViewPositions();

	/**
	* @brief [PYTHON] restore all split positions from a string storage,
	* the split's sequence must the same than when saving
	*/
	void restoreSplitViewPositions(QString stringStates);


	/*********************************************************
	 * MANAGE PLUGINS
	 *********************************************************/

public slots:
	/**
	* @brief[PYTHON]  Add a directory for searching available plugin
	* @param path path od directory
	*/
	void registerPluginsDirectory(const QString& path);

	/**
	* @brief [PYTHON] Load and enable a plugin
	* @param pluginName plugin name
	*/
	Plugin* enablePlugin(const QString& pluginName);

	/**
	* @brief [PYTHON] Disable and unload a plugin
	* @param pluginName plugin name
	*/
	void disablePlugin(const QString& pluginName);

	/**
	* @brief [PYTHON] Get plugin object from name
	* @param name name of plugin
	*/
	Plugin* getPlugin(const QString& name) const;

	/// get set of loaded plugins
	const PluginSet& getPluginSet() const { return m_plugins; }

	/// get a QMap of available plugins
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
	/**
	* @brief [PYTHON] add a new empty map
	* @param name name given to the map
	* @param dim dimension of the map
	*/
	MapHandlerGen* addMap(const QString& name, unsigned int dim);

	/**
	* @brief [PYTHON] Remove a map
	* @param name name of map
	*/
	void removeMap(const QString& name);

	/**
	* @brief [PYTHON] Duplicated (copy) a map
	* @param name of map to copy
	* @param properties  copy BB & VBO
	*/
	MapHandlerGen* duplicateMap(const QString& name, bool properties);

	/**
	* @brief [PYTHON] Set the current selected map
	* @param mapName name of the map to be selected
	*/
	void setSelectedMap(const QString& mapName);

	/**
	* @brief [PYTHON] Get a map object from its name
	* @param name name of map
	*/
	MapHandlerGen* getMap(const QString& name) const;

	/// get the set of maps available in application
	const MapSet& getMapSet() const { return m_maps; }

	/// notify to the application and plugins that current selected map has changed
	void notifySelectedMapChanged(MapHandlerGen* old, MapHandlerGen* cur) { DEBUG_EMIT("selectedMapChanged"); emit(selectedMapChanged(old, cur)); }

	/**
	* @brief [PYTHON] Get the current selected map
	* @return the selected map
	*/
	MapHandlerGen* getSelectedMap() const;

	/**
	* @brief [PYTHON] Get the current selected tab orbit in interface
	* @return 0:Dart / 1:Vertex / 2:Edge / 3:Face / 4:Volume
	*/
	unsigned int getCurrentOrbit() const;

	/// notify to the application and plugins that current selected cell-selection has changed
	void notifySelectedCellSelectorChanged(CellSelectorGen* cs) { DEBUG_EMIT("selectedCellSelectorChanged"); emit(selectedCellSelectorChanged(cs)); }

	/**
	* @brief [PYTHON] Get cell selector
	* @param orbit Orbit (0:Dart / 1:Vertex / 2:Edge / 3:Face / 4:Volume)
	*/
	CellSelectorGen* getSelectedSelector(unsigned int orbit) const;

	/**
	* @brief [PYTHON] Set the selector of the current map (warning change the current orbit)
	* @param orbit the orbit (0:Dart / 1:Vertex / 2:Edge / 3:Face / 4:Volume)
	* @param name name of selector (must exist)
	*/
	void setSelectedSelectorCurrentMap(unsigned int orbit, const QString& name);

	/// internal use (for static declaration / shared library problem)
	const StaticPointers& getStaticPointers() const { return m_sp; }

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
	/**
	 * @brief add an entry in menu for a plugin
	 * @param plugin plugin object ptr
	 * @param menuPath path of menu (ex: "Surface; Import Mesh"
	 * @param action action to associate with entry (see Qt menu system if obscur to you)
	 */
	void addMenuAction(Plugin* plugin, const QString& menuPath, QAction* action);

	/**
	 * @brief remove an entry in the menu (when plugin disable)
	 * @param plugin plugin object ptr
	 * @param action action entry to remove
	 */
	void removeMenuAction(Plugin* plugin, QAction* action);

	/**
	 * @brief executee the Python script corresponding to a shortcut
	 * @param key the short-cut code (MSB: event->modifiers(), LSB: event->key())
	 * @return true if short-cuts has been assigned
	 */
	bool execPythonShortcut(quint64 key);


public slots:
	void aboutSCHNApps();
	void aboutCGoGN();

	void showHideControlDock();
	void showHidePluginDock();
	void showHidePythonDock();

	/**
	* @brief [PYTHON] set if control dock is floating or docked
	* @param 1: floating / 0: docked
	*/
	void setFloatingControlDock(bool f);

	/**
	* @brief [PYTHON] set if plugin dock is floating or docked
	* @param 1: floating / 0: docked
	*/
	void setFloatingPluginDock(bool f);

	/**
	* @brief [PYTHON] set if python dock is floating or docked
	* @param 1: floating / 0: docked
	*/
	void setFloatingPythonDock(bool f);


	/**
	* @brief [PYTHON] Load a python script store in a file.
	* @param fileName the filename with absolute path or relative to path set with setPythonPath
	* @return true if reading file ok
	*/
	bool loadPythonScriptFromFile(const QString& fileName);

	/**
	* @brief [PYTHON] Associate a python command with a key shortcut
	* @param keys example "control a", "alt shift B" "control keypad +"
	* @param command a python cmd
	*/
	void setPythonShortcut(const QString& keys, const QString& command);

	/**
	* @brief [PYTHON] Print a message in the status bar
	* @param msg the message
	* @param msec number of milli-second that message stay printed
	*/
	void statusBarMessage(const QString& msg, int msec);

	/**
	 * @brief [PYTHON] open a FileDialog for file name selection (read)
	 * @param title
	 * @param dir default directory opening
	 * @param filter type of file to filter, for example: "(*.ply *.off *.trian)"
	 * @return the file name selected
	 */
	QString openFileDialog(const QString& title, const QString& dir = QString(), const QString& filter = QString());

	/**
	 * @brief [PYTHON] open a FileDialog for file name selection (save)
	 * @param title
	 * @param dir default directory opening
	 * @param filter type of file to filter, for example: "(*.ply *.off *.trian)"
	 * @return the file name selected
	 */
	 QString saveFileDialog(const QString& title, const QString& dir = QString(), const QString& filter = QString());

	/**
	* @brief [PYTHON] Fix the window size
	* @param w width of window
	* @param h height of window
	*/
	inline void setWindowSize(int w, int h) { this->resize(w, h); }

	/**
	* @brief [PYTHON] set the default path used when opening python file dialog.
	* @param path the default path
	*/
	void setPythonPath(const QString& path);

private slots:
	void loadPythonScriptFromFileDialog();


	/*********************************************************
	* MANAGE PYTHON RECORDING
	*********************************************************/
protected:
	QTextStream* m_pyRecording;
	QFile* m_pyRecFile;
	QList<QString> m_pyVarNames;
	QString m_pyBuffer;
	QMap<quint64, QString > m_pythonShortCuts;
	QString m_pyPathFile;

private slots:
	void pyRecording();
	void appendPyRecording();
	void cleanAll();

public:
	inline QTextStream* pythonStreamRecorder()  { return m_pyRecording; }
	inline void pythonVarDeclare(const QString& var) { m_pyVarNames.push_back(var); }
	inline void pythonVarsClear() { m_pyVarNames.clear(); }

	/// replace \ by / in file path for window 
	static QString noBackSlash(const QString& name);

	/// transform QString in std::string with 
	static std::string niceStdString(const QString& qstr);

	///  force QString contain to be ascii (non ascii char are replaced by '_')
	static QString forceASCII(const QString& qstr);


signals:
	void cameraAdded(Camera* camera);
	void cameraRemoved(Camera* camera);

	void viewAdded(View* view);
	void viewRemoved(View* view);
	void selectedViewChanged(View* old, View* cur);

	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);
	void selectedMapChanged(MapHandlerGen* old, MapHandlerGen* cur);
	void selectedCellSelectorChanged(CellSelectorGen* cs);

	void pluginAvailableAdded(QString name);
	void pluginEnabled(Plugin* plugin);
	void pluginDisabled(Plugin* plugin);

	void schnappsClosing();

protected:
	QString m_appPath;
	PythonQtObjectPtr& m_pythonContext;
	PythonQtScriptingConsole& m_pythonConsole;

	void execPythonCmd(const QString& fileName);

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

	StaticPointers m_sp;

	void closeEvent(QCloseEvent *event);


};

} // namespace SCHNApps

} // namespace CGoGN

#endif
