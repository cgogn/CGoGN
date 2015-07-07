#include "view.h"

#include "camera.h"
#include "viewButtonArea.h"
#include "plugin_interaction.h"
#include "mapHandler.h"

#include "Utils/GLSLShader.h"
#include "Algo/Geometry/boundingbox.h"

#include "slot_debug.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMessageBox>
#include "Utils/Qt/qtInputs.h"

namespace CGoGN
{

namespace SCHNApps
{

unsigned int View::viewCount = 0;

View::View(const QString& name, SCHNApps* s, const QGLWidget* shareWidget) :
	QGLViewer( NULL, shareWidget),
	b_updatingUI(false),
	m_name(name),
	m_schnapps(s),
	m_currentCamera(NULL),
	m_bbMin(0.0, 0.0, 0.0),
	m_bbMax(0.0, 0.0, 0.0),
	m_buttonArea(NULL),
	m_closeButton(NULL),
	m_VsplitButton(NULL),
	m_HsplitButton(NULL),
	m_buttonAreaLeft(NULL),
	m_mapsButton(NULL),
	m_pluginsButton(NULL),
	m_camerasButton(NULL),
	m_dialogMaps(NULL),
	m_dialogPlugins(NULL),
	m_dialogCameras(NULL),
	m_frameDrawer(NULL),
	m_textureWallpaper(NULL),
	m_shaderWallpaper(NULL),
	b_saveSnapshots(false)
{
	++viewCount;

	setSnapshotFormat("BMP");
	setSnapshotFileName(m_name);
	setSnapshotQuality(100);

	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*,MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*,MapHandlerGen*)));

	m_dialogMaps = new ListPopUp("Linked Maps");
	m_dialogPlugins = new ListPopUp("Enabled Plugins");
	m_dialogCameras = new ListPopUp("Cameras");

	connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));
	connect(m_dialogMaps->list(), SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(mapCheckStateChanged(QListWidgetItem*)));

	foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
		mapAdded(map);

	connect(m_schnapps, SIGNAL(pluginEnabled(Plugin*)), this, SLOT(pluginEnabled(Plugin*)));
	connect(m_schnapps, SIGNAL(pluginDisabled(Plugin*)), this, SLOT(pluginDisabled(Plugin*)));
	connect(m_dialogPlugins->list(), SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(pluginCheckStateChanged(QListWidgetItem*)));

	foreach(Plugin* plug, m_schnapps->getPluginSet().values())
		pluginEnabled(plug);

	connect(m_schnapps, SIGNAL(cameraAdded(Camera*)), this, SLOT(cameraAdded(Camera*)));
	connect(m_schnapps, SIGNAL(cameraRemoved(Camera*)), this, SLOT(cameraRemoved(Camera*)));
	connect(m_dialogCameras->list(), SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(cameraCheckStateChanged(QListWidgetItem*)));

	foreach(Camera* cam, m_schnapps->getCameraSet().values())
		cameraAdded(cam);

	m_currentCamera = m_schnapps->addCamera();
	m_currentCamera->linkView(this);
	m_dialogCameras->check(m_currentCamera->getName(),Qt::Checked);

	connect(m_schnapps, SIGNAL(schnappsClosing()), this, SLOT(closeDialogs()));
}

View::~View()
{
	qglviewer::Camera* c = new qglviewer::Camera();
	this->setCamera(c);
	m_currentCamera->unlinkView(this);

	foreach(PluginInteraction* p, l_plugins)
		unlinkPlugin(p);

	foreach(MapHandlerGen* m, l_maps)
		unlinkMap(m);

	delete m_buttonArea;
	delete m_buttonAreaLeft;

	delete m_dialogMaps;
	delete m_dialogPlugins;
	delete m_dialogCameras;
}

