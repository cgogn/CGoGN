#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <QtPlugin>
#include <QAction>

#include "types.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;

class Plugin : public QObject
{
public:
	Plugin();
	virtual ~Plugin();

	const QString& getName() { return m_name; }
	void setName(const QString& name) { m_name = name; }

	const QString& getFilePath() { return m_filePath; }
	void setFilePath(const QString& f) { m_filePath = f; }

	Window* getWindow() { return m_window; }
	void setWindow(Window* w) { m_window = w; }

	bool isUsed() const { return !l_views.empty(); }

	bool getProvidesRendering() { return b_providesRendering; }
	void setProvidesRendering(bool b) {	b_providesRendering = b; }

	virtual bool enable() = 0;
	virtual void disable() = 0;

	virtual void redraw(View* view) = 0;

	virtual void keyPress(View* view, int key) = 0;
	virtual void keyRelease(View* view, int key) = 0;
	virtual void mousePress(View* view, int button, int x, int y) = 0;
	virtual void mouseRelease(View* view, int button, int x, int y) = 0;
	virtual void mouseMove(View* view, int buttons, int x, int y) = 0;
	virtual void wheelEvent(View* view, int delta, int x, int y) = 0;

//	virtual void viewLinked(View* view) = 0;
//	virtual void viewUnlinked(View* view) = 0;
//	virtual void currentViewChanged(View* view) = 0;

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

	void linkView(View* view);
	void unlinkView(View* view);
	const QList<View*>& getLinkedViews() const { return l_views; }
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

	/*********************************************************
	 * MANAGE SHADERS
	 *********************************************************/

	void registerShader(Utils::GLSLShader* shader);
	void unregisterShader(Utils::GLSLShader* shader);
	const QList<Utils::GLSLShader*> getShaders() const { return l_shaders; }

	/*********************************************************
	 * MANAGE DOCK TABS
	 *********************************************************/

	bool addTabInDock(QWidget* tabWidget, const QString& tabText);
	void removeTabInDock(QWidget* tabWidget);
	const QList<QWidget*>& getTabWidgets() const { return l_tabWidgets; }

	/*********************************************************
	 * MANAGE MENU ACTIONS
	 *********************************************************/

	bool addMenuAction(const QString& menuPath, QAction* action);
	void removeMenuAction(QAction* action);

	/*********************************************************
	 * MANAGE TOOLBAR ACTIONS
	 *********************************************************/

	bool addToolbarAction(QAction* action);
	void removeToolbarAction(QAction* action);

protected:
	QString m_name;
	QString m_filePath;
	Window* m_window;

	bool b_providesRendering;

	QList<View*> l_views;
	QList<QWidget*> l_tabWidgets;
	QList<QAction*> l_menuActions;
	QList<QAction*> l_toolbarActions;

	QList<Utils::GLSLShader*> l_shaders;
};

} // namespace SCHNApps

} // namespace CGoGN

Q_DECLARE_INTERFACE(CGoGN::SCHNApps::Plugin, "CGoGN.SCHNapps.Plugin")

#endif
