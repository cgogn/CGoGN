#ifndef _SURFACE_SUBDIVISION_PLUGIN_H_
#define _SURFACE_SUBDIVISION_PLUGIN_H_

#include "plugin_processing.h"

#include "dialog_surface_subdivision.h"

namespace CGoGN
{

namespace SCHNApps
{

class Surface_Subdivision_Plugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	Surface_Subdivision_Plugin()
	{}

	~Surface_Subdivision_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

private slots:
	void openSubdivisionDialog();
	void subdivideFromDialog();
	void appsFinished();

public slots:
	void loopSubdivision(
		const QString& mapName,
		const QString& positionAttributeName = "position"
	);
	void CCSubdivision(
		const QString& mapName,
		const QString& positionAttributeName = "position"
	);
	void trianguleFaces(
		const QString& mapName,
		const QString& positionAttributeName = "position"
	);

private:
	Dialog_Surface_Subdivision* m_subdivisionDialog;

	QAction* m_subdivisionAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
