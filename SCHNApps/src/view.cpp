#include "view.h"

#include "window.h"
#include "plugin.h"
#include "camera.h"
#include "viewButtonArea.h"

#include "cameraViewDialog.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

unsigned int View::viewCount = 0;

View::View(const QString& name, Window* w, QWidget* parent, const QGLWidget* shareWidget) :
//	QGLViewer(new Context(NULL, QGLFormat(QGL::Rgba | QGL::DoubleBuffer | QGL::DepthBuffer))),
	QGLViewer(parent, shareWidget),
	m_name(name),
	m_window(w),
	m_currentCamera(NULL),
	m_cameraButton(NULL),
	m_pluginsButton(NULL),
	m_mapsButton(NULL),
	m_closeButton(NULL)
{
	++viewCount;

	Camera* c = m_window->addCamera();
	setCurrentCamera(c);

	m_buttonArea = new ViewButtonArea(this);
	m_buttonArea->setTopRightPosition(this->width(), 0);

	updateTextInfo();
}

View::~View()
{
	m_currentCamera->unlinkView(this);

	foreach(Plugin* plugin, l_plugins)
		unlinkPlugin(plugin);

	foreach(MapHandler* map, l_maps)
		unlinkMap(map);

	delete m_buttonArea;
}

void View::initGL()
{
	m_cameraButton = new ViewButton(":icons/icons/camera_32.png", this);
	connect(m_cameraButton, SIGNAL(clicked()), this, SLOT(cb_cameraView()));

	m_pluginsButton = new ViewButton(":icons/icons/plugins_32.png", this);

	m_mapsButton = new ViewButton(":icons/icons/maps_32.png", this);

	m_closeButton = new ViewButton(":icons/icons/close_32.png", this);
}

void View::updateGL()
{
	QGLViewer::updateGL();
}

void View::draw()
{
	const float nbSteps = 200.0;
	glBegin(GL_QUAD_STRIP);
	for (float i = 0; i < nbSteps; ++i)
	{
		float ratio = i/nbSteps;
		float angle = 21.0*ratio;
		float c = cos(angle);
		float s = sin(angle);
		float r1 = 1.0 - 0.8f*ratio;
		float r2 = 0.8f - 0.8f*ratio;
		float alt = ratio - 0.5f;
		const float nor = 0.5f;
		const float up = sqrt(1.0-nor*nor);
		glColor3f(1.0-ratio, 0.2f , ratio);
		glNormal3f(nor*c, up, nor*s);
		glVertex3f(r1*c, alt, r1*s);
		glVertex3f(r2*c, alt+0.05f, r2*s);
	}
	glEnd();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
//	drawButtons();
	drawText();
	glPopAttrib();
}

void View::drawText()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	QGLViewer::drawText(10, 20, m_textInfo);
}

void View::drawButtons()
{
	glColor3f(1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	startScreenCoordinatesSystem();
	m_buttonArea->draw();
	stopScreenCoordinatesSystem();
	glDisable(GL_TEXTURE_2D);
}

void View::keyPressEvent(QKeyEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_keyPress(this, event->key());
	QGLViewer::keyPressEvent(event);
}

void View::keyReleaseEvent(QKeyEvent *event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_keyRelease(this, event->key());
	QGLViewer::keyReleaseEvent(event);
}

void View::mousePressEvent(QMouseEvent* event)
{
	if(m_buttonArea->isClicked(event->x(), event->y()))
		m_buttonArea->clickButton(event->x(), event->y());
	else
	{
		foreach(Plugin* plugin, l_plugins)
			plugin->cb_mousePress(this, event->button(), event->pos().x(), event->pos().y());
		QGLViewer::mousePressEvent(event);
	}
}

void View::mouseReleaseEvent(QMouseEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_mouseRelease(this, event->button(), event->pos().x(), event->pos().y());
	QGLViewer::mouseReleaseEvent(event);
}

void View::mouseMoveEvent(QMouseEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_mouseMove(this, event->button(), event->pos().x(), event->pos().y());
	QGLViewer::mouseMoveEvent(event);
}

void View::wheelEvent(QWheelEvent* event)
{
	foreach(Plugin* plugin, l_plugins)
		plugin->cb_mouseMove(this, event->delta(), event->pos().x(), event->pos().y());
	QGLViewer::wheelEvent(event);
}

void View::resizeGL(int width, int height)
{
	QGLViewer::resizeGL(width, height);
	if(m_buttonArea)
		m_buttonArea->setTopRightPosition(width, 0);
}

void View::drawOverpaint(QPainter *painter)
{
	painter->save();
	painter->setOpacity(0.8);
    painter->restore();
}

void View::setCurrentCamera(Camera* c)
{
	if(c != m_currentCamera)
	{
		if(m_currentCamera != NULL)
			m_currentCamera->unlinkView(this);
		m_currentCamera = c;
		m_currentCamera->linkView(this);
		this->setCamera(m_currentCamera);
		updateTextInfo();
		updateGL();
	}
}

void View::linkPlugin(Plugin* plugin)
{
	if(plugin && !l_plugins.contains(plugin))
		l_plugins.push_back(plugin);
}

void View::unlinkPlugin(Plugin* plugin)
{
	l_plugins.removeOne(plugin);
}

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

void View::cb_cameraView()
{
	CameraViewDialog cvd(m_window, this);
	cvd.exec();
}
