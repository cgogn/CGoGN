#ifndef _SURFACE_RADIANCE_DIALOG_COMPUTEDISTANCE_H_
#define _SURFACE_RADIANCE_DIALOG_COMPUTEDISTANCE_H_

#include "ui_dialog_computeRadianceDistance.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class MapHandlerGen;

class Dialog_ComputeRadianceDistance : public QDialog, public Ui::Dialog_ComputeRadianceDistance
{
	Q_OBJECT

public:
	Dialog_ComputeRadianceDistance(SCHNApps* s);

private:
	SCHNApps* m_schnapps;
	MapHandlerGen* m_selectedMap1;
	MapHandlerGen* m_selectedMap2;

private slots:
	void selectedMap1Changed();
	void selectedMap2Changed();
	void addMapToList(MapHandlerGen* map);
	void removeMapFromList(MapHandlerGen* map);
	void addAttributeToList1(unsigned int orbit, const QString& nameAttr);
	void addAttributeToList2(unsigned int orbit, const QString& nameAttr);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif // _SURFACE_RADIANCE_DIALOG_COMPUTEDISTANCE_H_
