#include "vboHandler.h"

#include "mapHandler.h"

bool VBOHandler::shareWith(MapHandler* vh){
	if(!vh || l_vizuHandler.contains(vh)){
		return false;
	}
	else{
		l_vizuHandler.push_back(vh);
		vh->addVBO(this);
		return true;
	}
}

void VBOHandler::unshareWith(MapHandler* vh){
	int i= l_vizuHandler.indexOf(vh);
	std::cout << "wunhare" << std::endl;
	if(i>=0){
		std::cout << "wouaf" << std::endl;
		if(vh){
			std::cout << "Miaou" << std::endl;
			vh->takeVBO(this);
		}
		l_vizuHandler.takeAt(i);
	}
}

bool VBOHandler::operator==(QString const & name){
	return m_name==name;
}
