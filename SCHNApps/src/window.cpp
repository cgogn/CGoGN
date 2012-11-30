#include "window.h"

#include <QMessageBox>
#include <QDockWidget>
#include <QPluginLoader>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "plugin.h"
#include "view.h"
#include "splitArea.h"

#include "viewSelector.h"
#include "cameraDialog.h"
#include "pluginDialog.h"

Window::Window(QWidget *parent) :
	QMainWindow(parent),
	m_firstView(NULL),
	m_dock(NULL),
	m_dockTabWidget(NULL)
{
	// program in its initialization phase
	m_initialization = true;

	m_pluginDialog = new PluginDialog(this);
	m_cameraDialog = new CameraDialog(this);

	this->setupUi(this);
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

	// add first view
	m_firstView = addView();
	m_splitArea->addFitElement(m_firstView);

	glewInit();

	// connect the basic actions
	connect(actionAboutSCHNApps, SIGNAL(triggered()), this, SLOT(cb_aboutSCHNApps()));
	connect(actionAboutCGoGN, SIGNAL(triggered()), this, SLOT(cb_aboutCGoGN()));

	connect(actionManagePlugins, SIGNAL(triggered()), this, SLOT(cb_managePlugins()));
	connect(actionManageViews, SIGNAL(triggered()), this, SLOT(cb_manageViews()));
	connect(actionManageCameras, SIGNAL(triggered()), this, SLOT(cb_manageCameras()));

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

	emit(cameraAdded(camera));

	return camera;
}

Camera* Window::addCamera()
{
	return addCamera(QString("camera_") + QString::number(Camera::cameraCount));
}

void Window::removeCamera(const QString& name)
{
	if (h_cameras.contains(name))
	{
		Camera* camera = h_cameras[name];
		h_cameras.remove(name);

		emit(cameraRemoved(camera));

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
 * MANAGE VIEWS
 *********************************************************/

View* Window::addView(const QString& name)
{
	if (h_views.contains(name))
	{
		System::Error::code = System::Error::VIEW_EXISTS;
		return NULL;
	}

	View* view = NULL;
	if(m_firstView == NULL)
		view = new View(name, this, this);
	else
		view = new View(name, this, this, m_firstView);
	h_views.insert(name, view);

	emit(viewAdded(view));

	return view;
}

View* Window::addView()
{
	return addView(QString("view_") + QString::number(View::viewCount));
}

void Window::removeView(const QString& name)
{
	if (h_views.contains(name))
	{
		if(h_views.count() > 1)
		{
			View* view = h_views[name];
			if(m_firstView == view)
				m_firstView = h_views.constBegin().value();
			h_views.remove(name);

			emit(viewRemoved(view));

			delete view;
		}
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
 * MANAGE PLUGINS
 *********************************************************/

Plugin* Window::loadPlugin(QString pluginFilePath)
{
	QString pluginName = QFileInfo(pluginFilePath).baseName().remove(0, 3);

	if (h_plugins.contains(pluginName))
	{
		// set message error + function fails
		System::Error::code = System::Error::PLUGIN_EXISTS;
		return NULL;
	}

	// QT's plugin loader class
	QPluginLoader loader(pluginFilePath);

	// if the loader loads a plugin instance
	if (QObject* pluginObject = loader.instance())
	{
		Plugin* plugin = qobject_cast<Plugin*>(pluginObject);

		// we set the plugin with correct parameters (name, filepath, window)
		plugin->setName(pluginName);
		plugin->setFilePath(pluginFilePath);
		plugin->setWindow(this);

		// then we call its enable() methods
		if (plugin->enable())
		{
			// if it succeeded we reference this plugin
			h_plugins.insert(pluginName, plugin);
			statusbar->showMessage(pluginName + QString(" successfully loaded."), 2000);

			emit(pluginAdded(plugin));

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

		emit(pluginRemoved(plugin));

		// delete plugin
		delete plugin;

		statusbar->showMessage(pluginName + QString(" successfully unloaded."), 2000);
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

	emit(mapAdded(map));

	return true;
}

void Window::removeMap(const QString& name)
{
	if (h_maps.contains(name))
	{
		MapHandler* map = h_maps[name];
		h_maps.remove(name);

		emit(mapRemoved(map));

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
	else if (event->key() == Qt::Key_Control)
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

void Window::cb_manageCameras()
{
	m_cameraDialog->show();
}

void Window::cb_manageViews()
{
	ViewPixMaps pm;
	pm.fromSplitArea(m_splitArea);

	ViewSelector selector(pm, this, ViewSelector::SELECT);
	selector.setInsertionName("new view");
	selector.exec();

	if (selector.result() == QDialog::Accepted)
	{
		View* v = addView();
		QPoint insertPoint = selector.getInsertPoint();
		m_splitArea->addElementAt(v, insertPoint.x(), insertPoint.y());
	}
}

void Window::cb_managePlugins()
{
	m_pluginDialog->show();
}
