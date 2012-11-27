#include "window.h"

#include <QApplication>
#include <QMessageBox>
#include <QDockWidget>
#include <QPluginLoader>
#include <QFileInfo>

#include "plugin.h"
#include "scene.h"
#include "view.h"
#include "camera.h"
//#include "context.h"
#include "mapHandler.h"

#include "splitArea.h"
#include "viewSelector.h"

//#include "pluginDialog.h"
//#include "sceneDialog.h"
//#include "cameraDialog.h"
//#include "mapDialog.h"

//#include "sceneSelector.h"

//#include "newSceneDialog.h"
//#include "globalCameraDialog.h"
//#include "linkViewDialog.h"
//#include "mapPluginDialog.h"

Window::Window(QWidget *parent) :
	QMainWindow(parent),
	m_dock(NULL),
	m_dockTabWidget(NULL)
{
	// program in its initialization phase
	m_initialization = true;

	glewInit();
	m_context = new QGLContext(QGLFormat(QGL::Rgba | QGL::DoubleBuffer | QGL::DepthBuffer));

	if (m_context->create())
		std::cout << "QGLContext created" << std::endl;
	else
		std::cout << "Failed to create QGLContext" << std::endl;

//	this->setupUi(this);
	System::splash->showMessage("Welcome to SCHNApps", Qt::AlignBottom | Qt::AlignCenter);
	sleep(1);

	// layout in which we store the main area
	m_verticalLayout = new QVBoxLayout(centralwidget);
	m_verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

	// the main area: multi GL views display area
	m_splitArea = new SplitArea(centralwidget);
	m_verticalLayout->addWidget(m_splitArea);

	// init keys as unpressed
	keys[0] = false;
	keys[1] = false;
	keys[2] = false;

	// connect the basic actions
	connect(actionAboutSCHNApps, SIGNAL(triggered()), this, SLOT(cb_aboutSCHNApps()));
	connect(actionAboutCGoGN, SIGNAL(triggered()), this, SLOT(cb_aboutCGoGN()));

//	connect(actionManagePlugins, SIGNAL(triggered()), this, SLOT(cb_managePlugins()));
//	connect(actionManageScenes, SIGNAL(triggered()), this, SLOT(cb_manageScenes()));
//	connect(actionManageCameras, SIGNAL(triggered()), this, SLOT(cb_manageCameras()));
//	connect(actionManageMaps, SIGNAL(triggered()), this, SLOT(cb_manageMaps()));

//	System::StateHandler::loadState(this, &h_plugin, &h_scene, m_splitArea);

	// program in its initialization phase
	m_initialization = false;
}

Window::~Window()
{
//	System::StateHandler::saveState(this, &h_plugins, &h_scenes, m_splitArea);
}

/*********************************************************
 * MANAGE DOCK
 *********************************************************/

QTabWidget* Window::getDockTabWidget()
{
	// if no dock or no tab widget: set error message
	if (!m_dock || !m_dockTabWidget)
		System::Error::code = System::Error::NO_DOCK;

	return m_dockTabWidget;
}

void Window::addTabInDock(QWidget *tabWidget, const QString& tabText)
{
	// if there is still no dock
	if (!m_dock)
	{
		m_dock = new QDockWidget(tr("Control"), this);
		m_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		m_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
		addDockWidget(Qt::RightDockWidgetArea, m_dock);
	}

	// if there is still no tab widget in the dock
	if (!m_dockTabWidget)
	{
		QWidget* dockWidgetContents = new QWidget();
		QVBoxLayout* verticalLayout = new QVBoxLayout(dockWidgetContents);
		m_dockTabWidget = new QTabWidget(dockWidgetContents);
		m_dockTabWidget->setObjectName(QString::fromUtf8("tabWidget"));
		m_dockTabWidget->setLayoutDirection(Qt::RightToLeft);
		m_dockTabWidget->setTabPosition(QTabWidget::East);

		verticalLayout->addWidget(m_dockTabWidget);

		m_dock->setWidget(dockWidgetContents);
	}

	// adding a new tab containing the specified widget
	m_dockTabWidget->addTab(tabWidget, tabText);
//	m_dockTabWidget->setTabText(m_dockTabWidget->indexOf(tabWidget), tabText);
}

