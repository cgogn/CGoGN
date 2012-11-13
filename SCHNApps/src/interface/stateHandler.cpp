#include "system.h"


#include "window.h"
#include "plugin.h"
#include "camera.h"
#include "splitArea.h"

#include <QFileInfo>
#include <QDir>
#include <QTextStream>

bool System::StateHandler::saveState(Window* window, PluginHash* pluginHash, SceneHash* sceneHash, SplitArea* splitArea){
	QList<Plugin*> plugins;
	QList<Scene*> scenes;


	Dialog::QuestionSessionDialog qsd(window);
	qsd.label->setText(QString::fromUtf8("Voulez-vous sauvergarder cette session (Plugins chargé, scènes, vues et caméras)?"));
	qsd.exec();
	if(qsd.result()!=QDialog::Accepted){
		unsaveState();

		return true;
	}


	QFile xmlFile(System::app_path.toStdString().c_str() +
			QString("/state_save.xml"));

	if(!xmlFile.exists())
	{
		if(!xmlFile.open(QIODevice::ReadWrite)){
			System::Error::code= System::Error::NO_PLUGIN_PATH_FILE;
			System::Error::showError(window);
			return false;
		}
	}
	else if(!xmlFile.open(QIODevice::ReadOnly)){
		System::Error::code= System::Error::ERROR_OPEN_PLUGIN_FILE;
		System::Error::showError(window);
		return false;
	}
	QDomDocument doc;
	if(!doc.setContent(&xmlFile)){
		System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
		xmlFile.close();
		System::Error::showError(window);
		return false;
	}
	xmlFile.close();

	QDomElement root= doc.documentElement();
	QDomElement plugins_node= root.firstChildElement("PLUGINS");
	if(plugins_node.isNull()){
		plugins_node= doc.createElement("PLUGINS");
		root.appendChild(plugins_node);
	}

	QDomElement plugins_subNode= plugins_node.firstChildElement();
	while(!plugins_subNode.isNull()){
		if(plugins_subNode.tagName()=="FILE"){
			QString pluginPath= plugins_subNode.attribute("path");
			if(!pluginPath.isEmpty()){
				if(plugins_subNode.attribute("active")=="yes"){
					QString pluginName= ((QFileInfo(pluginPath)).baseName()).replace(0,3,"");
					PluginHash::iterator it;
					if((it=pluginHash->find(pluginName))!=pluginHash->end() && !plugins.contains((*it))){
						plugins.push_back(*it);
					}
					else if(it==pluginHash->end()){
						plugins_subNode.setAttribute("active", "no");
					}
				}
			}
		}
		else if(plugins_subNode.tagName()=="DIR"){
			std::cout << "> Dir" << std::endl;
			QString pluginDirPath= plugins_subNode.attribute("path");
			if(!pluginDirPath.isEmpty()){
				std::cout << "> Dir path" << std::endl;
				QDir pluginDir(pluginDirPath);
				if(pluginDir.exists()){

					std::cout << ">>> exists" << std::endl;
					QDomElement pluginDir_subNode= plugins_subNode.firstChildElement("ACTIVE");
					while(!pluginDir_subNode.isNull()){
						std::cout << "  _t> subnode" << std::endl;
						QString activePluginName= pluginDir_subNode.text();
						std::cout << activePluginName.toStdString() << std::endl;
						PluginHash::iterator it;
						if((it=pluginHash->find(activePluginName))!=pluginHash->end() && !plugins.contains((*it))){
							std::cout << "-> pb" << std::endl;
							plugins.push_back(*it);
							pluginDir_subNode= pluginDir_subNode.nextSiblingElement("ACTIVE");
						}
						else if(it==pluginHash->end()){
							QDomElement n_plugins_subNode= pluginDir_subNode.nextSiblingElement("ACTIVE");
							plugins_subNode.removeChild(pluginDir_subNode);
							pluginDir_subNode.clear();

							pluginDir_subNode= n_plugins_subNode;
						}

					}
				}
			}
		}

		plugins_subNode= plugins_subNode.nextSiblingElement();
	}

	plugins_node= root.firstChildElement("CAMERAS");
	if(plugins_node.isNull()){
		plugins_node= doc.createElement("CAMERAS");
		root.appendChild(plugins_node);
	}
	else{
		QDomElement camera_node= plugins_node.firstChildElement("CAMERA");
		while(!camera_node.isNull()){
			QDomElement n_camera_node= camera_node.nextSiblingElement("CAMERA");
			plugins_node.removeChild(camera_node);
			camera_node.clear();

			camera_node= n_camera_node;
		}
	}

	QList<Camera*> cameras;
	for(SceneHash::iterator it= sceneHash->begin(); it!=sceneHash->end(); ++it){
		int t= (*it)->countViews();
		for(int i=0; i<t; ++i){
			QList<Camera*> l_c= (*it)->getView(i)->cameras();
			for(QList<Camera*>::iterator c_it= l_c.begin(); c_it!=l_c.end(); ++c_it){
				if(!cameras.contains(*c_it)){
					cameras.push_back(*c_it);
				}
			}
		}
		scenes.push_back(*it);
	}

	for(QList<Camera*>::iterator it= cameras.begin(); it!=cameras.end(); ++it){
		QDomElement newCameraElem= doc.createElement("CAMERA");
		newCameraElem.setAttribute("name", (*it)->getName());
		plugins_node.appendChild(newCameraElem);
		newCameraElem.appendChild((*it)->domElement("MY_CAMERA", doc));
	}

	plugins_node= root.firstChildElement("SCENES");
	if(plugins_node.isNull()){
		plugins_node= doc.createElement("SCENES");
		root.appendChild(plugins_node);
	}
	else{
		QDomElement scene_node= plugins_node.firstChildElement("SCENE");
		while(!scene_node.isNull()){
			QDomElement n_scene_node= scene_node.nextSiblingElement("SCENE");
			plugins_node.removeChild(scene_node);
			scene_node.clear();

			scene_node= n_scene_node;
		}
	}

	for(SceneHash::iterator it= sceneHash->begin(); it!=sceneHash->end(); ++it){
		QDomElement newSceneElem= doc.createElement("SCENE");
		newSceneElem.setAttribute("name", (*it)->getName());
		plugins_node.appendChild(newSceneElem);
		QList<Plugin*> scenePlugins= (*it)->linkedPlugins();
		for(QList<Plugin*>::iterator p_it= scenePlugins.begin(); p_it!=scenePlugins.end(); ++p_it){
			int n= plugins.indexOf((*p_it));
			std::cout << ">>>> n?" << std::endl;
			if(n>=0){
				std::cout << ">>>>>>>>>>>yup" << std::endl;
				QDomElement newScenePluginElem= doc.createElement("PLUGIN");
				QDomText text= doc.createTextNode(QString::number(n));
				newSceneElem.appendChild(newScenePluginElem);
				newScenePluginElem.appendChild(text);
			}
		}
	}

	plugins_node= root.firstChildElement("DISPLAY");
	if(plugins_node.isNull()){
		plugins_node= doc.createElement("DISPLAY");
		root.appendChild(plugins_node);
	}
	else{
		QDomElement row_node= plugins_node.firstChildElement("ROW");
		while(!row_node.isNull()){
			QDomElement n_row_node= row_node.nextSiblingElement("ROW");
			plugins_node.removeChild(row_node);
			row_node.clear();

			row_node= n_row_node;
		}
	}

	int areaH= splitArea->height();
	int areaW= splitArea->width();

	for(int i=0; i<splitArea->count(); ++i){
		double lineWidthRatio= ((double)(splitArea->widget(i)->height()))/((double)areaH);
		QDomElement newLineElem= doc.createElement("ROW");
		newLineElem.setAttribute("h_ratio",QString::number(lineWidthRatio));
		plugins_node.appendChild(newLineElem);
		QSplitter* hsplit= (QSplitter*)(splitArea->widget(i));
		for(int j=0; j<hsplit->count(); ++j){
			View* view= (View*)(hsplit->widget(j));
			double columnWidthRatio= ((double)(view->width()))/((double)(areaW));
			QDomElement newColumnElem = doc.createElement("VIEW");
			newColumnElem.setAttribute("w_ratio", QString::number(columnWidthRatio));
			int n= scenes.indexOf(view->getScene());
			if(n>=0){
				newColumnElem.setAttribute("scene_num", QString::number(n));
			}
			newLineElem.appendChild(newColumnElem);

			QList<Camera*> viewCameras= view->cameras();
			foreach(Camera* c, viewCameras){
				int nc= cameras.indexOf(c);
				if(nc>=0 && nc<cameras.size()){
					QDomElement newCameraElem= doc.createElement("CAMERA");
					newColumnElem.appendChild(newCameraElem);
					QDomText strCameraNum= doc.createTextNode(QString::number(nc));
					newCameraElem.appendChild(strCameraNum);
				}
			}
		}
	}

	if(!xmlFile.open(QIODevice::WriteOnly)){
		System::Error::code= System::Error::ERROR_OPEN_PLUGIN_FILE;
		System::Error::showError(window);
		return false;
	}

	QTextStream out;
	out.setDevice(&xmlFile);

	doc.save(out, 2);

	xmlFile.close();


	return true;
}

