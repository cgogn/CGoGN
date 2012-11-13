#include "window.h"

#include <QApplication>
#include <QMessageBox>
#include <QDockWidget>
#include <QPluginLoader>
#include <QFileInfo>

#include "scene.h"
#include "sceneSelector.h"
#include "view.h"
#include "newSceneDialog.h"
#include "globalCameraDialog.h"
#include "plugin.h"
#include "pluginDialog.h"
#include "linkViewDialog.h"
#include "context.h"
#include "mapPluginDialog.h"
#include "mapHandler.h"


Window::Window(QWidget* parent): QMainWindow(parent),
	verticalLayout(NULL),
	m_splitArea(NULL),
	m_dock(NULL),
	m_dockTabWidget(NULL)
{
	//program in its initialization phase
	m_initialization=true;

	m_context= new Context(this,QGLFormat(QGL::Rgba | QGL::DoubleBuffer| QGL::DepthBuffer));
	if(m_context->create()){
		std::cout << "->Creation!!!" << std::endl;
	}
	else{
		std::cout << "->NoooOOooooooooooooooooooooooooooooOOon!!!" << std::endl;
	}

	this->setupUi(this);
	System::splash->showMessage("Welcome to CGoGN", Qt::AlignBottom | Qt::AlignCenter);
	sleep(1);

	//layout in wich we store the main area
	verticalLayout= new QVBoxLayout(centralwidget);
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

	//the main area: multi GL views display area
	m_splitArea= new SplitArea(centralwidget);
	verticalLayout->addWidget(m_splitArea);

	//init keys as unpressed
	keys[0]=false; keys[1]=false; keys[2]=false;


	//the basic actions connect
	connect(actionAbout_CGoGN, SIGNAL(triggered()), this, SLOT(cb_about_CGoGN()));
	connect(actionPlugins, SIGNAL(triggered()), this, SLOT(cb_pluginDialog()));

	connect(actionNew_Scene, SIGNAL(triggered()), this, SLOT(cb_niewScene()));
	connect(actionGlobalCamera, SIGNAL(triggered()), this, SLOT(cb_globalCamera()));
	connect(actionLinkPluginMap, SIGNAL(triggered()), this, SLOT(cb_mapPlugin()));

//	QuestionSessionDialog qsd(this);
//	qsd.exec();
//	if(qsd.result()==QDialog::Accepted){
//		System::StateHandler::loadState(this, &h_plugin, &h_scene, m_splitArea);
//	}
////	else{
////		System::StateHandler::unsaveState();
////	}
	System::StateHandler::loadState(this, &h_plugin, &h_scene, m_splitArea);

	actionGlobalCamera->setEnabled(h_scene.size()>1);

	//program in its initialization phase
	m_initialization=false;
}

Window::~Window(){
	System::StateHandler::saveState(this, &h_plugin, &h_scene, m_splitArea);
}

bool Window::addNewEmptyScene(QString name, Scene* &scene, bool dialog, Camera* sharedCamera){
	//if there's no main area allocated or
	//a viewer with the same name is found
	if(!m_splitArea || h_scene.find(name)!=h_scene.end()){
		//set message error + function fails
		System::Error::code= System::Error::SCENE_EXISTS;
		return false;
	}
	//if the glviewer doesn't exists and the main area allocated
	else{
		//the GLviewer is created, set with the correct parameters (name, creating plugin)
		//and is put in the correct place in the main area
		scene= new Scene(name, this, sharedCamera);

		View* sceneView= scene->getView(0);
		//the GLView selection dialog is relevant only if their is some
		//view already displayed
		if(!h_scene.isEmpty() && dialog){
			//we map the splitArea
			ViewPixMaps pm;
			pm.fromSplitArea(m_splitArea);

			//we build a GLViewerSelector using this map
			ViewSelector selector(pm, this, ViewSelector::SELECT);

			selector.setInsertionName(name);

			//we show the move dialog box
			selector.exec();

			//if the dialog is accepted: it has some modification
			if(selector.result()==QDialog::Accepted){
				QPoint insertPoint= selector.getInsertPoint();

				//we're inserting the view in the desired place in the window
				m_splitArea->addElementAt(sceneView,insertPoint.x(),insertPoint.y());
			}
			//if not, usual insertion
			else{
				m_splitArea->addFitElement(sceneView);
			}
		}
		else if(!m_initialization){
			m_splitArea->addFitElement(sceneView);
		}
		h_scene.insert(name,scene);

		if(h_scene.count()<=1){
			actionGlobalCamera->setEnabled(false);
		}
		else{
			actionGlobalCamera->setEnabled(true);
		}

		return true;
	}

}

