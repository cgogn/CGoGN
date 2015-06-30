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

public slots:
	/**
	 * @brief get the name
	 * @return name
	 */
	QString getName();

	SCHNApps* getSCHNApps() const;

	/**
	 * @brief  test if camera is used by one view
	 * @return used / not used
	 */
	bool isUsed() const;

	/**
	 * @brief test is camera is used by several view
	 * @return shared / not shared (by view)
	 */
	bool isShared()	const;

	qglviewer::Camera::Type getProjectionType();
	/// drawing of camera (do not use)
	bool getDraw() const;
	bool getDrawPath() const;

	const QList<View*>& getLinkedViews() const;
	bool isLinkedToView(View* view) const;

	/**
	* @briefset the projection type
	* @param t 0:perspective / 1::orthogonal
	*/
	void setProjectionType(int t);

	/// drawing of camera (do not use)
	void setDraw(bool b);
	void setDrawPath(bool b);

	/**
	* @brief Enable the camera to update automatically with view bounding box
	*/
	void enableViewsBoundingBoxFitting();

	/**
	* @brief Enable the camera to update automatically with view bounding box
	*/
	void disableViewsBoundingBoxFitting();

	/**
	* @brief store position and rotationof camera into a string
	* @return the storage string
	*/
	QString toString();


	/**
	* @brief restore a camera from string storage
	* @param cam the string containing data
	*/
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
