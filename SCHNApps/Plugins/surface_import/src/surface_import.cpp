#include "surface_import.h"

#include "schnapps.h"
#include "mapHandler.h"

#include "Algo/Import/import.h"

#include <QFileDialog>
#include <QFileInfo>

namespace CGoGN
{

namespace SCHNApps
{

bool ImportSurfacePlugin::enable()
{
	importAction = new QAction("import", this);
	m_schnapps->addMenuAction(this, "Surface;Import", importAction);
	connect(importAction, SIGNAL(triggered()), this, SLOT(importFromFileDialog()));
	return true;
}

MapHandlerGen* ImportSurfacePlugin::importFromFile(const QString& fileName)
{
	QFileInfo fi(fileName);
	if(fi.exists())
	{
		MapHandlerGen* mhg = m_schnapps->addMap(fi.baseName(), 2);
		if(mhg)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
			PFP2::MAP* map = mh->getMap();

			std::vector<std::string> attrNames;
			Algo::Surface::Import::importMesh<PFP2>(*map, fileName.toStdString(), attrNames);

			// get vertex position attribute
			VertexAttribute<PFP2::VEC3> position = map->getAttribute<PFP2::VEC3, VERTEX>(attrNames[0]);
			mh->registerAttribute(position);

			// update corresponding VBO & emit attribute update signal
			mh->notifyAttributeModification(position);

			// compute map bounding box
			mh->updateBB(position);
		}
		return mhg;
	}
	else
		return NULL;
}

void ImportSurfacePlugin::importFromFileDialog()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(m_schnapps, "Import surfaces", m_schnapps->getAppPath(), "Surface mesh Files (*.ply *.off *.trian)");
	QStringList::Iterator it = fileNames.begin();
	while(it != fileNames.end()) {
		importFromFile(*it);
		++it;
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(ImportSurfacePlugin, ImportSurfacePlugin)
#else
Q_EXPORT_PLUGIN2(ImportSurfacePluginD, ImportSurfacePlugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
