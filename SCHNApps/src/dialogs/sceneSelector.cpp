/**
 * \file ViewSelector.h
 * \brief Header for "GLVSelector": the class that provides insertion and move interface for GLViewers
 * \author Hurstel Alexandre
 * \version 0.1
 *
 * This file implements the class "GLVSelector" which provides an interface for the GLViewer gesture.
 */


#include "sceneSelector.h"
#include "splitArea.h"

#include <QPainter>
#include <QMouseEvent>

#include <iostream>


ViewPixMaps::ViewPixMaps() : QList<QList<PixElem> >(),
	maxX(0),
	minX(0),
	Y(0),
	YminX(0),
	originX(0), originY(0),
	tmpX(-2), tmpY(-2),
	modif(false)
{

}

ViewPixMaps::~ViewPixMaps(){}

void ViewPixMaps::fromSplitArea(SplitArea* splitArea){
	this->clear();

	int nbRow= splitArea->count();
	std::cout << "nbRow= " << nbRow << std::endl;
	int nbColumn;

	for (int j=0; j<nbRow; ++j){
		QSplitter* row= ((QSplitter*)(splitArea->widget(j)));
		nbColumn= row->count();
		std::cout << "\ton row " << j << " nbColumn " << nbColumn << std::endl;
		QList<PixElem> l;
		for(int i=0; i<nbColumn; ++i){
			PixElem p((View*)(row->widget(i)));
			l.push_back(p);
			std::cout << "\t\tcolumn " << i << "view " << ((View*)(row->widget(i)))->getName().toStdString() << std::endl;
		}
		this->push_back(l);
	}

	computeSize();
}

int ViewPixMaps::getMaxX(){
	return maxX;
}

int ViewPixMaps::getMinX(){
	return minX;
}

int ViewPixMaps::getY(){
	return Y;
}

int ViewPixMaps::getYMinX(){
	return YminX;
}

void ViewPixMaps::computeSize(){
	Y= this->size();

	maxX= this->at(0).size();
	minX= this->at(0).size();
	YminX= 0;

	int t_maxX;
	int t_minX;
	for(int y=1; y<this->Y; ++y){
		if((t_maxX=this->at(y).size())>maxX){
			maxX= t_maxX;
		}
		if((t_minX= this->at(y).size())<minX){
			YminX= y;
			minX= t_minX;
		}
	}
}

PixElem& ViewPixMaps::getAt(int x, int y){
	return (*this)[y][x];
}

void ViewPixMaps::beginMove(int x, int y){
	originX=x; originY=y;
	(*this)[y][x].color= Qt::red;
	tmp= this->at(y).at(x);
	origin=tmp;
	tmpX=x; tmpY=y;
}

void ViewPixMaps::moveTmp(int x, int y){
	int destX=x, destY=y;

	//si depart =/= origine
	if(originX!=tmpX || originY!=tmpY){
		(*this)[tmpY][tmpX].color=Qt::gray;

		//travail sur depart

		//si depart même ligne que origine
		if(tmpY==originY){
			//on retire depart
			(*this)[tmpY].removeAt(tmpX);
			//si depart était gauche de origine
			if(tmpX<originX){
				--originX;
			}
		}
		//si depart pas meme ligne qu'origine
		else{
			//on retire départ
			(*this)[tmpY].removeAt(tmpX);
			//si départ était tout seul sur ligne
			if(this->at(tmpY).isEmpty()){
				this->removeAt(tmpY);
				//si ligne départ était avant ligne arrivée
				if(tmpY<destY){
					--destY;
				}
				//si ligne départ était avant ligne origine
				if(tmpY<originY){
					--originY;
				}
			}
		}

		//si depart meme ligne qu'arrivée
		if(tmpY==destY){
			//si depart à gauche arrivée
			if(tmpX<destX){
				--destX;
			}
		}
	}

	//si arrivée =/= origine
	if(destX!=originX || destY!=originY){
		//si arrivée même ligne que origine
		if(destY==originY){
			//on ajoute arrivée
			(*this)[destY].insert(destX,tmp);
			//si arrivee a gauche de origine
			if(destX<=originX){
				++originX;
			}
		}
		//si arrivee pas même ligne que origine
		else{
			//si arrivee en premier
			if(destY<0){
				//si première ligne pas déjà occupé par tmp
				if(!(this->first().size()==1 && this->first().first()==tmp)){
					//ajout nouvelle ligne au début
					QList<PixElem> l;
					l.push_back(tmp);
					this->push_front(l);

					++originY;
				}
			}
			//si arrivee en dernier
			else if(destY>=this->size()){
				//si dernière ligne pas déjà occupée par tmp
				if(!(this->last().size()==1 && this->last().first()==tmp)){
					//ajout nouvelle ligne à la fin
					QList<PixElem> l;
					l.push_back(tmp);
					this->push_back(l);
				}

			}
			//sinon (ni premier ni dernier ni même ligne qu'origine)
			else{
				(*this)[destY].insert(destX,tmp);
//				//si ligne arrivee avant ligne origine
//				if(destY<originY){
//					++originY;
//				}
			}
		}
	}

	tmpX= (destX<0)?0:destX;
	tmpY= (destY<0)?0:destY;

	if(originX!=tmpX || originY!=tmpY)
		(*this)[tmpY][tmpX].color=Qt::green;

	computeSize();
}

