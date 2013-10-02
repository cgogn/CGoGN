#include "view.h"

#include "camera.h"
#include "viewButtonArea.h"
#include "plugin_interaction.h"
#include "mapHandler.h"

#include "Utils/GLSLShader.h"
#include "Algo/Geometry/boundingbox.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

namespace CGoGN
{

namespace SCHNApps
{

unsigned int View::viewCount = 0;

View::View(const QString& name, SCHNApps* s, const QGLWidget* shareWidget) :
	QGLViewer(NULL, shareWidget),
	m_name(name),
	m_schnapps(s),
	m_currentCamera(NULL),
	m_buttonArea(NULL),
	m_closeButton(NULL),
	m_VsplitButton(NULL),
	m_HsplitButton(NULL)
{
	++viewCount;

	m_currentCamera = m_schnapps->addCamera();

	connect(m_schnapps, SIGNAL(selectedMapChanged(MapHandlerGen*,MapHandlerGen*)), this, SLOT(selectedMapChanged(MapHandlerGen*,MapHandlerGen*)));
	connect(m_schnapps, SIGNAL(selectedCellSelectorChanged(CellSelectorGen*)), this, SLOT(updateGL()));
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
}

void View::setCurrentCamera(Camera* c)
{
	if(c != m_currentCamera && c != NULL)
	{
		Camera* prev = m_currentCamera;
		if(prev)
			prev->unlinkView(this);

		m_currentCamera = c;
		this->setCamera(m_currentCamera);
		m_currentCamera->linkView(this);

		emit(currentCameraChanged(prev, c));

		updateCurrentCameraBB();
		updateGL();
	}
}

void View::setCurrentCamera(const QString& name)
{
	Camera* c = m_schnapps->getCamera(name);
	if(c)
		setCurrentCamera(c);
}

bool View::usesCamera(const QString& cameraName) const
{
	return m_currentCamera->getName() == cameraName;
}

void View::linkPlugin(PluginInteraction* plugin)
{
	if(plugin && !l_plugins.contains(plugin))
	{
		l_plugins.push_back(plugin);
		plugin->linkView(this);
		emit(pluginLinked(plugin));
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
	if(l_plugins.removeOne(plugin))
	{
		plugin->unlinkView(this);
		emit(pluginUnlinked(plugin));
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
	if(map && !l_maps.contains(map))
	{
		l_maps.push_back(map);
		map->linkView(this);
		emit(mapLinked(map));

		updateCurrentCameraBB();
		updateGL();

		connect(map->getFrame(), SIGNAL(modified()), this, SLOT(updateGL()));
		connect(map, SIGNAL(selectedCellsChanged(CellSelectorGen*)), this, SLOT(updateGL()));

		if(map->isSelectedMap())
			setManipulatedFrame(map->getFrame());
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
	if(l_maps.removeOne(map))
	{
		map->unlinkView(this);
		emit(mapUnlinked(map));

		updateCurrentCameraBB();
		updateGL();

		disconnect(map->getFrame(), SIGNAL(modified()), this, SLOT(updateGL()));
		disconnect(map, SIGNAL(selectedCellsChanged(CellSelectorGen*)), this, SLOT(updateGL()));

		if(map == m_schnapps->getSelectedMap())
			setManipulatedFrame(NULL);
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

	qglviewer::Camera* c = this->camera();
	this->setCamera(m_currentCamera);
	delete c;

	this->setBackgroundColor(QColor(0,0,0));
}

void View::preDraw()
{
	m_currentCamera->setScreenWidthAndHeight(width(), height());

	QGLViewer::preDraw();
}

void View::draw()
{
	foreach(Camera* camera, m_schnapps->getCameraSet().values())
	{
		if(camera != m_currentCamera)
		{
			if(camera->getDraw()) camera->draw();
			if(camera->getDrawPath()) camera->drawAllPaths();
		}
	}

	glm::mat4 mm = getCurrentModelViewMatrix();
	glm::mat4 pm = getCurrentProjectionMatrix();

	MapHandlerGen* selectedMap = m_schnapps->getSelectedMap();

	foreach(MapHandlerGen* map, l_maps)
	{
		glm::mat4 map_mm = mm * map->getFrameMatrix();

		if(map == selectedMap)
		{
			Utils::GLSLShader* bbShader = map->getBBDrawerShader();
			if(bbShader)
				bbShader->updateMatrices(pm, map_mm);
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
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	drawButtons();
	if(isSelectedView())
		drawFrame();
	glPopAttrib();

	QGLViewer::postDraw();
}

void View::resizeGL(int width, int height)
{
	QGLViewer::resizeGL(width, height);
	if(m_buttonArea)
		m_buttonArea->setTopRightPosition(width, 0);
}

void View::drawButtons()
{
	glEnable(GL_TEXTURE_2D);
	startScreenCoordinatesSystem();
	m_buttonArea->draw();
	stopScreenCoordinatesSystem();
	glDisable(GL_TEXTURE_2D);
}

void View::drawFrame()
{
	startScreenCoordinatesSystem();
	glColor3f(0.0f, 1.0f, 0.0f);
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
		glVertex2i(1, 1);
		glVertex2i(1, height()-1);
		glVertex2i(width()-1, height()-1);
		glVertex2i(width()-1, 1);
	glEnd();
	stopScreenCoordinatesSystem();
}

void View::keyPressEvent(QKeyEvent* event)
{
	foreach(PluginInteraction* plugin, l_plugins)
		plugin->keyPress(this, event);
	QGLViewer::keyPressEvent(event);
}

void View::keyReleaseEvent(QKeyEvent *event)
{
	foreach(PluginInteraction* plugin, l_plugins)
		plugin->keyRelease(this, event);
	QGLViewer::keyReleaseEvent(event);
}

void View::mousePressEvent(QMouseEvent* event)
{
	if(m_buttonArea->isClicked(event->x(), event->y()))
		m_buttonArea->clickButton(event->x(), event->y(), event->globalX(), event->globalY());
	else
	{
		if(!isSelectedView())
			m_schnapps->setSelectedView(this);
		foreach(PluginInteraction* plugin, l_plugins)
			plugin->mousePress(this, event);
		QGLViewer::mousePressEvent(event);
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

void View::updateCurrentCameraBB()
{
	qglviewer::Vec bbMin(0,0,0);
	qglviewer::Vec bbMax(1,1,1);
	if(!l_maps.empty())
	{
		bbMin = l_maps[0]->getBBmin();
		bbMax = l_maps[0]->getBBmax();
		for(int i = 1; i < l_maps.size(); ++i)
		{
			MapHandlerGen* m = l_maps[i];
			qglviewer::Vec min = m->getBBmin();
			qglviewer::Vec max = m->getBBmax();
			for(unsigned int dim = 0; dim < 3; ++dim)
			{
				if(min[dim] < bbMin[dim])
					bbMin[dim] = min[dim];
				if(max[dim] > bbMax[dim])
					bbMax[dim] = max[dim];
			}
		}
	}
	camera()->setSceneBoundingBox(bbMin, bbMax);
	camera()->showEntireScene();
}

void View::selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur)
{
	if(cur && isLinkedToMap(cur))
		setManipulatedFrame(cur->getFrame());
	updateGL();
}

void View::ui_verticalSplitView(int x, int y, int globalX, int globalY)
{
	m_schnapps->splitView(m_name, Qt::Horizontal);
}

void View::ui_horizontalSplitView(int x, int y, int globalX, int globalY)
{
	m_schnapps->splitView(m_name, Qt::Vertical);
}

void View::ui_closeView(int x, int y, int globalX, int globalY)
{
	m_schnapps->removeView(m_name);
}

} // namespace SCHNApps

} // namespace CGoGN