bool Window::addNewSceneView(Scene* scene, View* view){
	if(scene && h_scene.find(scene->getName())!=h_scene.end() && m_splitArea){
		if(!m_initialization)
			m_splitArea->addElementRightTo(view, scene->getView(0));
		return true;
	}
	else{
		return false;
	}
}

bool Window::associateSceneWithPlugin(QString glviewer, Plugin* plugin, Scene* &scene, bool cb_initGL){
	//if the main area is allocated and some GLViewers are allocated
	if(m_splitArea && !h_scene.empty()){
		//try to find the corresponding GLViewer
		SceneHash::iterator it= h_scene.find(glviewer);
		//if found return its reference
		if (it!=h_scene.end()){
			scene=(*it);

			//adding a new plugin to the found GLViewers operating plugin list
			(*it)->associateNewPlugin(plugin,cb_initGL);

			return true;
		}
		//if not found: set error message + returns NULL
		else{
			System::Error::code= System::Error::SCENE_UNREFERENCED;
			return false;
		}
	}
	//if no area or no glviewers already referenced,
	else{
		//set error message + returns NULL
		System::Error::code= System::Error::NO_SCENE;
		return false;
	}
}

bool Window::addNewSceneFromPlugin(QString name, Plugin* plugin, Scene* &scene){
	//if there's no main area allocated or
	//a viewer with the same name is found
	if(!m_splitArea || h_scene.find(name)!=h_scene.end()){
		//set message error + function fails
		System::Error::code= System::Error::SCENE_EXISTS;
		return false;
	}
	//if the glviewer doesn't exists and the main area allocated
	else{
		//the GLviewer is created, set with the correct parameters (name, creating plugin)
		//and is put in the correct place in the main area
		scene= new Scene(name, plugin, this);
		scene->setName(name);
		//if we are in the initialization phase, the glviews won't be
		//added now, but when the info about their size will be read in
		//the xml settings file
		//otherwise we add the here.
//		if(!m_initialization)
			m_splitArea->addFitElement(scene->getView(0));
		h_scene.insert(name,scene);
		scene->updateGL();

		if(h_scene.count()<=1){
			actionGlobalCamera->setEnabled(false);
		}
		else{
			actionGlobalCamera->setEnabled(true);
		}

			//function succeeded
		return true;
	}
}

bool Window::addNewSceneFromPluginDialog(QString name, Plugin* plugin, Scene* &scene){
	//if there's no main area allocated or
	//a viewer with the same name is found
	if(!m_splitArea || h_scene.find(name)!=h_scene.end()){
		//set message error + function fails
		System::Error::code= System::Error::SCENE_EXISTS;
		return false;
	}
	//if the glviewer doesn't exists and the main area allocated
	else{
		//the GLviewer is created, set with the correct parameters (name, creating plugin)
		//and is put in the correct place in the main area
		scene= new Scene(name, plugin, this);
		scene->setName(name);

		//the GLView selection dialog is relevant only if their is some
		//view already displayed
		if(!h_scene.isEmpty()){
			//we map the splitArea
			ViewPixMaps pm, finalPm;
			pm.fromSplitArea(m_splitArea);

			//we build a ViewSelector using this map
			ViewSelector selector(pm, this, ViewSelector::SELECT);

			selector.setInsertionName(name);

			//we show the move dialog box
			selector.exec();

			//if the dialog is accepted: it has some modification
			if(selector.result()==QDialog::Accepted){
				QPoint insertPoint= selector.getInsertPoint();

				//we're inserting the view in the desired place in the window
				m_splitArea->addElementAt(scene->getView(0),insertPoint.x(),insertPoint.y());
			}
			//if not, usual insertion
			else{
				m_splitArea->addFitElement(scene->getView(0));
			}
		}
		else{
			m_splitArea->addFitElement(scene->getView(0));
		}
		h_scene.insert(name,scene);
		scene->updateGL();

		if(h_scene.count()<=1){
			actionGlobalCamera->setEnabled(false);
		}
		else{
			actionGlobalCamera->setEnabled(true);
		}

		//function succeeded
		return true;
	}
}

