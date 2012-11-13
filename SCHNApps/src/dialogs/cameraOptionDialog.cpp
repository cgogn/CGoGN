#include "cameraOptionDialog.h"

#include "camera.h"

CameraOptionDialog::CameraOptionDialog(Camera* camera, QWidget* parent=0) : QDialog(parent),
	m_camera(camera)
{
	this->setupUi(this);

	if(camera){
		groupBox->setTitle(QString::fromUtf8("Camera \"") + camera->getName() + "\" draw option");
		if(camera->isDrawn()){
			draw_Check->setChecked(true);

			farPlane_Check->setChecked(camera->isFarPlaneDrawn());
			farPlane_Check->setEnabled(true);

			label->setEnabled(true);
			scale_Spin->setEnabled(true);
			scale_Spin->setValue(camera->getDrawScale());

			if(camera->isPathDrawn()){
				groupBox_2->setEnabled(true);
				drawPath_Check->setChecked(true);
			}
			drawAxis_Check->setChecked(camera->isPathAxisDrawn());
			frameScale_Spin->setValue(camera->getPathDrawScale());
		}
	}

	connect(draw_Check, SIGNAL(toggled(bool)), this, SLOT(drawChecked(bool)));
	connect(drawPath_Check, SIGNAL(toggled(bool)), this, SLOT(drawPathChecked(bool)));
}

void CameraOptionDialog::drawChecked(bool b){
	farPlane_Check->setEnabled(b);
	scale_Spin->setEnabled(b);
}

void CameraOptionDialog::drawPathChecked(bool b){
	groupBox_2->setEnabled(b);
}

void CameraOptionDialog::accept(){
	if(m_camera){
		m_camera->enableDraw(draw_Check->isChecked());
		if(draw_Check->isChecked()){
			m_camera->enableDrawFarPlan(farPlane_Check->isChecked());
			m_camera->setDrawScale(scale_Spin->value());
		}
		m_camera->enablePathDraw(drawPath_Check->isChecked());
		if(drawPath_Check->isChecked()){
			m_camera->enablePathAxisDrawn(drawAxis_Check->isChecked());
			m_camera->setPathDrawScale(frameScale_Spin->value());
		}
	}
	QDialog::accept();
}
