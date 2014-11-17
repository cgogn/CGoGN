#include "viewButtonArea.h"

#include "schnapps.h"
#include "view.h"
//#include "texture.h"
//#include "camera.h"

#include "Utils/Shaders/shaderWallPaper.h"

#include <iostream>

namespace CGoGN
{

namespace SCHNApps
{


ViewButton::ViewButton(const QString& image, View* view) :
	m_img(image),
	m_view(view)
{
	m_texture = new Utils::Texture<2,Geom::Vec3uc>(GL_UNSIGNED_BYTE);

	if (!m_texture->load(m_img.toStdString()))
		std::cerr << "Problem loading icon "<< m_img.toStdString() << std::endl;

	m_texture->update();
	m_texture->setWrapping(GL_CLAMP_TO_EDGE);
}

ViewButton::~ViewButton()
{
	m_view->getSCHNApps()->releaseTexture(m_img);
}

void ViewButton::click(int x, int y, int globalX, int globalY)
{
	emit clicked(x, y, globalX, globalY);
}

void ViewButton::drawAt(int x, int y, Utils::ShaderWallPaper* shader)
{
	QSize szw = m_view->size();

	shader->drawFront(szw.width(),szw.height(),x,y,ViewButton::SIZE,ViewButton::SIZE, m_texture);

}

ViewButtonArea::ViewButtonArea(View* view) :
	m_view(view),
	m_form(0,0,0,0)
{
	m_shaderButton = new CGoGN::Utils::ShaderWallPaper();
	m_shaderButton->setTextureUnit(GL_TEXTURE0);

}

ViewButtonArea::~ViewButtonArea()
{
	delete m_shaderButton;
}



void ViewButtonArea::addButton(ViewButton* button)
{
	if(!l_buttons.contains(button))
	{
		m_form.setWidth(m_form.width() + ViewButton::SIZE + ViewButton::SPACE);
		m_form.moveTopLeft(QPoint(m_form.x() - ViewButton::SIZE - ViewButton::SPACE, m_form.y()));
		m_form.setHeight(ViewButton::SIZE + 2 * ViewButton::SPACE);

		l_buttons.push_back(button);
	}
}

void ViewButtonArea::removeButton(ViewButton* button)
{
	if(l_buttons.removeOne(button))
	{
		m_form.setWidth(m_form.width() - ViewButton::SIZE - ViewButton::SPACE);
		m_form.moveTopLeft(QPoint(m_form.x() + ViewButton::SIZE + ViewButton::SPACE, m_form.y()));
		m_form.setHeight(ViewButton::SIZE + 2 * ViewButton::SPACE);
	}
}

bool ViewButtonArea::isClicked(int x, int y)
{
	return m_form.contains(x, y);
}

void ViewButtonArea::clickButton(int x, int y, int globalX, int globalY)
{
	QPoint p = m_form.topLeft();
	p.setY(p.y() + ViewButton::SPACE);
	foreach(ViewButton* b, l_buttons)
	{
		if(QRect(p, QSize(ViewButton::SIZE, ViewButton::SIZE)).contains(x, y))
		{
			b->click(x, y, globalX, globalY);
			return;
		}
		p.setX(p.x() + ViewButton::SPACE + ViewButton::SIZE);
	}
}

void ViewButtonArea::setTopRightPosition(int x, int y)
{
	m_form.moveTopRight(QPoint(x,y));
}

void ViewButtonArea::setTopLeftPosition(int x, int y)
{
	m_form.moveTopLeft(QPoint(x+ViewButton::SPACE,y));
}

void ViewButtonArea::draw()
{
	int p_x = m_form.x();
	int p_y = m_form.y();

	foreach(ViewButton* b, l_buttons)
	{
		b->drawAt(p_x, p_y + ViewButton::SPACE, m_shaderButton);
		p_x += ViewButton::SIZE + ViewButton::SPACE;
	}
}

} // namespace SCHNApps

} // namespace CGoGN
