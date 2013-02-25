#include <QtGui>

#include "colorComboBox.h"

ColorComboBox::ColorComboBox(QWidget *widget) : QComboBox(widget)
{
	//connect( this, SIGNAL(highlighted(int)), this, SLOT(slotHighlight(int)) );
	populateList();
}

QColor ColorComboBox::color() const
{
	return qvariant_cast<QColor>(itemData(currentIndex(), Qt::DecorationRole));
}

void ColorComboBox::setColor(QColor color)
{
	setCurrentIndex(findData(color, int(Qt::DecorationRole)));
}

void ColorComboBox::populateList()
{
	//QStringList colorNames = QColor::colorNames();
	QStringList colorNames;
	colorNames <<"darkGreen"<<"green"<<"gray"<<"red"<<"white"<<"blue"<<"cyan"<<"darkMagenta"<<"yellow"<<"darkRed"<<"black"<<"magenta";

	for (int i = 0; i < colorNames.size(); ++i) {
		QColor color(colorNames[i]);

		insertItem(i, colorNames[i]);
		setItemData(i, color, Qt::DecorationRole);
	}
}

void ColorComboBox::slotHighlight(int index)
{
	const QStringList colorNames = QColor::colorNames();
	QColor color(colorNames.at(index));

	QPalette palette = this->palette();
	palette.setColor(QPalette::Highlight, color);
	setPalette(palette);
}
