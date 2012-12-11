#include "window.h"

#include <QVBoxLayout>
#include <QSplitter>
#include <QMessageBox>
#include <QDockWidget>
#include <QPluginLoader>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "plugin.h"
#include "view.h"
#include "texture.h"

#include "dialogs/camerasDialog.h"
#include "dialogs/pluginsDialog.h"
#include "dialogs/mapsDialog.h"

namespace CGoGN
{

namespace SCHNApps
{

Window::Window(const QString& appPath, QWidget *parent) :
	QMainWindow(parent),
	m_appPath(appPath),
	m_firstView(NULL),
	m_currentView(NULL)
{
	// program in its initialization phase
	m_initialization = true;

	m_camerasDialog = new CamerasDialog(this);
	m_pluginsDialog = new PluginsDialog(this);
	m_mapsDialog = new MapsDialog(this);

	this->setupUi(this);

	m_dock = new QDockWidget(tr("Control"), this);
	m_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
	addDockWidget(Qt::RightDockWidgetArea, m_dock);
	m_dock->setVisible(false);

	m_dockTabWidget = new QTabWidget(m_dock);
	m_dockTabWidget->setObjectName("DockTabWidget");
	m_dockTabWidget->setLayoutDirection(Qt::LeftToRight);
	m_dockTabWidget->setTabPosition(QTabWidget::East);
	m_dock->setWidget(m_dockTabWidget);

	connect(actionShowHideDock, SIGNAL(triggered()), this, SLOT(cb_showHideDock()));

	m_centralLayout = new QVBoxLayout(centralwidget);

	m_rootSplitter = new QSplitter(centralwidget);
	b_rootSplitterInitialized = false;
	m_centralLayout->addWidget(m_rootSplitter);

	// add first view
	m_firstView = addView();
	m_currentView = m_firstView;
	m_rootSplitter->addWidget(m_firstView);

	glewInit();

	// connect the basic actions
	connect(actionAboutSCHNApps, SIGNAL(triggered()), this, SLOT(cb_aboutSCHNApps()));
	connect(actionAboutCGoGN, SIGNAL(triggered()), this, SLOT(cb_aboutCGoGN()));

	connect(actionManageCameras, SIGNAL(triggered()), this, SLOT(cb_manageCameras()));
	connect(actionManagePlugins, SIGNAL(triggered()), this, SLOT(cb_managePlugins()));
	connect(actionManageMaps, SIGNAL(triggered()), this, SLOT(cb_manageMaps()));

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

void Window::addTabInDock(QWidget* tabWidget, const QString& tabText)
{
	if(tabWidget)
	{
		int idx = m_dockTabWidget->addTab(tabWidget, tabText);
		m_dock->setVisible(true);
		m_dockTabWidget->setTabEnabled(idx, false);
	}
}

void Window::removeTabInDock(QWidget *tabWidget)
{
	if(tabWidget)
		m_dockTabWidget->removeTab(m_dockTabWidget->indexOf(tabWidget));
}

void Window::enablePluginTabWidgets(Plugin* plugin)
{
	const QList<QWidget*> tabWidgets = plugin->getTabWidgets();
	foreach(QWidget* w, tabWidgets)
	{
		int idx = m_dockTabWidget->indexOf(w);
		m_dockTabWidget->setTabEnabled(idx, true);
	}
}

void Window::disablePluginTabWidgets(Plugin* plugin)
{
	const QList<QWidget*> tabWidgets = plugin->getTabWidgets();
	foreach(QWidget* w, tabWidgets)
	{
		int idx = m_dockTabWidget->indexOf(w);
		m_dockTabWidget->setTabEnabled(idx, false);
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

Camera* Window::getCamera(const QString& name) const
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
		view = new View(name, this);
	else
		view = new View(name, this, m_firstView);
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
			if(view == m_firstView)
			{
				ViewHash::const_iterator i = h_views.constBegin();
				while (i != h_views.constEnd())
				{
					if(i.value() != view)
					{
						m_firstView = i.value();
						i = h_views.constEnd();
					}
					else
						++i;
				}
			}
//			if(view == m_currentView)
			setCurrentView(m_firstView);
			h_views.remove(name);

			emit(viewRemoved(view));

			delete view;
		}
	}
}

View* Window::getView(const QString& name) const
{
	if (h_views.contains(name))
		return h_views[name];
	else
	{
		System::Error::code = System::Error::VIEW_DOES_NOT_EXIST;
		return NULL;
	}
}

void Window::setCurrentView(View* view)
{
	const QList<Plugin*>& oldPlugins = m_currentView->getLinkedPlugins();
	foreach(Plugin* p, oldPlugins)
		disablePluginTabWidgets(p);

	View* oldCurrent = m_currentView;
	m_currentView = view;

	const QList<Plugin*>& newPlugins = m_currentView->getLinkedPlugins();
	foreach(Plugin* p, newPlugins)
	{
		enablePluginTabWidgets(p);
		p->currentViewChanged(m_currentView);
	}

	oldCurrent->updateGL();
	m_currentView->updateGL();
}

void Window::splitView(const QString& name, Qt::Orientation orientation)
{
	View* newView = addView();

//	std::cout << "splitView" << std::endl;

	View* view = h_views[name];
	QSplitter* parent = (QSplitter*)(view->parentWidget());
	if(parent == m_rootSplitter && !b_rootSplitterInitialized)
	{
//		std::cout << "init root splitter" << std::endl;
		m_rootSplitter->setOrientation(orientation);
		b_rootSplitterInitialized = true;
	}
	if(parent->orientation() == orientation)
	{
//		std::cout << "same orientation" << std::endl;
		parent->insertWidget(parent->indexOf(view)+1, newView);
	}
	else
	{
//		std::cout << "new orientation" << std::endl;
		int idx = parent->indexOf(view);
		view->setParent(NULL);
		QSplitter* spl = new QSplitter(orientation);
		spl->addWidget(view);
		spl->addWidget(newView);
		parent->insertWidget(idx, spl);
	}
}

/*********************************************************
 * MANAGE PLUGINS
 *********************************************************/

Plugin* Window::loadPlugin(const QString& pluginFilePath)
{
	QString pluginName = QFileInfo(pluginFilePath).baseName().remove(0, 3);

	if (h_plugins.contains(pluginName))
	{
		// set message error + function fails
		System::Error::code = System::Error::PLUGIN_EXISTS;
		return NULL;
	}

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

		QPluginLoader loader(plugin->getFilePath());
		loader.unload();

		statusbar->showMessage(pluginName + QString(" successfully unloaded."), 2000);
		emit(pluginRemoved(plugin));

		// delete plugin
		delete plugin;
	}
}

Plugin* Window::getPlugin(const QString& name) const
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

bool Window::addMap(MapHandlerGen* map)
{
	if (h_maps.contains(map->getName()))
	{
		System::Error::code = System::Error::MAP_EXISTS;
		return false;
	}

	h_maps.insert(map->getName(), map);

	emit(mapAdded(map));

	return true;
}

void Window::removeMap(const QString& name)
{
	if (h_maps.contains(name))
	{
		MapHandlerGen* map = h_maps[name];
		h_maps.remove(name);

		emit(mapRemoved(map));

		delete map;
	}
}

MapHandlerGen* Window::getMap(const QString& name) const
{
	if (h_maps.contains(name))
		return h_maps[name];
	else
	{
		System::Error::code = System::Error::MAP_DOES_NOT_EXIST;
		return NULL;
	}
}

/*********************************************************
 * MANAGE TEXTURES
 *********************************************************/

Texture* Window::getTexture(const QString& image)
{
	if(h_textures.contains(image))
	{
		Texture* t = h_textures[image];
		t->ref++;
		return t;
	}
	else
	{
		Texture* t = NULL;
		QImage img(image);
		if(!img.isNull())
		{
			GLuint texID = m_firstView->bindTexture(img);
			t = new Texture(texID, img.size(), 1);
			h_textures.insert(image, t);
		}
		return t;
	}
}

void Window::releaseTexture(const QString& image)
{
	if(h_textures.contains(image))
	{
		Texture* t = h_textures[image];
		t->ref--;
		if(t->ref == 0)
		{
			m_firstView->deleteTexture(h_textures[image]->texID);
			h_textures.remove(image);
			delete t;
		}
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

void Window::cb_showHideDock()
{
	m_dock->setVisible(m_dock->isHidden());
}

void Window::cb_manageCameras()
{
	m_camerasDialog->show();
}

void Window::cb_managePlugins()
{
	m_pluginsDialog->show();
}

void Window::cb_manageMaps()
{
	m_mapsDialog->show();
}

} // namespace SCHNApps

} // namespace CGoGN
