#include "view.h"

#include "plugin.h"
#include "camera.h"
#include "viewButtonArea.h"
#include "mapHandler.h"

#include "dialogs/cameraViewDialog.h"
#include "dialogs/pluginsViewDialog.h"
#include "dialogs/mapsViewDialog.h"

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

View::View(const QString& name, Window* w, const QGLWidget* shareWidget) :
	QGLViewer(NULL, shareWidget),
	m_name(name),
	m_window(w),
	m_cameraButton(NULL),
	m_pluginsButton(NULL),
	m_mapsButton(NULL),
	m_closeButton(NULL)
{
	++viewCount;

	m_currentCamera = m_window->addCamera();

	m_cameraViewDialog = new CameraViewDialog(m_window, this);
	m_pluginsViewDialog = new PluginsViewDialog(m_window, this);
	m_mapsViewDialog = new MapsViewDialog(m_window, this);
}

View::~View()
{
	m_currentCamera->unlinkView(this);

	qglviewer::Camera* c = new qglviewer::Camera();
	this->setCamera(c);

	foreach(Plugin* plugin, l_plugins)
	{
		plugin->unlinkView(this);
//		unlinkPlugin(plugin);
	}

	foreach(MapHandlerGen* map, l_maps)
	{
		map->unlinkView(this);
//		unlinkMap(map);
	}

	delete m_buttonArea;
}

void View::init()
{
	m_buttonArea = new ViewButtonArea(this);
	m_buttonArea->setTopRightPosition(this->width(), 0);

	m_cameraButton = new ViewButton(":icons/icons/cameras.png", this);
	m_buttonArea->addButton(m_cameraButton);
	connect(m_cameraButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(cb_cameraView(int, int, int, int)));

	m_pluginsButton = new ViewButton(":icons/icons/plugins.png", this);
	m_buttonArea->addButton(m_pluginsButton);
	connect(m_pluginsButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(cb_pluginsView(int, int, int, int)));

	m_mapsButton = new ViewButton(":icons/icons/maps.png", this);
	m_buttonArea->addButton(m_mapsButton);
	connect(m_mapsButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(cb_mapsView(int, int, int, int)));

	m_VsplitButton = new ViewButton(":icons/icons/Vsplit.png", this);
	m_buttonArea->addButton(m_VsplitButton);
	connect(m_VsplitButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(cb_VsplitView(int, int, int, int)));

	m_HsplitButton = new ViewButton(":icons/icons/Hsplit.png", this);
	m_buttonArea->addButton(m_HsplitButton);
	connect(m_HsplitButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(cb_HsplitView(int, int, int, int)));

	m_closeButton = new ViewButton(":icons/icons/close.png", this);
	m_buttonArea->addButton(m_closeButton);
	connect(m_closeButton, SIGNAL(clicked(int, int, int, int)), this, SLOT(cb_closeView(int, int, int, int)));

	qglviewer::Camera* c = this->camera();
	this->setCamera(m_currentCamera);
	delete c;

	this->setBackgroundColor(QColor(0,0,0));
}

void View::preDraw()
{
	m_currentCamera->setScreenWidthAndHeight(width(), height());

	glm::mat4 mm = getCurrentModelViewMatrix();
	glm::mat4 pm = getCurrentProjectionMatrix();
	foreach(Plugin* plugin, l_plugins)
	{
		const QList<CGoGN::Utils::GLSLShader*>& shaders = plugin->getShaders();
		foreach(CGoGN::Utils::GLSLShader* shader, shaders)
			shader->updateMatrices(pm, mm);
	}

	QGLViewer::preDraw();
}

void View::draw()
{
	QList<Camera*> cameras = m_window->getCamerasList();
	foreach(Camera* camera, cameras)
	{
		if(camera != m_currentCamera)
		{
			if(camera->getDraw()) camera->draw();
			if(camera->getDrawPath()) camera->drawAllPaths();
		}
	}

	foreach(Plugin* plugin, l_plugins)
		plugin->redraw(this);
}