void View::setCurrentCamera(Camera* c)
{
//	DEBUG_SLOT();
	if(c != m_currentCamera && c)
	{
		// RECORDING
		QTextStream* rec = m_schnapps->pythonStreamRecorder();
		if (rec)
			*rec << this->getName() << ".setCurrentCamera(\"" << c->getName() << "\");" << endl;

		Camera* prev = m_currentCamera;
		if(prev)
			prev->unlinkView(this);

		m_currentCamera = c;
		this->setCamera(m_currentCamera);
		m_currentCamera->linkView(this);

		DEBUG_EMIT("currentCameraChanged");
		emit(currentCameraChanged(prev, c));

		if (prev)
		{
			QListWidgetItem* prevItem = m_dialogCameras->findItem(prev->getName());
			if(prevItem)
			{
				b_updatingUI = true;
				prevItem->setCheckState(Qt::Unchecked);
				b_updatingUI = false;
			}
		}

		if (m_currentCamera)
		{
			QListWidgetItem* curItem = m_dialogCameras->findItem(m_currentCamera->getName());
			if(curItem)
			{
				b_updatingUI = true;
				curItem->setCheckState(Qt::Checked);
				b_updatingUI = false;
			}
		}

		m_currentCamera->fitToViewsBoundingBox();
		updateGL();
	}
}

void View::setCurrentCamera(const QString& name)
{	
	Camera* c = m_schnapps->getCamera(name);
	if(c)
		setCurrentCamera(c);
}

bool View::usesCamera(const QString& name) const
{
	Camera* c = m_schnapps->getCamera(name);
	return usesCamera(c);
}



void View::linkPlugin(PluginInteraction* plugin)
{
	// RECORDING
	QTextStream* rec = m_schnapps->pythonStreamRecorder();
	if (rec)
		*rec << this->getName() << ".linkPlugin(\"" << plugin->getName() << "\");" << endl;

	DEBUG_SLOT();
	if(plugin && !l_plugins.contains(plugin))
	{
		l_plugins.push_back(plugin);
		plugin->linkView(this);

		DEBUG_EMIT("pluginLinked");
		emit(pluginLinked(plugin));

		b_updatingUI = true;
		m_dialogPlugins->check(plugin->getName(), Qt::Checked);
		b_updatingUI = false;

		updateGL();
	}
}

void View::linkPlugin(const QString& name)
{
	PluginInteraction* p = dynamic_cast<PluginInteraction*>(m_schnapps->getPlugin(name));
	if(p)
		linkPlugin(p);
}

void View::unlinkPlugin(PluginInteraction* plugin)
{
	DEBUG_SLOT();
	if(l_plugins.removeOne(plugin))
	{
		// RECORDING
		QTextStream* rec = m_schnapps->pythonStreamRecorder();
		if (rec)
			*rec << this->getName() << ".unlinkPlugin(\"" << plugin->getName() << "\");" << endl;


		plugin->unlinkView(this);

		DEBUG_EMIT("pluginUnlinked");
		emit(pluginUnlinked(plugin));

		b_updatingUI = true;
		m_dialogPlugins->check(plugin->getName(), Qt::Unchecked);
		b_updatingUI = false;

		updateGL();
	}
}

void View::unlinkPlugin(const QString& name)
{
	PluginInteraction* p = dynamic_cast<PluginInteraction*>(m_schnapps->getPlugin(name));
	if(p)
		unlinkPlugin(p);
}

bool View::isLinkedToPlugin(const QString& name) const
{
	PluginInteraction* p = dynamic_cast<PluginInteraction*>(m_schnapps->getPlugin(name));
	return l_plugins.contains(p);
}

void View::linkMap(MapHandlerGen* map)
{
	// RECORDING
	QTextStream* rec = m_schnapps->pythonStreamRecorder();
	if (rec)
		*rec << this->getName() << ".linkMap(\"" << map->getName() << "\");" << endl;

	DEBUG_SLOT();
	if(map && !l_maps.contains(map))
	{
		l_maps.push_back(map);
		map->linkView(this);

		DEBUG_EMIT("mapLinked");
		emit(mapLinked(map));

		connect(map, SIGNAL(selectedCellsChanged(CellSelectorGen*)), this, SLOT(updateGL()));
		connect(map, SIGNAL(boundingBoxModified()), this, SLOT(updateBoundingBox()));

		if(map->isSelectedMap())
			setManipulatedFrame(map->getFrame());

		updateBoundingBox();

		b_updatingUI = true;
		m_dialogMaps->check(map->getName(), Qt::Checked);
		b_updatingUI = false;
	}
}

