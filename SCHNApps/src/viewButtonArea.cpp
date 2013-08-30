#include "viewButtonArea.h"

#include "schnapps.h"
#include "view.h"
#include "texture.h"
//#include "camera.h"

#include <iostream>

namespace CGoGN
{

namespace SCHNApps
{

ViewButton::ViewButton(const QString& image, View* view) :
	m_img(image),
	m_view(view)
{
	m_tex = m_view->getSCHNApps()->getTexture(m_img);
}

ViewButton::~ViewButton()
{
	m_view->getSCHNApps()->releaseTexture(m_img);
}

QSize ViewButton::getSize()
{
	return m_tex->size;
}

void ViewButton::click(int x, int y, int globalX, int globalY)
{
	emit clicked(x, y, globalX, globalY);
}

void ViewButton::drawAt(int x, int y)
{
	glBindTexture(GL_TEXTURE_2D, m_tex->texID);
	glBegin (GL_QUADS);
		glTexCoord2i(0, 1);
		glVertex2i(x, y);
		glTexCoord2i(0, 0);
		glVertex2i(x, y + SIZE);
		glTexCoord2i(1, 0);
		glVertex2i(x + SIZE, y + SIZE);
		glTexCoord2i(1, 1);
		glVertex2i(x + SIZE, y);
	glEnd();
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

void ViewButtonArea::draw()
{
	int p_x = m_form.x();
	int p_y = m_form.y();

	foreach(ViewButton* b, l_buttons)
	{
		b->drawAt(p_x, p_y + ViewButton::SPACE);
		p_x += ViewButton::SIZE + ViewButton::SPACE;
	}
}

} // namespace SCHNApps

} // namespace CGoGN
