/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <GL/glew.h>
#include <iostream>
#include "Utils/qtgl.h"
#include "Utils/trackball.h"

#include "Utils/qtSimple.h"

namespace CGoGN
{

namespace Utils
{

namespace QT
{


float GLWidget::FAR_PLANE=500.0f;


GLWidget::GLWidget(SimpleQT* cbs, QWidget *parent) :
	QGLWidget(QGLFormat(QGL::Rgba | QGL::DoubleBuffer| QGL::DepthBuffer), parent),
	m_cbs(cbs),
	m_state_modifier(0)
{
	makeCurrent();
	glewInit();

	newModel = 1;
	m_cbs->trans_x() = 0.;
	m_cbs->trans_y() = 0.;
	float f = FAR_PLANE;
	m_cbs->trans_z() = -f / 5.0f;
	foc = 2.0f;

	// init trackball
	trackball(m_cbs->curquat(), 0.0f, 0.0f, 0.0f, 0.0f);
}

GLWidget::~GLWidget()
{
}

void GLWidget::setParamObject(float width, float* pos)
{
	m_obj_sc = ((FAR_PLANE / 5.0f) / foc) / width;
	m_obj_pos = glm::vec3(-pos[0], -pos[1], -pos[2]);
}

void  GLWidget::setFocal(float df)
{
	if (df > 5.0f)
		df = 5.0f;
	if (df < 0.2f)
		df = 0.2f;

	m_obj_sc *= foc / df;

	foc = df;
	resizeGL(W, H);
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(200, 200);
}

QSize GLWidget::sizeHint() const
{
    return QSize(500, 500);
}

void GLWidget::recalcModelView()
{
	glm::mat4 m;

	oglPushModelViewMatrix();

	// positionne l'objet / mvt souris
	oglTranslate(m_cbs->trans_x(), m_cbs->trans_y(), m_cbs->trans_z());

	// tourne l'objet / mvt souris
	build_rotmatrixgl3(m, m_cbs->curquat());
	// update matrice
	m_cbs->modelViewMatrix() *= m;

	// transfo pour que l'objet soit centre et a la bonne taille
	oglScale(m_obj_sc, m_obj_sc, m_obj_sc);
	oglTranslate(m_obj_pos[0], m_obj_pos[1], m_obj_pos[2]);

	// ajout transformation in screen
	m_cbs->modelViewMatrix()*= m_cbs->transfoMatrix();

	newModel = 0;

	if (m_cbs)
		m_cbs->cb_updateMatrix();

	oglPopModelViewMatrix();
}

void GLWidget::initializeGL()
{
	glEnable(GL_DEPTH_TEST);

	if (m_cbs)
		m_cbs->cb_initGL();
}

void GLWidget::resizeGL(int w, int h)
{
	if (w > 0) W = w;
	if (h > 0) H = h;

	glViewport(0, 0, W, H);
	float f = FAR_PLANE;
	m_cbs->projectionMatrix() = glm::frustum(-1.0f, 1.0f, -1.0f * H / W, 1.0f * H / W, foc, f);

	recalcModelView();
}

void GLWidget::paintGL()
{
	if (newModel)
	    recalcModelView();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (m_cbs)
		m_cbs->cb_redraw();
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
	beginx = event->x();
	beginy = event->y();
	clickPoint = event->pos();
	m_current_button = event->button();

	if (m_cbs)
		m_cbs->cb_mousePress(event->button(), event->x(), getHeight() - event->y());
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_cbs)
		m_cbs->cb_mouseRelease(event->button(), event->x(), getHeight() - event->y());

	if(event->pos() == clickPoint)
		mouseClickEvent(event) ;
}

void GLWidget::mouseClickEvent(QMouseEvent* event)
{
	if (m_cbs)
		m_cbs->cb_mouseClick(event->button(), event->x(), getHeight() - event->y());
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
	// move object only if no special keys pressed
	if (!(m_state_modifier & ( Qt::ShiftModifier|Qt::ControlModifier|Qt::AltModifier| Qt::MetaModifier)))
	{
		int x = event->x();
		int y = event->y();

		switch (m_current_button)
		{
			case Qt::RightButton:
			{
				float wl;
				if (m_cbs->trans_z() > -20.0f)
					wl = 20.0f / foc;
				else
					wl = -2.0f * m_cbs->trans_z() / foc;
				m_cbs->trans_x() += wl / W * (x - beginx);
				m_cbs->trans_y() += wl / H * (beginy - y);
			}
				break;
			case Qt::MidButton:
			{
				float wl = -0.5f * FAR_PLANE / foc;
				m_cbs->trans_z() -= wl / W * (x - beginx);
				m_cbs->trans_z() -= wl / H * (y - beginy);
			}
				break;
			case Qt::LeftButton:
			{
				trackball(
					m_cbs->lastquat(),
					(2.0f * beginx - W) / W,
					(H - 2.0f * beginy) / H,
					(2.0f * x - W) / W,(H - 2.0f * y) / H
				);
				add_quats(m_cbs->lastquat(), m_cbs->curquat(), m_cbs->curquat());
			}
				break;
		}

		beginx = x;
		beginy = y;
		newModel = 1;
		updateGL();
	}

	if (m_cbs)
		m_cbs->cb_mouseMove(event->button(), event->x(), getHeight() - event->y());
}

void GLWidget::wheelEvent ( QWheelEvent * event )
{
	float wl = -0.05f * FAR_PLANE / foc;

	if (event->delta() > 0)
		m_cbs->trans_z() += wl;
	else
		m_cbs->trans_z() -= wl;

	newModel = 1;
	updateGL();
}

void GLWidget:: keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        close();
    else
    	QWidget::keyPressEvent(event);

    m_state_modifier = event->modifiers();

    int k = event->key();
    if ( (k >= 65) && (k <= 91) && !(event->modifiers() & Qt::ShiftModifier) )
    	k += 32;

    if (m_cbs)
		m_cbs->cb_keyPress(k);
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
	QWidget::keyReleaseEvent(event);

	m_state_modifier = event->modifiers();

    int k = event->key();
    if ( (k >= 65) && (k <= 91) && (event->modifiers() != Qt::ShiftModifier) )
    	k += 32;

	if (m_cbs)
		m_cbs->cb_keyRelease(k);
}

void GLWidget::oglRotate(float angle, float x, float y, float z)
{
	m_cbs->modelViewMatrix() = glm::rotate(m_cbs->modelViewMatrix(), angle, glm::vec3(x,y,z));
}

void GLWidget::oglTranslate(float tx, float ty, float tz)
{
	m_cbs->modelViewMatrix() = glm::translate(m_cbs->modelViewMatrix(), glm::vec3(tx,ty,tz));
}

void GLWidget::oglScale(float sx, float sy, float sz)
{
	m_cbs->modelViewMatrix() = glm::scale(m_cbs->modelViewMatrix(), glm::vec3(sx,sy,sz));
}

void GLWidget::oglPushModelViewMatrix()
{
	m_stack_mv.push(m_cbs->modelViewMatrix());
}

bool GLWidget::oglPopModelViewMatrix()
{
	if (m_stack_mv.empty())
		return false;
	m_cbs->modelViewMatrix() = m_stack_mv.top();
	m_stack_mv.pop();
	return true;
}

} // namespace QT

} // namespace Utils

} // namespace CGoGN
