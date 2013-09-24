#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "view.h"
#include "QGLViewer/camera.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;

class Camera : public qglviewer::Camera
{
	Q_OBJECT

	friend class View;

public:
	static unsigned int cameraCount;

	Camera(const QString& name, SCHNApps* s);
	~Camera();
	const QString& getName() const { return m_name; }

public slots:
	QString getName() { return m_name; }
	SCHNApps* getSCHNApps() const { return m_schnapps; }

	bool isUsed() const { return !l_views.empty(); }
	bool isShared()	const { return l_views.size() > 1; }

	qglviewer::Camera::Type getProjectionType() { return type(); }
	bool getDraw() const { return m_draw; }
	bool getDrawPath() const { return m_drawPath; }

	const QList<View*>& getLinkedViews() const { return l_views; }
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

private:
	void linkView(View* view);
	void unlinkView(View* view);

private slots:
	void frameModified();

public slots:
	void setProjectionType(int t);
	void setDraw(bool b);
	void setDrawPath(bool b);

signals:
	void projectionTypeChanged(int);
	void drawChanged(bool);
	void drawPathChanged(bool);

protected:
	QString m_name;
	SCHNApps* m_schnapps;

	QList<View*> l_views;

	bool m_draw;
	bool m_drawPath;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
