#ifndef _VIEWBUTTONAREA_H_
#define _VIEWBUTTONAREA_H_

#include <QPixmap>
#include <QSize>
#include <QRect>
#include <QPainter>
#include <QList>

#include <iostream>

namespace CGoGN
{

namespace SCHNApps
{

class View;
class Texture;

class ViewButton : public QObject
{
	Q_OBJECT

public:
	ViewButton(const QString& image, View* view);
	~ViewButton();

	QSize getSize();

	void click(int x, int y, int globalX, int globalY);
	void drawAt(int x, int y);

	static const int SIZE = 24;
	static const int SPACE = 4;

protected:
	QString m_img;
	View* m_view;
	Texture* m_tex;

signals:
	void clicked(int x, int y, int globalX, int globalY);
};

class ViewButtonArea : public QObject
{
	Q_OBJECT

public:
	ViewButtonArea(View* view) :
		m_view(view),
		m_form(0,0,0,0)
	{}
	~ViewButtonArea()
	{}

	void addButton(ViewButton* button);
	void removeButton(ViewButton* button);

	bool isClicked(int x, int y);
	void clickButton(int x, int y, int globalX, int globalY);

	const QRect& getForm() { return m_form; }

	void setTopRightPosition(int x, int y);

	void draw();

protected:
	View* m_view;
	QRect m_form;
	QList<ViewButton*> l_buttons;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
