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

	virtual void viewLinked(View* view) = 0;
	virtual void viewUnlinked(View* view) = 0;
	virtual void currentViewChanged(View* view) = 0;

	/*********************************************************
	 * MANAGE LINKED VIEWS
	 *********************************************************/

	bool linkView(View* view);
	void unlinkView(View* view);
	const QList<View*>& getLinkedViews() const { return l_views; }
	bool isLinkedToView(View* view) const { return l_views.contains(view); }

	/*********************************************************
	 * MANAGE SHADERS
	 *********************************************************/

	void registerShader(Utils::GLSLShader* shader);
	void unregisterShader(Utils::GLSLShader* shader);

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

//	QList<Plugin*> l_dependencies;
//	QList<Plugin*> l_dependantPlugins;

/*
	void addDependantPlugin(Plugin* p) { l_dependantPlugins.push_back(p); }
	void removeDependantPlugin(Plugin* p) { l_dependantPlugins.removeAll(p); }
	void removeAllDependantPlugins() { l_dependantPlugins.clear(); }
	bool hasDependantPlugins() { return !l_dependantPlugins.isEmpty(); }
	QStringList getDependantPluginNames()
	{
		QStringList l;
		for(QList<Plugin*>::iterator it = l_dependantPlugins.begin(); it != l_dependantPlugins.end(); ++it)
			l.push_back((*it)->getName());
		return l;
	}

	Plugin* addDependency(QString dependency)
	{
		Plugin* plugin;
		if((plugin = m_window->checkPluginDependency(dependency, this)))
		{
			l_dependencies.push_back(plugin);
			return plugin;
		}
		else
			return NULL;
	}

	template<typename T>
	T* getDependency(QString dependency)
	{
		return (T*)(this->addDependency(dependency));
	}

	virtual void removeDependencyLink(Plugin* p)
	{
		p->removeDependantPlugin(this);
		l_dependencies.removeAll(p);
	}
	virtual void removeAllDependencyLinks()
	{
		foreach(Plugin* p, l_dependencies)
			p->removeDependantPlugin(this);
		l_dependencies.clear();
	}
	void removeDependantLink(Plugin* p)
	{
		int i = l_dependantPlugins.indexOf(p);
		if(i >= 0)
			p->removeDependencyLink(this);
	}
	void removeAllDependantLinks()
	{
		foreach(Plugin* p, l_dependantPlugins)
			p->removeDependencyLink(this);
	}

	virtual void unloadDependantPlugins()
	{
		while(!l_dependantPlugins.empty())
		{
			if(l_dependantPlugins.back())
				m_window->unloadPlugin(l_dependantPlugins.back()->getName());
		}
	}
*/
};

} // namespace SCHNApps

} // namespace CGoGN

Q_DECLARE_INTERFACE(CGoGN::SCHNApps::Plugin, "CGoGN.SCHNapps.Plugin")

#endif