void View::linkMap(const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(name);
	if(m)
		linkMap(m);
}

void View::unlinkMap(MapHandlerGen* map)
{
	DEBUG_SLOT();
	if(l_maps.removeOne(map))
	{
		QTextStream* rec = m_schnapps->pythonStreamRecorder();
		if (rec)
			*rec << this->getName() << ".unlinkMap(\"" << map->getName() << "\");" << endl;


		map->unlinkView(this);

		DEBUG_EMIT("mapUnlinked");
		emit(mapUnlinked(map));

		disconnect(map, SIGNAL(selectedCellsChanged(CellSelectorGen*)), this, SLOT(updateGL()));
		disconnect(map, SIGNAL(boundingBoxModified()), this, SLOT(updateBoundingBox()));

		if(map == m_schnapps->getSelectedMap())
			setManipulatedFrame(NULL);

		updateBoundingBox();

		b_updatingUI = true;
		m_dialogMaps->check(map->getName(), Qt::Unchecked);
		b_updatingUI = false;
	}
}

void View::unlinkMap(const QString& name)
{
	MapHandlerGen* m = m_schnapps->getMap(name);
	if(m)
		unlinkMap(m);
}

bool View::isLinkedToMap(const QString& name) const
{
	MapHandlerGen* m = m_schnapps->getMap(name);
	return l_maps.contains(m);
}





void View::init()
{
	glewExperimental = GL_TRUE; // needed for GL Core Profile 3.3
	glewInit();

	std::cout << "using " << glGetString(GL_VERSION) << std::endl;

	qglviewer::Camera* c = this->camera();
	this->setCamera(m_currentCamera);
//	delete c;

	this->setBackgroundColor(QColor(0,0,0));
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	m_buttonArea = new ViewButtonArea(this);
	m_buttonArea->setTopRightPosition(this->width(), 0);

	m_VsplitButton = new ViewButton(":icons/icons/Vsplit.png", this);
	m_buttonArea->addButton(m_VsplitButton);
	connect(m_VsplitButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(ui_verticalSplitView(int, int, int, int)));

	m_HsplitButton = new ViewButton(":icons/icons/Hsplit.png", this);
	m_buttonArea->addButton(m_HsplitButton);
	connect(m_HsplitButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(ui_horizontalSplitView(int, int, int, int)));

	m_closeButton = new ViewButton(":icons/icons/close.png", this);
	m_buttonArea->addButton(m_closeButton);
	connect(m_closeButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(ui_closeView(int, int, int, int)));

	m_buttonAreaLeft = new ViewButtonArea(this);
	m_buttonAreaLeft->setTopLeftPosition(0, 0);

	m_mapsButton = new ViewButton(":icons/icons/maps.png", this);
	m_buttonAreaLeft->addButton(m_mapsButton);
	connect(m_mapsButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(ui_mapsListView(int, int, int, int)));

	m_pluginsButton = new ViewButton(":icons/icons/plugins.png", this);
	m_buttonAreaLeft->addButton(m_pluginsButton);
	connect(m_pluginsButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(ui_pluginsListView(int, int, int, int)));

	m_camerasButton = new ViewButton(":icons/icons/cameras.png", this);
	m_buttonAreaLeft->addButton(m_camerasButton);
	connect(m_camerasButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(ui_camerasListView(int, int, int, int)));

	// FRAME DRAWER

	m_frameDrawer = new Utils::Drawer(1);
	glm::mat4 mm(1.0);
	glm::mat4 pm(1.0);
//	m_frameDrawer->getShader()->updateMatrices(mm, pm);
	m_frameDrawer->updateMatrices(mm, pm);

	m_frameDrawer->newList(GL_COMPILE);
	m_frameDrawer->color3f(0.0f,1.0f,0.0f);
	m_frameDrawer->lineWidth(4.0f);
	m_frameDrawer->begin(GL_LINE_LOOP);
	m_frameDrawer->vertex3f(-1.0f,-1.0f, 0.0f);
	m_frameDrawer->vertex3f( 1.0f,-1.0f, 0.0f);
	m_frameDrawer->vertex3f( 1.0f, 1.0f, 0.0f);
	m_frameDrawer->vertex3f(-1.0f, 1.0f, 0.0f);
	m_frameDrawer->end();
	m_frameDrawer->endList();

	// WALLPAPER

	m_textureWallpaper = new Utils::Texture<2, Geom::Vec3uc>(GL_UNSIGNED_BYTE);
	m_textureWallpaper->create(Geom::Vec2ui(16, 16));
	const float max = m_textureWallpaper->size()[0];
	for (unsigned int ki = 0; ki < m_textureWallpaper->size()[0]; ++ki)
	{
		for (unsigned int kj = 0; kj < m_textureWallpaper->size()[1]; ++kj)
		{
			float col = 255 - (255 * ((kj/max)*0.6 + 0.2));
			(*m_textureWallpaper)(ki, kj) = Geom::Vec3f(col, col, col);
		}
	}
	m_textureWallpaper->update();
	m_textureWallpaper->setWrapping(GL_CLAMP_TO_EDGE);

	m_shaderWallpaper = new Utils::ShaderWallPaper();
	m_shaderWallpaper->setTextureUnit(GL_TEXTURE0);
	m_shaderWallpaper->setTexture(m_textureWallpaper);

	this->setBackgroundColor(QColor(0,0,0));
}