void Window::removeScene(QString name){
	//if the glviewer is referenced and found
	SceneHash::iterator it= h_scene.find(name);
	std::cout << "Window removing " << name.toStdString() << std::endl;
	if(it!=h_scene.end()){
		std::cout << "found" << std::endl;
		//it is remove form the referencing hash table and destroyed
		Scene* scene= (*it);
		h_scene.erase(it);
		delete scene;
	}

	if(h_scene.count()<=1){
		actionGlobalCamera->setEnabled(false);
	}
	else{
		actionGlobalCamera->setEnabled(true);
	}
}

void Window::addEmptyDock(){
	//if there is still no dock
	if(!m_dock){
		//creating it and its areas
		m_dock = new QDockWidget(tr("Control"), this);
		m_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		m_dock->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetClosable);
		addDockWidget(Qt::RightDockWidgetArea, m_dock);
	}
}

QTabWidget* Window::getDockTabWidget(){
	if(!m_dock || !m_dockTabWidget){
		//if no dock or no tab widget: set error message
		System::Error::code= System::Error::NO_DOCK;
	}
	return m_dockTabWidget;
}

void Window::addWidgetInDockTab(QWidget* widget, QString tabText){
	//if there is still no dock
	if(!m_dock){
		//it is created
		this->addEmptyDock();
	}
	//if there is still no tab widget in the dock
	if(!m_dockTabWidget){
		//it is created
		QWidget* dockWidgetContents = new QWidget();
        QVBoxLayout* verticalLayout = new QVBoxLayout(dockWidgetContents);
        m_dockTabWidget = new QTabWidget(dockWidgetContents);
        m_dockTabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        m_dockTabWidget->setLayoutDirection(Qt::RightToLeft);
        m_dockTabWidget->setTabPosition(QTabWidget::East);

        verticalLayout->addWidget(m_dockTabWidget);

        m_dock->setWidget(dockWidgetContents);
	}

	//adding a new tab contain the specified widget into the dock
	m_dockTabWidget->addTab(widget,tabText);
	m_dockTabWidget->setTabText(m_dockTabWidget->indexOf(widget),tabText);
}

void Window::removeTabInDock(QWidget* tabWidget){
	//if there is a dock and a tab widget
	if(m_dock && m_dockTabWidget){
		//remove the tab containing the specified widget
		m_dockTabWidget->removeTab(m_dockTabWidget->indexOf(tabWidget));
	}
}

bool Window::addMenuAction(QString menuPath, QAction* act){
	//if menu path = empty string: set error + failure
	if(menuPath.isEmpty()){
		System::Error::code= System::Error::BAD_ACTION_MENU_PATH_f(menuPath);
		return false;
	}

	//extracting all the substring separated by ';'
	QStringList stepNames= menuPath.split(";");
	stepNames.removeAll("");
	unsigned int nbStep= stepNames.count();

	//if only one substring: error + failure
	//	No action directly in the menu bar
	if(nbStep<1){
		System::Error::code= System::Error::BAD_ACTION_MENU_PATH_f(menuPath);
		return false;
	}

	//for every extracted substring
	unsigned int i=0;
	QMenu* lastMenu;
	QAction* lastAction;
	foreach(QString step, stepNames){
		++i;

		bool newMenu= true;
		//if not last substring (= menu)
		if(i<nbStep){
			//if first substring
			if(i==1){
				//checking if its an existing menu or not
				QList<QAction *> actions= menubar->actions();
				foreach(QAction* action, actions){
					if(action->menu()->title()==step){
						lastAction= action;
						newMenu= false;
						break;
					}
				}
				//if new menu: adding it to menubar
				if(newMenu){
					lastMenu= menubar->addMenu(step);
					lastMenu->setParent(menubar);
				}
				//if not: next operation in the already existing corresponding menu
				else{
					lastMenu= lastAction->menu();
					//setting the parent will be useful for the deleting function
					lastMenu->setParent(menubar);
				}
			}
			//if not first substring
			else{
				//same thing: except not operating on the menubar
				QList<QAction *> actions= lastMenu->actions();
				foreach(QAction* action, actions){
					if(action->menu()->title()==step){
						lastAction= action;
						newMenu= false;
						break;
					}
				}
				if(newMenu){
					QMenu* tmp= lastMenu;
					lastMenu= lastMenu->addMenu(step);
					//setting the parent will be useful for the deleting function
					lastMenu->setParent(tmp);
				}
				else{
					QMenu* tmp= lastMenu;
					lastMenu= lastAction->menu();
					//setting the parent will be useful for the deleting function
					lastMenu->setParent(tmp);
				}
			}
		}
		//if last substring (= action name)
		else{
			//adding it to the corresponding menu
			lastMenu->addAction(act);
			act->setText(step);
			//setting the parent will be useful for the deleting function
			act->setParent(lastMenu);
			break;
		}
	}

	//success
	return true;
}

