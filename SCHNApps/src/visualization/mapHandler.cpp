#include "mapHandler.h"

#include "Topology/generic/genericmap.h"

#include "vboHandler.h"

#include "system.h"


MapHandler::MapHandler(CGoGN::GenericMap* map) :
	m_map(map)
{}

MapHandler::~MapHandler(){
	while(!l_vbo.isEmpty()){
		VBOHandler* vboH= l_vbo.first();
		bool destroy= !vboH->isShared();
		takeVBO(vboH);
		if(destroy)
			delete vboH;
	}
	if(m_map){
		delete m_map;
	}
}

VBOHandler* MapHandler::addNewVBO(QString name){
	foreach(VBOHandler* vbo, l_vbo){
		if((*vbo)==name){
			System::Error::code= System::Error::VBO_EXISTS_f(name);

			return NULL;
		}
	}
	VBOHandler* newVBO= new VBOHandler(name);
	newVBO->shareWith(this);

	return newVBO;
}

bool MapHandler::addVBO(VBOHandler* vbo){
	if(vbo){
		foreach(VBOHandler* vboH, l_vbo){
			if(vbo==vboH || vbo->getName()==vboH->getName()){
				System::Error::code= System::Error::VBO_EXISTS_f(vbo->getName());

				return false;
			}
		}
		l_vbo.push_back(vbo);
		return true;
	}
	else{
		return false;
	}
}

VBOHandler* MapHandler::findVBO(QString name){
	foreach(VBOHandler* vbo, l_vbo){
		if(vbo->getName()==name){
			return vbo;
		}
	}
	return NULL;
}

VBOHandler* MapHandler::findFirstVBOMatching(QRegExp regexp){
	foreach(VBOHandler* vbo, l_vbo){
		if(vbo->getName().contains(regexp)){
			return vbo;
		}
	}

	return NULL;
}

QList<VBOHandler*> MapHandler::findVBOsMatching(QRegExp regexp){
	QList<VBOHandler*> rlist;
	foreach(VBOHandler* vbo, l_vbo){
		if(vbo->getName().contains(regexp)){
			rlist.push_back(vbo);
		}
	}

	return rlist;
}


VBOHandler* MapHandler::takeVBO(VBOHandler* vbo){
	int i= l_vbo.indexOf(vbo);
	if(i>=0){
		VBOHandler* vboh= l_vbo.takeAt(i);
		vboh->unshareWith(this);
		return vboh;
	}
	else{
		return NULL;
	}
}