bool System::StateHandler::loadState(Window* window, PluginHash* pluginHash, SceneHash* sceneHash, SplitArea* splitArea){
	QList<Plugin*> plugins;
	QList<Scene*> scenes;
	QList<Camera*> cameras;
	QHash<int,int> sceneNumAutoViewMap;

	std::cout << "load state" << std::endl;

	QFile xmlFile(System::app_path.toStdString().c_str() +
			QString("/state_save.xml"));

	if(!xmlFile.exists())
	{
		System::Error::code= System::Error::NO_PLUGIN_PATH_FILE;
		System::Error::showError(window);
		return false;
	}

	if(!xmlFile.open(QIODevice::ReadOnly)){
		System::Error::code= System::Error::ERROR_OPEN_PLUGIN_FILE;
		System::Error::showError(window);
		return false;
	}
	QDomDocument doc;
	if(!doc.setContent(&xmlFile)){
		System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
		xmlFile.close();
		System::Error::showError(window);
		return false;
	}
	xmlFile.close();

	QDomElement root= doc.documentElement();

	bool load=false;
	QDomElement rootChild= root.firstChildElement();
	while(!rootChild.isNull() && !load){
		if(rootChild.tagName()!="PLUGINS"){
			load= rootChild.hasChildNodes();
		}
		else{
			std::cout << "load0" << std::endl;
			QDomElement rootChildChild= rootChild.firstChildElement();
			while(!rootChildChild.isNull() && !load){
				std::cout << "load1" << std::endl;
				if(rootChildChild.tagName()=="DIR"){
					std::cout << "load2" << std::endl;
					QDomElement activeChild= rootChildChild.firstChildElement("ACTIVE");
					load= !activeChild.isNull();
				}
				else if(rootChildChild.tagName()=="FILE"){
					load= rootChildChild.attribute("active")=="yes";
				}

				rootChildChild= rootChildChild.nextSiblingElement();
			}
		}
		rootChild= rootChild.nextSiblingElement();
	}

	if(load){
		Dialog::QuestionSessionDialog qsd(window);
		qsd.exec();
		load= (qsd.result()==QDialog::Accepted);
	}

	if(!load)
		return true;

	QDomElement plugins_node= root.firstChildElement("PLUGINS");

	if(!plugins_node.isNull()){
		QDomElement plugins_subNode= plugins_node.firstChildElement();
		while(!plugins_subNode.isNull()){
			if(plugins_subNode.tagName()=="FILE"){
				QString pluginPath= plugins_subNode.attribute("path");
				if(!pluginPath.isEmpty()){
					QString isActive= plugins_subNode.attribute("active");
					if(isActive=="yes" && (QFileInfo(pluginPath)).exists()){
						Plugin* p;
						if((p=window->loadPlugin(pluginPath))){
							QString pluginName= (QFileInfo(pluginPath)).baseName();
							pluginName= pluginName.remove(0,3);
							plugins.push_back(p);
						}
					}
				}
			}
			else if (plugins_subNode.tagName()=="DIR"){
				QString pluginDirPath= plugins_subNode.attribute("path");
				if(!pluginDirPath.isEmpty() && (QDir(pluginDirPath)).exists()){
					QDomElement pluginDirActive_node= plugins_subNode.firstChildElement("ACTIVE");
					while(!pluginDirActive_node.isNull()){
						QString pluginName= pluginDirActive_node.text();
						if(!pluginName.isEmpty()){
							QString pluginPath= pluginDirPath+"/lib"+pluginName+".so";
							if((QFileInfo(pluginPath)).exists()){
								Plugin* p;
								if((p=window->loadPlugin(pluginPath))){
									QString pluginName= (QFileInfo(pluginPath)).baseName();
									pluginName= pluginName.remove(0,3);
									plugins.push_back(p);
								}
							}
						}

						pluginDirActive_node= pluginDirActive_node.nextSiblingElement("ACTIVE");
					}
				}
			}

			plugins_subNode= plugins_subNode.nextSiblingElement();
		}
	}


	plugins_node= root.firstChildElement("CAMERAS");
	if(!plugins_node.isNull()){
		QDomElement camera_node= plugins_node.firstChildElement("CAMERA");
		while(!camera_node.isNull()){
			QString cameraName= camera_node.attribute("name", "camera");
			Camera* c= new Camera(NULL);
			c->setName(cameraName);
			std::cout << "save : saving camera" <<  c->getName().toStdString() << std::endl;
			QDomElement cameraInfo_node= camera_node.firstChildElement("MY_CAMERA");
			if(!cameraInfo_node.isNull()){
				c->initFromDOMElement(cameraInfo_node);
			}
			cameras.push_back(c);

			camera_node= camera_node.nextSiblingElement("CAMERA");
		}
	}

	std::cout << "load >>>>DISPLAY<<<<<" << std::endl;
	plugins_node= root.firstChildElement("DISPLAY");
	if(!plugins_node.isNull()){
		int y=0;

		QDomElement row_node= plugins_node.firstChildElement("ROW");
		QList<double> sizeRatios;
		while(!row_node.isNull()){
			double h_ratio= row_node.attribute("h_ratio","1.0").toDouble();
			sizeRatios.push_back(h_ratio);

			int x=0;
			QDomElement column_node= row_node.firstChildElement("VIEW");
			while(!column_node.isNull()){
				View* v=NULL;
				Scene* s=NULL;
				QList<Camera*> viewCameras;

				QDomElement viewCamera_node= column_node.firstChildElement("CAMERA");
				while(!viewCamera_node.isNull()){
					QString strCameraNum= viewCamera_node.text();
					int ic;
					if(!strCameraNum.isEmpty() && (ic=strCameraNum.toInt())>=0 && ic<cameras.size()){
						viewCameras.push_back(cameras[ic]);
						std::cout << "------------ view cameras pushing camera " << cameras[ic]->getName().toStdString() << std::endl;
					}

					viewCamera_node= viewCamera_node.nextSiblingElement("CAMERA");
				}

				QString strSceneNum=  column_node.attribute("scene_num");
				if(!strSceneNum.isEmpty()){
					int n= strSceneNum.toInt();

					QDomElement scenes_node= root.firstChildElement("SCENES");
					if(!scenes_node.isNull() && n>=0){
						std::cout << "in SCENES (looking for " << n << ')' << std::endl;

						int i=0;
						QDomElement scene_node= scenes_node.firstChildElement("SCENE");
						while(!scene_node.isNull() && i<n){ scene_node=scene_node.nextSiblingElement("SCENE"); ++i;}
						if(i==n /*&& !scene_node.isNull()*/){
							QString sceneName= scene_node.attribute("name", QString("Scene")+QString::number(n));
							SceneHash::iterator it;
							if((it=sceneHash->find(sceneName))!=sceneHash->end()){
								std::cout << "scene " << sceneName.toStdString() << " found " << std::endl;
								s=(*it);
								if(!s->isManual() && sceneNumAutoViewMap.find(n)==sceneNumAutoViewMap.end()){
									std::cout << "dddd" << std::endl;
									sceneNumAutoViewMap.insert(n,s->countViews());
									delete s->getView(0)->takeCamera(s->getView(0)->currentCamera());
								}
								if(sceneNumAutoViewMap.value(n)==0){
									std::cout << "has NO auto view" << std::endl;
									if(viewCameras.isEmpty()){
										Camera* c= s->getView(0)->currentCamera();
										v= s->addNewView(c);
									}
									else{
										v= s->addNewView(viewCameras.takeFirst());
										std::cout << "ici" << std::endl;
										foreach(Camera* c, viewCameras){
											v->insertCamera(0,c);
										}
									}
								}
								else{
									std::cout << "has auto view" << std::endl;
									int t=sceneNumAutoViewMap.value(n);
									int iv= s->countViews()-t;

									v= s->getView(iv);
									foreach(Camera* c, viewCameras){
										v->insertCamera(0,c);
									}


									sceneNumAutoViewMap[n]--;
								}
							}
							else{
								QList<Plugin> scenePlugins;

								QDomElement scenePlugin_node= scene_node.firstChildElement("PLUGIN");
								bool first=true;
								while(!scenePlugin_node.isNull()){
									QString strPluginNum= scenePlugin_node.text();
									int np;
									if(!strPluginNum.isEmpty() && (np= strPluginNum.toInt())>=0 && np<plugins.size()){
										if(first){
											std::cout << "tomato" << std::endl;
//											window->addNewSceneFromPlugin(sceneName, plugins[np],s);
											if(viewCameras.isEmpty()){
												std::cout << "empty!!!" << std::endl;
												window->addNewEmptyScene(sceneName,s,false);
											}
											else{
												std::cout << "not empty with size" << viewCameras.size() << std::endl;
												window->addNewEmptyScene(sceneName,s,false, viewCameras.takeFirst());
												foreach(Camera* c, viewCameras){
													std::cout << "inserting " << c->getName().toStdString() << std::endl;
													s->getView(0)->insertCamera(0,c);
												}
											}
											plugins[np]->recieveScene(s);
											v= s->getView(0);

											first=false;
										}
										else{
											std::cout << "calig" << std::endl;
											plugins[np]->recieveScene(s);
										}
									}

									scenePlugin_node= scenePlugin_node.nextSiblingElement("PLUGIN");
								}
								if(first){
									if(viewCameras.isEmpty()){
										window->addNewEmptyScene(sceneName,s,false);
										v= s->getView(0);
									}
									else{
										window->addNewEmptyScene(sceneName, s, false, viewCameras.takeFirst());
										std::cout << "là" << std::endl;
										v= s->getView(0);

										foreach(Camera* c, viewCameras){
											v->insertCamera(0,c);
										}
									}

									first= false;
									sceneNumAutoViewMap.insert(n,0);
								}
								else{
									sceneNumAutoViewMap.insert(n,s->countViews()-1);
								}
							}
						}
					}
				}

				if(v!=NULL){
					splitArea->addElementAt(v,x,y);
					std::cout << "add view " << x << ',' << y << std::endl;
					double w_ratio= column_node.attribute("w_ratio","1.0").toDouble();
					sizeRatios.push_back(w_ratio);
					std::cout << "ajout " << w_ratio << std::endl;
				}

				column_node= column_node.nextSiblingElement("VIEW");
				++x;
			}

			row_node= row_node.nextSiblingElement("ROW");
			++y;
		}



		int areaH= splitArea->height();
		int areaW= splitArea->width();

		int w,h;
		for(int i=0; i<splitArea->count(); ++i){
			QSplitter* hsplit= (QSplitter*)(splitArea->widget(i));
			std::cout << "sparea size=" << splitArea->count() << std::endl;
			std::cout << "initsize=" << sizeRatios.size() << std::endl;
			h= areaH * sizeRatios.takeFirst();
			for(int j=0; j<hsplit->count(); j++){
				std::cout << "j" << std::endl;
				View* v= (View*)(hsplit->widget(j));
				w= areaW * sizeRatios.takeFirst();
				v->resize(w,h);
			}
		}
	}
	else{
		foreach(Camera* camera, cameras){
			delete camera;
		}
	}


	return true;

}