void Window::deleteMenuAction(QAction* act){
	act->setEnabled(false);
	//parent of the action
	//wich is an instance of QMenu if the action was created
	//	using the addMenuActionMethod()
	QObject* parent= act->parent();
	//deleting action
	delete act;
	//if parent is an instance of QMenu
	if(parent->inherits("QMenu")){
		//for this menu and all of its parent that are instance of QMenu
		//	that are also fit for deleting
		while(parent!=NULL){
			QMenu* menuParent= (QMenu*)(parent);
			parent= parent->parent();

			//if the menu dont have any other actions/menus
			//	it is deleted
			if(menuParent->actions().empty()){
				delete menuParent;
			}
			//otherwise: end function
			else{
				break;
			}

			if(!parent->inherits("QMenu")){
				parent=NULL;
			}
		}
	}
}

bool Window::addToolbarAction(QAction* act){
	if(act){
		toolBar->addAction(act);
		return true;
	}
	return false;
}

Plugin* Window::loadPlugin(QString pluginPath){
	//QT's plugin loader class
	QPluginLoader loader(pluginPath);
	//if the loader loads a plugin instance
	if(QObject *pluginObject= loader.instance()){
		Plugin* plugin= qobject_cast<Plugin*>(pluginObject);

		QString pluginName= QFileInfo(pluginPath).fileName();
		pluginName.remove(0,3);
		pluginName.remove(pluginName.lastIndexOf(".so"),3);

		//we set the plugin with correct parameters (name, filepath)
		plugin->setWindow(this);
		plugin->setName(pluginName);
		plugin->setPluginFilePath(pluginPath);
		//then we call its activate() methods
		if(plugin->activate(m_initialization)){

			//if it succeeded we reference this plugin
			h_plugin.insert(pluginName, plugin);

			//method success
			return plugin;
		}
		else {
			//if it fails: method failure
			//error message already set within method activate
			return NULL;
		}
	}
	//if loading fails
	else{
		//set error message + method failure
		System::Error::code= System::Error::ERROR_PLUGIN_LOAD_f(pluginPath);
		return NULL;
	}
}

void Window::unloadPlugin(QString pluginName){
	//if the plugin is referenced and found
	PluginHash::iterator it;
	if((it=h_plugin.find(pluginName))!=h_plugin.end()){
		Plugin* plugin= (*it);

		//calling its disable() method and dereferencing it
		plugin->disable();
		h_plugin.erase(it);

		//deleting plugin
		delete(plugin);

		if(m_splitArea)
			m_splitArea->updateSize();
	}
}


Plugin* Window::checkPluginDependencie(QString name, Plugin* dependantPlugin){
	//if the plugin is referenced and found
	PluginHash::iterator it;
	if((it=h_plugin.find(name))!=h_plugin.end()){
		//the plugin calling for the depencie is added to the found plugin's list of dependant plugins
		(*it)->addDependantPlugin(dependantPlugin);
		return (*it);
	}
	//if not found: set error message + failure
	else{
		System::Error::code= System::Error::UNSATSIFIED_PLUGIN_DEPENDENCIE_f(name, dependantPlugin->getName());
		return NULL;
	}
}

void Window::linkDialog(Scene* scene){
	LinkViewDialog lvDialog(this, &h_plugin,scene);
	lvDialog.exec();
}

void Window::unlinkDialog(Scene* scene, QList<Plugin*> dependingPlugins){
	LinkViewDialog lvDialog(this, dependingPlugins, scene);
	lvDialog.exec();
}

