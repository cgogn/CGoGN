#include "pluginDialog.h"

#include <libxml2/libxml/tree.h>
#include <QFile>
#include <QRegExp>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QDomText>
#include <QMessageBox>
#include <QTextBrowser>

#include "system.h"
#include "plugin.h"


PluginDialog::PluginDialog(Window* parent, PluginHash* activePlugins) : QDialog(parent),
//	xmlFile(System::app_path.toStdString().c_str() +
//			QString("/state_save.xml")),
	activePlugins(activePlugins),
	parentWindow(parent),
	init(true)
{


//	if(!xmlFile.exists())
//	{
//		System::Error::code= System::Error::NO_PLUGIN_PATH_FILE;
//		System::Error::showError(this);
//		return;
//	}
//
//	if(!xmlFile.open(QIODevice::ReadOnly)){
//		System::Error::code= System::Error::ERROR_OPEN_PLUGIN_FILE;
//		System::Error::showError(this);
//		return;
//	}
//	if(!doc.setContent(&xmlFile)){
//		System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
//		xmlFile.close();
//		System::Error::showError(this);
//		return;
//	}
//	xmlFile.close();

	this->setupUi(this);

	treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addButton, SIGNAL(pressed()), this, SLOT(cb_addPlugins()));
	connect(removeButton, SIGNAL(pressed()), this, SLOT(cb_removePlugins()));
	connect(directoryButton, SIGNAL(pressed()), this, SLOT(cb_addPluginDirectory()));

	connect(treeWidget, SIGNAL(itemChanged (QTreeWidgetItem*, int)),
			this, SLOT(cb_activePlugin(QTreeWidgetItem*, int)));
	connect(treeWidget, SIGNAL(customContextMenuRequested (const QPoint &)),
			this, SLOT(customContextMenu(const QPoint &)));

	connect(this, SIGNAL(accepted()), this, SLOT(cb_acceptDialog()));

//	showPlugins();
	loadInfoPlugins();

	if(System::Error::code!=System::Error::SUCCESS){
		System::Error::showError(this);
	}

	init=false;
}

PluginDialog::~PluginDialog(){}

bool PluginDialog::loadInfoPlugins(){
	QFile xmlFile(System::app_path.toStdString().c_str() +
				QString("/state_save.xml"));

	if(!xmlFile.exists())
	{
		System::Error::code= System::Error::NO_PLUGIN_PATH_FILE;
		return false;
	}

	if(!xmlFile.open(QIODevice::ReadOnly)){
		System::Error::code= System::Error::ERROR_OPEN_PLUGIN_FILE;
		return false;
	}
	QDomDocument doc;
	if(!doc.setContent(&xmlFile)){
		System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
		xmlFile.close();
		return false;
	}
	xmlFile.close();



	QDomElement root= doc.documentElement();
	QDomElement plugins_node= root.firstChildElement("PLUGINS");
	if(!plugins_node.isNull()){
		QDomElement plugins_subNode= plugins_node.firstChildElement();
		while(!plugins_subNode.isNull()){
			if(plugins_subNode.tagName()=="DIR"){
				QString pluginDirPath= plugins_subNode.attribute("path","./plugins");
				QFileInfo fi(pluginDirPath);
				if(fi.exists() && fi.isDir()){
					QDir pluginDir(pluginDirPath);

					QTreeWidgetItem* dirItem= new QTreeWidgetItem(treeWidget, DIR);
					dirItem->setText(1,pluginDir.path());

					QStringList	filters, dirFiles;
					filters << "lib*.so";

					dirFiles= pluginDir.entryList(filters,QDir::Files);
					foreach(QString fileName, dirFiles){
						QString pluginName= fileName;
						pluginName.remove(0,3);
						pluginName.remove(pluginName.lastIndexOf(".so"),3);

						QTreeWidgetItem* item= new QTreeWidgetItem(dirItem, FILE_DIR);
						item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
						if(activePlugins->find(pluginName)!=activePlugins->end()){
							item->setCheckState(0,Qt::Checked);
						}
						else{
							item->setCheckState(0,Qt::Unchecked);
						}
						item->setText(1,pluginDir.absoluteFilePath(fileName));
					}
				}
			}
			else if(plugins_subNode.tagName()=="FILE"){
				QString pluginPath= plugins_subNode.attribute("path");
				if(!pluginPath.isEmpty()){
					QFileInfo fi(pluginPath);
					if(fi.exists() && pluginPath.left(3)=="lib" && fi.suffix()=="so"){
						QString pluginName= fi.baseName().remove(0,3);
						QTreeWidgetItem* item= new QTreeWidgetItem(treeWidget, FILE);
						item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
						if(activePlugins->find(pluginName)!=activePlugins->end()){
							item->setCheckState(0,Qt::Checked);
						}
						else{
							item->setCheckState(0,Qt::Unchecked);
						}
						item->setText(1,pluginPath);
					}
				}
			}

			plugins_subNode= plugins_subNode.nextSiblingElement();
		}
	}
	return true;
}

