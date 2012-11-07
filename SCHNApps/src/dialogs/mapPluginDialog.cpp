#include "mapPluginDialog.h"

#include "window.h"
#include "plugin.h"
#include "mapHandler.h"

Q_DECLARE_METATYPE(Plugin*)
Q_DECLARE_METATYPE(MapHandler*)

MapPluginDialog::MapPluginDialog(Window* w,  MapHash* maps) : QDialog(w)
{
	l_plugin= w->plugins();

	this->setupUi(this);

	plugin_list->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(plugin_list, SIGNAL(customContextMenuRequested (const QPoint &)),
			this, SLOT(customPluginContextMenu(const QPoint &)));

	foreach(Plugin* plugin, l_plugin){
		QVariant var;
		var.setValue(plugin);

		QListWidgetItem* item= new QListWidgetItem(plugin_list);
		item->setText(plugin->getName());
		item->setData(Qt::UserRole, var);
	}

	for(MapHash::iterator it= maps->begin(); it!=maps->end();++it){
		QVariant var;
		var.setValue(it.value());

		QListWidgetItem* item= new QListWidgetItem(map_list);
		item->setText(it.key());
		item->setData(Qt::UserRole, var);
	}

	connect(plugin_list, SIGNAL(itemSelectionChanged()), this, SLOT(pluginSelection()));

	connect(map_list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(actionMap(QListWidgetItem*)));
}

void MapPluginDialog::pluginSelection(){
	Plugin* plugin= plugin_list->currentItem()->data(Qt::UserRole).value<Plugin*>();
	std::cout << "hu" << std::endl;
	QList<MapHandler*> l_pluginMap= plugin->workingMaps();
	int t= map_list->count();

	QListWidgetItem* item;
	for(int i=0; i<t; ++i){
		item= map_list->item(i);
		bool found= false;
		foreach(MapHandler* map, l_pluginMap){
			if(item->data(Qt::UserRole).value<MapHandler*>()==map){
				found= true;
				break;
			}
		}

		if(found){
			item->setIcon(QIcon(":icons/icons/check.png"));
		}
		else{
			item->setIcon(QIcon());
		}
	}
}

void MapPluginDialog::actionMap(QListWidgetItem* item){
	if(plugin_list->count()>0){
		QIcon icon= item->icon();
		if(icon.isNull()){
			QListWidgetItem* plugin_item= plugin_list->currentItem();
			if(item!=NULL){
				MapHandler* map= item->data(Qt::UserRole).value<MapHandler*>();
				Plugin* plugin=  plugin_item->data(Qt::UserRole).value<Plugin*>();
				if(map && plugin && plugin->addMap(map)){
					item->setIcon(QIcon(":icons/icons/check.png"));
				}
			}
		}
		else{
			QListWidgetItem* plugin_item= plugin_list->currentItem();
			if(item!=NULL){
				MapHandler* map= item->data(Qt::UserRole).value<MapHandler*>();
				Plugin* plugin=  plugin_item->data(Qt::UserRole).value<Plugin*>();
				if(map && plugin && plugin->takeMap(map)){
					item->setIcon(QIcon());
				}
			}
		}
	}
}

void MapPluginDialog::customPluginContextMenu(const QPoint & pos){
	QPoint globalPos= plugin_list->mapToGlobal(pos);

	QListWidgetItem* item= plugin_list->itemAt(pos);

	if(item){
		item->setSelected(true);

		QMenu myMenu("Plus...", this);
		QAction pluginInfo("Informations sur le plugin", this);
		myMenu.addAction(&pluginInfo);
		connect(&pluginInfo, SIGNAL(triggered()), this, SLOT(showPluginInfo()));

		myMenu.exec(globalPos);
	}
}

void MapPluginDialog::showPluginInfo(){
	QListWidgetItem* item= plugin_list->currentItem();
	if(item){
		Plugin* plugin= item->data(Qt::UserRole).value<Plugin*>();
		System::Info::showPluginInfo(plugin->getPluginFilePath());
	}
}


