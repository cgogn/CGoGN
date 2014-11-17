#ifndef _VIEWBUTTONAREA_H_
#define _VIEWBUTTONAREA_H_

#include <QPixmap>
#include <QSize>
#include <QRect>
#include <QPainter>
#include <QList>

#include <iostream>

#define WITH_QT 1
#include "Utils/textures.h"
#include "Utils/Shaders/shaderWallPaper.h"

namespace CGoGN
{

namespace SCHNApps
{

class View;

class ViewButton : public QObject
{
	Q_OBJECT

public:
	ViewButton(const QString& image, View* view);
	~ViewButton();

	QSize getSize();

	void click(int x, int y, int globalX, int globalY);
	void drawAt(int x, int y, Utils::ShaderWallPaper* shader);

	static const int SIZE = 24;
	static const int SPACE = 4;

protected:
	QString m_img;
	View* m_view;
	Utils::Texture<2,Geom::Vec3uc>* m_texture;

signals:
	void clicked(int x, int y, int globalX, int globalY);
};

class ViewButtonArea : public QObject
{
	Q_OBJECT

public:
	ViewButtonArea(View* view);

	~ViewButtonArea();

	void addButton(ViewButton* button);
	void removeButton(ViewButton* button);

	bool isClicked(int x, int y);
	void clickButton(int x, int y, int globalX, int globalY);

	const QRect& getForm() { return m_form; }

	void setTopRightPosition(int x, int y);

	void setTopLeftPosition(int x, int y);

	void draw();

protected:
	View* m_view;
	QRect m_form;
	QList<ViewButton*> l_buttons;
	Utils::ShaderWallPaper* m_shaderButton;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
