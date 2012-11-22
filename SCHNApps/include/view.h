#ifndef _VIEW_H_
#define _VIEW_H_

#include <iostream>

#include "types.h"
#include "viewButtonArea.h"

# include <QKeyEvent>
#include <QList>
#include <QWidget>

#include "Utils/gl_matrices.h"

//forward declaration
class Scene;
class Camera;
class Context;

class View : public QGLViewer
{
	Q_OBJECT

public:
	View(Scene* s, QString name, Camera* c, QGLWidget* shareWidget = NULL, Context* context = NULL, QWidget* parent = NULL);
	~View();

	virtual void updateGL();
	void simpleUpdate();
	virtual void draw();
	virtual void init();

	void drawCameras(View* view);
	void drawText();
	void drawButtons();

	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent *e);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);

	virtual void resizeGL(int width, int height);

	void drawOverpaint(QPainter *painter);

//	virtual void paintGL() { update(); }
//	virtual void paintEvent(QPaintEvent *event);

	Scene* getScene() { return m_scene; }

	void setName(QString name) { m_name = name; }
	QString getName() { return m_name; }

	void enableLinking(bool b = true);
	void enableUnlinking(bool b = true);
	void enableCameraGesture(bool b = true);
	void enableSceneCameraGesture(bool b = true);
	void enableViewClose(bool b = true);

	Camera* currentCamera() { return m_currentCamera; }
	void setCurrentCamera(Camera* c);
	QList<Camera*> cameras() { return l_camera; }
	int countCameras() { return l_camera.size(); }

	void removeCamera(Camera* c);
	Camera* takeCamera(Camera* c);
	Camera* addCamera();
	void insertCamera(int index, Camera* c);
	void shareCamera(Camera* c, int index = 0);

//	void addUnlinkButton();
//	void removeUnlinkButton();

	void updateTextInfo();

	glm::mat4 getCurrentModelViewMatrix();
	glm::mat4 getCurrentProjectionMatrix();
	glm::mat4 getCurrentModelViewProjectionMatrix();

	void setCurrentModelViewMatrix(glm::mat4 mvm);
	void setCurrentProjectionMatrix(glm::mat4 pm);

	void addCustomViewButton(ViewButton* viewButton);
	void removeCustomViewButton(ViewButton* viewButton);

	void setShowButtons(bool b) { b_showButtons = b; }

protected:
	Scene* m_scene;

	QString m_name;

	QList<Camera*> l_camera;
	Camera* m_currentCamera;

	ViewButtonArea* m_buttonArea;

	ViewButton* m_linkButton;
	bool m_linkViewEnabled;
	ViewButton* m_unlinkButton;
	bool m_unlinkViewEnabled;
	ViewButton* m_cameraButton;
	bool m_cameraEnabled;
	ViewButton* m_cameraSceneButton;
	bool m_cameraSceneEnabled;
	ViewButton* m_closeViewButton;
	bool m_closeViewEnabled;

	QString m_textInfo;

	Context* m_context;

	bool b_showButtons;

public slots:
	void linkView();
	void unlinkView();
	void cameraGesture();
	void cameraSceneGesture();
	void closeView();

	void clickButton(ViewButton* viewButton);

private:
	bool b_destroyView;

signals:
	void currentCameraChanged(Camera* camera);
	void viewButtonClicked(ViewButton* viewButton);
};

#endif