//void PluginDialog::showPlugins(){
//	QDomElement root= doc.documentElement();
//	QDomNode node= root.firstChild();
//	QDomElement balise;
//	bool pluginInfoFound=false;
//	while(!node.isNull() && !pluginInfoFound){
//		balise= node.toElement();
//
//		pluginInfoFound= (balise.tagName()=="PLUGIN_PATH");
//
//		node= node.nextSibling();
//	}
//	if(!pluginInfoFound){
//		System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
//		return;
//	}
//
//	if(treeWidget){
//		treeWidget->clear();
//	}
//
//	node= balise.firstChild();
//	while(!node.isNull()){
//		balise= node.toElement();
//
//		QString baliseName= balise.tagName();
//		if(baliseName=="DIR"){
//			showPluginsDir(QDir(balise.text()));
//		}
//		else if(baliseName=="FILE"){
//			QString pluginPath= balise.text();
//			QFile pluginFile(pluginPath);
//			if(!pluginFile.exists()){
//				System::Error::code= System::Error::BAD_PLUGIN_PATH_IN_FILE_f(pluginPath);
//			}
//			else{
//				QString pluginName= pluginFile.fileName();
//				pluginName.remove(0,3);
//				pluginName.remove(pluginName.lastIndexOf(".so"),3);
//
//				QTreeWidgetItem* item= new QTreeWidgetItem(treeWidget, FILE);
//				item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
//				if(activePlugins->find(pluginName)!=activePlugins->end()){
//					item->setCheckState(0,Qt::Checked);
//				}
//				else{
//					item->setCheckState(0,Qt::Unchecked);
//				}
//				item->setText(1,pluginPath);
//			}
//		}
//
//		node= node.nextSibling();
//	}
//
//}

void PluginDialog::showPluginsDir(QDir directory){
	if(!directory.exists()){
		System::Error::code= System::Error::BAD_PLUGIN_PATH_IN_FILE_f(directory.absolutePath());
	}

	QTreeWidgetItem* dirItem= new QTreeWidgetItem(treeWidget, DIR);
	dirItem->setText(1,directory.path());

	QStringList	filters, dirFiles;
	filters << "lib*.so";

	dirFiles= directory.entryList(filters, QDir::Files);

	foreach(QString file, dirFiles){
		QString pluginName= file;
		pluginName.remove(0,3);
		pluginName.remove(pluginName.lastIndexOf(".so"),3);

		QTreeWidgetItem* item= new QTreeWidgetItem(dirItem, FILE_DIR);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		if(activePlugins->find(pluginName)!=activePlugins->end()){
			item->setCheckState(0,Qt::Checked);
		}
		else{
			item->setCheckState(0,Qt::Unchecked);
		}
		item->setText(1,directory.absoluteFilePath(file));

	}

	if(dirFiles.isEmpty()){
		System::Error::code= System::Error::NO_PLUGIN_IN_DIR_f(directory.absolutePath());
	}
}

