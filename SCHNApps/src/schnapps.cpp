#include <QVBoxLayout>
#include <QSplitter>
#include <QMessageBox>
#include <QDockWidget>
#include <QPluginLoader>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFile>
#include <QByteArray>

#include "mapHandler.h"
#include "schnapps.h"

#include "controlDock_cameraTab.h"
#include "controlDock_mapTab.h"
#include "controlDock_pluginTab.h"

#include "camera.h"
#include "view.h"
#include "texture.h"
#include "plugin.h"
#include "plugin_interaction.h"
#include "plugin_processing.h"

#include "plugins_default_directory.h"
#include "Topology/generic/genericmap.h"

namespace CGoGN
{

namespace SCHNApps
{

SCHNApps::SCHNApps(const QString& appPath, PythonQtObjectPtr& pythonContext, PythonQtScriptingConsole& pythonConsole) :
	QMainWindow(),
	m_pyRecording(NULL),
	m_pyRecFile(NULL),
	m_appPath(appPath),
	m_pythonContext(pythonContext),
	m_pythonConsole(pythonConsole),
	m_firstView(NULL),
	m_selectedView(NULL)
{
	GenericMap::initAllStatics(&m_sp);

	this->setupUi(this);

	// create & setup control dock

	m_controlDock = new QDockWidget("Control Dock", this);
	m_controlDock->setAllowedAreas(Qt::LeftDockWidgetArea);
	m_controlDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
	m_controlDock->setMaximumWidth(250);

	m_controlDockTabWidget = new QTabWidget(m_controlDock);
	m_controlDockTabWidget->setObjectName("ControlDockTabWidget");
	m_controlDockTabWidget->setLayoutDirection(Qt::LeftToRight);
	m_controlDockTabWidget->setTabPosition(QTabWidget::North);
	m_controlDockTabWidget->setMovable(true);

	addDockWidget(Qt::LeftDockWidgetArea, m_controlDock);
	m_controlDock->setVisible(true);
	m_controlDock->setWidget(m_controlDockTabWidget);

	m_controlCameraTab = new ControlDock_CameraTab(this);
	m_controlDockTabWidget->addTab(m_controlCameraTab, m_controlCameraTab->title());
	m_controlMapTab = new ControlDock_MapTab(this);
	m_controlDockTabWidget->addTab(m_controlMapTab, m_controlMapTab->title());
	m_controlPluginTab = new ControlDock_PluginTab(this);
	m_controlDockTabWidget->addTab(m_controlPluginTab, m_controlPluginTab->title());

	connect(action_ShowHideControlDock, SIGNAL(triggered()), this, SLOT(showHideControlDock()));

	// create & setup plugin dock

	m_pluginDock = new QDockWidget("Plugins Dock", this);
	m_pluginDock->setAllowedAreas(Qt::RightDockWidgetArea);
	m_pluginDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);

	m_pluginDockTabWidget = new QTabWidget(m_pluginDock);
	m_pluginDockTabWidget->setObjectName("PluginDockTabWidget");
	m_pluginDockTabWidget->setLayoutDirection(Qt::LeftToRight);
	m_pluginDockTabWidget->setTabPosition(QTabWidget::East);
	m_pluginDockTabWidget->setMovable(true);

	addDockWidget(Qt::RightDockWidgetArea, m_pluginDock);
	m_pluginDock->setVisible(false);
	m_pluginDock->setWidget(m_pluginDockTabWidget);

	connect(action_ShowHidePluginDock, SIGNAL(triggered()), this, SLOT(showHidePluginDock()));

	// create & setup python dock

	m_pythonDock = new QDockWidget("Python Dock", this);
	m_pythonDock->setAllowedAreas(Qt::BottomDockWidgetArea);
	m_pythonDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);

	addDockWidget(Qt::BottomDockWidgetArea, m_pythonDock);
	m_pythonDock->setVisible(false);
	m_pythonDock->setWidget(&m_pythonConsole);

	connect(action_ShowHidePythonDock, SIGNAL(triggered()), this, SLOT(showHidePythonDock()));
	connect(action_LoadPythonScript, SIGNAL(triggered()), this, SLOT(loadPythonScriptFromFileDialog()));

	connect(action_Python_Recording, SIGNAL(triggered()), this, SLOT(pyRecording()));
	connect(action_Append_Python_Recording, SIGNAL(triggered()), this, SLOT(appendPyRecording()));
	connect(action_Clean_All, SIGNAL(triggered()), this, SLOT(cleanAll()));


