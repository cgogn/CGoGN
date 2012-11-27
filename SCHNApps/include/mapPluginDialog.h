#ifndef _MAPPLUGINDIALOG_H_
#define _MAPPLUGINDIALOG_H_


#include "ui_mapPluginDialog.h"

#include "types.h"

class Window;
class Plugin;
class MapHandler;

namespace CGoGN { class GenericMap; }

class MapPluginDialog : public QDialog, Ui::MPDialog{
	Q_OBJECT
public:
	MapPluginDialog(Window* w, MapHash* maps);

protected:
	QList<Plugin*> l_plugin;


public slots:
	void pluginSelection();
	void actionMap(QListWidgetItem* item);
	void customPluginContextMenu(const QPoint & pos);
	void showPluginInfo();
};


#endif
