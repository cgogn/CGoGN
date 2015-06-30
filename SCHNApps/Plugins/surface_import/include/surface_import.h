#ifndef _SURFACE_IMPORT_PLUGIN_H_
#define _SURFACE_IMPORT_PLUGIN_H_

#include "plugin_processing.h"

namespace CGoGN
{

namespace SCHNApps
{

class Surface_Import_Plugin : public PluginProcessing
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif

public:
	Surface_Import_Plugin()
	{}

	~Surface_Import_Plugin()
	{}

private:
	virtual bool enable();
	virtual void disable() {}

public slots:
	/**
	 * @brief import a mesh from a file
	 * @param fileName file name of mesh file
	 * @return the new maphandler that handle the mesh
	 */
	MapHandlerGen* importMeshFromFile(const QString& fileName);

	/**
	 * @brief import a mesh by opening a FileDialog
	 */
	void importMeshFromFileDialog();

	/**
	 * @brief import an image into a mesh from a file
	 * @param fileName file name of mesh file
	 * @return
	 */
	MapHandlerGen* importImageFromFile(const QString& fileName);

	/**
	 * @brief import image into a mesh by opening a FileDialog
	 */
	void importImageFromFileDialog();

private:
	QAction* importMeshAction;
	QAction* importImageAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
