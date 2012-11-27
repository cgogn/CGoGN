#include "view.h"

#include "system.h"
#include "window.h"
#include "plugin.h"
#include "scene.h"
#include "camera.h"
#include "cameraViewDialog.h"
#include "cameraSceneDialog.h"

unsigned int View::viewCount = 0;

View::View(const QString& name, Window* w, QWidget* parent) :
//	QGLViewer(new Context(NULL, QGLFormat(QGL::Rgba | QGL::DoubleBuffer | QGL::DepthBuffer))),
	QGLViewer(w->getContext(), parent),
	m_name(name),
	m_window(w),
	m_context(w->getContext()),
	b_drawText(true)
//	m_linkButton(NULL),
//	m_linkViewEnabled(false),
//	m_unlinkButton(NULL),
//	m_unlinkViewEnabled(false),
//	m_cameraButton(NULL),
//	m_cameraEnabled(false),
//	m_cameraSceneButton(NULL),
//	m_cameraSceneEnabled(false),
//	m_closeViewButton(NULL),
//	m_closeViewEnabled(false),
//	b_showButtons(true)
{
	++viewCount;

//	((Context*)(this->context()))->setDevice(this);
//	((Context*)(this->context()))->create(context);

//	makeCurrent();
//	glewInit();

//	qglviewer::Camera* defaultCamera = this->camera();
//
//	if(c)
//	{
//		m_currentCamera = c;
//		this->setCamera(c);
//		c->linkView(this);
//	}
//	else
//	{
//		m_currentCamera = new Camera(m_window, this, *defaultCamera);
//	}
//
//	delete defaultCamera;

//	m_buttonArea = new ViewButtonArea(this);
//	m_buttonArea->setTopRightPosition(this->width(), 0);

	updateTextInfo();
}

View::~View()
{
	m_scene->unlinkView(this);
	m_currentCamera->unlinkView(this);

//	if(m_buttonArea)
//		delete m_buttonArea;
}

void View::initGL()
{
	if(m_scene)
		m_scene->initGL();

//	m_linkButton = new ViewButton(":icons/icons/link.png", this);
//	if(m_linkViewEnabled)
//	{
//		m_linkViewEnabled = false;
//		enableLinking(true);
//	}
//
//	m_cameraButton = new ViewButton(":icons/icons/camera.png", this);
//	if(m_cameraEnabled)
//	{
//		m_cameraEnabled = false;
//		enableCameraGesture(true);
//	}
//
//	m_unlinkButton = new ViewButton(":icons/icons/broken_link.png", this);
//	if(m_unlinkViewEnabled)
//	{
//		m_unlinkViewEnabled = false;
//		enableUnlinking(true);
//	}
//
//	m_cameraSceneButton = new ViewButton(":icons/icons/separate_camera.png", this);
//	if(m_cameraSceneEnabled)
//	{
//		m_cameraSceneEnabled = false;
//		enableSceneCameraGesture(true);
//	}
//
//	m_closeViewButton = new ViewButton(":icons/icons/close.png", this);
//	if(m_closeViewEnabled)
//	{
//		m_closeViewEnabled = false;
//		enableViewClose(true);
//	}
}

void View::updateGL()
{
	QGLViewer::updateGL();
}

void View::draw()
{
	if(m_scene && m_currentCamera)
		m_scene->draw(this);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
//	if(b_drawButtons)
//		drawButtons();
	if(b_drawText)
		drawText();
	glPopAttrib();
}

void View::drawText()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	QGLViewer::drawText(10, 20, m_textInfo);
}

//void View::drawButtons()
//{
//	glColor3f(1.0f,1.0f,1.0f);
//	glEnable(GL_TEXTURE_2D);
//	if(b_showButtons)
//	{
//		startScreenCoordinatesSystem();
//		m_buttonArea->draw();
//		stopScreenCoordinatesSystem();
//	}
//	glDisable(GL_TEXTURE_2D);
//}

void View::keyPressEvent(QKeyEvent* event)
{
	m_scene->keyPressEvent(event);
	QGLViewer::keyPressEvent(event);
}

void View::keyReleaseEvent(QKeyEvent *e)
{
	m_scene->keyReleaseEvent(e);
	QGLViewer::keyReleaseEvent(e);
}

void View::mousePressEvent(QMouseEvent* event)
{
//	if(m_buttonArea->isClicked(event->x(), event->y()))
//		m_buttonArea->clickButton(event->x(), event->y());
//	else
//	{
		m_scene->mousePressEvent(event);
		QGLViewer::mousePressEvent(event);
//	}
}

void View::mouseReleaseEvent(QMouseEvent* event)
{
	m_scene->mouseReleaseEvent(event);
	QGLViewer::mouseReleaseEvent(event);
}

void View::mouseMoveEvent(QMouseEvent* event)
{
	m_scene->mouseMoveEvent(event);
	QGLViewer::mouseMoveEvent(event);
}

