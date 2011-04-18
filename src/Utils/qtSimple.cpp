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

#include "Utils/qtSimple.h"
#include "Utils/qtgl.h"
#include "Utils/GLSLShader.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_precision.hpp"

namespace CGoGN
{
namespace Utils
{
namespace QT
{


SimpleQT::SimpleQT():
m_dock(NULL)
{

	m_glWidget = new GLWidget(this);
	setCentralWidget(m_glWidget);
	setWindowTitle(tr("CGoGN"));

	m_fileMenu =menuBar()->addMenu(tr("&File"));

	QAction* action= new QAction(tr("New"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(cb_New()));
	m_fileMenu->addAction(action);

	action= new QAction(tr("Open"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(cb_Open()));
	m_fileMenu->addAction(action);

	action= new QAction(tr("Save"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(cb_Save()));
	m_fileMenu->addAction(action);

	m_fileMenu->addSeparator();

	action= new QAction(tr("Quit"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(cb_Quit()));
	m_fileMenu->addAction(action);

	m_appMenu = menuBar()->addMenu(tr("&Application"));


	QMenu* m_helpMenu = menuBar()->addMenu(tr("&Help"));

	action= new QAction(tr("About"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(cb_about()));
	m_helpMenu->addAction(action);

	action= new QAction(tr("About CGoGN"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(cb_about_cgogn()));
	m_helpMenu->addAction(action);

}

SimpleQT::~SimpleQT()
{
	delete m_glWidget; // ??
}



void SimpleQT::cb_about_cgogn()
{
	QString str("CGoGN:\nCombinatorial and Geometric modeling\n"
			"with Generic N-dimensional Maps\n"
			"Web site: https://cgogn.u-strasbg.fr \n"
			"Contact information: cgogn@unistra.fr");
	QMessageBox::about(this, tr("About CGoGN"),str);
}

void SimpleQT::cb_about()
{
   QMessageBox::about(this, tr("About App"), m_helpString.c_str());
}

void SimpleQT::setHelpMsg(const std::string& msg)
{
	m_helpString = msg;
}



void SimpleQT::glMousePosition(int& x, int& y)
{
	QPoint xy = m_glWidget->mapFromGlobal(QCursor::pos());
	x = xy.x();
	y = m_glWidget->getHeight()- xy.y();
}

QDockWidget* SimpleQT::addEmptyDock()
{
		m_dock = new QDockWidget(tr("Control"), this);
		m_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea|Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
		m_dock->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
		addDockWidget(Qt::RightDockWidgetArea, m_dock);

		m_dock->hide();
		m_dockOn=false;

		return m_dock;
}

void SimpleQT::visibilityDock(bool visible)
{
	if (visible)
	{
		m_dock->show();
		m_dockOn=true;
	}
	else
	{
		m_dock->hide();
		m_dockOn=false;
	}
}


void SimpleQT::windowTitle(const char* windowTitle)
{
	setWindowTitle(tr(windowTitle));
}

void SimpleQT::dockTitle(const char* dockTitle)
{
	if (m_dock)
		m_dock->setWindowTitle(tr(dockTitle));
}

void SimpleQT::statusMsg(const char* msg)
{
	if (msg)
	{
		QString message = tr(msg);
		statusBar()->showMessage(message);
	}
	else
		statusBar()->hide();
}

void SimpleQT::setCallBack( const QObject* sender, const char* signal, const char* method)
{
	connect(sender, signal, this, method);
}


void SimpleQT::keyPressEvent(QKeyEvent *e)
{
	if ((e->key() == Qt::Key_Return) && m_dock != NULL) // m for menu
	{

		if (m_dockOn)
		{
			m_dock->hide();
			m_dockOn=false;
		}
		else
		{
			m_dock->show();
			m_dockOn=true;
		}
	}

    if (e->key() == Qt::Key_Escape)
    {
    	close();
    }
    else
        QWidget::keyPressEvent(e);

    m_glWidget->keyPressEvent(e);
}


void SimpleQT::keyReleaseEvent(QKeyEvent *e)
{
    m_glWidget->keyReleaseEvent(e);
}



void SimpleQT::setDock(QDockWidget *dock)
{
	m_dock = dock;
	addDockWidget(Qt::RightDockWidgetArea, m_dock);
	m_dockOn=true;
	m_dock->show();
}


QDockWidget* SimpleQT::dockWidget()
{
	return m_dock;
}


void SimpleQT::updateGL()
{
	m_glWidget->updateGL();
}

void SimpleQT::updateGLMatrices()
{
	cb_updateMatrix();
	m_glWidget->updateGL();
}

void SimpleQT::cb_updateMatrix()
{
	for (std::set< std::pair<void*, GLSLShader*> >::iterator it=GLSLShader::m_registredRunning.begin(); it!=GLSLShader::m_registredRunning.end(); ++it)
	{
		if ((it->first == NULL) || (it->first == this))
		{
			it->second->updateMatrices(m_projection_matrix, m_modelView_matrix);
		}
	}

}

void SimpleQT::synchronize(SimpleQT* sqt)
{
	m_projection_matrix = sqt->m_projection_matrix;
	m_modelView_matrix = sqt->m_modelView_matrix;
	for (unsigned int i=0; i< 4; ++i)
	{
		m_curquat[i] = sqt->m_curquat[i];
		m_lastquat[i] = sqt->m_lastquat[i];
	}
	m_trans_x = sqt->trans_x();
	m_trans_y = sqt->trans_y();
	m_trans_z = sqt->trans_z();

	SimpleQT::cb_updateMatrix();

	m_glWidget->modelModified();
	m_glWidget->updateGL();
}



void SimpleQT::add_menu_entry(const std::string label, const char* method )
{
	QAction * action = new QAction(tr(label.c_str()), this);
	connect(action, SIGNAL(triggered()), this, method);
	m_appMenu->addAction(action);
}


/**
 * initialize app menu
 */
void SimpleQT::init_app_menu()
{
	m_appMenu->clear();
}

void SimpleQT::registerRunning(GLSLShader* ptr)
{
	GLSLShader::m_registredRunning.insert(std::pair<void*,GLSLShader*>(this,ptr));
}

void SimpleQT::unregisterRunning(GLSLShader* ptr)
{
	GLSLShader::m_registredRunning.erase(std::pair<void*,GLSLShader*>(this,ptr));
}


GLfloat SimpleQT::getOrthoScreenRay(int x, int y, Geom::Vec3f& rayA, Geom::Vec3f& rayB, int radius)
{
	// get Z from depth buffer
//	int yy =  m_glWidget->getHeight() - y;
	int yy = y;
	GLfloat depth;
	glReadPixels(x, yy,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);


	glm::i32vec4 viewport;
	glGetIntegerv(GL_VIEWPORT,&(viewport[0]));

	glm::vec3 win(x, yy, 0.0f);

	glm::vec3 P = glm::unProject(win, m_modelView_matrix, m_projection_matrix, viewport);

	rayA[0] = P[0];
	rayA[1] = P[1];
	rayA[2] = P[2];

	win[2]=depth;

	P =  glm::unProject(win, m_modelView_matrix, m_projection_matrix, viewport);
	rayB[0] = P[0];
	rayB[1] = P[1];
	rayB[2] = P[2];

	if (depth == 1.0f)	// depth vary in [0-1]
		depth = 0.5f;

	win[0] += radius;

	P =  glm::unProject(win, m_modelView_matrix, m_projection_matrix, viewport);
	Geom::Vec3f Q;
	Q[0] = P[0];
	Q[1] = P[1];
	Q[2] = P[2];

	// compute & return distance
	Q -= rayB;
	return float(Q.norm());
}



}
}
}
