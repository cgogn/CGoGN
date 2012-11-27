#ifndef _VIEWBUTTONAREA_H_
#define _VIEWBUTTONAREA_H_

#include <QPixmap>
#include <QSize>
#include <QRect>
#include <QPainter>
#include <QList>

#include <iostream>

class View;
class Context;


class ViewButton : public QObject{
	Q_OBJECT
public:
	ViewButton(QString image, View* view);

	void click(View* view){emit clicked(); emit viewClicked(view);}

	void drawAt(int x, int y);

	QSize getSize(){return m_GLimg.size();}

protected:
	QSize m_size;
	QImage m_GLimg;
	int m_texID;

signals:
	void clicked();
	void viewClicked(View* view);
};

class ViewButtonArea : public QObject{
	Q_OBJECT
public:
	ViewButtonArea(View* view) : m_view(view) {}
	~ViewButtonArea();

	void addButton(ViewButton* button);
	ViewButton* takeButton(ViewButton* button);

	bool isIn(int x, int y);

	ViewButton* clickAt(int x, int y);

	QRect form(){return m_form;}

	void setTopRightPosition(int x, int y);

	void draw();

protected:
	QRect m_form;

	QList<ViewButton*> l_button;

	View* m_view;

public slots:
	void buttonDestroyed(QObject* button);

signals:
	void buttonClicked(ViewButton* button);
};



#endif