void PluginDialog::cb_addPlugins(){
	init=true;
	QStringList files = QFileDialog::getOpenFileNames(
							 this,
							 "Select one or more plugin to open",
							 "/home",
							 "Plugins (lib*.so)");
	if(!files.empty()){
//		QDomElement root= doc.documentElement();
//		QDomNode node= root.firstChild();
//		QDomElement balise;
//		bool pluginInfoFound=false;
//		while(!node.isNull() && !pluginInfoFound){
//			balise= node.toElement();
//
//			pluginInfoFound= (balise.tagName()=="PLUGIN_PATH");
//
//			node= node.nextSibling();
//		}
//		if(!pluginInfoFound){
//			System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
//			System::Error::showError(this);
//			return;
//		}
		foreach(QString pluginFile, files){
//			QDomElement newPluginFile= doc.createElement("FILE");
//			balise.appendChild(newPluginFile);
//
//			QDomText path= doc.createTextNode(pluginFile);
//			newPluginFile.appendChild(path);

			QTreeWidgetItem* item= new QTreeWidgetItem(treeWidget, FILE);
			item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
			item->setCheckState(0,Qt::Unchecked);
			item->setText(1,pluginFile);
		}
	}

	if(System::Error::code!=System::Error::SUCCESS){
		System::Error::showError(this);
	}
	init=false;
}

void PluginDialog::cb_addPluginDirectory(){
	init= true;
	QString dir = QFileDialog::getExistingDirectory(this, tr("Plugin Directory"),
	                                                 "/home",
	                                                 QFileDialog::ShowDirsOnly
	                                                 | QFileDialog::DontResolveSymlinks);
	if(!dir.isEmpty()){
//		QDomElement root= doc.documentElement();
//		QDomNode node= root.firstChild();
//		QDomElement balise;
//		bool pluginInfoFound=false;
//		while(!node.isNull() && !pluginInfoFound){
//			balise= node.toElement();
//
//			pluginInfoFound= (balise.tagName()=="PLUGIN_PATH");
//
//			node= node.nextSibling();
//		}
//		if(!pluginInfoFound){
//			System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
//			System::Error::showError(this);
//			return;
//		}
//
//		QDomElement newPluginDir= doc.createElement("DIR");
//		balise.appendChild(newPluginDir);
//
//		QDomText path= doc.createTextNode(dir);
//		newPluginDir.appendChild(path);

		showPluginsDir(QDir(dir));
	}

	if(System::Error::code!=System::Error::SUCCESS){
		System::Error::showError(this);
	}
	init=false;
}

