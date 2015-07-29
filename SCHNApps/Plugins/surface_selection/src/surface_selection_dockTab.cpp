#include "surface_selection_dockTab.h"

#include "surface_selection.h"
#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

Surface_Selection_DockTab::Surface_Selection_DockTab(SCHNApps* s, Surface_Selection_Plugin* p) :
	m_schnapps(s),
	m_plugin(p),
	b_updatingUI(false)
{
	setupUi(this);

	combo_color->setEnabled(true);

	connect(combo_positionAttribute, SIGNAL(currentIndexChanged(int)), this, SLOT(positionAttributeChanged(int)));
	connect(combo_normalAttribute, SIGNAL(currentIndexChanged(int)), this, SLOT(normalAttributeChanged(int)));
	connect(combo_selectionMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(selectionMethodChanged(int)));
	connect(slider_verticesScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(verticesScaleFactorChanged(int)));
	connect(slider_verticesScaleFactor, SIGNAL(sliderPressed()), this, SLOT(verticesScaleFactorPressed()));
	connect(combo_color, SIGNAL(currentIndexChanged(int)), this, SLOT(colorChanged(int)));
	connect(button_clear, SIGNAL(pressed()), this, SLOT(clearPressed()));

	// force color for map parameter updating
//	combo_color->setCurrentIndex(0);
}





void Surface_Selection_DockTab::positionAttributeChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(map);
			m_plugin->h_parameterSet[map].positionAttribute = mh->getAttribute<PFP2::VEC3, VERTEX>(combo_positionAttribute->currentText());
			m_plugin->updateSelectedCellsRendering();
			m_plugin->pythonRecording("changePositionAttribute", "", map->getName(), combo_positionAttribute->currentText());
		}
	}
}

void Surface_Selection_DockTab::normalAttributeChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(map)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(map);
			m_plugin->h_parameterSet[map].normalAttribute = mh->getAttribute<PFP2::VEC3, VERTEX>(combo_normalAttribute->currentText());
			m_plugin->updateSelectedCellsRendering();
			m_plugin->pythonRecording("changeNormalAttribute", "", map->getName(), combo_normalAttribute->currentText());
		}
	}
}

void Surface_Selection_DockTab::selectionMethodChanged(int index)
{
	if(!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (map)
		{
			m_plugin->h_parameterSet[map].selectionMethod = SelectionMethod(index);
			m_plugin->pythonRecording("changeSelectionMethod", "", map->getName(), index);
		}
		
		switch (index)
		{
		case 0:
			this->spin_angle_radius->setHidden(true);
			this->label_angle_radius->setText(QString());
			break;
		case 1:
			this->spin_angle_radius->setHidden(false);
			this->spin_angle_radius->setValue(m_plugin->m_selectionRadiusBase * m_plugin->m_selectionRadiusCoeff);
			this->label_angle_radius->setText(QString("Radius:"));
			break;
		case 2:
			this->spin_angle_radius->setHidden(false);
			this->spin_angle_radius->setValue(m_plugin->m_normalAngleThreshold / M_PI * 180);
			this->label_angle_radius->setText(QString("Angle:"));
			break;
		default:
			break;
		}

	}
}

void Surface_Selection_DockTab::verticesScaleFactorPressed()
{
	if (!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (map)
		{
			m_plugin->h_parameterSet[map].basePSradius = map->getBBdiagSize() / (std::sqrt(map->getNbOrbits(EDGE)));
			m_plugin->pythonRecording("changeVerticesBaseSize", "", map->getName(), m_plugin->h_parameterSet[map].basePSradius);

			View* view = m_schnapps->getSelectedView();
			if (view)
				view->updateGL();
		}
	}
}


void Surface_Selection_DockTab::verticesScaleFactorChanged(int i)
{
	if (!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (map)
		{
			m_plugin->h_parameterSet[map].verticesScaleFactor = i / 50.0;
			m_plugin->pythonRecording("changeVerticesScaleFactor", "", map->getName(), i / 50.0);

			View* view = m_schnapps->getSelectedView();
			if (view)
				view->updateGL();

		}
	}
}



void Surface_Selection_DockTab::addVertexAttribute(const QString& nameAttr)
{
	b_updatingUI = true;
	QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));
	const QString& typeAttr = m_schnapps->getSelectedMap()->getAttributeTypeName(VERTEX, nameAttr);
	if(typeAttr == vec3TypeName)
	{
		combo_positionAttribute->addItem(nameAttr);
		combo_normalAttribute->addItem(nameAttr);
	}
	b_updatingUI = false;
}


void Surface_Selection_DockTab::colorChanged(int i)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();

		if (view && map )
		{
			QColor& col = m_plugin->h_parameterSet[map].color;
			if (col != combo_color->color())
			{
				col = combo_color->color();
				m_plugin->updateSelectedCellsRendering();
				m_plugin->pythonRecording("changeSelectedColor", "", map->getName(), combo_color->color().name());
			}
			view->updateGL();
		}
	}
}


void Surface_Selection_DockTab::clearPressed()
{
	if (!b_updatingUI)
	{
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		unsigned int orbit = m_schnapps->getCurrentOrbit();

		if (map)
		{
			m_plugin->clearSelection(map->getName(), orbit, m_schnapps->getSelectedSelector(orbit)->getName());
			m_plugin->pythonRecording("clearSelection", "", map->getName(), orbit, m_schnapps->getSelectedSelector(orbit)->getName());
		}
	}
}

void Surface_Selection_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionAttribute->clear();
	combo_positionAttribute->addItem("- select attribute -");
	combo_normalAttribute->clear();
	combo_normalAttribute->addItem("- select attribute -");

	MapHandlerGen* map = m_schnapps->getSelectedMap();

	if(map)
	{
		const MapParameters& p = m_plugin->h_parameterSet[map];

		QString vec3TypeName = QString::fromStdString(nameOfType(PFP2::VEC3()));

		unsigned int i = 1;
		const AttributeSet& attribs = map->getAttributeSet(VERTEX);
		for(AttributeSet::const_iterator it = attribs.constBegin(); it != attribs.constEnd(); ++it)
		{
			if(it.value() == vec3TypeName)
			{
				combo_positionAttribute->addItem(it.key());
				if(p.positionAttribute.isValid() && it.key() == QString::fromStdString(p.positionAttribute.name()))
					combo_positionAttribute->setCurrentIndex(i);

				combo_normalAttribute->addItem(it.key());
				if(p.normalAttribute.isValid() && it.key() == QString::fromStdString(p.normalAttribute.name()))
					combo_normalAttribute->setCurrentIndex(i);

				++i;
			}
		}

		combo_selectionMethod->setCurrentIndex(p.selectionMethod);
		combo_color->setColor(p.color);

		slider_verticesScaleFactor->setValue(int(50.0*p.verticesScaleFactor));

		switch (p.selectionMethod)
		{
		case 0:
			this->spin_angle_radius->setHidden(true);
			this->label_angle_radius->setText(QString());
			break;
		case 1:
			this->spin_angle_radius->setHidden(false);
			this->spin_angle_radius->setValue(m_plugin->m_selectionRadiusBase * m_plugin->m_selectionRadiusCoeff);
			this->label_angle_radius->setText(QString("Radius:"));
			break;
		case 2:
			this->spin_angle_radius->setHidden(false);
			this->spin_angle_radius->setValue(m_plugin->m_normalAngleThreshold / M_PI * 180);
			this->label_angle_radius->setText(QString("Angle:"));
			break;
		default:
			break;
		}

	}

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
