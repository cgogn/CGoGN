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

class PluginDialog : public QDialog, public Ui::Dialog{
	Q_OBJECT
public:
	PluginDialog(Window* parent=0, PluginHash* activePlugins=NULL);
	~PluginDialog();

protected:
//	QDomDocument doc;
//	QFile xmlFile;
//	QTextStream out;

	PluginHash* activePlugins;
	Window* parentWindow;

	bool loadInfoPlugins();
//	void showPlugins();

private:
	bool init;

	enum EntryType{DIR=1,FILE, FILE_DIR};


	void showPluginsDir(QDir directory);

protected slots:
	void cb_addPlugins();
	void cb_removePlugins();
	void cb_addPluginDirectory();

	void cb_activePlugin(QTreeWidgetItem* item, int column);

	void cb_acceptDialog();

	void customContextMenu(const QPoint & pos);

	void showPluginInfo();
};

#endif