void PluginDialog::cb_removePlugins(){
	QList<QTreeWidgetItem *> itemList= treeWidget->selectedItems();

	if(!itemList.isEmpty()){
//		QDomElement root= doc.documentElement();
//		QDomNode node= root.firstChild();
//		QDomElement balise;
//		bool pluginInfoFound=false;
//		while(!node.isNull() && !pluginInfoFound){
//			balise= node.toElement();
//
//			pluginInfoFound= (balise.tagName()=="PLUGIN_PATH");
//
//			node= node.nextSibling();
//		}
//		if(!pluginInfoFound){
//			System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
//			System::Error::showError(this);
//			return;
//		}
		foreach(QTreeWidgetItem* item, itemList){
			if(item->type()==FILE){
				if(item->checkState(0)==Qt::Checked){
					QMessageBox msgBox;
					msgBox.setText("Le plugin\n"
							"\t"+item->text(1)+
							" doit être désactivé avant de pouvoir être\n"
							"supprimé.");
					msgBox.exec();
				}
				else{
//					QDomNode subNode= balise.firstChild();
//					QDomElement subBalise;
//					while(!subNode.isNull()){
//						subBalise= subNode.toElement();
//
//						if(subBalise.tagName()=="FILE" && subBalise.text()==item->text(1)){
//							break;
//						}
//
//						subNode= subNode.nextSibling();
//					}
//					if(!subBalise.isNull()){
//						balise.removeChild(subNode);
//					}

					item= treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(item));
					delete item;
				}
			}
			else if(item->type()==FILE_DIR){
				QMessageBox msgBox;
				msgBox.setText("Le plugin\n"
						"\t"+item->text(1)+
						" fait partit d'un paquet/directory.\n"
						"Il ne peut être supprimé séparément.");
				msgBox.exec();
			}
			else if(item->type()==DIR){
				bool isAnyPluginActive= false;
				QTreeWidgetItem* fileItem;
				for(int i=0; (i<item->childCount() && !isAnyPluginActive); ++i){
					fileItem= item->child(i);
					if(fileItem->checkState(0)==Qt::Checked){
						isAnyPluginActive= true;
					}
				}
				if(isAnyPluginActive){
					QMessageBox msgBox;
					msgBox.setText("Un ou plusieurs plugins du dossier sont actifs\n"
							"Veuillez désactiver tous les plugins du paquet avant de supprimer celui-ci.");
					msgBox.exec();
				}
				else{
//					QDomNode subNode= balise.firstChild();
//					QDomElement subBalise;
//					while(!subNode.isNull()){
//						subBalise= subNode.toElement();
//
//						if(subBalise.tagName()=="DIR" && subBalise.text()==item->text(1)){
//							break;
//						}
//
//						subNode= subNode.nextSibling();
//					}
//					if(!subBalise.isNull()){
//						balise.removeChild(subNode);
//					}
					item= treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(item));
					delete item;
				}
			}
		}

	}

	if(System::Error::code!=System::Error::SUCCESS){
		System::Error::showError(this);
	}
}

