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


void SimpleQT::add_menu_entry(const std::string label, const char* method )
{
	QAction * action = new QAction(tr(label.c_str()), this);
	connect(action, SIGNAL(triggered()), this, method);
	m_appMenu->addAction(action);
}



//QMenu *fileMenu


/**
 * initialize popup menu
 */
void SimpleQT::init_app_menu()
{
	m_appMenu->clear();
}

//void SimpleQT::contextMenuEvent(QContextMenuEvent *event)
//{
//	QMenu menu(this);
//
//	for (std::vector<QAction*>::iterator act = m_popup_actions.begin(); act != m_popup_actions.end(); ++act)
//			menu.addAction(*act);
//
//	menu.exec(event->globalPos());
//}


}
}
}
