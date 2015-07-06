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
/**
* @brief The camera class inherit from [qglviewer::Camera](http://libqglviewer.com/refManual/classqglviewer_1_1Camera.html)
* A camera object is generated with each new view (named camera_0 for view_O, etc..).
* Cameras can be shared among views.
*
* Python callable slots are tagged with [PYTHON]
*/
class SCHNAPPS_API Camera : public qglviewer::Camera
{
	Q_OBJECT

	friend class View;

public:
	/// camera counter for easy camera unique naming
	static unsigned int cameraCount;

	/**
	 * @brief Camera constructor
	 * @param name
	 * @param s
	 */
	Camera(const QString& name, SCHNApps* s);

	~Camera();

	/**
	 * @brief get the name of Camera object
	 * @return const ref on name
	 */
	const QString& getName() const { return m_name; }

public slots:
	/**
	 * @brief [PYTHON] get the name of Camera object
	 * @return name
	 */
	QString getName();

	/**
	 * @brief get the schnapps objet ptr
	 * @return the ptr
	 */
	SCHNApps* getSCHNApps() const;

	/**
	 * @brief [PYTHON] test if camera is used by one view
	 * @return used / not used
	 */
	bool isUsed() const;

	/**
	 * @brief [PYTHON] test is camera is used by several view
	 * @return shared / not shared (by view)
	 */
	bool isShared()	const;

	/**
	 * @brief get the projection type
	 * @return PERSPECTIVE or ORTHOGRAPHIC
	 */
	qglviewer::Camera::Type getProjectionType();

	/// is camera drawn ?
	bool getDraw() const;

	/// is camera path drawn ?
	bool getDrawPath() const;

	/**
	 * @brief get the list of views linked with the camera
	 * @warning not python callable
	 * @return the list
	 */
	const QList<View*>& getLinkedViews() const;

	/**
	 * @brief is the camera linked to theiven view
	 * @param view
	 * @return
	 */
	bool isLinkedToView(View* view) const;

	/**
	* @brief [PYTHON] set the projection type
	* @param t 0:perspective / 1::orthogonal
	*/
	void setProjectionType(int t);


	/// [PYTHON] draw (or not) the camera (do not use)
	void setDraw(bool b);

	/// [PYTHON] draw (or not) the camera path (do not use)
	void setDrawPath(bool b);

	/**
	* @brief [PYTHON] Enable the camera to update automatically with view bounding box
	*/
	void enableViewsBoundingBoxFitting();

	/**
	* @brief [PYTHON] Disable the camera to update automatically with view bounding box
	*/
	void disableViewsBoundingBoxFitting();

	/**
	* @brief [PYTHON] store position and rotationof camera into a string
	* @return the storage string
	*/
	QString toString();


	/**
	* @brief [PYTHON] restore a camera from string storage
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
	/// camera name
	QString m_name;

	/// pointer to schnapps object
	SCHNApps* m_schnapps;

	/// list of views that are using this camera
	QList<View*> l_views;


	bool b_draw;
	bool b_drawPath;

	/// fit the camera to the bounding box of view (xxxViewsBoundingBoxFitting())
	bool b_fitToViewsBoundingBox;

};

} // namespace SCHNApps

} // namespace CGoGN

#endif
