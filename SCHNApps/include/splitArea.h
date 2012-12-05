#ifndef _SPLITAREA_H_
#define _SPLITAREA_H_

#include <QSplitter>

class SplitArea : public QSplitter
{
	Q_OBJECT

public:
	SplitArea(QWidget* parent = NULL);

	int getNbRows() { return nbRows; }

	void addFitElement(QWidget* element);
	void addElementAt(QWidget* element, int x, int y);

	void addElementRightTo(QWidget* element, QWidget* left);

	bool isEmpty();

	void updateSize();

protected:
	int nbRows;
	int maxNbColumns;
	int rowMin;
	int nbElements;

protected slots:
	void elementRemoved();

private:
	bool sizeUpdated;
	void determineRowMin();
};

#endif