bool System::StateHandler::savePluginsInfo(Window* window, PluginHash* pluginHash, QStringList paths){
	QFile xmlFile(System::app_path.toStdString().c_str() +
				QString("/state_save.xml"));

	if(!xmlFile.exists())
	{
		if(!xmlFile.open(QIODevice::ReadWrite)){
			System::Error::code= System::Error::NO_PLUGIN_PATH_FILE;
			System::Error::showError(window);
			return false;
		}
	}
	else if(!xmlFile.open(QIODevice::ReadOnly)){
		System::Error::code= System::Error::ERROR_OPEN_PLUGIN_FILE;
		System::Error::showError(window);
		return false;
	}
	QDomDocument doc;
	if(!doc.setContent(&xmlFile)){
		System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
		xmlFile.close();
		System::Error::showError(window);
		return false;
	}
	xmlFile.close();

	QDomElement root= doc.documentElement();
	QDomElement plugins_node= root.firstChildElement("PLUGINS");
	if(!plugins_node.isNull()){
		QDomElement plugins_subNode= plugins_node.firstChildElement();
		while(!plugins_subNode.isNull()){
			QDomElement n_plugins_subNode= plugins_subNode.nextSiblingElement();
			plugins_node.removeChild(plugins_subNode);
			plugins_subNode.clear();

			plugins_subNode= n_plugins_subNode;
		}
	}
	else{
		plugins_node= doc.createElement("PLUGINS");
		root.appendChild(plugins_node);
	}

	foreach(QString path, paths){
		QFileInfo fi(path);
		if(fi.exists()){
			if(fi.isDir()){
				QDomElement pluginDir_node= doc.createElement("DIR");
				plugins_node.appendChild(pluginDir_node);
				pluginDir_node.setAttribute("path", path);

				QDir pluginDir(path);
				QStringList	filters, dirFiles;
				filters << "lib*.so";

				dirFiles= pluginDir.entryList(filters,QDir::Files);
				foreach(QString fileName, dirFiles){
					QString pluginName= fileName;
					pluginName.remove(0,3);
					pluginName.remove(pluginName.lastIndexOf(".so"),3);
					if(pluginHash->find(pluginName)!=pluginHash->end()){
						QDomElement dirFile_node= doc.createElement("ACTIVE");
						pluginDir_node.appendChild(dirFile_node);

						QDomText strPlugin= doc.createTextNode(pluginName);
						dirFile_node.appendChild(strPlugin);
					}
				}
			}
			else if(fi.baseName().left(3)=="lib" && fi.suffix()=="so"){
				QDomElement plugin_node= doc.createElement("FILE");
				plugin_node.appendChild(plugin_node);

				QDomText strPlugin= doc.createTextNode(path);
				plugin_node.appendChild(plugin_node);

				QString pluginName= (QFileInfo(path)).baseName().remove(0,3);
				if(pluginHash->find(pluginName)!=pluginHash->end()){
					plugin_node.setAttribute("active","yes");
				}
				else{
					plugin_node.setAttribute("active","no");
				}
			}
		}

		if(!xmlFile.open(QIODevice::WriteOnly)){
			System::Error::code= System::Error::ERROR_OPEN_PLUGIN_FILE;
			System::Error::showError(window);
			return false;
		}

		QTextStream out;
		out.setDevice(&xmlFile);

		doc.save(out, 2);

		xmlFile.close();

	}

	return true;
}