void Window::removeTabInDock(QWidget *tabWidget)
{
	// if there is a dock and a tab widget
	if (m_dock && m_dockTabWidget)
	{
		// remove the tab containing the specified widget
		m_dockTabWidget->removeTab(m_dockTabWidget->indexOf(tabWidget));
	}
}

/*********************************************************
 * MANAGE MENU ACTIONS
 *********************************************************/

bool Window::addMenuAction(const QString& menuPath, QAction* action)
{
	// if menu path = empty string: set error + failure
	if (menuPath.isEmpty())
	{
		System::Error::code = System::Error::BAD_ACTION_MENU_PATH_f(menuPath);
		return false;
	}

	if (!action)
		return false;

	// extracting all the substring separated by ';'
	QStringList stepNames = menuPath.split(";");
	stepNames.removeAll("");
	unsigned int nbStep = stepNames.count();

	// if only one substring: error + failure
	// No action directly in the menu bar
	if (nbStep < 1)
	{
		System::Error::code = System::Error::BAD_ACTION_MENU_PATH_f(menuPath);
		return false;
	}

	// for every extracted substring
	unsigned int i = 0;
	QMenu* lastMenu;
	QAction* lastAction;
	foreach(QString step, stepNames)
	{
		++i;
		bool newMenu = true;
		// if not last substring (= menu)
		if (i < nbStep)
		{
			// if first substring
			if (i == 1)
			{
				// checking if it's an existing menu or not
				QList<QAction *> actions = menubar->actions();
				foreach(QAction * action, actions)
				{
					if (action->menu()->title() == step)
					{
						lastAction = action;
						newMenu = false;
						break;
					}
				}
				// if new menu: adding it to menubar
				if (newMenu)
				{
					lastMenu = menubar->addMenu(step);
					lastMenu->setParent(menubar);
				}
				// if not: next operation in the already existing corresponding menu
				else
				{
					lastMenu = lastAction->menu();
					// setting the parent will be useful for the deleting function
					lastMenu->setParent(menubar);
				}
			}
			// if not first substring
			else
			{
				// same thing: except not operating on the menubar
				QList<QAction *> actions = lastMenu->actions();
				foreach(QAction * action, actions)
				{
					if (action->menu()->title() == step)
					{
						lastAction = action;
						newMenu = false;
						break;
					}
				}
				if (newMenu)
				{
					QMenu *tmp = lastMenu;
					lastMenu = lastMenu->addMenu(step);
					// setting the parent will be useful for the deleting function
					lastMenu->setParent(tmp);
				}
				else
				{
					QMenu *tmp = lastMenu;
					lastMenu = lastAction->menu();
					// setting the parent will be useful for the deleting function
					lastMenu->setParent(tmp);
				}
			}
		}
		// if last substring (= action name)
		else
		{
			// adding it to the corresponding menu
			lastMenu->addAction(action);
			action->setText(step);
			// setting the parent will be useful for the deleting function
			action->setParent(lastMenu);
			break;
		}
	}

	// success
	return true;
}

void Window::removeMenuAction(QAction *action)
{
	if(action)
	{
		action->setEnabled(false);
		// parent of the action
		// which is an instance of QMenu if the action was created
		// using the addMenuActionMethod()
		QObject* parent = action->parent();
		// deleting action
		delete action;

		//if parent is an instance of QMenu
		if (parent->inherits("QMenu"))
		{
			// for this menu and all of its parent that are instance of QMenu
			// that are also fit for deleting
			while (parent != NULL)
			{
				QMenu *menuParent = (QMenu*)(parent);
				parent = parent->parent();

				// if the menu doesn't have any other actions/menus
				// it is deleted
				if (menuParent->actions().empty())
					delete menuParent;
				// otherwise: end function
				else
					break;

				if (!parent->inherits("QMenu"))
					parent = NULL;
			}
		}
	}
}

