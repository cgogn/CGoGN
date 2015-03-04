/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "Utils/GLSLShader.h"
#include "Utils/Qt/qtQGLV.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_precision.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <QTextEdit>
#include <QImage>
#include <QKeyEvent>
#include <QMouseEvent>


namespace CGoGN
{

namespace Utils
{

namespace QT
{


QGLView::QGLView(SimpleQGLV* ptr, QGLFormat& format, QWidget *parent):
	QGLViewer(format,parent),
	m_sqgl(ptr),m_state_modifier(0)
{
}

QGLView::~QGLView()
{
}

void QGLView::setParamObject(float width, float* pos)
{
	qglviewer::Vec bbMin(pos[0]-width/2.0f, pos[1]-width/2.0f, pos[2]-width/2.0f);
	qglviewer::Vec bbMax(pos[0]+width/2.0f, pos[1]+width/2.0f, pos[2]+width/2.0f);
	camera()->setSceneBoundingBox(bbMin, bbMax);
	camera()->showEntireScene();
}

void QGLView::setObjectBB(float* bbmin, float* bbmax)
{
	qglviewer::Vec bbMin(bbmin[0], bbmin[1], bbmin[2]);
	qglviewer::Vec bbMax(bbmax[0], bbmax[1], bbmax[2]);
	camera()->setSceneBoundingBox(bbMin, bbMax);
	camera()->showEntireScene();
}

QSize QGLView::minimumSizeHint() const
{
	return QSize(200, 200);
}

QSize QGLView::sizeHint() const
{
	return QSize(500, 500);
}

void QGLView::init()
{
	glewExperimental = GL_TRUE;
	glewInit();
	if (m_sqgl)
		m_sqgl->cb_initGL();
}

void QGLView::preDraw()
{
	GLdouble gl_mvm[16];
	camera()->getModelViewMatrix(gl_mvm);

	glm::mat4& mvm = m_sqgl->modelViewMatrix();
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			mvm[i][j] = (float)gl_mvm[i*4+j];
	}

	GLdouble gl_pm[16];
	camera()->getProjectionMatrix(gl_pm);
	glm::mat4& pm = m_sqgl->projectionMatrix();
	for(unsigned int i = 0; i < 4; ++i)
	{
		for(unsigned int j = 0; j < 4; ++j)
			pm[i][j] = (float)gl_pm[i*4+j];
	}

	m_sqgl->cb_updateMatrix();
}


void QGLView::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (m_sqgl)
	{
		Utils::GLSLShader::s_current_matrices = m_sqgl->matricesPtr();
		m_sqgl->cb_redraw();
	}
}


void QGLView::resizeGL(int w, int h)
{
	W = w;
	H = h;
	QGLViewer::resizeGL(w,h);
}


void QGLView::keyPressEvent(QKeyEvent* event)
{
	QGLViewer::keyPressEvent(event);

	if (m_sqgl)
	{
		m_state_modifier = event->modifiers();
		int k = event->key();
		if ( (k >= 65) && (k <= 91) && !(m_state_modifier & Qt::ShiftModifier) )
			k += 32;
		m_sqgl->cb_keyPress(k);
	}
}

void QGLView::keyReleaseEvent(QKeyEvent* event)
{
	QGLViewer::keyReleaseEvent(event);
	if (m_sqgl)
	{
		m_state_modifier = event->modifiers();
		int k = event->key();
		if ( (k >= 65) && (k <= 91) && !(m_state_modifier & Qt::ShiftModifier) )
			k += 32;
		m_sqgl->cb_keyRelease(k);
	}
}


void QGLView::mousePressEvent(QMouseEvent* event)
{
//	beginx = event->x();
//	beginy = event->y();
//	clickPoint = event->pos();
	m_current_button = event->button();
	if (m_sqgl)
		m_sqgl->cb_mousePress(event->button(), event->x(), getHeight() - event->y());

	QGLViewer::mousePressEvent(event);
}

void QGLView::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_sqgl)
		m_sqgl->cb_mouseRelease(event->button(), event->x(), getHeight() - event->y());
	m_current_button = 0;
	QGLViewer::mouseReleaseEvent(event);
}

void QGLView::mouseClickEvent(QMouseEvent* event)
{
	if (m_sqgl)
		m_sqgl->cb_mouseClick(event->button(), event->x(), getHeight() - event->y());
}

void QGLView::mouseMoveEvent(QMouseEvent* event)
{
	if (m_sqgl)
		m_sqgl->cb_mouseMove(m_current_button, event->x(), getHeight() - event->y());

	QGLViewer::mouseMoveEvent(event);
}

void QGLView::glMousePosition(int& x, int& y)
{
	QPoint xy = mapFromGlobal(QCursor::pos());
	x = xy.x();
	y = getHeight() - xy.y();
}

GLfloat QGLView::getOrthoScreenRay(int x, int y, Geom::Vec3f& rayA, Geom::Vec3f& rayB, int radius)
{
	// get Z from depth buffer
	int yy = y;
	GLfloat depth_t[25];
	glReadPixels(x-2, yy-2, 5, 5, GL_DEPTH_COMPONENT, GL_FLOAT, depth_t);

	GLfloat depth=0.0f;
	unsigned int nb=0;
	for (unsigned int i=0; i< 25; ++i)
	{
		if (depth_t[i] != 1.0f)
		{
			depth += depth_t[i];
			nb++;
		}
	}
	if (nb>0)
		depth /= float(nb);
	else
		depth = 0.5f;

	glm::i32vec4 viewport;
	glGetIntegerv(GL_VIEWPORT, &(viewport[0]));

	glm::vec3 win(x, yy, 0.0f);

	glm::vec3 P = glm::unProject(win, m_sqgl->modelViewMatrix(), m_sqgl->projectionMatrix(), viewport);

	rayA[0] = P[0];
	rayA[1] = P[1];
	rayA[2] = P[2];

	win[2] = depth;

	P = glm::unProject(win, m_sqgl->modelViewMatrix(), m_sqgl->projectionMatrix(), viewport);
	rayB[0] = P[0];
	rayB[1] = P[1];
	rayB[2] = P[2];

	if (depth == 1.0f)	// depth vary in [0-1]
		win[2] = 0.5f;

	win[0] += radius;
	P = glm::unProject(win, m_sqgl->modelViewMatrix(), m_sqgl->projectionMatrix(), viewport);
	Geom::Vec3f Q;
	Q[0] = P[0];
	Q[1] = P[1];
	Q[2] = P[2];

	// compute & return distance
	Q -= rayB;
	return float(Q.norm());
}


float QGLView::getWidthInWorld(unsigned int pixel_width, const Geom::Vec3f& center)
{

	glm::i32vec4 viewport;
	glGetIntegerv(GL_VIEWPORT, &(viewport[0]));

	glm::vec3 win = glm::project(glm::vec3(center[0],center[1],center[2]), m_sqgl->modelViewMatrix(), m_sqgl->projectionMatrix(), viewport);

	win[0]-= pixel_width/2;

	glm::vec3 P = glm::unProject(win, m_sqgl->modelViewMatrix(), m_sqgl->projectionMatrix(), viewport);

	win[0] += pixel_width;

	glm::vec3 Q = glm::unProject(win, m_sqgl->modelViewMatrix(), m_sqgl->projectionMatrix(), viewport);

	return glm::distance(P,Q);
}

} // namespace QT

} // namespace Utils

} // namespace CGoGN
