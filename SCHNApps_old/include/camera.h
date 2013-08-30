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
	Q_OBJECT

public:
	static unsigned int cameraCount;

	Camera(const QString& name, Window* window);
	~Camera();
	const QString& getName() const { return m_name; }

public slots:
	QString getName() { return m_name; }
	void setName(const QString& name) { m_name = name; }

	Window* getWindow() const { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	bool isUsed() const { return !l_views.empty(); }
	bool isShared()	const { return l_views.size() > 1; }

	qglviewer::Camera::Type getType() { return type(); }
	void changeType(qglviewer::Camera::Type type);

	/*********************************************************
	 * CAMERA DRAWING
	 *********************************************************/

	bool getDraw() const { return m_draw; }
	void setDraw(bool b);

	bool getDrawPath() const { return m_drawPath; }
	void setDrawPath(bool b);

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

public:
	void linkView(View* view);
	void unlinkView(View* view);
	const QList<View*>& getLinkedViews() const { return l_views; }
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

	/*********************************************************
	 * SNAPSHOTS
	 *********************************************************/

public slots:
	void resetSnapCount() { m_snapCount = 0; }
	void saveSnapshot(QString snapPathName);

protected:
	QString m_name;
	Window* m_window;

	QList<View*> l_views;

	bool m_draw;
	bool m_drawPath;

	int m_snapCount;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