/*********************************************************
 * MANAGE TOOLBAR ACTIONS
 *********************************************************/

bool Window::addToolbarAction(QAction* action)
{
	if (action)
	{
		toolBar->addAction(action);
		return true;
	}
	return false;
}

void Window::removeToolbarAction(QAction* action)
{
	if (action)
		toolBar->removeAction(action);
}

/*********************************************************
 * MANAGE PLUGINS
 *********************************************************/

Plugin* Window::loadPlugin(const QString& pluginPath)
{
	QString pluginName = QFileInfo(pluginPath).baseName().remove(0, 3);

	if (h_plugins.contains(pluginName))
	{
		// set message error + function fails
		System::Error::code = System::Error::PLUGIN_EXISTS;
		return NULL;
	}

	// QT's plugin loader class
	QPluginLoader loader(pluginPath);

	// if the loader loads a plugin instance
	if (QObject* pluginObject = loader.instance())
	{
		Plugin* plugin = qobject_cast<Plugin*>(pluginObject);

		// we set the plugin with correct parameters (name, filepath, window)
		plugin->setName(pluginName);
		plugin->setFilePath(pluginPath);
		plugin->setWindow(this);

		// then we call its enable() methods
		if (plugin->enable())
		{
			// if it succeeded we reference this plugin
			h_plugins.insert(pluginName, plugin);
			// method success
			return plugin;
		}
		else
		{
			delete plugin;
			return NULL;
		}
	}
	// if loading fails
	else
	{
		// set error message + method failure
		System::Error::code = System::Error::ERROR_PLUGIN_LOAD_f(pluginName);
		return NULL;
	}
}

void Window::unloadPlugin(const QString& pluginName)
{
	if (h_plugins.contains(pluginName))
	{
		Plugin* plugin = h_plugins[pluginName];

		// calling its disable() method and dereferencing it
		plugin->disable();
		h_plugins.remove(pluginName);

		// delete plugin
		delete plugin;
	}
}

Plugin* Window::getPlugin(const QString& name)
{
	if (h_plugins.contains(name))
		return h_plugins[name];
	else
	{
		System::Error::code = System::Error::PLUGIN_DOES_NOT_EXIST;
		return NULL;
	}
}
/*
Plugin* Window::checkPluginDependencie(QString name, Plugin* dependantPlugin)
{
	// if the plugin is referenced and found
	PluginHash::iterator it;

	if ((it = h_plugin.find(name)) != h_plugin.end())
	{
		// the plugin calling for the depencie is added to the found plugin's list of dependant plugins
		(*it)->addDependantPlugin(dependantPlugin);
		return (*it);
	}
	//if not found: set error message + failure
	else
	{
		System::Error::code = System::Error::UNSATSIFIED_PLUGIN_DEPENDENCIE_f(name, dependantPlugin->getName());
		return NULL;
	}
}
*/

/*********************************************************
 * MANAGE SCENES
 *********************************************************/

Scene* Window::addScene(const QString& name)
{
	if (h_scenes.contains(name))
	{
		// set message error + function fails
		System::Error::code = System::Error::SCENE_EXISTS;
		return NULL;
	}

	Scene* scene = new Scene(name, this);
	h_scenes.insert(name, scene);
	return scene;
}

void Window::removeScene(const QString& name)
{
	if (h_scenes.contains(name))
	{
		Scene* scene = h_scenes[name];
		h_scenes.remove(name);
		delete scene;
	}
}

Scene* Window::getScene(const QString& name)
{
	if (h_scenes.contains(name))
		return h_scenes[name];
	else
	{
		System::Error::code = System::Error::SCENE_DOES_NOT_EXIST;
		return NULL;
	}
}

