#include "import.h"


#include <QMessageBox>

ImportMap::ImportMap(){}

ImportMap::~ImportMap(){}


bool ImportMap::activate(){
	QAction* action= new QAction( m_window);
	action->setIcon(QIcon(":icon/map.png"));
	QObject::connect(action, SIGNAL(triggered()), this, SLOT(saveMap()));
	addMenuAction("Visu;Imorter une Map", action);
	addToolbarAction(action);

	waitingForScene(0);

	return true;
}

void ImportMap::disable(){}


bool ImportMap::importMap(QString filename){
	QFileInfo fileInfo(filename);
	QString extension= fileInfo.suffix();

	PFP::MAP* myMap= new PFP::MAP();

	VertexAttribute<PFP::VEC3> position;
	VertexAttribute<PFP::VEC3> normal;

	if (extension == "map")
	{
		myMap->loadMapBin(filename.toStdString());
		position = myMap->getAttribute<PFP::VEC3, VERTEX>("position") ;
	}
	else
	{
		std::vector<std::string> attrNames ;
		if(!Algo::Import::importMesh<PFP>(*myMap, filename.toStdString().c_str(), attrNames))
		{
			return false;
		}
		position = myMap->getAttribute<PFP::VEC3, VERTEX>(attrNames[0]) ;
		std::cout << "import map position ok? " << ((myMap->getAttribute<PFP::VEC3, VERTEX>(attrNames[0]).isValid())?"yes":"no") << std::endl;
		std::cout << "import map position ok? " << ((myMap->getAttribute<PFP::VEC3, VERTEX>("position").isValid())?"yes":"no") << std::endl;
	}


	normal = myMap->getAttribute<PFP::VEC3, VERTEX>("normal") ;
	if(!normal.isValid())
		normal = myMap->addAttribute<PFP::VEC3, VERTEX>("normal") ;

	Algo::Geometry::computeNormalVertices<PFP>(*myMap, position, normal) ;

	std::cout << "import map normal ok? " << ((myMap->getAttribute<PFP::VEC3, VERTEX>("normal").isValid())?"yes":"no") << std::endl;

	MapHandler* vizuHandler= new MapHandler(myMap);
	if(!addReferencedMap(fileInfo.baseName()+"_map", vizuHandler)){
		System::Error::showError();

		delete vizuHandler;

		return false;
	}

//	myMap->check();

	return true;
}

void ImportMap::saveMap(){
	QString fileName= QFileDialog::getOpenFileName(m_window, System::app_path+"../files", tr("3D files (*)"));
	if(!fileName.isEmpty()){
		if(!importMap(fileName)){
			QMessageBox msg;
			msg.setText(QString::fromUtf8("Une erreur est survenue lors de la création de la carte"));
			if(System::Error::code!=System::Error::SUCCESS){
				System::Error::showError();
			}
		}
	}
}



#ifndef DEBUG
	Q_EXPORT_PLUGIN2(ImportMap, ImportMap)
#else
	Q_EXPORT_PLUGIN2(ImportMapD, ImportMap)
#endif
