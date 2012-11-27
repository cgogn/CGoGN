#ifndef _SPLITAREA_H_
#define _SPLITAREA_H_

#include <QSplitter>

class SplitArea : public QSplitter{
	Q_OBJECT
public:
	SplitArea(QWidget* parent=0);

	void addFitElement(QWidget* element);
	void addElementAt(QWidget* element, int x, int y);

	void addElementRightTo(QWidget* element, QWidget* left);

	bool isEmpty();
	int getNbRow(){return nbRow;}

	void updateSize();

protected:
	int nbRow, nbMaxColumn;
	int rowMin;
	int nbElement;

protected slots:
	void elementRemoved();

private:
	bool sizeUpdated;

	void determineRowMin();

};


#endif
