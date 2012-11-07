#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "view.h"

//forward declaration


class Camera : public qglviewer::Camera{
public:
	Camera(View* v);
	Camera(View* v, Camera c);
	~Camera();

	bool isShared(){return (l_views.size()>1);}

	QString getName(){return m_name;}
	void setName(QString name){m_name= name;}

	void draw();
	void enableDraw(bool b=true){m_draw=b;}
	bool isDrawn(){return m_draw;}
	void enableDrawFarPlan(bool b=true){m_drawFarPlane=b;}
	bool isFarPlaneDrawn(){return m_drawFarPlane;}
	void setDrawScale(double s){m_drawScale=s;}
	double getDrawScale(){return m_drawScale;}

	bool isPathDrawn(){return m_drawUserPath;}
	void enablePathDraw(bool b=true){m_drawUserPath=b;}
	bool isPathAxisDrawn(){return m_drawUserPathAxis;}
	void enablePathAxisDrawn(bool b=true){m_drawUserPathAxis=b;}
	void setPathDrawScale(double s){m_drawUserPathScale= s;}
	double getPathDrawScale(){return m_drawUserPathScale;}

	void takenFrom(View* v);
	void sharedWith(View* v);

	void fitParamWith(View* v);

	void resetSnapCount(){m_snapCount=0;}
	void saveSnapshot(QString snapPathName);

	void updateGL();

	void viewShowButton(bool b);

protected:
	QList<View*> l_views;

	QString m_name;

	bool m_draw;
	bool m_drawFarPlane;
	double m_drawScale;

	bool m_drawUserPath;
	bool m_drawUserPathAxis;
	double m_drawUserPathScale;

	View* m_lastWorkingView;

	int m_snapCount;
};


#endif
