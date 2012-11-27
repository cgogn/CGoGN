#ifndef _VIEW_H_
#define _VIEW_H_

#include <iostream>

#include "types.h"
#include "viewButtonArea.h"

#include <QKeyEvent>
#include <QList>
#include <QWidget>
#include <QGLContext>

#include "Utils/gl_matrices.h"

class Window;
class Scene;
class Camera;
//class Context;

class View : public QGLViewer
{
	Q_OBJECT

public:
	View(const QString& name, Window* w, QWidget* parent);
	~View();

	const QString& getName() { return m_name; }
	void setName(const QString& name) { m_name = name; }

	Window* getWindow() { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	Scene* getScene() { return m_scene; }
	void setScene(Scene* s) { m_scene = s; }

	Camera* getCurrentCamera() { return m_currentCamera; }
	void setCurrentCamera(Camera* c)
	{
		m_currentCamera = c;
		updateTextInfo();
		updateGL();
	}

	QGLContext* getContext() { return m_context; }
	void setContext(QGLContext* c) { m_context = c; }

	virtual void initGL();
	virtual void updateGL();
	virtual void draw();
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

	void enableLinking(bool b = true);
	void enableUnlinking(bool b = true);
	void enableCameraGesture(bool b = true);
	void enableSceneCameraGesture(bool b = true);
	void enableViewClose(bool b = true);

//	void addUnlinkButton();
//	void removeUnlinkButton();

	void updateTextInfo();

	glm::mat4 getCurrentModelViewMatrix();
	glm::mat4 getCurrentProjectionMatrix();
	glm::mat4 getCurrentModelViewProjectionMatrix();

	void setCurrentModelViewMatrix(const glm::mat4& mvm);
	void setCurrentProjectionMatrix(const glm::mat4& pm);

//	void addViewButton(ViewButton* viewButton);
//	void removeViewButton(ViewButton* viewButton);

//	void setShowButtons(bool b) { b_showButtons = b; }

protected:
	static unsigned int viewCount;

	QString m_name;
	Window* m_window;
	QGLContext* m_context;

	Scene* m_scene;
	Camera* m_currentCamera;

//	ViewButtonArea* m_buttonArea;

//	ViewButton* m_linkButton;
//	bool m_linkViewEnabled;
//	ViewButton* m_unlinkButton;
//	bool m_unlinkViewEnabled;
//	ViewButton* m_cameraButton;
//	bool m_cameraEnabled;
//	ViewButton* m_cameraSceneButton;
//	bool m_cameraSceneEnabled;
//	ViewButton* m_closeViewButton;
//	bool m_closeViewEnabled;

//	bool b_showButtons;

	QString m_textInfo;
	bool b_drawText;

//public slots:
//	void linkView();
//	void unlinkView();
//	void cameraGesture();
//	void cameraSceneGesture();
//	void closeView();

//signals:
//	void currentCameraChanged(Camera* camera);
};

#endif