//bool Window::addNewEmptyScene(QString name, Scene *&scene, bool dialog, Camera *sharedCamera)
//{
//	// if there's no main area allocated or
//	// a viewer with the same name is found
//	if (!m_splitArea || h_scenes.find(name) != h_scenes.end())
//	{
//		//set message error + function fails
//		System::Error::code = System::Error::SCENE_EXISTS;
//		return false;
//	}
//	//if the glviewer doesn't exists and the main area allocated
//	else
//	{
//		//the GLviewer is created, set with the correct parameters (name, creating plugin)
//		//and is put in the correct place in the main area
//		scene = new Scene(name, this, sharedCamera);
//
//		View *sceneView = scene->getView(0);
//
//		//the GLView selection dialog is relevant only if their is some
//		//view already displayed
//		if (!h_scene.isEmpty() && dialog)
//		{
//			//we map the splitArea
//			ViewPixMaps pm;
//			pm.fromSplitArea(m_splitArea);
//
//			//we build a GLViewerSelector using this map
//			ViewSelector selector(pm, this, ViewSelector::SELECT);
//
//			selector.setInsertionName(name);
//
//			//we show the move dialog box
//			selector.exec();
//
//			//if the dialog is accepted: it has some modification
//			if (selector.result() == QDialog::Accepted)
//			{
//				QPoint insertPoint = selector.getInsertPoint();
//
//				//we're inserting the view in the desired place in the window
//				m_splitArea->addElementAt(sceneView, insertPoint.x(), insertPoint.y());
//			}
//			//if not, usual insertion
//			else
//			{
//				m_splitArea->addFitElement(sceneView);
//			}
//		}
//		else if (!m_initialization)
//		{
//			m_splitArea->addFitElement(sceneView);
//		}
//
//		h_scene.insert(name, scene);
//
//		if (h_scene.count() <= 1)
//		{
//			actionGlobalCamera->setEnabled(false);
//		}
//		else
//		{
//			actionGlobalCamera->setEnabled(true);
//		}
//
//		return true;
//	}
//}
//
//bool Window::addNewSceneView(Scene *scene, View *view)
//{
//	if (scene && h_scene.find(scene->getName()) != h_scene.end() && m_splitArea)
//	{
//		if (!m_initialization)
//			m_splitArea->addElementRightTo(view, scene->getView(0));
//
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
//
//bool Window::associateSceneWithPlugin(QString glviewer, Plugin *plugin, Scene *&scene, bool cb_initGL)
//{
//	//if the main area is allocated and some GLViewers are allocated
//	if (m_splitArea && !h_scene.empty())
//	{
//		//try to find the corresponding GLViewer
//		SceneHash::iterator it = h_scene.find(glviewer);
//
//		//if found return its reference
//		if (it != h_scene.end())
//		{
//			scene = (*it);
//
//			//adding a new plugin to the found GLViewers operating plugin list
//			(*it)->associateNewPlugin(plugin, cb_initGL);
//
//			return true;
//		}
//		//if not found: set error message + returns NULL
//		else
//		{
//			System::Error::code = System::Error::SCENE_UNREFERENCED;
//			return false;
//		}
//	}
//	//if no area or no glviewers already referenced,
//	else
//	{
//		//set error message + returns NULL
//		System::Error::code = System::Error::NO_SCENE;
//		return false;
//	}
//}
//
//bool Window::addNewSceneFromPlugin(QString name, Plugin *plugin, Scene *&scene)
//{
//	//if there's no main area allocated or
//	//a viewer with the same name is found
//	if (!m_splitArea || h_scene.find(name) != h_scene.end())
//	{
//		//set message error + function fails
//		System::Error::code = System::Error::SCENE_EXISTS;
//		return false;
//	}
//	//if the glviewer doesn't exists and the main area allocated
//	else
//	{
//		//the GLviewer is created, set with the correct parameters (name, creating plugin)
//		//and is put in the correct place in the main area
//		scene = new Scene(name, plugin, this);
//		scene->setName(name);
//		//if we are in the initialization phase, the glviews won't be
//		//added now, but when the info about their size will be read in
//		//the xml settings file
//		//otherwise we add the here.
////		if(!m_initialization)
//		m_splitArea->addFitElement(scene->getView(0));
//		h_scene.insert(name, scene);
//		scene->updateGL();
//
//		if (h_scene.count() <= 1)
//		{
//			actionGlobalCamera->setEnabled(false);
//		}
//		else
//		{
//			actionGlobalCamera->setEnabled(true);
//		}
//
//		//function succeeded
//		return true;
//	}
//}
//
//bool Window::addNewSceneFromPluginDialog(QString name, Plugin *plugin, Scene *&scene)
//{
//	//if there's no main area allocated or
//	//a viewer with the same name is found
//	if (!m_splitArea || h_scene.find(name) != h_scene.end())
//	{
//		//set message error + function fails
//		System::Error::code = System::Error::SCENE_EXISTS;
//		return false;
//	}
//	//if the glviewer doesn't exists and the main area allocated
//	else
//	{
//		//the GLviewer is created, set with the correct parameters (name, creating plugin)
//		//and is put in the correct place in the main area
//		scene = new Scene(name, plugin, this);
//		scene->setName(name);
//
//		//the GLView selection dialog is relevant only if their is some
//		//view already displayed
//		if (!h_scene.isEmpty())
//		{
//			//we map the splitArea
//			ViewPixMaps pm, finalPm;
//			pm.fromSplitArea(m_splitArea);
//
//			//we build a ViewSelector using this map
//			ViewSelector selector(pm, this, ViewSelector::SELECT);
//
//			selector.setInsertionName(name);
//
//			//we show the move dialog box
//			selector.exec();
//
//			//if the dialog is accepted: it has some modification
//			if (selector.result() == QDialog::Accepted)
//			{
//				QPoint insertPoint = selector.getInsertPoint();
//
//				//we're inserting the view in the desired place in the window
//				m_splitArea->addElementAt(scene->getView(0), insertPoint.x(), insertPoint.y());
//			}
//			//if not, usual insertion
//			else
//			{
//				m_splitArea->addFitElement(scene->getView(0));
//			}
//		}
//		else
//		{
//			m_splitArea->addFitElement(scene->getView(0));
//		}
//
//		h_scene.insert(name, scene);
//		scene->updateGL();
//
//		if (h_scene.count() <= 1)
//		{
//			actionGlobalCamera->setEnabled(false);
//		}
//		else
//		{
//			actionGlobalCamera->setEnabled(true);
//		}
//
//		//function succeeded
//		return true;
//	}
//}

