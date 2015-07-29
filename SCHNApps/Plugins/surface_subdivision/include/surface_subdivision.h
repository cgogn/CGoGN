#ifndef _SURFACE_SUBDIVISION_PLUGIN_H_
#define _SURFACE_SUBDIVISION_PLUGIN_H_

#include "plugin_processing.h"

#include "dialog_surface_subdivision.h"

namespace CGoGN
{

namespace SCHNApps
{
/**
 * @brief Plugin that manage the subdivision of surface algorithms
 * Supported algorithms:
 * - Loop
 * - Catmull-Clark approximation
 * - Catmull-Clark interpolation
 * - Do-Sabin
 * - linear triangulation
 * - linear quadrangulation
 */
class Surface_Subdivision_Plugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
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
	void schnappsClosing();

public slots:
	/**
	 * @brief [PYTHON] apply a Loop subdivision
	 * @param mapName name of map
	 * @param positionAttributeName position attribute name
	 */
	void loopSubdivision(
		const QString& mapName,
		const QString& positionAttributeName = "position");

	/**
	 * @brief [PYTHON] apply a Catmull-Clark subdivision
	 * @param mapName name of map
	 * @param positionAttributeName position attribute name
	 * @param interp use interpolation scheme instead of approximation ?
	 */
	void CCSubdivision(
		const QString& mapName,
		const QString& positionAttributeName = "position",
		bool interp = false );

	/**
	 * @brief [PYTHON] apply a Do-Sabin subdivision
	 * @param mapName name of map
	 * @param positionAttributeName position attribute name
	 */
	void DoSabinSubdivision(
		const QString& mapName,
		const QString& positionAttributeName = "position");

	/**
	 * @brief [PYTHON] triangule all faces (with central point)
	 * @param mapName name of map
	 * @param positionAttributeName position attribute name
	 */
	void trianguleFaces(
		const QString& mapName,
		const QString& positionAttributeName = "position");

	/**
	 * @brief [PYTHON] quandrangule all faces (cut edges and add central points)
	 * @param mapName name of map
	 * @param positionAttributeName position attribute name
	 */
	void quadranguleFaces(
		const QString& mapName,
		const QString& positionAttributeName = "position");

private:
	Dialog_Surface_Subdivision* m_subdivisionDialog;

	QAction* m_subdivisionAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
