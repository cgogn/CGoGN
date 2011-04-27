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

#ifndef __SIMPLE_QT_GL2__
#define __SIMPLE_QT_GL2__

#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QWidget>
#include <QtGui>
#include "Utils/qtgl.h"
#include <set>
#include <string>
#include "Geometry/vector_gen.h"

namespace CGoGN { namespace Utils { class GLSLShader; } }
//namespace CGoGN { namespace Geom { class Vec3f; } }

namespace CGoGN
{

namespace Utils
{

namespace QT
{

// forward definition
class GLWidget;

class SimpleQT : public QMainWindow
{
	Q_OBJECT

public:
	SimpleQT();

	virtual ~SimpleQT();

	/**
	 * set the main widget of the dock
	 */
//	void setDockWidget(QWidget *wdg);
	void setDock(QDockWidget *dock);

	/**
	 * set the main widget of the dock
	 */
	QDockWidget* dockWidget();

	/**
	 * connect a widget/signal with a method of inherited object
	 * @param sender the widget sending signal
	 * @param signal use macro SIGNAL(qt_signal)
	 * @param method use macro SLOT(name_of_method(params))
	 */
	void setCallBack( const QObject* sender, const char* signal, const char* method);

	/**
	 * set window Title
	 */
	void windowTitle(const char* windowTitle);

	/**
	 * set dock Title
	 */
	void dockTitle(const char* dockTitle);

	/**
	 * draw a message in status bar
	 * @param msg message in C format (if NULL swp show/hidden)
	 * @param timeoutms number of ms message stay (0 = until next msg)
	 */
	void statusMsg(const char* msg, int timeoutms = 0);

	/**
	 * add an empty dock to the window
	 */
	QDockWidget* addEmptyDock();

	/**
	 * change dock visibility
	 */
	void visibilityDock(bool visible);

	/**
	 * change console visibility
	 */
	void visibilityConsole(bool visible);

	/**
	 * change dock visibility
	 */
	void toggleVisibilityDock();

	/**
	 * change console visibility
	 */
	void toggleVisibilityConsole();

	/**
	 * add an entry to popup menu
	 */
	void add_menu_entry(const std::string label, const char* method);

	/**
	 * re-initialize popup menu
	 */
	void init_app_menu();

	/**
	 * set the help message
	 */
	void setHelpMsg(const std::string& msg);

//	void contextMenuEvent(QContextMenuEvent *event);

protected:
	GLWidget *m_glWidget;

	QDockWidget *m_dock;

	QDockWidget *m_dockConsole;

	QTextEdit* m_textConsole;

	bool m_dockOn;

	// mouse & matrix
	glm::mat4 m_projection_matrix;
	glm::mat4 m_modelView_matrix;
	float m_curquat[4];
	float m_lastquat[4];
	float m_trans_x;
	float m_trans_y;
	float m_trans_z;

	QMenu* m_fileMenu;

	QMenu* m_appMenu;

	std::string m_helpString;

	void keyPressEvent(QKeyEvent *event);

	void keyReleaseEvent(QKeyEvent *e);

public:
	/**
	 * set width and pos center of object to draw
	 */
	void setParamObject(float width, float* pos) { m_glWidget->setParamObject(width, pos); }

	/**
	 * get the mouse position in GL widget
	 */
	void glMousePosition(int& x, int& y);

	/**
	 * get a ray (2 points) from a pick point in GL area
	 * @param x mouse position
	 * @param y mouse position
	 * @param rayA first computed point
	 * @param rayA second computed point
	 * @param radius radius on pixel for clicking precision
	 * @return the distance in modelview world corresponding to radius pixel in screen
	 */
	GLfloat getOrthoScreenRay(int x, int y, Geom::Vec3f& rayA, Geom::Vec3f& rayB, int radius = 4);

	/**
	 * current modelview matrix
	 */
	glm::mat4& modelViewMatrix () { return m_modelView_matrix; }

	/**
	 * current projection matrix
	 */
	glm::mat4& projectionMatrix () { return m_projection_matrix; }


	float* curquat() { return m_curquat; }
	float* lastquat() { return m_lastquat; }

	float& trans_x() { return m_trans_x; }
	float& trans_y() { return m_trans_y; }
	float& trans_z() { return m_trans_z; }

	/**
	 * shift key pressed ?
	 */
	bool Shift() const { return m_glWidget->Shift(); }

	/**
	 * control key pressed ?
	 */
	bool Control() const { return m_glWidget->Control(); }

	/**
	 * alt key pressed ?
	 */
	bool Alt() const  { return m_glWidget->Alt(); }

	/**
	 * height of OpenGL widget (for classic yy = H-y
	 */
	int getHeight() const { return m_glWidget->getHeight(); }

	/**
	 * console QTextEdit ptr
	 */
	QTextEdit* console() { return m_textConsole; }

	/**
	 * syncronization between SimpleQTs (experimental)
	 */
	void synchronize(SimpleQT* sqt);

	/**
	 * Register a shader as running in this Widget.
	 * Needed for automatic matrices update
	 */
	void registerShader(GLSLShader* ptr);

	/**
	 * Unregister a shader as running in this Widget.
	 */
	void unregisterShader(GLSLShader* ptr);

	/**
	 * GL initialization CB (context is ok)
	 */
	virtual void cb_initGL() {}

	/**
	 * what to you want to draw ? (need to be implemented)
	 */
	virtual void cb_redraw() = 0;

	/**
	 * Mouse button has been pressed
	 */
	virtual void cb_mousePress(int button, int x, int y) {}

	/**
	 * Mouse button has been released
	 */
	virtual void cb_mouseRelease(int button, int x, int y) {}

	/**
	 * the mouse has been move (with button still pressed)
	 */
	virtual void cb_mouseMove(int x, int y) {}

	/**
	 * key press CB (context is ok)
	 */
	virtual void cb_keyPress(int code) {}

	/**
	 * key releaase CB (context is ok)
	 */
	virtual void cb_keyRelease(int code) {}

	/**
	 * matrices need to be updated (context is ok)
	 */
	virtual void cb_updateMatrix();

	/**
	 * end of program, some things to clean ?
	 */
	virtual void cb_exit() {}

	/**
	 * Ask to Qt to update the GL widget.
	 * Equivalent of glutPostRedisplay()
	 */
	void updateGL();

	/**
	 * Ask to Qt to update matrices and then the GL widget.
	 */
	void updateGLMatrices();

	/**
	 * Open a file selector and return the filename
	 * @param title title of window
	 * @param dir base directory
	 * @param filters file filters (syntax: "label1 (filter1);; label2 (filter2);; ...")
	 */
	std::string selectFile(const std::string& title = "open file", const std::string& dir =  ".", const std::string& filters = "all (*.*)");

public slots:
	virtual void cb_New() { std::cerr << "callback not implemented" << std::endl; }
	virtual void cb_Open() { std::cerr << "callback not implemented" << std::endl; }
	virtual void cb_Save() { std::cerr << "callback not implemented" << std::endl; }
	virtual void cb_Quit() { exit(0); }
	void cb_about_cgogn();
	void cb_about();
	void cb_consoleOnOff() { toggleVisibilityConsole(); }
	void cb_consoleClear() { m_textConsole->clear(); }
};

} // namespace QT

} // namespace Utils

} // namespace CGoGN

#endif
