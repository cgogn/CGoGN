#ifndef _PLUGINSVIEWDIALOG_H_
#define _PLUGINSVIEWDIALOG_H_

#include "ui_pluginsViewDialog.h"

class Window;
class View;
class Plugin;

class PluginsViewDialog : public QDialog, Ui::PluginsViewDialog
{
	Q_OBJECT

public:
	PluginsViewDialog(Window* window, View* view);
	~PluginsViewDialog();

private:
	Window* m_window;
	View* m_view;

	void selectCurrentPlugins();

public slots:
	void cb_selectedPluginsChanged();
	void cb_addPluginToList(Plugin* p);
	void cb_removePluginFromList(Plugin* p);
};

#endif