//void Window::linkDialog(Scene *scene)
//{
//	LinkViewDialog lvDialog(this, &h_plugins, scene);
//	lvDialog.exec();
//}
//
//void Window::unlinkDialog(Scene *scene, QList<Plugin *> dependingPlugins)
//{
//	LinkViewDialog lvDialog(this, dependingPlugins, scene);
//	lvDialog.exec();
//}

/*********************************************************
 * MANAGE VIEWS
 *********************************************************/

View* Window::addView(const QString& name)
{
	if (h_views.contains(name))
	{
		System::Error::code = System::Error::VIEW_EXISTS;
		return NULL;
	}

	View* view = new View(name, this, this);
	h_views.insert(name, view);
	return view;
}

void Window::removeView(const QString& name)
{
	if (h_views.contains(name))
	{
		View* view = h_views[name];
		h_views.remove(name);
		delete view;
	}
}

View* Window::getView(const QString& name)
{
	if (h_views.contains(name))
		return h_views[name];
	else
	{
		System::Error::code = System::Error::VIEW_DOES_NOT_EXIST;
		return NULL;
	}
}

/*********************************************************
 * MANAGE CAMERAS
 *********************************************************/

Camera* Window::addCamera(const QString& name)
{
	if (h_cameras.contains(name))
	{
		System::Error::code = System::Error::CAMERA_EXISTS;
		return NULL;
	}

	Camera* camera = new Camera(name, this);
	h_cameras.insert(name, camera);
	return camera;
}

void Window::removeCamera(const QString& name)
{
	if (h_cameras.contains(name))
	{
		Camera* camera = h_cameras[name];
		h_cameras.remove(name);
		delete camera;
	}
}

Camera* Window::getCamera(const QString& name)
{
	if (h_cameras.contains(name))
		return h_cameras[name];
	else
	{
		System::Error::code = System::Error::CAMERA_DOES_NOT_EXIST;
		return NULL;
	}
}