void ViewPixMaps::confirmMove(){
	if(tmpX!=originX || tmpY!=originY){
		(*this)[tmpY][tmpX].color=Qt::gray;
		(*this)[originY].removeAt(originX);
		if(this->at(originY).isEmpty()){
			this->removeAt(originY);
		}
		modif=true;
	}
	else{
		modif= false;
	}

	computeSize();

	for(y_iterator y_it= y_begin(); y_it!=y_end(); ++y_it){
		for(x_iterator x_it= x_begin(y_it); x_it!=x_end(y_it); ++x_it){
			std::cout << (*x_it).view->getName().toStdString() << " | ";
		}
		std::cout << std::endl;
	}
}

void ViewPixMaps::cancelMove(){
	(*this)[originY][originX].color= Qt::gray;
	(*this)[tmpY].removeAt(tmpX);
	if(this->at(tmpY).isEmpty()){
		this->removeAt(tmpY);
	}

	computeSize();
}

void ViewPixMaps::insertion(int x, int y){
	int destX=x, destY=y;

	std::cout << "insertion " << x << ',' << y << std::endl;
	//changer map utilise seulement si arrivée != position précédente
	if(destY!=tmpY || destX!=tmpX){
		std::cout << "\t remove tmp at " << tmpX << ',' << tmpY << std::endl;
		if(tmpX>=0 && tmpY>=0){
			//si ancienne insertion seule sur ligne
			if(this->at(tmpY).size()==1){
				//on supprime cette ligne
				this->removeAt(tmpY);
				//si ligne ancienne insertion avant nouvelle
				if(tmpY<destY)
					--destY;
			}
			//si ancienne insertion sur même ligne que nouvelle
			else if(tmpY==destY){
				//on supprime ancienne insertion
				(*this)[tmpY].removeAt(tmpX);
				//si ancienne sur ligne  avant nouvelle insertion
				if(tmpX<destX)
					--destX;
			}
			//sinon
			else{
				//suppression ancienne insertion
				(*this)[tmpY].removeAt(tmpX);
			}
		}
		std::cout << "\t insert dest at " << destX << ',' << destY << std::endl;
		//si curseur en première ligne
		if(destY<0){
				QList<PixElem> l;
				l.push_back(PixElem());
				l.first().color= Qt::green;
				this->push_front(l);
		}
		//sinon curseur à la fin
		else if(destY>=this->size()){
			QList<PixElem> l;
			l.push_back(PixElem());
			l.first().color= Qt::green;
			this->push_back(l);
		}
		//sinon
		else{
			PixElem pglv;
			pglv.color= Qt::green;
			(*this)[destY].insert(destX,pglv);
		}


		tmpX= (destX<0)?0:destX;
		tmpY= (destY<0)?0:destY;

		computeSize();
	}
}






ViewSelector::ViewSelector(QWidget* parent, SelectorDialogType type) :
		QDialog(parent, ((type==SELECT)?(Qt::Widget):(Qt::FramelessWindowHint))),
		viewPixMap(viewPixMap),
		mouseX(0), mouseY(0),
		pixmapGrabbed(false),
		type(type)
{
	keys[0]= false; keys[1]= true; keys[2]= true;

	this->resize(400,400);
	this->setWindowModality(Qt::ApplicationModal);
	this->setMouseTracking(true);
}

ViewSelector::ViewSelector(ViewPixMaps viewPixMap, QWidget* parent, SelectorDialogType type) :
		QDialog(parent, ((type==SELECT)?(Qt::Widget):(Qt::FramelessWindowHint))),
		viewPixMap(viewPixMap),
		mouseX(0), mouseY(0),
		pixmapGrabbed(false),
		type(type)
{
	keys[0]= false; keys[1]= true; keys[2]= true;

	this->resize(400,400);
	this->setWindowModality(Qt::ApplicationModal);
	this->setMouseTracking(true);
}

ViewSelector::~ViewSelector(){}

void ViewSelector::setGLVMap(ViewPixMaps viewPixMap){
	this->viewPixMap= viewPixMap;
}

QPoint ViewSelector::getInsertPoint(){
	if(insertPoint.y()==0 && viewPixMap[0].size()==1){
		insertPoint.setY(-1);
	}

	return insertPoint;
}

