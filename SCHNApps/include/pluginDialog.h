#ifndef _PLUGINDIALOG_H_
#define _PLUGINDIALOG_H_

#include "ui_pluginDialog.h"

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QTextStream>

#include "types.h"

class Plugin;
class Window;

class PluginDialog : public QDialog, public Ui::Dialog
{
	Q_OBJECT

public:
	PluginDialog(Window* parent = 0, PluginHash* activePlugins = NULL);
	~PluginDialog();

protected:
	Window* parentWindow;
	PluginHash* activePlugins;

	bool restoreState();

private:
	bool init;

	enum EntryType
	{
		DIR = 1,
		FILE,
		FILE_DIR
	};

protected slots:
	void cb_addPlugins();
	void cb_removePlugins();
	void cb_addPluginsDirectory();

	void cb_togglePlugin(QTreeWidgetItem* item, int column);

	void customContextMenu(const QPoint & pos);

	void cb_acceptDialog();

	void showPluginInfo();
};

#endif
