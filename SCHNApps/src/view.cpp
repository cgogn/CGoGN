#include "view.h"

#include "window.h"
#include "plugin.h"
#include "camera.h"
#include "viewButtonArea.h"

#include "cameraViewDialog.h"
#include "pluginsViewDialog.h"
#include "mapsViewDialog.h"

#include "Utils/GLSLShader.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

unsigned int View::viewCount = 0;

View::View(const QString& name, Window* w, QWidget* parent, const QGLWidget* shareWidget) :
	QGLViewer(parent, shareWidget),
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
		unlinkPlugin(plugin);

	foreach(MapHandler* map, l_maps)
		unlinkMap(map);

	delete m_buttonArea;
}

void View::init()
{
	m_buttonArea = new ViewButtonArea(this);
	m_buttonArea->setTopRightPosition(this->width(), 0);

	m_cameraButton = new ViewButton(":icons/icons/camera_32.png", this);
	m_buttonArea->addButton(m_cameraButton);
	connect(m_cameraButton, SIGNAL(clicked(int, int)), this, SLOT(cb_cameraView(int, int)));

	m_pluginsButton = new ViewButton(":icons/icons/plugins_32.png", this);
	m_buttonArea->addButton(m_pluginsButton);
	connect(m_pluginsButton, SIGNAL(clicked(int, int)), this, SLOT(cb_pluginsView(int, int)));

	m_mapsButton = new ViewButton(":icons/icons/maps_32.png", this);
	m_buttonArea->addButton(m_mapsButton);
	connect(m_mapsButton, SIGNAL(clicked(int, int)), this, SLOT(cb_mapsView(int, int)));

	m_closeButton = new ViewButton(":icons/icons/close_32.png", this);
	m_buttonArea->addButton(m_closeButton);
	connect(m_closeButton, SIGNAL(clicked(int, int)), this, SLOT(cb_closeView(int, int)));

	qglviewer::Camera* c = this->camera();
	this->setCamera(m_currentCamera);
	delete c;

	this->setBackgroundColor(QColor(0,0,0));

	updateTextInfo();
}

void View::preDraw()
{
	QGLViewer::preDraw();

	glm::mat4 mm = getCurrentModelViewMatrix();
	glm::mat4 pm = getCurrentProjectionMatrix();
	for(std::set< std::pair<void*, CGoGN::Utils::GLSLShader*> >::iterator it = CGoGN::Utils::GLSLShader::m_registeredShaders.begin();
		it != CGoGN::Utils::GLSLShader::m_registeredShaders.end();
		++it)
	{
		it->second->updateMatrices(pm, mm);
	}
}

void View::draw()
{
	foreach(Plugin* plugin, l_plugins)
		plugin->redraw(this);
}

void View::postDraw()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	drawButtons();
	drawText();
	glPopAttrib();

	QGLViewer::postDraw();
}

void View::resizeGL(int width, int height)
{
	QGLViewer::resizeGL(width, height);
	if(m_buttonArea)
		m_buttonArea->setTopRightPosition(width, 0);
}

void View::drawText()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	QGLViewer::drawText(10, 20, m_textInfo);
}

void View::drawButtons()
{
	glEnable(GL_TEXTURE_2D);
	startScreenCoordinatesSystem();
	m_buttonArea->draw();
	stopScreenCoordinatesSystem();
	glDisable(GL_TEXTURE_2D);
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
	if(m_buttonArea->isClicked(event->x(), event->y()))
		m_buttonArea->clickButton(event->x(), event->y());
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
}

void View::wheelEvent(QWheelEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->wheelEvent(this, event->delta(), event->pos().x(), event->pos().y());
	QGLViewer::wheelEvent(event);
}

void View::drawOverpaint(QPainter *painter)
{
	painter->save();
	painter->setOpacity(0.8);
    painter->restore();
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
		updateTextInfo();
		updateGL();
	}
}

/*********************************************************
 * MANAGE LINKED PLUGINS
 *********************************************************/

void View::linkPlugin(Plugin* plugin)
{
	if(plugin && !l_plugins.contains(plugin))
		l_plugins.push_back(plugin);
}

void View::unlinkPlugin(Plugin* plugin)
{
	l_plugins.removeOne(plugin);
}

/*********************************************************
 * MANAGE LINKED MAPS
 *********************************************************/

void View::linkMap(MapHandler* map)
{
	if(map && !l_maps.contains(map))
		l_maps.push_back(map);
}

void View::unlinkMap(MapHandler* map)
{
	l_maps.removeOne(map);
}




void View::updateTextInfo()
{
	m_textInfo =
		QString("view: ") + m_name +
		QString(" / camera: ") + m_currentCamera->getName();
}

glm::mat4 View::getCurrentModelViewMatrix()
{
	GLdouble gl_mvm[16];
	this->camera()->getModelViewMatrix(gl_mvm);
	glm::mat4 mvm;
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			mvm[i][j] = (float)gl_mvm[i*4+j];
	}
	return mvm;
}

glm::mat4 View::getCurrentProjectionMatrix()
{
	GLdouble gl_pm[16];
	this->camera()->getProjectionMatrix(gl_pm);
	glm::mat4 pm;
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			pm[i][j] = (float)gl_pm[i*4+j];
	}
	return pm;
}

glm::mat4 View::getCurrentModelViewProjectionMatrix()
{
	GLdouble gl_mvpm[16];
	this->camera()->getModelViewProjectionMatrix(gl_mvpm);
	glm::mat4 mvpm;
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			mvpm[i][j] = (float)gl_mvpm[i*4+j];
	}
	return mvpm;
}

void View::setCurrentModelViewMatrix(const glm::mat4& mvm)
{
	GLdouble gl_mvm[16];
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			gl_mvm[i*4+j] = mvm[i][j];
	}
	this->camera()->setFromModelViewMatrix(gl_mvm);
}

void View::setCurrentProjectionMatrix(const glm::mat4& pm)
{
	float gl_pm[12];
	for(unsigned int i = 0; i < 3; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			gl_pm[i*3+j] = pm[i][j];
	}
	this->camera()->setFromProjectionMatrix(gl_pm);
}

void View::cb_cameraView(int x, int y)
{
//	m_cameraViewDialog->setGeometry(x, y, 150, 300);
	m_cameraViewDialog->show();
}

void View::cb_pluginsView(int x, int y)
{
	m_pluginsViewDialog->show();
}

void View::cb_mapsView(int x, int y)
{
	m_mapsViewDialog->show();
}

void View::cb_closeView(int x, int y)
{
	m_window->removeView(m_name);
}