void ViewSelector::paintEvent(QPaintEvent*){
	QPainter painter(this);
	QRect rect(0,0,399,399);
	painter.fillRect(rect,Qt::lightGray);
	painter.drawRect(rect);

	if(!viewPixMap.isEmpty() && !viewPixMap.first().isEmpty()){
		int sizeY= viewPixMap.getY();
		int sizeX, pixWidth, pixHeight, posX, posY;
		pixHeight= (360 - (sizeY-1)*10)/sizeY;
		for(int j=0; j<sizeY; ++j){
			sizeX= viewPixMap[j].size();
			pixWidth= (360 - (sizeX-1)*10)/sizeX;
			posY= 20+ j*(pixHeight+10);
			for(int i=0; i<sizeX; ++i){
				posX= 20+ i*(pixWidth+10);
				painter.setOpacity(0.5);
				QRect r(posX,posY,pixWidth,pixHeight);
				viewPixMap[j][i].rect= r;
				painter.fillRect(r, viewPixMap[j][i].color);
				painter.drawRect(r);
				painter.setPen(Qt::black);
				if(viewPixMap[j][i].view){
					painter.drawText(r,Qt::AlignCenter,viewPixMap[j][i].view->getName());
				}
				else{
					if(insertionName.isEmpty()){
						painter.drawText(r,Qt::AlignCenter,"New GLView");
					}
					else{
						painter.drawText(r,Qt::AlignCenter,insertionName);
					}
				}
			}
		}
	}
	if(type==MOVE && pixmapGrabbed){
		grabbed.moveTopLeft(QPoint(mouseX-(grabbed.width()/2),mouseY-(grabbed.height()/2)));
		painter.fillRect(grabbed, Qt::blue);
		painter.drawRect(grabbed);
	}
	painter.end();
}

void ViewSelector::mouseMoveEvent( QMouseEvent * event ){
	std::cout << "hey" << std::endl;
	mouseX= event->x();
	mouseY= event->y();
	if(type==MOVE && pixmapGrabbed){
		QPoint p= mouseToIndice();

		viewPixMap.moveTmp(p.x(), p.y());

		update();
	}
	else if(type==SELECT){
		QPoint p= mouseToIndice();

		viewPixMap.insertion(p.x(), p.y());

		update();
	}
}

void ViewSelector::mousePressEvent ( QMouseEvent * event ){
	mouseX=event->x();
	mouseY= event->y();
	if(type==MOVE && !pixmapGrabbed && event->button()==Qt::LeftButton){
		QPoint p= mouseToIndice();

		grabbed= QRect(viewPixMap.getAt(p.x(),p.y()).rect);

		viewPixMap.beginMove(p.x(),p.y());

		pixmapGrabbed=true;

		update();
	}
	else if(type==SELECT && event->button()==Qt::LeftButton){
		insertPoint= mouseToIndice();

		this->accept();
	}
}

void ViewSelector::mouseReleaseEvent( QMouseEvent * event ){
	mouseX=event->x();
	mouseY= event->y();
	if(type==MOVE && pixmapGrabbed && event->button()==Qt::LeftButton){
		viewPixMap.confirmMove();

		pixmapGrabbed= false;
	}
	update();
}

void ViewSelector::keyPressEvent( QKeyEvent * event ){
	int key= event->key();
	if(event->key()==Qt::Key_Escape){
		if(type==MOVE && pixmapGrabbed)
			viewPixMap.cancelMove();
		this->reject();
	}
	if(key==Qt::Key_M){
		keys[0]=true;
	}
	if(key==Qt::Key_Shift){
		keys[1]=true;
	}
	if(key==Qt::Key_Control){
		keys[2]=true;
	}
}

void ViewSelector::keyReleaseEvent( QKeyEvent * event ){
	int key= event->key();

	if(type==MOVE && (key==Qt::Key_Shift || key==Qt::Key_Control)){
		if(key==Qt::Key_Shift){
			keys[1]=false;
		}
		if(key==Qt::Key_Control){
			keys[2]=false;
		}
		if(pixmapGrabbed){
					viewPixMap.cancelMove();
					this->reject();
		}
		else{
			if(viewPixMap.isModified())
				this->accept();
			else{
				this->reject();
			}
		}
	}
}

QPoint ViewSelector::mouseToIndice(){
	int m_x= mouseX-20;
	m_x= (m_x>360)?360:m_x;
	int m_y= mouseY-20;
	m_y= (m_y>360)?360:m_y;


	int Y= viewPixMap.getY();

	int pixH= 360/Y;

	int idY= (m_y<0)?-1:m_y/pixH;


	int X, pixW, idX;
	if(m_y>=0 && m_y<360){
		X= viewPixMap[idY].size();

		pixW= 360/X;

		idX= (m_x<0)?-1:m_x/pixW;
	}
	else{
		idX=0;
	}

	return QPoint(idX,idY);
}
