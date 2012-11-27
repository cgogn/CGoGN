#include "viewButtonArea.h"

#include "camera.h"
#include "scene.h"
//#include "context.h"

#include <iostream>

ViewButton::ViewButton(const QString& image, View* view) :
	m_view(view)
{
	m_GLimg.load(image);
	if(!m_GLimg.isNull())
	{
		m_size = m_GLimg.size();
		m_GLimg = QGLWidget::convertToGLFormat(m_GLimg);
		m_texID = m_view->bindTexture(m_GLimg, GL_TEXTURE_2D, GL_RGBA);
	}
}

ViewButton::~ViewButton()
{
	m_view->deleteTexture(m_texID);
}

void ViewButton::click()
{
	emit clicked();
}

void ViewButton::drawAt(int x, int y)
{
	glBindTexture(GL_TEXTURE_2D, m_texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_GLimg.bits());
	glBegin (GL_QUADS);
		glTexCoord2i(0, 1);
		glVertex2i(x, y);
		glTexCoord2i(0, 0);
		glVertex2i(x, y + m_size.height());
		glTexCoord2i(1, 0);
		glVertex2i(x + m_size.width(), y + m_size.height());
		glTexCoord2i(1, 1);
		glVertex2i(x + m_size.width(), y);
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

ViewButton* ViewButtonArea::clickButton(int x, int y)
{
	QPoint p = m_form.topLeft();
	p.setY(p.y() + 3);
	foreach(ViewButton* b, l_buttons)
	{
		if(QRect(p, b->getSize()).contains(x,y))
		{
			b->click();
			return b;
		}
		p.setX(p.x() + 3 + b->getSize().width());
	}
	return NULL;
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