bool Window::addReferencedMap(QString map_name, MapHandler* map){
	//if a map isn't already referenced under that name
//	MapHash::iterator it;
//	if((it=h_map.find(map_name))==h_map.end()){
//		//the map is created, inserted, and reference under the given name
//		h_map.insert(map_name,map);
//
//		return true;
//	}
	MapHash::iterator it;
	if((it=h_vizu.find(map_name))==h_vizu.end()){
		h_vizu.insert(map_name, map);

		return true;
	}

	//if a map already has that name
	else{
		//failure + map affected with the address of the already existing map
		System::Error::code= System::Error::MAP_EXISTS_f(map_name);
		return false;
	}
}

MapHandler* Window::getReferencedMap(QString map_name){
	MapHash::iterator it;
	if((it=h_vizu.find(map_name))!=h_vizu.end()){
		return (*it);
	}
	else{
		System::Error::code= System::Error::MAP_UNREFERENCED_f(map_name);
		return NULL;
	}
}

void Window::keyPressEvent( QKeyEvent * event ){
	std::cout << "KeyPressEvent" << std::endl;
	if(event->key()==Qt::Key_M){
		std::cout << "M" << std::endl;
		keys[0]= true;
	}
	else if(event->key()==Qt::Key_Shift){
		std::cout << "Shift" << std::endl;
		keys[1]= true;
	}
	else if(event->key()==Qt::Key_Control){
		std::cout << "CTRL" << std::endl;
		keys[2]= true;
	}

	if(keys[0] && keys[1] && keys[2]){
		std::cout << "CTRL+Maj+M" << std::endl;
		this->moveView();
	}
}

void Window::keyReleaseEvent( QKeyEvent * event ){
	if(event->key()==Qt::Key_M){
		keys[0]= false;
	}
	else if(event->key()==Qt::Key_Shift){
		keys[1]= false;
	}
	else if(event->key()==Qt::CTRL){
		keys[2]= false;
	}
}

void Window::moveView(){
	//if splitArea not empty or have more than 1 element
	if(!m_splitArea->isEmpty() &&
			(m_splitArea->getNbRow()>1 || ((QSplitter*)m_splitArea->widget(0))->count()>1)){
		//we map the splitArea
		ViewPixMaps pm, finalPm;
		pm.fromSplitArea(m_splitArea);

		//we build a GLViewerSelector using this map
		ViewSelector selector(pm, this, ViewSelector::MOVE);

		//we show the move dialog box
		selector.exec();

		//if the dialog is acceteped: it has some modification
		if(selector.result()==QDialog::Accepted){
			//get back the modifier map
			finalPm=selector.getGLVMap();

			//creating a new split area and switch it with the old one
			SplitArea* old= m_splitArea;
			m_splitArea= new SplitArea(centralwidget);
			verticalLayout->addWidget(m_splitArea);

			//we fill the new SplitArea using the modified map
			int x=0,y=0;
			for(ViewPixMaps::y_iterator y_it= finalPm.y_begin(); y_it!=finalPm.y_end(); ++y_it){
				for(ViewPixMaps::x_iterator x_it= finalPm.x_begin(y_it); x_it!=finalPm.x_end(y_it); ++x_it){
					x_it->view->setParent(m_splitArea);
					m_splitArea->addElementAt(x_it->view,x,y);

					x++;
				}
				x=0;
				++y;
			}

			//we suppress the old splitArea
			delete(old);
		}

		//key states at the end of the mov dialog
		keys[0]= selector.keys[0]; keys[1]= selector.keys[1]; keys[2]= selector.keys[2];
	}
}

void Window::cb_about_CGoGN()
{
	QString str("CGoGN:\nCombinatorial and Geometric modeling\n"
				"with Generic N-dimensional Maps\n"
				"Web site: http://cgogn.unistra.fr \n"
				"Contact information: cgogn@unistra.fr");
	QMessageBox::about(this, tr("About CGoGN"), str);
}

void Window::cb_pluginDialog(){
	PluginDialog plugDial(this, &h_plugin);
	plugDial.exec();
}

void Window::cb_niewScene(){
	NewSceneDialog ns(this);
	ns.exec();
}

void Window::cb_globalCamera(){
	GlobalCameraDialog gcd(this);
	gcd.exec();

	if(h_scene.count()<=1){
		actionGlobalCamera->setEnabled(false);
	}
	else{
		actionGlobalCamera->setEnabled(true);
	}
}

void Window::cb_mapPlugin(){
	MapPluginDialog mpd(this, &h_vizu);
	mpd.exec();
}
