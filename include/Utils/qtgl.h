/*******************************************************************************
 * CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
 * version 0.1                                                                  *
 * Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
 *                                                                              *
 * This library is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU Lesser General Public License as published by the *
 * Free Software Foundation; either version 2.1 of the License, or (at your     *
 * option) any later version.                                                   *
 *                                                                              *
 * This library is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
 * for more details.                                                            *
 *                                                                              *
 * You should have received a copy of the GNU Lesser General Public License     *
 * along with this library; if not, write to the Free Software Foundation,      *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
 *                                                                              *
 * Web site: http://cgogn.u-strasbg.fr/                                         *
 * Contact information: cgogn@unistra.fr                                        *
 *                                                                              *
 *******************************************************************************/

#ifndef __QT_GL2__
#define __QT_GL2__

#include <GL/glew.h>
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include <stack>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_projection.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace CGoGN
{

namespace Utils
{

namespace QT
{

class SimpleQT;

class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(SimpleQT* cbs, QWidget *parent = 0);

	~GLWidget();

    QSize minimumSizeHint() const;

    QSize sizeHint() const;

protected:

	static float FAR_PLANE;

	SimpleQT* m_cbs;

	int m_current_button;
	QPoint clickPoint;
	int beginx;
	int beginy;
	int newModel;

	int moving;
	int scaling;
	int translating;

	float scalefactor;
	float foc;

	float m_obj_sc;
	glm::vec3 m_obj_pos;

	// width and height of windows
	int W;
	int H;

	int m_state_modifier;

	/**
	 * met a jour la matrice modelview
	 */
	void recalcModelView();

	/**
	 * recalcul le quaternion ainsi que le deplacement courant
	 * pour un nouveau centre de rotation
	 */
	void changeCenterOfRotation(const glm::vec3& newCenter);

public:
	void setParamObject(float width, float* pos);

	void initializeGL();

	void paintGL();

	void resizeGL(int width, int height);

public:
	void mousePressEvent(QMouseEvent* event);

	void mouseReleaseEvent(QMouseEvent* event);

	void mouseClickEvent(QMouseEvent* event);

	void mouseDoubleClickEvent(QMouseEvent* event);

	void mouseMoveEvent(QMouseEvent* event);

	void closeEvent(QCloseEvent *event);

	void keyPressEvent(QKeyEvent* event);

	void keyReleaseEvent(QKeyEvent* event);

	void wheelEvent(QWheelEvent* event);

	bool Shift() { return m_state_modifier & Qt::ShiftModifier; }

	bool Control() { return m_state_modifier & Qt::ControlModifier; }

	bool Alt() { return m_state_modifier & Qt::AltModifier; }

	int getHeight() const { return H; }
	int getWidth() const { return W; }

	/**
	 * set the focale distance (for a screen width of 2), default value is 1
	 */
	void setFocal(float df);

	/**
	 * get the focale distance
	 */
	float getFocal() const { return foc; }

	/**
	 * get current state
	 */
	int getStateModifier() const { return m_state_modifier ; }
	int getCurrentButton() const { return m_current_button ; }

	static float getFarPlane() { return FAR_PLANE ; }

	glm::vec3& getObjPos() ;

	void modelModified() { newModel = 1; }

protected:
	/**
	 * equivalent to old school glRotate
	 */
	void oglRotate(float angle, float x, float y, float z);

	/**
	 * equivalent to old school glTranslate
	 */
	void oglTranslate(float tx, float ty, float tz);

	/**
	 * equivalent to old school glScale
	 */
	void oglScale(float sx, float sy, float sz);

	/**
	 * get the focale distance
	 */
	float getScale() { return scalefactor / foc; }
};

} // namespace QT

} // namespace Utils

} // namespace CGoGN

#endif