void View::preDraw()
{
	if (Utils::GLSLShader::CURRENT_OGL_VERSION >= 3)
		makeCurrent();
	m_currentCamera->setScreenWidthAndHeight(width(), height());

	QGLViewer::preDraw();
}

void View::draw()
{
	if (Utils::GLSLShader::CURRENT_OGL_VERSION >= 3)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	}

	m_shaderWallpaper->draw();

	const CameraSet& cams = m_schnapps->getCameraSet();
	QList<Camera*> lc = cams.values();
	foreach (Camera* camera, lc)
	{
		if (camera != m_currentCamera)
		{
			if (camera->getDraw()) camera->draw();
			if (camera->getDrawPath()) camera->drawAllPaths();
		}
	}

	glm::mat4 mm = getCurrentModelViewMatrix();
	glm::mat4 pm = getCurrentProjectionMatrix();

	MapHandlerGen* selectedMap = m_schnapps->getSelectedMap();

	foreach(MapHandlerGen* map, l_maps)
	{
		glm::mat4 map_mm = mm * map->getFrameMatrix() * map->getTransfoMatrix();

		if(map == selectedMap)
		{
			Utils::Drawer* bbDr = map->getBBDrawer();
			if(bbDr)
				bbDr->updateMatrices(pm, map_mm);
			map->drawBB();
		}

		foreach(PluginInteraction* plugin, l_plugins)
		{
			foreach(Utils::GLSLShader* shader, plugin->getShaders())
				shader->updateMatrices(pm, map_mm);
			plugin->drawMap(this, map);
		}
	}

	foreach(PluginInteraction* plugin, l_plugins)
		plugin->draw(this);
}

void View::postDraw()
{
	if(isSelectedView())
		drawFrame();
	drawButtons();
	QGLViewer::postDraw();
}

void View::resizeGL(int width, int height)
{
	QGLViewer::resizeGL(width, height);

	if(m_buttonArea)
		m_buttonArea->setTopRightPosition(width/ this->pixelRatio(), 0);

	if(m_buttonAreaLeft)
		m_buttonAreaLeft->setTopLeftPosition(0, 0);
}

void View::drawButtons()
{
	m_buttonArea->draw();
	m_buttonAreaLeft->draw();
}

void View::drawFrame()
{
	glDisable(GL_DEPTH_TEST);
	m_frameDrawer->callList();
	glEnable(GL_DEPTH_TEST);
}