	action_Python_Recording->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	action_Append_Python_Recording->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_P));




	// create & setup central widget (views)
	m_centralLayout = new QVBoxLayout(centralwidget);
	m_centralLayout->setMargin(2);

	m_rootSplitter = new QSplitter(centralwidget);
	b_rootSplitterInitialized = false;
	m_centralLayout->addWidget(m_rootSplitter);

	if (Utils::GLSLShader::CURRENT_OGL_VERSION >= 3)
	{
		QGLFormat glFormat;
		glFormat.setVersion( Utils::GLSLShader::MAJOR_OGL_CORE, Utils::GLSLShader::MINOR_OGL_CORE);
		glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
		glFormat.setSampleBuffers( true );
		QGLFormat::setDefaultFormat(glFormat);
	}

	m_firstView = addView();
	setSelectedView(m_firstView);
	m_rootSplitter->addWidget(m_firstView);

	// connect basic actions

	connect(action_AboutSCHNApps, SIGNAL(triggered()), this, SLOT(aboutSCHNApps()));
	connect(action_AboutCGoGN, SIGNAL(triggered()), this, SLOT(aboutCGoGN()));

	// register a first plugins directory
//	registerPluginsDirectory(m_appPath + QString("/../lib"));

	// using macro in plugins_default_directory.h generated by cmake for lib/Release or lib/Debug auto selection !
	registerPluginsDirectory(QString(PLUGINS_DEFAULT_DIRECTORY));
}

SCHNApps::~SCHNApps()
{
	// TODO check that cameras are destructed
	m_cameras.clear();
}

/*********************************************************
 * MANAGE CAMERAS
 *********************************************************/

Camera* SCHNApps::addCamera(const QString& name)
{
	if (m_cameras.contains(name))
		return NULL;

	Camera* camera = new Camera(name, this);
	m_cameras.insert(name, camera);
	DEBUG_EMIT("cameraAdded");
	emit(cameraAdded(camera));
	return camera;
}

Camera* SCHNApps::addCamera()
{
	return addCamera(QString("camera_") + QString::number(Camera::cameraCount));
}

void SCHNApps::removeCamera(const QString& name)
{
	Camera* camera = getCamera(name);
	if (camera && !camera->isUsed())
	{
		m_cameras.remove(name);
		DEBUG_EMIT("cameraRemoved");
		emit(cameraRemoved(camera));
		delete camera;
	}
}

Camera* SCHNApps::getCamera(const QString& name) const
{
	if (m_cameras.contains(name))
		return m_cameras[name];
	else
		return NULL;
}

/*********************************************************
 * MANAGE VIEWS
 *********************************************************/

void SCHNApps::redrawAllViews()
{
	foreach(View *v, m_views)
	{
		v->updateGL();
	}
}

View* SCHNApps::addView(const QString& name)
{
	if (m_views.contains(name))
		return NULL;

	View* view = new View(name, this, m_firstView);

	m_views.insert(name, view);

	DEBUG_EMIT("viewAdded");
	emit(viewAdded(view));

	// RECORDING
	if (m_pyRecording)
		*m_pyRecording << view->getName() << " = schnapps.getView(\"" << view->getName() << "\");" << endl;

	return view;
}

View* SCHNApps::addView()
{
	return addView(QString("view_") + QString::number(View::viewCount));
}

void SCHNApps::removeView(const QString& name)
{
	if (m_views.contains(name))
	{
		if(m_views.count() > 1)
		{
			View* view = m_views[name];
			if(view == m_firstView)
			{
				ViewSet::const_iterator i = m_views.constBegin();
				while (i != m_views.constEnd())
				{
					if(i.value() != view)
					{
						m_firstView = i.value();
						i = m_views.constEnd();
					}
					else
						++i;
				}
			}
			if(view == m_selectedView)
				setSelectedView(m_firstView);

			m_views.remove(name);

			DEBUG_EMIT("viewRemoved");
			emit(viewRemoved(view));

			delete view;
		}
	}
}

View* SCHNApps::getView(const QString& name) const
{
	if (m_views.contains(name))
		return m_views[name];
	else
		return NULL;
}

void SCHNApps::setSelectedView(View* view)
{
	int currentTab = m_pluginDockTabWidget->currentIndex();

	if(m_selectedView)
	{
		foreach(PluginInteraction* p, m_selectedView->getLinkedPlugins())
			disablePluginTabWidgets(p);
		disconnect(m_selectedView, SIGNAL(pluginLinked(PluginInteraction*)), this, SLOT(enablePluginTabWidgets(PluginInteraction*)));
		disconnect(m_selectedView, SIGNAL(pluginUnlinked(PluginInteraction*)), this, SLOT(disablePluginTabWidgets(PluginInteraction*)));
	}

	View* oldSelected = m_selectedView;
	m_selectedView = view;
	if (oldSelected)
		oldSelected->hideDialogs();

	foreach(PluginInteraction* p, m_selectedView->getLinkedPlugins())
		enablePluginTabWidgets(p);
	connect(m_selectedView, SIGNAL(pluginLinked(PluginInteraction*)), this, SLOT(enablePluginTabWidgets(PluginInteraction*)));
	connect(m_selectedView, SIGNAL(pluginUnlinked(PluginInteraction*)), this, SLOT(disablePluginTabWidgets(PluginInteraction*)));

	m_pluginDockTabWidget->setCurrentIndex(currentTab);

	DEBUG_EMIT("selectedViewChanged");
	emit(selectedViewChanged(oldSelected, m_selectedView));

	if(oldSelected)
		oldSelected->updateGL();
	m_selectedView->updateGL();
}

