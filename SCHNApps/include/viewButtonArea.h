#ifndef _VIEWBUTTONAREA_H_
#define _VIEWBUTTONAREA_H_

#include <QPixmap>
#include <QSize>
#include <QRect>
#include <QPainter>
#include <QList>

#include <iostream>

class View;

class ViewButton : public QObject
{
	Q_OBJECT

public:
	ViewButton(const QString& image, View* view);
	~ViewButton();

	QSize getSize() { return m_GLimg.size(); }

	void click(int x, int y);
	void drawAt(int x, int y);

protected:
	View* m_view;

	QSize m_size;
	QImage m_GLimg;
	int m_texID;

signals:
	void clicked(int x, int y);
};

class ViewButtonArea : public QObject
{
	Q_OBJECT

public:
	ViewButtonArea(View* view) : m_view(view)
	{}
	~ViewButtonArea()
	{}

	void addButton(ViewButton* button);
	void removeButton(ViewButton* button);

	bool isClicked(int x, int y);
	void clickButton(int x, int y);

	const QRect& getForm() { return m_form; }

	void setTopRightPosition(int x, int y);

	void draw();

protected:
	View* m_view;
	QRect m_form;
	QList<ViewButton*> l_buttons;
};

#endif