void System::StateHandler::unsaveState(){
	QFile xmlFile(System::app_path.toStdString().c_str() +
				QString("/state_save.xml"));

	if(!xmlFile.exists())
	{
		System::Error::code= System::Error::NO_PLUGIN_PATH_FILE;
		return;
	}

	if(!xmlFile.open(QIODevice::ReadOnly)){
		System::Error::code= System::Error::ERROR_OPEN_PLUGIN_FILE;
		return;
	}
	QDomDocument doc;
	if(!doc.setContent(&xmlFile)){
		System::Error::code= System::Error::BAD_PLUGIN_PATH_FILE;
		xmlFile.close();
		return;
	}
	xmlFile.close();

	QDomElement root= doc.documentElement();
	QDomElement plugins_node= root.firstChildElement("PLUGINS");
	if(plugins_node.isNull()){
		QDomElement plugins_node= doc.createElement("PLUGINS");
	}
	else{
		QDomElement plugins_subNode= plugins_node.firstChildElement();
		while(!plugins_subNode.isNull()){
			if(plugins_subNode.tagName()=="FILE"){
				plugins_subNode.setAttribute("active", "no");
			}
			else if(plugins_subNode.tagName()=="DIR"){
				QDomElement activePlugins_node= plugins_subNode.firstChildElement("ACTIVE");
				while(!activePlugins_node.isNull()){
					QDomElement n_activePlugins_node= activePlugins_node.nextSiblingElement("ACTIVE");
					plugins_subNode.removeChild(activePlugins_node);
					activePlugins_node.clear();

					activePlugins_node= n_activePlugins_node;
				}
			}

			plugins_subNode= plugins_subNode.nextSiblingElement();
		}
	}

	plugins_node= root.firstChildElement("CAMERAS");
	if(!plugins_node.isNull()){
		QDomElement camera_node= plugins_node.firstChildElement("CAMERA");
		while(!camera_node.isNull()){
			QDomElement n_camera_node= camera_node.nextSiblingElement("CAMERA");
			plugins_node.removeChild(camera_node);
			camera_node.clear();

			camera_node= n_camera_node;
		}
	}

	plugins_node= root.firstChildElement("SCENES");
	if(!plugins_node.isNull()){
		QDomElement camera_node= plugins_node.firstChildElement("SCENE");
		while(!camera_node.isNull()){
			QDomElement n_camera_node= camera_node.nextSiblingElement("SCENE");
			plugins_node.removeChild(camera_node);
			camera_node.clear();

			camera_node= n_camera_node;
		}
	}

	plugins_node= root.firstChildElement("DISPLAY");
	if(!plugins_node.isNull()){
		QDomElement camera_node= plugins_node.firstChildElement("ROW");
		while(!camera_node.isNull()){
			QDomElement n_camera_node= camera_node.nextSiblingElement("ROW");
			plugins_node.removeChild(camera_node);
			camera_node.clear();

			camera_node= n_camera_node;
		}
	}

	if(!xmlFile.open(QIODevice::WriteOnly)){
		System::Error::code= System::Error::ERROR_OPEN_PLUGIN_FILE;
		return;
	}

	QTextStream out;
	out.setDevice(&xmlFile);

	doc.save(out, 2);

	xmlFile.close();
}

