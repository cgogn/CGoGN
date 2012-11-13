#include "viewButtonArea.h"


#include "camera.h"
#include "scene.h"
#include "context.h"

#include <iostream>


ViewButton::ViewButton(QString image, View* view)
{
	m_GLimg.load(image);
	if(!m_GLimg.isNull()){
		std::cout << "button " << image.toStdString() << " size: " << m_GLimg.size().width() << ','
				<< m_GLimg.size().height() << std::endl;
		m_size= m_GLimg.size();
		m_GLimg= QGLWidget::convertToGLFormat(m_GLimg);
		m_texID= view->bindTexture(m_GLimg,GL_TEXTURE_2D, GL_RGBA);
	}
}

void ViewButton::drawAt(int x, int y){
	glBindTexture(GL_TEXTURE_2D, m_texID);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 32,32, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_GLimg.bits() );
	glBegin (GL_QUADS);
		glTexCoord2i(0,1);
		glVertex2i(x,y);
		glTexCoord2i(0,0);
		glVertex2i(x,y+m_size.height());
		glTexCoord2i(1,0);
		glVertex2i(x+m_size.width(), y+m_size.height());
		glTexCoord2i(1,1);
		glVertex2i(x+m_size.width(),y);
	glEnd();
}





ViewButtonArea::~ViewButtonArea(){}

void ViewButtonArea::addButton(ViewButton* button){
	std::cout << "adding" << std::endl;
	if(!l_button.contains(button)){
		QSize b_size= button->getSize();

		std::cout << "ADDBUTON " << b_size.width() << ',' << b_size.height() << std::endl;
		m_form.setWidth(m_form.width()+b_size.width()+3);
		m_form.moveTopLeft(QPoint(m_form.x()-b_size.width()-3, m_form.y()));

		if(b_size.height()>m_form.height()){
			m_form.setHeight(b_size.height()+6);
		}

		l_button.push_back(button);

		connect(button, SIGNAL(destroyed(QObject*)), this, SLOT(buttonDestroyed(QObject*)));
	}
}

ViewButton* ViewButtonArea::takeButton(ViewButton* button){
	std::cout << "taking" << std::endl;
	ViewButton* b=NULL;
	int i;
	while((i=l_button.indexOf(button))>=0){
		QSize b_size= button->getSize();

		m_form.setWidth(m_form.width()-b_size.width()-3);
		m_form.moveTopLeft(QPoint(m_form.x()+b_size.width()+3, m_form.y()));

		int h_min=l_button.first()->getSize().height();
		foreach(ViewButton* b, l_button){
			int h= b->getSize().height();
			if(h>h_min){
				h_min=h;
			}
		}
		m_form.setHeight(h_min+6);

		b= l_button.takeAt(i);
	}

	disconnect(button, SIGNAL(destroyed(QObject*)), this, SLOT(buttonDestroyed(QObject*)));

	return b;
}

bool ViewButtonArea::isIn(int x, int y){
	return m_form.contains(x,y);
}

ViewButton* ViewButtonArea::clickAt(int x, int y){
	QPoint p= m_form.topLeft();
	p.setY(p.y()+3);
	for(QList<ViewButton*>::iterator it= l_button.begin(); it!=l_button.end(); ++it){
		if(QRect(p,(*it)->getSize()).contains(x,y)){
			emit(buttonClicked((*it)));
			(*it)->click(m_view);
			return (*it);
		}
		p.setX(p.x()+3+(*it)->getSize().width());
	}

	return NULL;
}

void ViewButtonArea::setTopRightPosition(int x, int y){
	m_form.moveTopRight(QPoint(x,y));

	std::cout << "toprightpos " << m_form.topRight().x() << ',' << m_form.topRight().y() << std::endl;
}

void ViewButtonArea::draw(){
	QList<ViewButton*>::iterator it;
	int p_x=m_form.x(), p_y=m_form.y();
	for(it=l_button.begin(); it!=l_button.end(); ++it){
		(*it)->drawAt(p_x,p_y+3);
		p_x+= (*it)->getSize().width()+3;
	}
}

void ViewButtonArea::buttonDestroyed(QObject* button){
	takeButton((ViewButton*)button);
}