void SCHNApps::setSelectedView(const QString& name)
{
	View* v = this->getView(name);
	setSelectedView(v);
}

View* SCHNApps::splitView(const QString& name, Qt::Orientation orientation)
{
	View* newView = addView();

	View* view = m_views[name];
	QSplitter* parent = (QSplitter*)(view->parentWidget());
	if(parent == m_rootSplitter && !b_rootSplitterInitialized)
	{
		m_rootSplitter->setOrientation(orientation);
		b_rootSplitterInitialized = true;
	}
	if (parent->orientation() == orientation)
	{
		parent->insertWidget(parent->indexOf(view) + 1, newView);
		QList<int> sz = parent->sizes();
		int tot = 0;
		for (int i = 0; i < parent->count(); ++i)
			tot += sz[i];
		sz[0] = tot / parent->count() + tot % parent->count();
		for (int i = 1; i < parent->count(); ++i)
			sz[i] = tot / parent->count();
		parent->setSizes(sz);
	}
	else
	{
		int idx = parent->indexOf(view);
		view->setParent(NULL);
		QSplitter* spl = new QSplitter(orientation);
		spl->addWidget(view);
		spl->addWidget(newView);
		parent->insertWidget(idx, spl);

		QList<int> sz = spl->sizes();
		int tot = sz[0] + sz[1];
		sz[0] = tot / 2;
		sz[1] = tot - sz[0];
		spl->setSizes(sz);
	}

	// RECORDING
	if (m_pyRecording)
		*m_pyRecording << newView->getName() << " = schnapps.splitView(\"" << name << "\", " << orientation << ");" << endl;

	return newView;
}

QString SCHNApps::saveSplitViewPositions()
{
	QList<QSplitter*> liste;
	liste.push_back(m_rootSplitter);

	QString result;
	QTextStream qts(&result);
	while (!liste.empty())
	{
		QSplitter* spl = liste.first();
		for (int i = 0; i < spl->count(); ++i)
		{
			QWidget *w = spl->widget(i);
			QSplitter* qw = dynamic_cast<QSplitter*>(w);
			if (qw != NULL)
				liste.push_back(qw);
		}
		QByteArray ba = spl->saveState();
		qts << ba.count() << " ";
		for (int j = 0; j < ba.count(); ++j)
			qts << int(ba[j]) << " ";
		//qts << endl;
		liste.pop_front();
	}
	return result;
}


void SCHNApps::restoreSplitViewPositions(QString stringStates)
{
	QList<QSplitter*> liste;
	liste.push_back(m_rootSplitter);

	QTextStream qts(&stringStates);
	while (!liste.empty())
	{
		QSplitter* spl = liste.first();
		for (int i = 0; i < spl->count(); ++i)
		{
			QWidget *w = spl->widget(i);
			QSplitter* qw = dynamic_cast<QSplitter*>(w);
			if (qw != NULL)
				liste.push_back(qw);
		}
		if (qts.atEnd())
		{
			std::cerr << "Problem restoring view split configuration" << std::endl;
			return;
		}

		int nb;
		qts >> nb;
		QByteArray ba(nb + 1, 0);
		for (int j = 0; j < nb; ++j)
		{
			int v;
			qts >> v;
			ba[j] = char(v);
		}
		spl->restoreState(ba);
		liste.pop_front();
	}
}




/*********************************************************
 * MANAGE PLUGINS
 *********************************************************/

void SCHNApps::registerPluginsDirectory(const QString& path)
{

#ifdef WIN32
#ifdef _DEBUG
	QDir directory(path+QString("Debug/"));
#else
	QDir directory(path + QString("Release/"));
#endif
#else
	QDir directory(path);
#endif
	if(directory.exists())
	{
		QStringList filters;
		filters << "lib*.so";
		filters << "lib*.dylib";
		filters << "*.dll";

		QStringList pluginFiles = directory.entryList(filters, QDir::Files);

		foreach(QString pluginFile, pluginFiles)
		{
			QFileInfo pfi(pluginFile);
#ifdef WIN32
			QString pluginName = pfi.baseName();
#else
			QString pluginName = pfi.baseName().remove(0, 3);
#endif
			QString pluginFilePath = directory.absoluteFilePath(pluginFile);

			if(!m_availablePlugins.contains(pluginName))
			{
				m_availablePlugins.insert(pluginName, pluginFilePath);
				DEBUG_EMIT("pluginAvailableAdded");
				emit(pluginAvailableAdded(pluginName));
			}
		}
	}
}

