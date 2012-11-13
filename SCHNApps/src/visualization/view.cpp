#include "view.h"

#include "system.h"
#include "camera.h"
#include "scene.h"
#include "cameraViewDialog.h"
#include "cameraSceneDialog.h"
#include "context.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"
#include "GL/freeglut_std.h"
#include "GL/freeglut.h"



View::View(Scene* s, QString name, Camera* c, QGLWidget * shareWidget,Context* context ,QWidget* parent) :
	QGLViewer(new Context(NULL,QGLFormat(QGL::Rgba | QGL::DoubleBuffer| QGL::DepthBuffer))),
	m_scene(s),
	m_name(name),
	m_linkButton(NULL),
	m_linkViewEnabled(false),
	m_unlinkButton(NULL),
	m_unlinkViewEnabled(false),
	m_cameraButton(NULL),
	m_cameraEnabled(false),
	m_cameraSceneButton(NULL),
	m_cameraSceneEnabled(false),
	m_closeViewButton(NULL),
	m_closeViewEnabled(false),
	m_context(context),
	b_showButtons(true),
	b_destroyView(false)
{
	((Context*)(this->context()))->setDevice(this);
	((Context*)(this->context()))->create(context);

	makeCurrent();
	glewInit();

	qglviewer::Camera* defaultCamera= this->camera();
	if(c){
		m_currentCamera= c;
		this->setCamera(c);
		c->sharedWith(this);
	}
	else{
		m_currentCamera= new Camera(this);
		this->setCamera(m_currentCamera);
	}
	l_camera.push_back(m_currentCamera);

	if(m_name.isEmpty()){
		m_name= QString(s->getName() + "_view" + s->countViews());
	}

	delete defaultCamera;

	m_buttonArea= new ViewButtonArea(this);
	m_buttonArea->setTopRightPosition(this->width(),0);
	connect(m_buttonArea, SIGNAL(buttonClicked(ViewButton*)), this, SIGNAL(clickButton(ViewButton*)));

	updateTextInfo();
}

View::~View(){
	while(!l_camera.isEmpty()){
		Camera* camera= l_camera.takeFirst();
		if(!camera->isShared() && m_currentCamera!=camera){
			delete camera;
		}
		else if (camera->isShared()){
			camera->takenFrom(this);
			if(m_currentCamera==camera){
				qglviewer::Camera* defaultCamera= new qglviewer::Camera(*camera);
				this->setCamera(defaultCamera);
			}
		}
	}

	if(m_buttonArea){
		delete m_buttonArea;
	}
}


void View::updateGL(){
	if(m_scene){
		m_scene->updateGL(this);
	}

	QGLViewer::updateGL();
}

void View::simpleUpdate(){
	QGLViewer::updateGL();
}