void View::wheelEvent(QWheelEvent* event)
{
	m_scene->wheelEvent(event);
	QGLViewer::wheelEvent(event);
}

void View::resizeGL(int width, int height)
{
	QGLViewer::resizeGL(width, height);
//	if(m_buttonArea)
//		m_buttonArea->setTopRightPosition(width, 0);
}

void View::drawOverpaint(QPainter *painter)
{
	painter->save();
	painter->setOpacity(0.8);
    painter->restore();
}

//void View::enableLinking(bool b)
//{
//	if(m_linkButton)
//	{
//		if(b && !m_linkViewEnabled)
//		{
//			m_buttonArea->addButton(m_linkButton);
//			connect(m_linkButton, SIGNAL(clicked()), this, SLOT(linkView()));
//		}
//		else if(!b && m_linkViewEnabled)
//		{
//			disconnect(m_linkButton, SIGNAL(clicked()), this, SLOT(linkView()));
//			m_buttonArea->removeButton(m_linkButton);
//		}
//	}
//	m_linkViewEnabled = b;
//}
//
//void View::enableUnlinking(bool b)
//{
//	if(m_unlinkButton)
//	{
//		if(b && !m_unlinkViewEnabled)
//		{
//			m_buttonArea->addButton(m_unlinkButton);
//			connect(m_unlinkButton, SIGNAL(clicked()), this, SLOT(unlinkView()));
//		}
//		else if(!b && m_unlinkViewEnabled)
//		{
//			disconnect(m_unlinkButton, SIGNAL(clicked()), this, SLOT(unlinkView()));
//			m_buttonArea->removeButton(m_unlinkButton);
//		}
//	}
//	m_unlinkViewEnabled = b;
//}
//
//void View::enableCameraGesture(bool b)
//{
//	if(m_cameraButton)
//	{
//		if(b && !m_cameraEnabled)
//		{
//			m_buttonArea->addButton(m_cameraButton);
//			connect(m_cameraButton, SIGNAL(clicked()), this, SLOT(cameraGesture()));
//		}
//		else if(!b && m_cameraEnabled)
//		{
//			disconnect(m_cameraButton, SIGNAL(clicked()), this, SLOT(cameraGesture()));
//			m_buttonArea->removeButton(m_cameraButton);
//		}
//	}
//	m_cameraEnabled = b;
//}
//
//void View::enableSceneCameraGesture(bool b)
//{
//	if(m_cameraSceneButton)
//	{
//		if(b && !m_cameraSceneEnabled)
//		{
//			m_buttonArea->addButton(m_cameraSceneButton);
//			connect(m_cameraSceneButton, SIGNAL(clicked()), this, SLOT(cameraSceneGesture()));
//		}
//		else if(!b && m_cameraSceneEnabled)
//		{
//			disconnect(m_cameraSceneButton, SIGNAL(clicked()), this, SLOT(cameraSceneGesture()));
//			m_buttonArea->removeButton(m_cameraSceneButton);
//		}
//	}
//	m_cameraSceneEnabled = b;
//}
//
//void View::enableViewClose(bool b)
//{
//	if(m_closeViewButton)
//	{
//		if(b && !m_closeViewEnabled)
//		{
//			m_buttonArea->addButton(m_closeViewButton);
//			connect(m_closeViewButton, SIGNAL(clicked()), this, SLOT(closeView()));
//		}
//		else if(!b && m_closeViewEnabled)
//		{
//			disconnect(m_closeViewButton, SIGNAL(clicked()), this, SLOT(closeView()));
//			m_buttonArea->removeButton(m_closeViewButton);
//		}
//	}
//	m_closeViewEnabled = b;
//}

void View::updateTextInfo()
{
	m_textInfo = QString("Scene: ") + m_scene->getName() + " > "
			   + QString("View: ") + this->m_name + " > "
			   + QString("Camera: ") + m_currentCamera->getName() + (m_currentCamera->isShared() ? " (shared)" : "");
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

//void View::linkView()
//{
//	m_scene->linkWithPlugin();
//}
//
//void View::unlinkView()
//{
//	m_scene->unlinkPlugin();
//}
//
//void View::cameraGesture()
//{
//	CameraViewDialog cvd(this, this);
//	cvd.exec();
//	updateTextInfo();
//	enableSceneCameraGesture(l_camera.size() > 1 || m_scene->countViews() > 1);
//}
//
//void View::cameraSceneGesture()
//{
//	CameraSceneDialog csd(m_scene);
//	csd.exec();
//	updateTextInfo();
//}
//
//void View::closeView()
//{
//	b_destroyView = true;
//}

//void View::addViewButton(ViewButton* viewButton)
//{
//	m_buttonArea->addButton(viewButton);
//}
//
//void View::removeViewButton(ViewButton* viewButton)
//{
//	m_buttonArea->takeButton(viewButton);
//}
