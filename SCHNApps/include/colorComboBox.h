#ifndef COLORCOMBOBOX_H
#define COLORCOMBOBOX_H

#include <QComboBox>

class QColor;
class QWidget;

class ColorComboBox : public QComboBox
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
