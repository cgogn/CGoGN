#ifndef _VIEWSELECTOR_H_
#define _VIEWSELECTOR_H_

#include "view.h"

#include <QDialog>
#include <QRect>

class SplitArea;

class PixElem
{
public:
	PixElem() :
		view(NULL),
		color(Qt::gray)
	{}

	PixElem(View* view) :
		view(view),
		color(Qt::gray)
	{}

	PixElem(const PixElem& pixElem) :
		view(pixElem.view),
		color(pixElem.color),
		rect(pixElem.rect)
	{}

	bool operator== (const PixElem& other) const
	{
		return view == other.view;
	}

	PixElem& operator= (const PixElem & other)
	{
		view = other.view;
		rect = other.rect;
		color = other.color;
		return *this;
	}

	View* view;
	QColor color;
	QRect rect;
};

class ViewPixMaps : public QList<QList<PixElem> >
{
public:
	typedef ViewPixMaps::iterator y_iterator;
	typedef QList<PixElem>::iterator x_iterator;

	ViewPixMaps();
	~ViewPixMaps()
	{}

	void fromSplitArea(SplitArea* splitArea);

	int getMaxX() { return maxX; }
	int getMinX() { return minX; }
	int getY() { return Y; }
	int getYMinX() { return YminX; }

	PixElem& getAt(int x, int y) { return (*this)[y][x]; }

	y_iterator y_begin(){return this->begin();}
	y_iterator y_end(){return this->end();}
	x_iterator x_begin(y_iterator y_it){return y_it->begin();}
	x_iterator x_end(y_iterator y_it){return y_it->end();}

	void beginMove(int x, int y);
	void moveTmp(int x, int y);
	void confirmMove();
	void cancelMove();

	void insertion(int x, int y);

	bool isModified(){return this->modif;}

private:
	int maxX;
	int minX;
	int Y;
	int YminX;

	int originX, originY;
	PixElem origin;
	int tmpX, tmpY;
	PixElem tmp;

	bool modif;

protected:
	void computeSize();
};

class ViewSelector : public QDialog
{
	Q_OBJECT

public:
	enum SelectorDialogType
	{
		MOVE,
		SELECT
	};

	ViewSelector(QWidget* parent = NULL, SelectorDialogType type = MOVE);
	ViewSelector(ViewPixMaps viewPixMap, QWidget* parent = NULL, SelectorDialogType type = MOVE);
	~ViewSelector();

	void setGLVMap(const ViewPixMaps& viewPixMap);
	ViewPixMaps getGLVMap() { return viewPixMap; }

	QPoint getInsertPoint();

	void setInsertionName(QString insertionName) { this->insertionName = insertionName; }

	bool keys[3];

protected:
	ViewPixMaps viewPixMap;

	int mouseX;
	int mouseY;

	bool pixmapGrabbed;
	QRect grabbed;

	SelectorDialogType type;

	QPoint insertPoint;
	QString insertionName;

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent ( QMouseEvent * event );
    void mousePressEvent ( QMouseEvent * event );
    void mouseReleaseEvent( QMouseEvent * event );
    void keyPressEvent( QKeyEvent * event );
    void keyReleaseEvent( QKeyEvent * event );

private:
    QPoint mouseToIndice();
};

#endif