void View::keyPressEvent(QKeyEvent* event)
{
	quint64 k = event->modifiers();
	k <<= 32;
	k |= event->key();

	// exec python shortcuts if exist and stop propagation of event !
	if (m_schnapps->execPythonShortcut(k))
		return;

	switch (event->key())
	{
		case Qt::Key_S:
		{
			b_saveSnapshots = !b_saveSnapshots;

			if (b_saveSnapshots)
			{
				QMessageBox msgBox;
				msgBox.setText("Snapshot every frame?");
				msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
				msgBox.setDefaultButton(QMessageBox::Ok);
				if (msgBox.exec() == QMessageBox::Ok)
				{
					m_schnapps->statusBarMessage("frame snapshot !!", 2000);
					connect(this, SIGNAL(drawFinished(bool)), this, SLOT(saveSnapshot(bool)));
				}
				else
				{
					m_schnapps->statusBarMessage("cancel frame snapshot", 2000);
					b_saveSnapshots = false;
				}
			}
			else
			{
				disconnect(this, SIGNAL(drawFinished(bool)), this, SLOT(saveSnapshot(bool)));
				m_schnapps->statusBarMessage("Stop frame snapshot", 2000);
			}

		}
		break;

		default:
			foreach(PluginInteraction* plugin, l_plugins)
				plugin->keyPress(this, event);

			if (event->key() == Qt::Key_Escape)
			{
				QMessageBox msgBox;
				msgBox.setText("Really quit SCHNApps ?");
				msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
				msgBox.setDefaultButton(QMessageBox::Ok);
				if (msgBox.exec() == QMessageBox::Ok)
					m_schnapps->close();
			}
			else
				QGLViewer::keyPressEvent(event);
	}

	//if (event->key() == Qt::Key_X)
	//{
	//	int isX = scaleIntSlideVal(m_scaleView[0][0]);
	//	int isY = scaleIntSlideVal(m_scaleView[1][1]);
	//	int isZ = scaleIntSlideVal(m_scaleView[2][2]);

	//	Utils::QT::inputValues(
	//		Utils::QT::VarSlider(0, 100, isX, "Scale X",
	//		Utils::QT::VarSlider(0, 100, isY, "Scale Y",
	//		Utils::QT::VarSlider(0, 100, isZ, "Scale Z"))));

	//	float sx = scaleRealSlideVal(isX);
	//	float sy = scaleRealSlideVal(isY);
	//	float sz = scaleRealSlideVal(isZ);

	//	setViewScaling(sx,sy,sz);

	//	QString msg = QString("Sx=") + QString::number(m_scaleView[0][0]) + QString(" / Sy=") + QString::number(m_scaleView[1][1]) + QString(" / Sz=") + QString::number(m_scaleView[2][2]);
	//	m_schnapps->statusBar()->showMessage(msg, 2000);

	//	QTextStream* rec = m_schnapps->pythonStreamRecorder();
	//	if (rec)
	//		*rec << this->getName() << ".setViewScaling(" << sx << ", "<< sy << ", "<< sz << ");" << endl;
	//}
	//else
	//if (event->key() == Qt::Key_S)
	//{
	//	b_saveSnapshots = !b_saveSnapshots;

	//	if (b_saveSnapshots)
	//	{
	//		QMessageBox msgBox;
	//		msgBox.setText("Snapshot every frame?");
	//		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	//		msgBox.setDefaultButton(QMessageBox::Ok);
	//		if (msgBox.exec() == QMessageBox::Ok)
	//		{
	//			m_schnapps->statusBarMessage("frame snapshot !!", 2000);
	//			connect(this, SIGNAL(drawFinished(bool)), this, SLOT(saveSnapshot(bool)));
	//		}
	//		else
	//		{
	//			m_schnapps->statusBarMessage("cancel frame snapshot", 2000);
	//			b_saveSnapshots = false;
	//		}
	//	}
	//	else
	//	{
	//		disconnect(this, SIGNAL(drawFinished(bool)), this, SLOT(saveSnapshot(bool)));
	//		m_schnapps->statusBarMessage("Stop frame snapshot", 2000);
	//	}
	//		
	//}
	//else
	//{
	//	foreach(PluginInteraction* plugin, l_plugins)
	//		plugin->keyPress(this, event);

	//	if (event->key() == Qt::Key_Escape)
	//	{
	//		QMessageBox msgBox;
	//		msgBox.setText("Really quit SCHNApps ?");
	//		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	//		msgBox.setDefaultButton(QMessageBox::Ok);
	//		if (msgBox.exec() == QMessageBox::Ok)
	//			m_schnapps->close();
	//	}
	//	else
	//		QGLViewer::keyPressEvent(event);
	//}
}

