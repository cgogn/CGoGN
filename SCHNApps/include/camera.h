#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "view.h"

//forward declaration

class Camera : public qglviewer::Camera
{
public:
	Camera(Window* window, View* v);
	Camera(Window* window, View* v, const qglviewer::Camera& c);
	~Camera();

	void setName(QString name) { m_name = name; }
	QString getName() { return m_name; }

	Window* getWindow() { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	bool isShared()	{ return l_views.size() > 1; }

	void draw();

	bool getDraw() { return m_draw; }
	void setDraw(bool b = true) { m_draw = b; }

	bool getDrawFarPlane() { return m_drawFarPlane; }
	void setDrawFarPlane(bool b = true) { m_drawFarPlane = b; }

	double getDrawScale() { return m_drawScale; }
	void setDrawScale(double s) { m_drawScale = s; }

	bool getDrawPath() { return m_drawPath; }
	void setDrawPath(bool b = true) { m_drawPath = b; }

	bool getDrawPathAxis() { return m_drawPathAxis; }
	void setDrawPathAxis(bool b = true) { m_drawPathAxis = b; }

	double getDrawPathScale() { return m_drawPathScale; }
	void setDrawPathScale(double s) { m_drawPathScale = s;}

	void linkView(View* view);
	void unlinkView(View* view);
	bool isLinkedWithView(View* view);

	void fitParamWith(View* view);

	void resetSnapCount() { m_snapCount = 0; }
	void saveSnapshot(QString snapPathName);

	void updateGL();

	void viewShowButton(bool b);

protected:
	static unsigned int cameraCount;

	QString m_name;
	Window* m_window;
	QList<View*> l_views;

	bool m_draw;
	bool m_drawFarPlane;
	double m_drawScale;

	bool m_drawPath;
	bool m_drawPathAxis;
	double m_drawPathScale;

	int m_snapCount;
};

#endif