void View::postDraw()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	drawButtons();
	if(isCurrentView())
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
	foreach(Plugin* plugin, l_plugins)
		plugin->keyPress(this, event->key());
	QGLViewer::keyPressEvent(event);
}

void View::keyReleaseEvent(QKeyEvent *event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->keyRelease(this, event->key());
	QGLViewer::keyReleaseEvent(event);
}

void View::mousePressEvent(QMouseEvent* event)
{
	if(!isCurrentView())
		m_window->setCurrentView(this);

	if(m_buttonArea->isClicked(event->x(), event->y()))
		m_buttonArea->clickButton(event->x(), event->y(), event->globalX(), event->globalY());
	else
	{
		foreach(Plugin* plugin, l_plugins)
			plugin->mousePress(this, event->button(), event->pos().x(), event->pos().y());
		QGLViewer::mousePressEvent(event);
	}
}

void View::mouseReleaseEvent(QMouseEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->mouseRelease(this, event->button(), event->pos().x(), event->pos().y());
	QGLViewer::mouseReleaseEvent(event);
}

void View::mouseMoveEvent(QMouseEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->mouseMove(this, event->button(), event->pos().x(), event->pos().y());
	QGLViewer::mouseMoveEvent(event);

	QList<View*> views = m_window->getViewsList();
	foreach(View* view, views)
	{
		if(view != this)
			view->updateGL();
	}
}

void View::wheelEvent(QWheelEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->wheelEvent(this, event->delta(), event->pos().x(), event->pos().y());
	QGLViewer::wheelEvent(event);

	QList<View*> views = m_window->getViewsList();
	foreach(View* view, views)
	{
		if(view != this)
			view->updateGL();
	}
}

/*********************************************************
 * MANAGE LINKED CAMERA
 *********************************************************/

void View::setCurrentCamera(Camera* c)
{
	if(c != m_currentCamera && c != NULL)
	{
		m_currentCamera = c;
		this->setCamera(m_currentCamera);
		emit(currentCameraChanged(c));
		updateGL();
	}
}

/*********************************************************
 * MANAGE LINKED PLUGINS
 *********************************************************/

void View::linkPlugin(Plugin* plugin)
{
	if(plugin && !l_plugins.contains(plugin))
	{
		l_plugins.push_back(plugin);
		emit(pluginLinked(plugin));
	}
}

void View::unlinkPlugin(Plugin* plugin)
{
	if(l_plugins.removeOne(plugin))
		emit(pluginUnlinked(plugin));
}

/*********************************************************
 * MANAGE LINKED MAPS
 *********************************************************/

void View::linkMap(MapHandlerGen* map)
{
	if(map && !l_maps.contains(map))
	{
		l_maps.push_back(map);
		emit(mapLinked(map));
		updateViewBB();
	}
}

void View::unlinkMap(MapHandlerGen* map)
{
	if(l_maps.removeOne(map))
	{
		emit(mapUnlinked(map));
		updateViewBB();
	}
}

void View::updateViewBB()
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

/*********************************************************
 * MANAGE MATRICES
 *********************************************************/

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

void View::cb_cameraView(int x, int y, int globalX, int globalY)
{
	m_cameraViewDialog->move(globalX, globalY);
	m_cameraViewDialog->show();
}

void View::cb_pluginsView(int x, int y, int globalX, int globalY)
{
	m_pluginsViewDialog->move(globalX, globalY);
	m_pluginsViewDialog->show();
}

void View::cb_mapsView(int x, int y, int globalX, int globalY)
{
	m_mapsViewDialog->move(globalX, globalY);
	m_mapsViewDialog->show();
}

void View::cb_closeView(int x, int y, int globalX, int globalY)
{
	m_window->removeView(m_name);
}

void View::cb_VsplitView(int x, int y, int globalX, int globalY)
{
	m_window->splitView(m_name, Qt::Horizontal);
}

void View::cb_HsplitView(int x, int y, int globalX, int globalY)
{
	m_window->splitView(m_name, Qt::Vertical);
}

} // namespace SCHNApps

} // namespace CGoGN
