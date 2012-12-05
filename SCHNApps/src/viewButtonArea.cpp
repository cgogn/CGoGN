#include "viewButtonArea.h"

#include "window.h"
#include "view.h"
#include "texture.h"
#include "camera.h"

#include <iostream>

ViewButton::ViewButton(const QString& image, View* view) :
	m_img(image),
	m_view(view)
{
	m_tex = m_view->getWindow()->getTexture(m_img);
}

ViewButton::~ViewButton()
{
	m_view->getWindow()->releaseTexture(m_img);
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
	int w = m_tex->size.width();
	int h = m_tex->size.height();
	glBindTexture(GL_TEXTURE_2D, m_tex->texID);
	glBegin (GL_QUADS);
		glTexCoord2i(0, 1);
		glVertex2i(x, y);
		glTexCoord2i(0, 0);
		glVertex2i(x, y + h);
		glTexCoord2i(1, 0);
		glVertex2i(x + w, y + h);
		glTexCoord2i(1, 1);
		glVertex2i(x + w, y);
	glEnd();
}




void ViewButtonArea::addButton(ViewButton* button)
{
	if(!l_buttons.contains(button))
	{
		QSize b_size = button->getSize();

		m_form.setWidth(m_form.width() + b_size.width() + 3);
		m_form.moveTopLeft(QPoint(m_form.x() - b_size.width() - 3, m_form.y()));

		if(b_size.height() > m_form.height())
			m_form.setHeight(b_size.height() + 6);

		l_buttons.push_back(button);
	}
}

void ViewButtonArea::removeButton(ViewButton* button)
{
	if(l_buttons.removeOne(button))
	{
		QSize b_size = button->getSize();

		m_form.setWidth(m_form.width() - b_size.width() - 3);
		m_form.moveTopLeft(QPoint(m_form.x() + b_size.width() + 3, m_form.y()));

		int h_max = l_buttons.first()->getSize().height();
		foreach(ViewButton* b, l_buttons)
		{
			int h = b->getSize().height();
			if(h > h_max)
				h_max = h;
		}
		m_form.setHeight(h_max + 6);
	}
}

bool ViewButtonArea::isClicked(int x, int y)
{
	return m_form.contains(x, y);
}

void ViewButtonArea::clickButton(int x, int y, int globalX, int globalY)
{
	QPoint p = m_form.topLeft();
	p.setY(p.y() + 3);
	foreach(ViewButton* b, l_buttons)
	{
		if(QRect(p, b->getSize()).contains(x, y))
		{
			b->click(x, y, globalX, globalY);
			return;
		}
		p.setX(p.x() + 3 + b->getSize().width());
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
		b->drawAt(p_x, p_y + 3);
		p_x += b->getSize().width() + 3;
	}
}