void View::draw(){

	if(m_scene && m_currentCamera){
		m_scene->draw(this);
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	drawButtons();
	drawText();
	glPopAttrib();


}

void View::init(){

//	makeCurrent();

	if(m_scene && m_scene->countViews()>1)
		m_scene->init();




	m_linkButton= new ViewButton(":icons/icons/link.png",this);
	if(m_linkViewEnabled){
		m_linkViewEnabled=false;
		enableLinking(true);
	}

	m_cameraButton= new ViewButton(":icons/icons/camera.png",this);
	if(m_cameraEnabled){
		m_cameraEnabled=false;
		enableCameraGesture(true);
	}

	m_unlinkButton= new ViewButton(":icons/icons/broken_link.png",this);
	if(m_unlinkViewEnabled){
		m_unlinkViewEnabled=false;
		enableUnlinking(true);
	}

	m_cameraSceneButton= new ViewButton(":icons/icons/separate_camera.png",this);
	if(m_cameraSceneEnabled){
		m_cameraSceneEnabled=false;
		enableSceneCameraGesture(true);
	}

	m_closeViewButton= new ViewButton(":icons/icons/close.png",this);
	if(m_closeViewEnabled){
		m_closeViewEnabled=false;
		enableViewClose(true);
	}

//	doneCurrent();

}


void View::drawCameras(View* view){
	foreach(Camera* camera, l_camera){
		if(camera!=view->currentCamera()){
			camera->draw();
		}
	}
}

void View::drawText(){
	glColor3f(1.0f,1.0f,1.0f);

	if(b_showButtons){
		QGLViewer::drawText(10,20,m_textInfo);
	}
}

void View::drawButtons(){
	glColor3f(1.0f,1.0f,1.0f);

	glEnable(GL_TEXTURE_2D);

	if(b_showButtons){
		startScreenCoordinatesSystem();
		m_buttonArea->draw();
		stopScreenCoordinatesSystem();
	}
	glDisable(GL_TEXTURE_2D);
}

void View::keyPressEvent(QKeyEvent* event){
	if(!m_scene->keyPressEvent(event)){
		QGLViewer::keyPressEvent(event);
	}
}
void View::keyReleaseEvent(QKeyEvent *e){
	if(!m_scene->keyReleaseEvent(e)){
		QGLViewer::keyReleaseEvent(e);
	}
}

void View::mousePressEvent(QMouseEvent* event){
	if(m_buttonArea->isIn(event->x(),event->y())){
		ViewButton* vb;
		if((vb=m_buttonArea->clickAt(event->x(),event->y()))){
			m_scene->viewClickedButton(this, vb);
		}
	}
	else if(!m_scene->mousePressEvent(event)){
		QGLViewer::mousePressEvent(event);
	}

	if(b_destroyView){
		m_scene->deleteView(this);
	}
}

void View::mouseReleaseEvent(QMouseEvent* event){
	if(!m_scene->mouseReleaseEvent(event)){
		QGLViewer::mouseReleaseEvent(event);
	}
}

void View::mouseMoveEvent(QMouseEvent* event){
	if(!m_scene->mouseMoveEvent(event)){
		QGLViewer::mouseMoveEvent(event);
	}
}

void View::wheelEvent(QWheelEvent* event){
	if(!m_scene->wheelEvent(event)){
		QGLViewer::wheelEvent(event);
	}
}

void View::resizeGL(int width, int height){
	QGLViewer::resizeGL(width, height);

	if(m_buttonArea){
		m_buttonArea->setTopRightPosition(width, 0);
	}
}

void View::drawOverpaint(QPainter *painter){
	painter->save();

	painter->setOpacity(0.8);

//	m_buttonArea->draw(painter);

    painter->restore();
}

//void View::paintEvent(QPaintEvent *event){
//	makeCurrent();
//	Q_UNUSED(event)
//
////	QPainter painter;
////	painter.begin(this);
////	painter.setRenderHint(QPainter::Antialiasing);
////
////		painter.beginNativePainting();
////	qglClearColor(backgroundColor());
////
////	glEnable(GL_DEPTH_TEST);
////	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////
////	preDraw();
////	draw();
////	postDraw();
////
////
////	drawOverpaint(&painter);
////	painter.endNativePainting();
//}

void View::enableLinking(bool b){
	if(m_linkButton){
		if(b && !m_linkViewEnabled){
	//		m_linkButton= new ViewButton(QPixmap(":icons/icons/link.png"),QSize(32,32),this);
			m_buttonArea->addButton(m_linkButton);
			connect(m_linkButton, SIGNAL(clicked()), this, SLOT(linkView()));
		}
		else if(!b && m_linkViewEnabled){
	//		delete m_linkButton;
	//		m_linkButton=NULL;
			disconnect(m_linkButton, SIGNAL(clicked()), this, SLOT(linkView()));
			m_buttonArea->takeButton(m_linkButton);
		}
	}
	m_linkViewEnabled=b;
}

void View::enableUnlinking(bool b){
	if(m_unlinkButton){
		if(b && !m_unlinkViewEnabled){
				m_buttonArea->addButton(m_unlinkButton);
				connect(m_unlinkButton, SIGNAL(clicked()), this, SLOT(unlinkView()));
			}
			else if(!b && m_unlinkViewEnabled){
		//		delete m_linkButton;
		//		m_linkButton=NULL;
				disconnect(m_unlinkButton, SIGNAL(clicked()), this, SLOT(unlinkView()));
				m_buttonArea->takeButton(m_unlinkButton);
			}
	}
	m_unlinkViewEnabled=b;
}

void View::enableCameraGesture(bool b){
	if(m_cameraButton){
		if(b && !m_cameraEnabled){
	//		m_cameraButton= new ViewButton(QPixmap(":icons/icons/camera.png"),QSize(32,32),this);
			m_buttonArea->addButton(m_cameraButton);
			connect(m_cameraButton, SIGNAL(clicked()), this, SLOT(cameraGesture()));
		}
		else if(!b && m_cameraEnabled){
	//		delete m_cameraButton;
	//		m_cameraButton=NULL;
			disconnect(m_cameraButton, SIGNAL(clicked()), this, SLOT(cameraGesture()));
			m_buttonArea->takeButton(m_cameraButton);
		}
	}
	m_cameraEnabled=b;
}

void View::enableSceneCameraGesture(bool b){
	if(m_cameraSceneButton){
		if(b && !m_cameraSceneEnabled){
		//		m_cameraSceneButton= new ViewButton(QPixmap(":icons/icons/separate_camera.png"),QSize(32,32),this);
			m_buttonArea->addButton(m_cameraSceneButton);
			connect(m_cameraSceneButton, SIGNAL(clicked()), this, SLOT(cameraSceneGesture()));
		}
		else if(!b && m_cameraSceneEnabled){
		//		delete m_cameraSceneButton;
		//		m_cameraSceneButton=NULL;
			disconnect(m_cameraSceneButton, SIGNAL(clicked()), this, SLOT(cameraSceneGesture()));
			m_buttonArea->takeButton(m_cameraSceneButton);
		}
	}
	m_cameraSceneEnabled=b;
}

void View::enableViewClose(bool b){
	if(m_closeViewButton){
		if(b && !m_closeViewEnabled){
			m_buttonArea->addButton(m_closeViewButton);
			connect(m_closeViewButton, SIGNAL(clicked()), this, SLOT(closeView()));
		}
		else if(!b && m_closeViewEnabled){
			disconnect(m_closeViewButton, SIGNAL(clicked()), this, SLOT(closeView()));
			m_buttonArea->takeButton(m_closeViewButton);
		}
	}
	m_closeViewEnabled=b;
}

void View::setCurrentCamera(Camera* c){
	if(c!=m_currentCamera && l_camera.contains(c)){
		if(m_currentCamera==NULL){
			qglviewer::Camera* defaultCam= this->camera();
			this->setCamera(c);
			delete defaultCam;
		}
		else{
			this->setCamera(c);
		}

		m_currentCamera=c;

		emit currentCameraChanged(c);

		this->updateTextInfo();
		this->updateGL();
	}
}

Camera* View::addCamera(){
	Camera* c;
	if(m_currentCamera!=NULL){
		c= new Camera(this,*m_currentCamera);
	}
	else{
		c= new Camera(this);
		this->setCurrentCamera(c);
	}
	l_camera.push_back(c);


	return c;
}

void View::removeCamera(Camera* c){
	int i= l_camera.indexOf(c);
	if(c && i>=0){
		l_camera.takeAt(i);
		c->takenFrom(this);
		if(!c->isShared()){
			delete c;
		}
	}
}

Camera* View::takeCamera(Camera* c){
	int i= l_camera.indexOf(c);
	if(i>=0){
		Camera* camera= l_camera.takeAt(i);
		if(m_currentCamera==camera && !l_camera.isEmpty()){
			m_currentCamera=l_camera.first();
			this->setCamera(m_currentCamera);
			emit currentCameraChanged(m_currentCamera);
			this->updateTextInfo();
			this->updateGL();
		}
		else if(l_camera.isEmpty()){
			this->setCamera(new qglviewer::Camera(*m_currentCamera));
			m_currentCamera=NULL;
			emit currentCameraChanged(NULL);
		}
		camera->takenFrom(this);
		return camera;
	}
	else{
		return NULL;
	}
}

void View::insertCamera(int index, Camera* c){
	l_camera.insert(index, c);
	if(m_currentCamera==NULL){
		this->setCurrentCamera(c);
	}
	c->sharedWith(this);
}
void View::shareCamera(Camera* c, int index){
	if(!l_camera.contains(c)){
		insertCamera(index, c);
	}
}

//void View::addUnlinkButton(){
////	if(!m_unlinkButton){
////		m_unlinkButton= new ViewButton(QPixmap(":icons/icons/broken_link.png"),QSize(32,32),this);
//		m_buttonArea->addButton(m_unlinkButton);
//		connect(m_unlinkButton, SIGNAL(clicked()), this, SLOT(unlinkView()));
////	}
//}

//void View::removeUnlinkButton(){
////	if(m_unlinkButton){
//		disconnect(m_unlinkButton, SIGNAL(clicked()), this, SLOT(unlinkView()));
////		delete m_unlinkButton;
////		m_unlinkButton=NULL;
//		m_buttonArea->takeButton(m_unlinkButton);
////	}
//}

void View::updateTextInfo(){
	m_textInfo= QString("Scene: ")+m_scene->getName()+ " > "
			+ this->m_name + " > "
			+ m_currentCamera->getName() + (m_currentCamera->isShared()?" (shared)" : "");
}


glm::mat4 View::getCurrentModelViewMatrice(){
	//before returning the matrix
	//it's converted to a generic type
	//used by CGoGN
	GLdouble gl_mvm[16];
	this->camera()->getModelViewMatrix(gl_mvm);

	glm::mat4 mvm;
	for(unsigned int i=0; i<4;i++){
		for(unsigned int j=0; j<4; j++){
			mvm[i][j]= (float)gl_mvm[i*4+j];
		}
	}
	return mvm;
}

glm::mat4 View::getCurrentProjectionMatrice(){
	//before returning the matrix
	//it's converted to a generic type
	//used by CGoGN
	GLdouble gl_pm[16];
	this->camera()->getProjectionMatrix(gl_pm);

	glm::mat4 pm;
	for(unsigned int i=0; i<4;i++){
		for(unsigned int j=0; j<4; j++){
			pm[i][j]= (float)gl_pm[i*4+j];
		}
	}
	return pm;
}

glm::mat4 View::getCurrentModelViewProjectionMatrice(){
	//before returning the matrix
	//it's converted to a generic type
	//used by CGoGN
	GLdouble gl_mvpm[16];
	this->camera()->getModelViewProjectionMatrix(gl_mvpm);
	glm::mat4 mvpm;
	for(unsigned int i=0; i<4;i++){
		for(unsigned int j=0; j<4; j++){
			mvpm[i][j]= (float)gl_mvpm[i*4+j];
		}
	}
	return mvpm;
}

void View::setCurrentModelViewMatrice(glm::mat4 mvm){
	//before setting the matrix, it's converted to the
	//type used by QGLViewer
	GLdouble gl_mvm[16];
	for(unsigned int i=0; i<4;i++){
		for(unsigned int j=0; j<4; j++){
			gl_mvm[i*4+j]= mvm[i][j];
		}
	}
	this->camera()->setFromModelViewMatrix(gl_mvm);
}

void View::setCurrentProjectionMatrice(glm::mat4 pm){
	//before setting the matrix, it's converted to the
	//type used by QGLViewer
	float gl_pm[12];
	for(unsigned int i=0; i<3;i++){
		for(unsigned int j=0; j<4; j++){
			gl_pm[i*3+j]= pm[i][j];
		}
	}
	this->camera()->setFromProjectionMatrix(gl_pm);
}

void View::linkView(){
	m_scene->linkWithPlugin();
}

void View::unlinkView(){
	m_scene->unlinkPlugin();
}

void View::cameraGesture(){
	CameraViewDialog cvd(this, this);
	cvd.exec();

	updateTextInfo();

	enableSceneCameraGesture(l_camera.size()>1 || m_scene->countViews()>1);
}

void View::cameraSceneGesture(){
	CameraSceneDialog csd(m_scene);
	csd.exec();
	updateTextInfo();
}

void View::closeView(){
//	m_scene->deleteView(this);
	b_destroyView= true;
}

void View::clickButton(ViewButton* viewButton){
	if(viewButton){
		emit(viewButtonClicked(viewButton));
	}
}

void View::addCustomViewButton(ViewButton* viewButton){
	m_buttonArea->addButton(viewButton);
}

void View::removeCustomViewButton(ViewButton* viewButton){
	m_buttonArea->takeButton(viewButton);
}

