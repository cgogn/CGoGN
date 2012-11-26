#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "view.h"

//forward declaration

class Camera : public qglviewer::Camera
{
public:
	Camera(View* v);
	Camera(View* v, Camera c);
	~Camera();

	bool isShared()	{ return l_views.size() > 1; }

	QString getName() { return m_name; }
	void setName(QString name) { m_name = name; }

	void draw();

	bool getDraw() { return m_draw; }
	bool getDrawFarPlane() { return m_drawFarPlane; }
	double getDrawScale() { return m_drawScale; }
	bool getDrawPath() { return m_drawPath; }
	bool getDrawPathAxis() { return m_drawPathAxis; }
	double getDrawPathScale() { return m_drawPathScale; }

	void setDraw(bool b = true) { m_draw = b; }
	void setDrawFarPlane(bool b = true) { m_drawFarPlane = b; }
	void setDrawScale(double s) { m_drawScale = s; }
	void setDrawPath(bool b = true) { m_drawPath = b; }
	void setDrawPathAxis(bool b = true) { m_drawPathAxis = b; }
	void setDrawPathScale(double s) { m_drawPathScale = s;}

	void takenFrom(View* v);
	void sharedWith(View* v);
	void fitParamWith(View* v);

	void resetSnapCount() { m_snapCount = 0; }
	void saveSnapshot(QString snapPathName);

	void updateGL();

	void viewShowButton(bool b);

protected:
	QList<View*> l_views;

	QString m_name;

	bool m_draw;
	bool m_drawFarPlane;
	double m_drawScale;

	bool m_drawPath;
	bool m_drawPathAxis;
	double m_drawPathScale;

	View* m_lastWorkingView;

	int m_snapCount;
};

#endif
