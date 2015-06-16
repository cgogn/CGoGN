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

//	const glm::mat4& getTransfoMatrix() const;

public slots:
	QString getName();
	SCHNApps* getSCHNApps() const;

	bool isUsed() const;
	bool isShared()	const;

	qglviewer::Camera::Type getProjectionType();
	bool getDraw() const;
	bool getDrawPath() const;

	const QList<View*>& getLinkedViews() const;
	bool isLinkedToView(View* view) const;

	void setProjectionType(int t);
	void setDraw(bool b);
	void setDrawPath(bool b);

	void enableViewsBoundingBoxFitting();
	void disableViewsBoundingBoxFitting();

	QString toString();
	void fromString(QString cam);
	
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

	bool b_draw;
	bool b_drawPath;

	bool b_fitToViewsBoundingBox;

};

} // namespace SCHNApps

} // namespace CGoGN

#endif
