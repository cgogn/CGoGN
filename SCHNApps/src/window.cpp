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
#include "camera.h"
#include "mapHandler.h"

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
//	System::StateHandler::saveState(this, h_plugins);
}

/*********************************************************
 * MANAGE DOCK
 *********************************************************/

void Window::addTabInDock(QWidget* tabWidget, const QString& tabText, bool enable)
{
	if(tabWidget)
	{
		int currentTab = m_dockTabWidget->currentIndex();

		int idx = m_dockTabWidget->addTab(tabWidget, tabText);
		m_dock->setVisible(true);
		m_dockTabWidget->setTabEnabled(idx, enable);

		if(currentTab != -1)
			m_dockTabWidget->setCurrentIndex(currentTab);
	}
}

void Window::removeTabInDock(QWidget *tabWidget)
{
	if(tabWidget)
		m_dockTabWidget->removeTab(m_dockTabWidget->indexOf(tabWidget));
}

void Window::enablePluginTabWidgets(Plugin* plugin)
{
	int currentTab = m_dockTabWidget->currentIndex();

	const QList<QWidget*> tabWidgets = plugin->getTabWidgets();
	foreach(QWidget* w, tabWidgets)
		m_dockTabWidget->setTabEnabled(m_dockTabWidget->indexOf(w), true);

	m_dockTabWidget->setCurrentIndex(currentTab);
}

void Window::disablePluginTabWidgets(Plugin* plugin)
{
	int currentTab = m_dockTabWidget->currentIndex();

	const QList<QWidget*> tabWidgets = plugin->getTabWidgets();
	foreach(QWidget* w, tabWidgets)
		m_dockTabWidget->setTabEnabled(m_dockTabWidget->indexOf(w), false);

	m_dockTabWidget->setCurrentIndex(currentTab);
}

/*********************************************************
 * MANAGE MENU ACTIONS
 *********************************************************/

bool Window::addMenuAction(const QString& menuPath, QAction* action)
{
	// if menu path = empty string: set error + failure
	if (menuPath.isEmpty())
		return false;

	if (!action)
		return false;

	// extracting all the substring separated by ';'
	QStringList stepNames = menuPath.split(";");
	stepNames.removeAll("");
	unsigned int nbStep = stepNames.count();

	// if only one substring: error + failure
	// No action directly in the menu bar
	if (nbStep < 1)
		return false;

	unsigned int i = 0;
	QMenu* lastMenu = NULL;
	foreach(QString step, stepNames)
	{
		++i;
		if (i < nbStep) // if not last substring (= menu)
		{
			// try to find an existing submenu with step name
			bool found = false;
			QList<QAction*> actions;
			if(i == 1) actions = menubar->actions();
			else actions = lastMenu->actions();
			foreach(QAction* action, actions)
			{
				QMenu* submenu = action->menu();
				if (submenu && submenu->title() == step)
				{
					lastMenu = submenu;
					found = true;
					break;
				}
			}
			if (!found)
			{
				QMenu* newMenu;
				if(i == 1)
				{
					newMenu = menubar->addMenu(step);
					newMenu->setParent(menubar);
				}
				else
				{
					newMenu = lastMenu->addMenu(step);
					newMenu->setParent(lastMenu);
				}
				lastMenu = newMenu;
			}
		}
		else // if last substring (= action name)
		{
			lastMenu->addAction(action);
			action->setText(step);
			action->setParent(lastMenu);
		}
	}

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
		delete action;

		while(parent != NULL)
		{
			QMenu* parentMenu = dynamic_cast<QMenu*>(parent);
			if(parentMenu && parentMenu->actions().empty())
			{
				parent = parent->parent();
				delete parentMenu;
			}
			else
				parent = NULL;
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
		return NULL;

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
		return NULL;
}

/*********************************************************
 * MANAGE VIEWS
 *********************************************************/

View* Window::addView(const QString& name)
{
	if (h_views.contains(name))
		return NULL;

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
		return NULL;
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
		return NULL;

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
		return NULL;
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
		return NULL;
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
		return NULL;
}
*/

/*********************************************************
 * MANAGE MAPS
 *********************************************************/

GenericMap* Window::createMap(unsigned int dim)
{
	GenericMap* map = NULL;
	switch(dim)
	{
		case 2 :
			map = new PFP2::MAP();
			break;
		case 3 :
			map = new PFP3::MAP();
			break;
	}
	return map;
}

bool Window::addMap(MapHandlerGen* map)
{
	if (h_maps.contains(map->getName()))
		return false;

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
		return NULL;
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
