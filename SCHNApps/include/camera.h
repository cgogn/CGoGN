#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "view.h"
#include "QGLViewer/camera.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;

class Camera : public qglviewer::Camera
{
public:
	static unsigned int cameraCount;

	Camera(const QString& name, Window* window);
	~Camera();

	const QString& getName() const { return m_name; }
	void setName(const QString& name) { m_name = name; }

	Window* getWindow() const { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	bool isUsed() const { return !l_views.empty(); }
	bool isShared()	const { return l_views.size() > 1; }

	/*********************************************************
	 * CAMERA DRAWING
	 *********************************************************/

	void draw();

	bool getDraw() const { return m_draw; }
	void setDraw(bool b = true) { m_draw = b; }

	bool getDrawFarPlane() const { return m_drawFarPlane; }
	void setDrawFarPlane(bool b) { m_drawFarPlane = b; }

	double getDrawScale() const { return m_drawScale; }
	void setDrawScale(double s) { m_drawScale = s; }

	bool getDrawPath() const { return m_drawPath; }
	void setDrawPath(bool b) { m_drawPath = b; }

	bool getDrawPathAxis() const { return m_drawPathAxis; }
	void setDrawPathAxis(bool b) { m_drawPathAxis = b; }

	double getDrawPathScale() const { return m_drawPathScale; }
	void setDrawPathScale(double s) { m_drawPathScale = s;}

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

	void linkView(View* view);
	void unlinkView(View* view);
	const QList<View*>& getLinkedViews() const { return l_views; }
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

	void fitParamWith(View* view);

	/*********************************************************
	 * SNAPSHOTS
	 *********************************************************/

	void resetSnapCount() { m_snapCount = 0; }
	void saveSnapshot(QString snapPathName);

protected:
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

} // namespace SCHNApps

} // namespace CGoGN

#endif