Plugin* SCHNApps::enablePlugin(const QString& pluginName)
{
	// RECORDING
	if (m_pyRecording)
		*m_pyRecording << pluginName << " = schnapps.enablePlugin(\"" << pluginName << "\");" << endl;

	if (m_plugins.contains(pluginName))
		return m_plugins[pluginName];

	if (m_availablePlugins.contains(pluginName))
	{
		QString pluginFilePath = m_availablePlugins[pluginName];

		QPluginLoader loader(pluginFilePath);

		// if the loader loads a plugin instance
		if (QObject* pluginObject = loader.instance())
		{
			Plugin* plugin = qobject_cast<Plugin*>(pluginObject);

			// set the plugin with correct parameters (name, filepath, SCHNApps)
			plugin->setName(pluginName);
			plugin->setFilePath(pluginFilePath);
			plugin->setSCHNApps(this);

			// then we call its enable() methods
			if (plugin->enable())
			{
				// if it succeeded we reference this plugin
				m_plugins.insert(pluginName, plugin);

				statusbar->showMessage(pluginName + QString(" successfully loaded."), 2000);
				DEBUG_EMIT("pluginEnabled");
				emit(pluginEnabled(plugin));
//				menubar->repaint();
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
			std::cout << "loader.instance() failed.." << std::endl;
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

void SCHNApps::disablePlugin(const QString& pluginName)
{
	if (m_plugins.contains(pluginName))
	{
		Plugin* plugin = m_plugins[pluginName];

		// unlink linked views (for interaction plugins)
		PluginInteraction* pi = dynamic_cast<PluginInteraction*>(plugin);
		if(pi)
		{
			foreach(View* view, pi->getLinkedViews())
				view->unlinkPlugin(pi);
		}

		// call disable() method and dereference plugin
		plugin->disable();
		m_plugins.remove(pluginName);

		// remove plugin dock tabs
		foreach(QWidget* tab, m_pluginTabs[plugin])
			removePluginDockTab(plugin, tab);
		// remove plugin menu actions
		foreach(QAction* action, m_pluginMenuActions[plugin])
			removeMenuAction(plugin, action);


		QPluginLoader loader(plugin->getFilePath());
		loader.unload();

		statusbar->showMessage(pluginName + QString(" successfully unloaded."), 2000);
		DEBUG_EMIT("pluginDisabled");
		emit(pluginDisabled(plugin));

		delete plugin;

		// RECORDING
		if (m_pyRecording)
			*m_pyRecording << "schnapps.disablePlugin(\"" << pluginName << "\");" << endl;


	}
}

Plugin* SCHNApps::getPlugin(const QString& name) const
{
	if (m_plugins.contains(name))
		return m_plugins[name];
	else
		return NULL;
}

void SCHNApps::addPluginDockTab(Plugin* plugin, QWidget* tabWidget, const QString& tabText)
{
	if(tabWidget && !m_pluginTabs[plugin].contains(tabWidget))
	{
		int currentTab = m_pluginDockTabWidget->currentIndex();

		int idx = m_pluginDockTabWidget->addTab(tabWidget, tabText);
		m_pluginDock->setVisible(true);

		PluginInteraction* pi = dynamic_cast<PluginInteraction*>(plugin);
		if(pi)
		{
			if(pi->isLinkedToView(m_selectedView))
				m_pluginDockTabWidget->setTabEnabled(idx, true);
			else
				m_pluginDockTabWidget->setTabEnabled(idx, false);
		}

		if(currentTab != -1)
			m_pluginDockTabWidget->setCurrentIndex(currentTab);

		m_pluginTabs[plugin].append(tabWidget);
	}
}

void SCHNApps::removePluginDockTab(Plugin* plugin, QWidget *tabWidget)
{
	if(tabWidget && m_pluginTabs[plugin].contains(tabWidget))
	{
		m_pluginDockTabWidget->removeTab(m_pluginDockTabWidget->indexOf(tabWidget));

		m_pluginTabs[plugin].removeOne(tabWidget);
	}
}

void SCHNApps::enablePluginTabWidgets(PluginInteraction* plugin)
{
	if(m_pluginTabs.contains(plugin))
	{
		foreach(QWidget* w, m_pluginTabs[plugin])
			m_pluginDockTabWidget->setTabEnabled(m_pluginDockTabWidget->indexOf(w), true);
	}
}

void SCHNApps::disablePluginTabWidgets(PluginInteraction* plugin)
{
	if(m_pluginTabs.contains(plugin))
	{
		foreach(QWidget* w, m_pluginTabs[plugin])
			m_pluginDockTabWidget->setTabEnabled(m_pluginDockTabWidget->indexOf(w), false);
	}
}

/*********************************************************
 * MANAGE MAPS
 *********************************************************/

MapHandlerGen* SCHNApps::duplicateMap(const QString& name, bool properties)
{
	if (! m_maps.contains(name))
		return NULL;

	QString newName = name + "_copy";

	if (m_maps.contains(newName))
		return NULL;

	// RECORDING
	QTextStream* rec = this->pythonStreamRecorder();
	if (rec)
		*rec << newName << " = schnapps.duplicateMap(" << name << ".getName(),1);" << endl;

	MapHandlerGen* maph = m_maps[name];
	MapHandlerGen* new_mh = NULL; // avoid warning

	unsigned int dim = maph->getGenericMap()->dimension();

	switch (dim)
	{
	case 2: {
				PFP2::MAP* map = new PFP2::MAP();
				map->copyFrom(*(maph->getGenericMap()));
				new_mh = new MapHandler<PFP2>(newName, this, map);

				for (unsigned int orbit = VERTEX; orbit <= VOLUME; orbit++)
				{
					AttributeContainer& cont = map->getAttributeContainer(orbit);
					std::vector<std::string> names;
					std::vector<std::string> types;
					cont.getAttributesNames(names);
					cont.getAttributesTypes(types);
					for (unsigned int i = 0; i < names.size(); ++i)
						new_mh->registerAttribute(orbit, QString::fromStdString(names[i]), QString::fromStdString(types[i]));
				}
				break;
			}
	case 3: {
				PFP3::MAP* map = new PFP3::MAP();
				map->copyFrom(*(maph->getGenericMap()));
				new_mh = new MapHandler<PFP3>(newName, this, map);
				for (unsigned int orbit = VERTEX; orbit <= VOLUME; orbit++)
				{
					AttributeContainer& cont = map->getAttributeContainer(orbit);
					std::vector<std::string> names;
					std::vector<std::string> types;
					cont.getAttributesNames(names);
					cont.getAttributesTypes(types);
					for (unsigned int i = 0; i < names.size(); ++i)
						new_mh->registerAttribute(orbit, QString::fromStdString(names[i]), QString::fromStdString(types[i]));
				}
				break;
			}
	default:
		std::cerr << "Dimension " << dim << " not yet supported in map duplication" << std::endl;
		break;
	}




	m_maps.insert(newName, new_mh);
	DEBUG_EMIT("mapAdded");
	emit(mapAdded(new_mh));

	if (properties)
	{
		// BB
		new_mh->setBBVertexAttribute(maph->getBBVertexAttributeName());

		//VBOs
		const VBOSet& vbos = maph->getVBOSet();
		foreach(QString s, vbos.keys())
		{
			new_mh->createVBO(s);
		}
	}

	pythonVarDeclare(new_mh->getName());

	return new_mh;
}




MapHandlerGen* SCHNApps::addMap(const QString& name, unsigned int dim)
{
	QString finalName = name;
	if (m_maps.contains(name))
	{
		int i = 1;
		do
		{
			finalName = name + QString("_") + QString::number(i);
			++i;
		} while (m_maps.contains(finalName));
	}

	MapHandlerGen* mh = NULL;
	switch(dim)
	{
		case 2 : {
			PFP2::MAP* map = new PFP2::MAP();
			mh = new MapHandler<PFP2>(finalName, this, map);
			break;
		}
		case 3 : {
			PFP3::MAP* map = new PFP3::MAP();
			mh = new MapHandler<PFP3>(finalName, this, map);
			break;
		}
	}

	m_maps.insert(finalName, mh);

	DEBUG_EMIT("mapAdded");
	emit(mapAdded(mh));

	return mh;
}

void SCHNApps::removeMap(const QString& name)
{
	if (m_maps.contains(name))
	{
		MapHandlerGen* map = m_maps[name];
		foreach(View* view, map->getLinkedViews())
		{
			view->unlinkMap(map);
		}

		m_maps.remove(name);

		DEBUG_EMIT("mapRemoved");
		emit(mapRemoved(map));

		// unselect map if it is removed
		if (this->getSelectedMap() == map)
		{
			setSelectedMap(QString("NONE"));
		}
		delete map;
	}
}

void SCHNApps::setSelectedMap(const QString& mapName)
{
	m_controlMapTab->setSelectedMap(mapName);
}

MapHandlerGen* SCHNApps::getMap(const QString& name) const
{
	if (m_maps.contains(name))
		return m_maps[name];
	else
		return NULL;
}

MapHandlerGen* SCHNApps::getSelectedMap() const
{
	return m_controlMapTab->getSelectedMap();
}

unsigned int SCHNApps::getCurrentOrbit() const
{
	return m_controlMapTab->getCurrentOrbit();
}

CellSelectorGen* SCHNApps::getSelectedSelector(unsigned int orbit) const
{
	return m_controlMapTab->getSelectedSelector(orbit);
}


void SCHNApps::setSelectedSelectorCurrentMap(unsigned int orbit, const QString& name)
{
	QList<QListWidgetItem*> items;
	switch (orbit)
	{
	case DART: items = m_controlMapTab->list_dartSelectors->findItems(name,Qt::MatchExactly); break;
	case VERTEX: items = m_controlMapTab->list_vertexSelectors->findItems(name, Qt::MatchExactly); break;
	case EDGE: items = m_controlMapTab->list_edgeSelectors->findItems(name, Qt::MatchExactly); break;
	case FACE: items = m_controlMapTab->list_faceSelectors->findItems(name, Qt::MatchExactly); break;
	case VOLUME: items = m_controlMapTab->list_volumeSelectors->findItems(name, Qt::MatchExactly); break;
	}
	m_controlMapTab->tabWidget_mapInfo->setCurrentIndex(orbit);
	if (!items.empty())
		items[0]->setSelected(true);
}

/*********************************************************
 * MANAGE TEXTURES
 *********************************************************/

Texture* SCHNApps::getTexture(const QString& image)
{
	if(m_textures.contains(image))
	{
		Texture* t = m_textures[image];
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
			m_textures.insert(image, t);
		}
		return t;
	}
}

void SCHNApps::releaseTexture(const QString& image)
{
	if(m_textures.contains(image))
	{
		Texture* t = m_textures[image];
		t->ref--;
		if(t->ref == 0)
		{
			m_firstView->deleteTexture(m_textures[image]->texID);
			m_textures.remove(image);
			delete t;
		}
	}
}

/*********************************************************
 * MANAGE MENU ACTIONS
 *********************************************************/

void SCHNApps::addMenuAction(Plugin* plugin, const QString& menuPath, QAction* action)
{
	if(action && !menuPath.isEmpty() && !m_pluginMenuActions[plugin].contains(action))
	{
		// extracting all the substring separated by ';'
		QStringList stepNames = menuPath.split(";");
		stepNames.removeAll("");
		unsigned int nbStep = stepNames.count();

		// if only one substring: error + failure
		// No action directly in the menu bar
		if (nbStep >= 1)
		{
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
						QMenu* subMenu = action->menu();
						if (subMenu && subMenu->title() == step)
						{
							lastMenu = subMenu;
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
		}
		// just for update the menu in buggy Qt5 on macOS
		#if (defined CGOGN_APPLE) && ((QT_VERSION>>16) == 5)
		QMenu* fakemenu = menuBar()->addMenu("X");
		delete fakemenu;
		#endif
		m_pluginMenuActions[plugin].append(action);
	}
}

void SCHNApps::removeMenuAction(Plugin* plugin, QAction *action)
{
	if(action)
	{
		action->setEnabled(false);

		m_pluginMenuActions[plugin].removeOne(action);

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





void SCHNApps::aboutSCHNApps()
{
	QString str("SCHNApps:\nS... CGoGN Holder for Nice Applications\n"
	            "Web site: http://cgogn.unistra.fr \n"
	            "Contact information: cgogn@unistra.fr");
	QMessageBox::about(this, "About SCHNApps", str);
}

void SCHNApps::aboutCGoGN()
{
	QString str("CGoGN:\nCombinatorial and Geometric modeling\n"
	            "with Generic N-dimensional Maps\n"
	            "Web site: http://cgogn.unistra.fr \n"
	            "Contact information: cgogn@unistra.fr");
	QMessageBox::about(this, "About CGoGN", str);
}

void SCHNApps::showHideControlDock()
{
	m_controlDock->setVisible(m_controlDock->isHidden());
}

void SCHNApps::showHidePluginDock()
{
	m_pluginDock->setVisible(m_pluginDock->isHidden());
}

void SCHNApps::showHidePythonDock()
{
	m_pythonDock->setVisible(m_pythonDock->isHidden());
}


void SCHNApps::setFloatingControlDock(bool f)
{
	m_controlDock->setFloating(f);
}

void SCHNApps::setFloatingPluginDock(bool f)
{
	m_pluginDock->setFloating(f);
}

void SCHNApps::setFloatingPythonDock(bool f)
{
	if (m_pythonDock->isVisible())
		m_pythonDock->setFloating(f);
}


void SCHNApps::execPythonCmd(const QString& cmd)
{
	m_pythonContext.evalScript(cmd);
}

bool SCHNApps::loadPythonScriptFromFile(const QString& fileName)
{
	QString fullName = fileName;
	if (!QFile::exists(fullName))
	{
		QString pypath = m_pyPathFile;
		if (pypath.isEmpty())
			pypath = this->getAppPath();
		fullName = pypath + "/" + fileName;
		if (!QFile::exists(fullName))
		{
			std::cerr << "python script " << fullName.toStdString() << " not found" << std::endl;
			return false;
		}
	}

	QFile file(fullName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	QTextStream in(&file);
	m_pythonContext.evalScript(in.readAll());

// not using eval file because of pb with non ascii char in file name and path
//	m_pythonContext.evalFile(fullName);

	return true;
}

void SCHNApps::loadPythonScriptFromFileDialog()
{
	QString pypath = m_pyPathFile;
	if (pypath.isEmpty())
		pypath = this->getAppPath();

	QString fileName = QFileDialog::getOpenFileName(this, "Load Python script", pypath, "Python script (*.py)");
	loadPythonScriptFromFile(fileName);
}

QString SCHNApps::noBackSlash(const QString& name)
{
#ifdef WIN32
	// if copy from interface need to replace \ by / !!
	QString slashed = name;
	slashed.replace(QString("\t"), QString("/t"));
	slashed.replace(QString("\n"), QString("/n"));
	slashed.replace(QString("\v"), QString("/v"));
	slashed.replace(QString("\f"), QString("/f"));
	slashed.replace(QString("\r"), QString("/r"));
	slashed.replace(QChar('\\'), QChar('/'));
	return slashed;
#else
	return name;
#endif
}


std::string SCHNApps::niceStdString(const QString& qstr)
{
	return std::string(qstr.toLocal8Bit().data());
}

QString SCHNApps::forceASCII(const QString& qstr)
{
	QString str(qstr);

	for (QString::iterator it = str.begin(); it != str.end(); ++it)
	{
		if (it->unicode() > 127)
			*it = '_';
	}
	return str;
}


void SCHNApps::setPythonPath(const QString& path)
{
	m_pyPathFile = SCHNApps::noBackSlash(path);
}


void SCHNApps::pyRecording()
{
	if (m_pyRecording != NULL) //  WRITE & CLOSE
	{
		QTextStream out(m_pyRecFile);
		foreach(QString var, m_pyVarNames)
		{
			m_pyBuffer.replace("\"" + var + "\"", var + ".getName()");
		}

		out << m_pyBuffer << endl;

		// split view positions
		out << "schnapps.restoreSplitViewPositions(\"" << saveSplitViewPositions() << "\")" << endl;

		// cameras
		foreach(Camera* cam, m_cameras)
		{
			out << "schnapps.getCamera(\"" << cam->getName() << "\").fromString(\"" << cam->toString() << "\")" << endl;
		}

		// save frames of maps
		foreach(MapHandlerGen* mhg, m_maps)
		{
			out << mhg->getName() << ".frameFromString(\"" << mhg->frameToString() << "\")" << endl;
		}

		//windows
		out << "schnapps.setWindowSize(" << this->width() << ", "<< this->height() << ")" << endl;

		// docks
		out << "schnapps.setFloatingControlDock(" << m_controlDock->isFloating() << ")" << endl;
		out << "schnapps.setFloatingPluginDock(" << m_pluginDock->isFloating() << ")" << endl;
		out << "schnapps.setFloatingPythonDock(" << m_pythonDock->isFloating() << ")" << endl;

		m_pyRecFile->close();

		delete m_pyRecording;
		delete m_pyRecFile;
		m_pyRecording = NULL;
		m_pyRecFile = NULL;
		statusbar->showMessage(QString(" Stop recording python"), 2000);
		action_Python_Recording->setText("Python Recording");
		return;
	}

	QString pypath = m_pyPathFile;
	if (pypath.isEmpty())
		pypath = this->getAppPath();

	QString fileName = QFileDialog::getSaveFileName(this, "Save python script", pypath, " python script (*.py)");
	if (fileName.size() != 0)
	{
		if (!fileName.endsWith(".py",Qt::CaseInsensitive))
			fileName += ".py";

		m_pyRecFile = new QFile(fileName);
		if (!m_pyRecFile->open(QIODevice::WriteOnly | QIODevice::Text))
			return;

		m_pyBuffer.clear();
		m_pyRecording = new QTextStream(&m_pyBuffer);
		

		foreach(View *v, m_views)
			*m_pyRecording << v->getName() << " = schnapps.getView(\"" << v->getName() << "\");" << endl;

		foreach(MapHandlerGen *m, m_maps)
			*m_pyRecording << m->getName() << " = schnapps.getMap(\"" << m->getName() << "\");" << endl;

		statusbar->showMessage(QString(" Recording python in ") + fileName);
		pythonVarsClear();
		action_Python_Recording->setText("Stop Python Recording");
	}
	else
	{
		m_pyRecFile = NULL;
		m_pyRecording = NULL;
		statusbar->showMessage(QString(" Cancel recording python"), 2000);
	}
}


void SCHNApps::appendPyRecording()
{
	if (m_pyRecording != NULL)	//  WRITE & CLOSE
	{
		QTextStream out(m_pyRecFile);
		foreach(QString var, m_pyVarNames)
		{
			m_pyBuffer.replace("\"" + var + "\"", var + ".getName()");
		}
		out << m_pyBuffer << endl;

		// split view positions
		out << "schnapps.restoreSplitViewPositions(\"" << saveSplitViewPositions() << "\")" << endl;

		// cameras
		foreach(Camera* cam, m_cameras)
		{
			out << "schnapps.getCamera(\"" << cam->getName() << "\").fromString(\"" << cam->toString() << "\")" << endl;
		}

		// save frames of maps
		foreach(MapHandlerGen* mhg, m_maps)
		{
			out << mhg->getName() << ".frameFromString(\"" << mhg->frameToString() << "\")" << endl;
		}


		//windows
		out << "schnapps.setWindowSize(" << this->width() << ", " << this->height() << ")" << endl;

		m_pyRecFile->close();

		delete m_pyRecording;
		delete m_pyRecFile;
		m_pyRecording = NULL;
		m_pyRecFile = NULL;
		statusbar->showMessage(QString(" Stop recording python"), 2000);
		action_Append_Python_Recording->setText("Append Python Recording");
		return;
	}

	QString pypath = m_pyPathFile;
	if (pypath.isEmpty())
		pypath = this->getAppPath();

	QString fileName = QFileDialog::getSaveFileName(this, "Append python script", pypath, " python script (*.py)");
	if (fileName.size() != 0)
	{
		if (!fileName.endsWith(".py",Qt::CaseInsensitive))
			fileName += ".py";

		m_pyRecFile = new QFile(fileName);
		if (!m_pyRecFile->open(QIODevice::Append | QIODevice::Text))
			return;	

		m_pyBuffer.clear();
		m_pyRecording = new QTextStream(&m_pyBuffer);
		pythonVarsClear();

		foreach(View *v, m_views)
			*m_pyRecording << v->getName() << " = schnapps.getView(\"" << v->getName() << "\");" << endl;

		foreach(MapHandlerGen *m, m_maps)
		{
			*m_pyRecording << m->getName() << " = schnapps.getMap(\"" << m->getName() << "\");  #replace by  = previous_var if appending" << endl;
			pythonVarDeclare(m->getName());
		}

		statusbar->showMessage(QString(" Append recording python in ") + fileName);
		action_Append_Python_Recording->setText("Stop Append Python Recording");
	}
	else
	{
		m_pyRecFile = NULL;
		m_pyRecording = NULL;
		statusbar->showMessage(QString(" Cancel recording python"), 2000);
	}
}

bool SCHNApps::execPythonShortcut(quint64 key)
{
	if (m_pythonShortCuts.contains(key))
	{
		execPythonCmd(m_pythonShortCuts[key]);
		return true;
	}
	return false;
}


void SCHNApps::setPythonShortcut(const QString& keys, const QString& command)
{
	quint64 k = 0;

	if (keys.length() == 0)
		return;

	if (keys.length() == 1)
	{
		k = keys[0].toUpper().unicode();
	}
	else
	{
		if (keys.contains("control", Qt::CaseInsensitive))
			k |= Qt::ControlModifier;
		if (keys.contains("shift", Qt::CaseInsensitive))
			k |= Qt::ShiftModifier;
		if (keys.contains("alt", Qt::CaseInsensitive))
			k |= Qt::AltModifier;
		if (keys.contains("meta", Qt::CaseInsensitive))
			k |= Qt::MetaModifier;
		if (keys.contains("keypad", Qt::CaseInsensitive))
			k |= Qt::KeypadModifier;
		k <<= 32;

		QChar c = keys[keys.length() - 1].toUpper();
		k |= c.unicode();
	}

	if (command.length() > 0)
		m_pythonShortCuts[k] = command;
	else
		m_pythonShortCuts.erase(m_pythonShortCuts.find(k));
}


void SCHNApps::closeEvent(QCloseEvent *event)
{
	DEBUG_EMIT("schnappsClosing");
	if (m_pyRecording != NULL)	//  WRITE & CLOSE
	{
		pyRecording();
	}
	emit(schnappsClosing());
	QMainWindow::closeEvent(event);
}

void SCHNApps::statusBarMessage(const QString& msg, int msec)
{
	statusbar->showMessage(msg, msec);
}


QString SCHNApps::openFileDialog(const QString& title, const QString& dir, const QString& filter)
{
	return QFileDialog::getOpenFileName(this, title, dir, filter);
}


QString SCHNApps::saveFileDialog(const QString& title, const QString& dir, const QString& filter)
{
	return QFileDialog::getSaveFileName(this, title, dir, filter);
}


void SCHNApps::cleanAll()
{
	//remove views
	foreach(View* v, m_views)
	{
		removeView(v->getName());
	}

	//  set the camera to camera_0 
	m_firstView->setCurrentCamera(QString("camera_0"));

	// change name of first view to view 0 if needed
	if (m_firstView->m_name != QString("view_0"))
	{
		m_views.remove(m_firstView->m_name);
		m_firstView->m_name = QString("view_0");
		m_views.insert(m_firstView->m_name, m_firstView);
	}

	//remove cameras
	foreach(Camera* cam, m_cameras)
	{
		removeCamera(cam->getName());
	}

	//remove maps
	foreach(MapHandlerGen* m, m_maps)
	{
		removeMap(m->getName());
	}

	// remove plugins
	foreach(Plugin* p, m_plugins)
	{
		disablePlugin(p->getName());
	}

	Camera::cameraCount = 1;
	View::viewCount = 1;

	m_pluginDock->setVisible(false);
}


} // namespace SCHNApps

} // namespace CGoGN