void PluginDialog::cb_activePlugin(QTreeWidgetItem* item, int column){
	if(!init && column==0){
		if(item->checkState(0)==Qt::Checked){
//			QDomElement root= doc.documentElement();
//			QDomNode node= root.firstChild();
//			QDomElement balise;
//			bool activePluginFound=false;
//			while(!node.isNull() && !activePluginFound){
//				balise= node.toElement();
//
//				activePluginFound= (balise.tagName()=="ACTIVE_PLUGIN");
//
//				node= node.nextSibling();
//			}
//			if(!activePluginFound){
//				System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
//				System::Error::showError(this);
//				init=true; item->setCheckState(0,Qt::Unchecked), init=false;
//				return;
//			}

			QString pluginFile= item->text(1);
			QFileInfo pluginInfo(pluginFile);
			QString pluginName= pluginInfo.fileName();

			pluginName.remove(0,3);
			pluginName.remove(pluginName.lastIndexOf(".so"),3);

			if(activePlugins->find(pluginName)!= activePlugins->end()){
				System::Error::code= System::Error::PLUGIN_EXISTS_f(pluginName);
				System::Error::showError(this);
				init=true; item->setCheckState(0,Qt::Unchecked), init=false;
				return;
			}

			if(!parentWindow->loadPlugin(item->text(1))){
				init=true; item->setCheckState(0,Qt::Unchecked), init=false;
				return;
			}

//			QDomElement newActivePlugin= doc.createElement("PLUGIN");
//			balise.appendChild(newActivePlugin);
//
//			QDomText name= doc.createTextNode(pluginName);
//			newActivePlugin.appendChild(name);
		}
		else if(item->checkState(0)==Qt::Unchecked){
//			QDomElement root= doc.documentElement();
//			QDomNode node= root.firstChild();
//			QDomElement balise;
//			bool activePluginFound=false;
//			while(!node.isNull() && !activePluginFound){
//				balise= node.toElement();
//
//				activePluginFound= (balise.tagName()=="ACTIVE_PLUGIN");
//
//				node= node.nextSibling();
//			}
//			if(!activePluginFound){
//				System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
//				System::Error::showError(this);
//				return;
//			}

			QString pluginFile= item->text(1);
			QString pluginName= QFileInfo(pluginFile).fileName();

			pluginName.remove(0,3);
			pluginName.remove(pluginName.lastIndexOf(".so"),3);



			PluginHash::iterator it= activePlugins->find(pluginName);
			bool pluginHasDependencies= (it!=activePlugins->end())?(*it)->hasDependantPlugins():false;
			QStringList depList;
			if(pluginHasDependencies){
				depList= (*it)->getDependantPluginNames();
			}

			parentWindow->unloadPlugin(pluginName);

//			QDomNode subNode= balise.firstChild();
//			QDomElement subBalise;
//			activePluginFound= false;
//			while(!subNode.isNull()){
//				subBalise= subNode.toElement();
//
//				if( (activePluginFound= (subBalise.tagName()=="PLUGIN" &&
//									subBalise.text()==pluginName) ))
//				{
//					break;
//				}
//
//				subNode= subNode.nextSibling();
//			}
//			if(!activePluginFound){
//				System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
//				System::Error::showError(this);
//			}
//
//			if(!subBalise.isNull()){
//				balise.removeChild(subNode);
//			}

			if(pluginHasDependencies){
				QTreeWidgetItemIterator tree_it(treeWidget, QTreeWidgetItemIterator::Checked);
				while(*tree_it){
					QString depPlugPath=(*tree_it)->text(1);
					QString depPlugName= QFileInfo(depPlugPath).fileName();
					depPlugName.remove(0,3);
					depPlugName.remove(depPlugName.lastIndexOf(".so"),3);

					if(depList.contains(depPlugName)){
						init=true;
						(*tree_it)->setCheckState(0,Qt::Unchecked);
						init=false;
//						subNode= balise.firstChild();
//						while(!subNode.isNull()){
//							subBalise= subNode.toElement();
//
//
//							if(subBalise.tagName()=="PLUGIN" && subBalise.text()==depPlugName){
//								balise.removeChild(subNode);
//								break;
//							}
//
//							subNode= subNode.nextSibling();
//						}
					}
					++tree_it;
				}
			}
		}
	}
}

void PluginDialog::cb_acceptDialog(){
//	if(!xmlFile.open(QIODevice::WriteOnly)){
//		System::Error::code= System::Error::ERROR_OPEN_PLUGIN_FILE;
//		System::Error::showError(this);
//	}

//	out.setDevice(&xmlFile);
//
//	doc.save(out,2);
//	xmlFile.close();
	QStringList paths;

	int t= treeWidget->topLevelItemCount();
	for(int i=0; i<t; ++i){
		QTreeWidgetItem* item= treeWidget->topLevelItem(i);
		if(item->type()==FILE || item->type()==DIR){
			QString path= item->text(1);
			paths.push_back(path);
		}
	}

	if(!System::StateHandler::savePluginsInfo(parentWindow,activePlugins,paths)){
		System::Error::showError();
	}
}

void PluginDialog::customContextMenu(const QPoint & pos){
	std::cout << "context menu?" << std::endl;

	QPoint globalPos= treeWidget->mapToGlobal(pos);

	QTreeWidgetItem* item= treeWidget->itemAt(pos);
	if(item && (item->type()==FILE || item->type()==FILE_DIR)){
		std::cout << "hello?" << std::endl;
		item->setSelected(true);

		QMenu myMenu("Plus...", this);
		QAction pluginInfo("Informations sur le plugin", this);
		myMenu.addAction(&pluginInfo);
		connect(&pluginInfo, SIGNAL(triggered()), this, SLOT(showPluginInfo()));

		myMenu.exec(globalPos);
	}
}

void PluginDialog::showPluginInfo(){
	QTreeWidgetItem* item= treeWidget->currentItem();
	QString strUrl= item->text(1);

	System::Info::showPluginInfo(strUrl);
}
