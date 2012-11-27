#include "splitArea.h"

#include <QLayout>
#include <iostream>

SplitArea::SplitArea(QWidget* parent) : QSplitter(Qt::Vertical, parent),
	nbRows(0),
	maxNbColumns(0),
	rowMin(0),
	nbElements(0),
	sizeUpdated(false)
{}

void SplitArea::addElementAt(QWidget* element, int x, int y)
{
	if(sizeUpdated)
		this->updateSize();

	if(y < 0)
	{
		QSplitter* hsplit = new QSplitter(Qt::Horizontal);
		hsplit->addWidget(element);
		if(maxNbColumns == 0)
			maxNbColumns = 1;
		nbRows++;
		this->insertWidget(0, hsplit);
	}
	else if(y >= nbRows)
	{
		QSplitter* hsplit = new QSplitter(Qt::Horizontal, this);
		hsplit->addWidget(element);
		if(maxNbColumns == 0)
			maxNbColumns = 1;
		nbRows++;
	}
	else
	{
		QSplitter* hsplit = (QSplitter*)(this->widget(y));
		int nbLocalColumns = hsplit->count();
		hsplit->insertWidget(x, element);
		if(nbLocalColumns == maxNbColumns)
			maxNbColumns++;
	}

	connect(element, SIGNAL(destroyed()), this, SLOT(elementRemoved()));
	nbElements++;
	this->determineRowMin();
}

void SplitArea::addElementRightTo(QWidget* element, QWidget* left)
{
	if(sizeUpdated)
		this->updateSize();

	bool found = false;
	int y = this->count();
	int x;
	for(int j = 0; j < y && !found; ++j)
	{
		QSplitter* hsplit = (QSplitter*)(this->widget(j));
		x = hsplit->count();
		for(int i = 0; i < x; ++i)
		{
			if(left == hsplit->widget(i))
			{
				hsplit->insertWidget(i + 1, element);
				found = true;
				break;
			}
		}
	}

	if(!found)
		this->addFitElement(element);
	else
		this->determineRowMin();
}

bool SplitArea::isEmpty()
{
	return (this->count() > 0) ? (((QSplitter*)this->widget(0))->count() <= 0) : true;
}

void SplitArea::updateSize()
{
	rowMin = 0;
	nbElements = 0;
	maxNbColumns = 0;
	nbRows = this->count();
	if(nbRows > 0)
	{
		int min = ((QSplitter*)(this->widget(0)))->count();
		if(min == 0)
		{
			delete this->widget(0);
			--nbRows;
		}
		int t = min;
		nbElements += t;
		for(int i = 0; i < nbRows; ++i)
		{
			if((t = ((QSplitter*)(this->widget(i)))->count()) == 0)
			{
				delete this->widget(i);
				--i;
				--nbRows;
			}
			else
			{
				if(t < min)
				{
					min = t;
					rowMin = i;
				}
				if(t > maxNbColumns)
					maxNbColumns= t;
			}
		}
	}
	sizeUpdated = false;
}

void SplitArea::addFitElement(QWidget* element)
{
	if(sizeUpdated)
		this->updateSize();

	if(nbElements == 0)
	{
		this->addElementAt(element, 0, 0);
	}
	else
	{
 		int nbColumnRowMin = ((QSplitter*)(this->widget(rowMin)))->count();
		if(nbColumnRowMin > nbRows)
			this->addElementAt(element, 0, nbRows);
		else if(nbColumnRowMin == maxNbColumns)
			this->addElementAt(element, maxNbColumns, 0);
		else
			this->addElementAt(element, nbColumnRowMin, rowMin);
	}
}

void SplitArea::elementRemoved()
{
	sizeUpdated = true;
}

void SplitArea::determineRowMin()
{
	if(this->count() == 0)
		rowMin = 0;
	else
	{
		rowMin = 0;
		int min = ((QSplitter*)(this->widget(0)))->count();
		if(min == 0)
		{
			delete this->widget(0);
			--nbRows;
		}
		int t = min;
		for(unsigned int i = 1; (int)i < nbRows; ++i)
		{
			if((t = ((QSplitter*)(this->widget(i)))->count()) == 0)
			{
				delete this->widget(i);
				--i;
				--nbRows;
			}
			else
			{
				if(t < min)
				{
					min = t;
					rowMin = i;
				}
			}
		}
	}
	sizeUpdated = false;
}