void View::keyReleaseEvent(QKeyEvent *event)
{
	foreach(PluginInteraction* plugin, l_plugins)
		plugin->keyRelease(this, event);
	QGLViewer::keyReleaseEvent(event);
}

void View::mousePressEvent(QMouseEvent* event)
{
	if (!isSelectedView())
	{
		m_schnapps->setSelectedView(this);
		m_schnapps->statusBarMessage(QString("Selecting ") + this->getName(), 2000);
	}
	else if (event->y() < 20)
		m_schnapps->statusBarMessage(this->getName(), 2000);

	if (m_buttonAreaLeft->isClicked(event->x(), event->y()))
		m_buttonAreaLeft->clickButton(event->x(), event->y(), event->globalX(), event->globalY());
	else
	{
		hideDialogs();
		if (m_buttonArea->isClicked(event->x(), event->y()))
			m_buttonArea->clickButton(event->x(), event->y(), event->globalX(), event->globalY());
		else
		{
			foreach(PluginInteraction* plugin, l_plugins)
				plugin->mousePress(this, event);

			QGLViewer::mousePressEvent(event);
		}
	}
}

void View::mouseReleaseEvent(QMouseEvent* event)
{
	foreach(PluginInteraction* plugin, l_plugins)
		plugin->mouseRelease(this, event);
	QGLViewer::mouseReleaseEvent(event);
}

void View::mouseMoveEvent(QMouseEvent* event)
{
	foreach(PluginInteraction* plugin, l_plugins)
		plugin->mouseMove(this, event);
	QGLViewer::mouseMoveEvent(event);
}

void View::wheelEvent(QWheelEvent* event)
{
	foreach(PluginInteraction* plugin, l_plugins)
		plugin->wheelEvent(this, event);
	QGLViewer::wheelEvent(event);
}

glm::mat4 View::getCurrentModelViewMatrix() const
{
	GLdouble gl_mvm[16];
	camera()->getModelViewMatrix(gl_mvm);
	glm::mat4 mvm;
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			mvm[i][j] = (float)gl_mvm[i*4+j];
	}
	return mvm;
}

glm::mat4 View::getCurrentProjectionMatrix() const
{
	GLdouble gl_pm[16];
	camera()->getProjectionMatrix(gl_pm);
	glm::mat4 pm;
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			pm[i][j] = (float)gl_pm[i*4+j];
	}
	return pm;
}

glm::mat4 View::getCurrentModelViewProjectionMatrix() const
{
	GLdouble gl_mvpm[16];
	camera()->getModelViewProjectionMatrix(gl_mvpm);
	glm::mat4 mvpm;
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			mvpm[i][j] = (float)gl_mvpm[i*4+j];
	}
	return mvpm;
}



void View::closeDialogs()
{
	m_dialogMaps->close();
	m_dialogPlugins->close();
	m_dialogCameras->close();
}

void View::hideDialogs()
{
	if (m_dialogCameras->isVisible())
		m_dialogCameras->hide();
	if (m_dialogMaps->isVisible())
		m_dialogMaps->hide();
	if (m_dialogPlugins->isVisible())
		m_dialogPlugins->hide();
}


void View::selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur)
{
	DEBUG_SLOT();
	if(cur && isLinkedToMap(cur))
		setManipulatedFrame(cur->getFrame());
	updateGL();
}

void View::ui_verticalSplitView(int x, int y, int /*globalX*/, int /*globalY*/)
{
	m_schnapps->splitView(m_name, Qt::Horizontal);
}

void View::ui_horizontalSplitView(int x, int y, int /*globalX*/, int /*globalY*/)
{
	m_schnapps->splitView(m_name, Qt::Vertical);
}

void View::ui_closeView(int x, int y, int /*globalX*/, int /*globalY*/)
{
	m_schnapps->removeView(m_name);
}

void View::ui_mapsListView(int x, int y, int globalX, int globalY)
{
	if (m_dialogMaps->isHidden())
	{
		m_dialogMaps->show();
		m_dialogMaps->move(QPoint(globalX,globalY+8));
		m_dialogCameras->hide();
		m_dialogPlugins->hide();
	}
	else
	{
		m_dialogMaps->hide();
	}

}

