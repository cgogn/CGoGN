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

bool Surface_Import_Plugin::enable()
{
	importAction = new QAction("import", this);
	m_schnapps->addMenuAction(this, "Surface;Import", importAction);
	connect(importAction, SIGNAL(triggered()), this, SLOT(importFromFileDialog()));
	return true;
}

MapHandlerGen* Surface_Import_Plugin::importFromFile(const QString& fileName)
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
			VertexAttribute<PFP2::VEC3, PFP2::MAP> position = map->getAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>(attrNames[0]);
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

void Surface_Import_Plugin::importFromFileDialog()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(m_schnapps, "Import surfaces", m_schnapps->getAppPath(), "Surface mesh Files (*.ply *.off *.trian)");
	QStringList::Iterator it = fileNames.begin();
	while(it != fileNames.end()) {
		importFromFile(*it);
		++it;
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(Surface_Import_Plugin, Surface_Import_Plugin)
#else
Q_EXPORT_PLUGIN2(Surface_Import_PluginD, Surface_Import_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
