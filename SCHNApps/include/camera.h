#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "view.h"
#include <QGLViewer/camera.h>
#include <QGLViewer/manipulatedCameraFrame.h>


#include "dll.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;

class SCHNAPPS_API Camera : public qglviewer::Camera
{
	Q_OBJECT

	friend class View;

public:
	static unsigned int cameraCount;

	Camera(const QString& name, SCHNApps* s);
	~Camera();
	const QString& getName() const { return m_name; }

	const glm::mat4& getTransfoMatrix() const;

public slots:
	QString getName() { return m_name; }
	SCHNApps* getSCHNApps() const { return m_schnapps; }

	bool isUsed() const { return !l_views.empty(); }
	bool isShared()	const { return l_views.size() > 1; }

	qglviewer::Camera::Type getProjectionType() { return type(); }
	bool getDraw() const { return b_draw; }
	bool getDrawPath() const { return b_drawPath; }

	const QList<View*>& getLinkedViews() const { return l_views; }
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

	void setProjectionType(int t);
	void setDraw(bool b);
	void setDrawPath(bool b);

	void enableViewsBoundingBoxFitting() { b_fitToViewsBoundingBox = true; }
	void disableViewsBoundingBoxFitting() { b_fitToViewsBoundingBox = false; }

	QString toString();
	void fromString(QString cam);

	void setScaling(float sx, float sy, float sz);

private:
	void linkView(View* view);
	void unlinkView(View* view);

private slots:
	void frameModified();
	void fitToViewsBoundingBox();

signals:
	void projectionTypeChanged(int);
	void drawChanged(bool);
	void drawPathChanged(bool);

protected:
	QString m_name;
	SCHNApps* m_schnapps;

	QList<View*> l_views;

	glm::mat4 m_transfoMatrix;

	bool b_draw;
	bool b_drawPath;

	bool b_fitToViewsBoundingBox;

};

} // namespace SCHNApps

} // namespace CGoGN

#endif