void View::ui_pluginsListView(int x, int y, int globalX, int globalY)
{
	if (m_dialogPlugins->isHidden())
	{
		m_dialogPlugins->show();
		m_dialogPlugins->move(QPoint(globalX,globalY+8));
		m_dialogMaps->hide();
		m_dialogCameras->hide();
	}
	else
	{
		m_dialogPlugins->hide();
	}
}

void View::ui_camerasListView(int x, int y, int globalX, int globalY)
{
	if (m_dialogCameras->isHidden())
	{
		m_dialogCameras->show();
		m_dialogCameras->move(QPoint(globalX,globalY+8));
		m_dialogPlugins->hide();
		m_dialogMaps->hide();
	}
	else
	{
		m_dialogCameras->hide();
	}
}

void View::mapAdded(MapHandlerGen* mh)
{
	DEBUG_SLOT();
	if (mh)
		m_dialogMaps->addItem(mh->getName());
}

void View::mapRemoved(MapHandlerGen* mh)
{
	DEBUG_SLOT();
	if (mh)
		m_dialogMaps->removeItem(mh->getName());
}

void View::mapCheckStateChanged(QListWidgetItem* item)
{
	if (!b_updatingUI)
	{
		DEBUG_SLOT();
		if (item->checkState() == Qt::Checked)
			linkMap(item->text());
		else
			unlinkMap(item->text());
	}
}

void View::pluginEnabled(Plugin *plugin)
{
	DEBUG_SLOT();
	if (dynamic_cast<PluginInteraction*>(plugin))
		m_dialogPlugins->addItem(plugin->getName());
}

void View::pluginDisabled(Plugin *plugin)
{
	DEBUG_SLOT();
	if (dynamic_cast<PluginInteraction*>(plugin))
		m_dialogPlugins->removeItem(plugin->getName());
}

void View::pluginCheckStateChanged(QListWidgetItem* item)
{
	if (!b_updatingUI)
	{
		DEBUG_SLOT();
		if (item->checkState() == Qt::Checked)
			linkPlugin(item->text());
		else
			unlinkPlugin(item->text());
	}
}

void View::cameraAdded(Camera* camera)
{
	DEBUG_SLOT();
	if (camera)
		m_dialogCameras->addItem(camera->getName());
}

void View::cameraRemoved(Camera* camera)
{
	DEBUG_SLOT();
	if (camera)
		m_dialogCameras->removeItem(camera->getName());
}

void View::cameraCheckStateChanged(QListWidgetItem* item)
{
	if (!b_updatingUI)
	{
		DEBUG_SLOT();
		if (item->checkState() == Qt::Checked)
			setCurrentCamera(item->text());
	}
}

void View::updateBoundingBox()
{
	if (!l_maps.empty())
	{
		bool initialized = false;

		foreach (MapHandlerGen* mhg, l_maps)
		{
			qglviewer::Vec minbb;
			qglviewer::Vec maxbb;
			if (mhg->transformedBB(minbb, maxbb))
			{
				if (initialized)
				{
					for (unsigned int dim = 0; dim < 3; ++dim)
					{
						if (minbb[dim] < m_bbMin[dim])
							m_bbMin[dim] = minbb[dim];
						if (maxbb[dim] > m_bbMax[dim])
							m_bbMax[dim] = maxbb[dim];
					}
				}
				else
				{
					for (unsigned int dim = 0; dim < 3; ++dim)
					{
						m_bbMin[dim] = minbb[dim];
						m_bbMax[dim] = maxbb[dim];
					}
					initialized = true;
				}
			}
		}

		if (!initialized)
		{
			m_bbMin.setValue(0, 0, 0);
			m_bbMax.setValue(0, 0, 0);
		}
	}
	else
	{
		m_bbMin.setValue(0, 0, 0);
		m_bbMax.setValue(0, 0, 0);
	}

	DEBUG_EMIT("View::boundingBoxChanged");
	emit(boundingBoxChanged());
}

} // namespace SCHNApps

} // namespace CGoGN
