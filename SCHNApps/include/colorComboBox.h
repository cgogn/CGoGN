#ifndef _COLORCOMBOBOX_H
#define _COLORCOMBOBOX_H

#include <QComboBox>

#include "dll.h"

class QColor;
class QWidget;

class SCHNAPPS_API ColorComboBox : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(QColor color READ color WRITE setColor USER true)

public:
	QColor color() const;
	void setColor(QColor c);

public:
	ColorComboBox(QWidget *widget = 0);

private:
	void populateList();

public slots:
	void slotHighlight(int index);
};

#endif
