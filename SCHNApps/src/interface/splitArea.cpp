#include "splitArea.h"


#include <QLayout>

#include <iostream>

SplitArea::SplitArea(QWidget* parent) : QSplitter(Qt::Vertical, parent),
	nbRow(0), nbMaxColumn(0),
	rowMin(0),
	nbElement(0),
	sizeUpdated(false)
{

}

void SplitArea::addElementAt(QWidget* element, int x, int y){
	std::cout << "SPLIT AREA ADD ELELEMNT AT " << x << ',' << y << std::endl;
	if(sizeUpdated){
		this->updateSize();
	}
	if(y<0){
		QSplitter* hsplit= new QSplitter(Qt::Horizontal);
		hsplit->addWidget(element);
		if(nbMaxColumn==0){
			nbMaxColumn=1;
		}
		nbRow++;
		this->insertWidget(0,hsplit);
	}
	else if(y>=nbRow){
		QSplitter* hsplit= new QSplitter(Qt::Horizontal, this);
		hsplit->addWidget(element);
		if(nbMaxColumn==0){
			nbMaxColumn=1;
		}
		nbRow++;
	}
	else{
		QSplitter* hsplit= (QSplitter*)(this->widget(y));
		int nbLocalColumn= hsplit->count();
		hsplit->insertWidget(x, element);
		if(nbLocalColumn==nbMaxColumn){
			nbMaxColumn++;
		}
	}
	connect(element, SIGNAL(destroyed()), this, SLOT(elementRemoved()));
	nbElement++;
	this->determineRowMin();
}

void SplitArea::addElementRightTo(QWidget* element, QWidget* left){
	std::cout << "addElementRightTo" << std::endl;
	if(sizeUpdated){
		this->updateSize();
	}

	bool found=false;
	int y= this->count(), x;
	for(int j=0; j<y; ++j){
		QSplitter* hsplit= (QSplitter*)(this->widget(j));
		x= hsplit->count();
		for(int i=0; i<x; ++i){
			if(left==hsplit->widget(i)){
				std::cout << "\tfound" << std::endl;
				hsplit->insertWidget(i+1,element);

				found=true;
				break;
			}
		}
		if(found) break;
	}

	if(!found){
		std::cout << "\twasn't found" << std::endl;
		this->addFitElement(element);
	}
	else{
		std::cout << "\twas found" << std::endl;
		this->determineRowMin();
	}
}

bool SplitArea::isEmpty(){
	return (this->count()>0)?(((QSplitter*)this->widget(0))->count()<=0):true;
}

void SplitArea::updateSize(){
	rowMin=0;
	nbElement=0;
	nbMaxColumn=0;
	nbRow=this->count();
	if(nbRow>0){
		int min= ((QSplitter*)(this->widget(0)))->count();
		if(min==0){
			delete this->widget(0);
			--nbRow;
		}
		int t= min;
		nbElement+=t;
		for(int i=0; i< nbRow; ++i){
			if((t=((QSplitter*)(this->widget(i)))->count())==0){
				delete this->widget(i);
				--i; --nbRow;
			}
			else{
				if(t<min){
					min=t;
					rowMin=i;
				}
				if(t>nbMaxColumn){
					nbMaxColumn= t;
				}
			}
		}
	}
	sizeUpdated=false;
}

void SplitArea::addFitElement(QWidget* element){
	if(sizeUpdated){
		this->updateSize();
	}
	if(nbElement==0){
		this->addElementAt(element,0,0);
	}
	else{
		std::cout << "row min=" << rowMin << std::endl;
 		int nbColumnRowMin= ((QSplitter*)(this->widget(rowMin)))->count();
		if(nbColumnRowMin>nbRow){
			this->addElementAt(element,0,nbRow);
		}
		else if(nbColumnRowMin==nbMaxColumn){
			this->addElementAt(element,nbMaxColumn,0);
		}
		else{
			this->addElementAt(element,nbColumnRowMin,rowMin);
		}

	}
}

void SplitArea::elementRemoved(){
	std::cout << "element removed" << std::endl;
	sizeUpdated=true;
}

void SplitArea::determineRowMin(){
	if(this->count()==0){
		rowMin= 0;
	}
	else{
		rowMin= 0;
		int min= ((QSplitter*)(this->widget(0)))->count();
		if(min==0){
			delete this->widget(0);
			--nbRow;
		}
		int t= min;
		for(unsigned int i=1; (int)i< nbRow; ++i){
			if((t=((QSplitter*)(this->widget(i)))->count())==0){
				delete this->widget(i);
				--i; --nbRow;
			}
			else{
				if(t<min){
					min=t;
					rowMin=i;
				}
			}
		}
	}
	sizeUpdated=false;
}