/*********************************************************
 * MANAGE MAPS
 *********************************************************/

bool Window::addMap(const QString& name, MapHandler* map)
{
	if (h_maps.contains(name))
	{
		System::Error::code = System::Error::MAP_EXISTS;
		return false;
	}

	h_maps.insert(name, map);
	return true;
}

void Window::removeMap(const QString& name)
{
	if (h_maps.contains(name))
	{
		MapHandler* map = h_maps[name];
		h_maps.remove(name);
		delete map;
	}
}

MapHandler* Window::getMap(const QString& name)
{
	if (h_maps.contains(name))
		return h_maps[name];
	else
	{
		System::Error::code = System::Error::MAP_DOES_NOT_EXIST;
		return NULL;
	}
}




void Window::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_M)
		keys[0] = true;
	else if (event->key() == Qt::Key_Shift)
		keys[1] = true;
	else if (event->key() == Qt::Key_Control)
		keys[2] = true;

	if (keys[0] && keys[1] && keys[2])
		this->moveView();
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_M)
		keys[0] = false;
	else if (event->key() == Qt::Key_Shift)
		keys[1] = false;
	else if (event->key() == Qt::CTRL)
		keys[2] = false;
}

void Window::moveView()
{
	// if splitArea not empty or has more than 1 element
	if (m_splitArea->getNbRows() > 1 || ((QSplitter *)m_splitArea->widget(0))->count() > 1)
	{
		// map the splitArea
		ViewPixMaps pm, finalPm;
		pm.fromSplitArea(m_splitArea);

		// build a GLViewerSelector using this map
		ViewSelector selector(pm, this, ViewSelector::MOVE);

		// show the move dialog box
		selector.exec();

		// the dialog is accepted: it has some modification
		if (selector.result() == QDialog::Accepted)
		{
			// get back the modifier map
			finalPm = selector.getGLVMap();

			// creating a new split area and switch it with the old one
			SplitArea *old = m_splitArea;
			m_splitArea = new SplitArea(centralwidget);
			m_verticalLayout->addWidget(m_splitArea);

			// fill the new SplitArea using the modified map
			int x = 0;
			int y = 0;

			for (ViewPixMaps::y_iterator y_it = finalPm.y_begin(); y_it != finalPm.y_end(); ++y_it)
			{
				for (ViewPixMaps::x_iterator x_it = finalPm.x_begin(y_it); x_it != finalPm.x_end(y_it); ++x_it)
				{
					x_it->view->setParent(m_splitArea);
					m_splitArea->addElementAt(x_it->view, x, y);
					++x;
				}
				x = 0;
				++y;
			}

			// delete the old splitArea
			delete old;
		}

		// key states at the end of the move dialog
		keys[0] = selector.keys[0];
		keys[1] = selector.keys[1];
		keys[2] = selector.keys[2];
	}
}

void Window::cb_aboutSCHNApps()
{
	QString str("SCHNApps:\nS... CGoGN Holder for Nice Applications\n"
	            "Web site: http://cgogn.unistra.fr \n"
	            "Contact information: cgogn@unistra.fr");
	QMessageBox::about(this, tr("About SCHNApps"), str);
}

void Window::cb_aboutCGoGN()
{
	QString str("CGoGN:\nCombinatorial and Geometric modeling\n"
	            "with Generic N-dimensional Maps\n"
	            "Web site: http://cgogn.unistra.fr \n"
	            "Contact information: cgogn@unistra.fr");
	QMessageBox::about(this, tr("About CGoGN"), str);
}

void Window::cb_managePlugins()
{
//	PluginDialog pd(this, &h_plugins);
//	pd.exec();
}

void Window::cb_manageScenes()
{
//	SceneDialog sd(this);
//	sd.exec();
}

void Window::cb_manageCameras()
{
//	CameraDialog cd(this);
//	cd.exec();
}

void Window::cb_manageMaps()
{
//	MapDialog md(this, &h_maps);
//	md.exec();
}
