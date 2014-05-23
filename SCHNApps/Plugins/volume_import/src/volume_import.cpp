#include "volume_import.h"

#include "schnapps.h"
#include "mapHandler.h"

#include "Algo/Import/import.h"

#include <QFileDialog>
#include <QFileInfo>

namespace CGoGN
{

namespace SCHNApps
{

bool Volume_Import_Plugin::enable()
{
	importAction = new QAction("import", this);
	m_schnapps->addMenuAction(this, "Volume;Import", importAction);
	connect(importAction, SIGNAL(triggered()), this, SLOT(importFromFileDialog()));
	return true;
}

MapHandlerGen* Volume_Import_Plugin::importFromFile(const QString& fileName)
{
	QFileInfo fi(fileName);
	if(fi.exists())
	{
		MapHandlerGen* mhg = m_schnapps->addMap(fi.baseName(), 3);
		if(mhg)
		{
			MapHandler<PFP3>* mh = static_cast<MapHandler<PFP3>*>(mhg);
			PFP3::MAP* map = mh->getMap();

			std::vector<std::string> attrNames ;
			Algo::Volume::Import::importMesh<PFP3>(*map, fileName.toStdString(), attrNames);

			// get vertex position attribute
			VertexAttribute<PFP3::VEC3, PFP3::MAP> position = map->getAttribute<PFP3::VEC3, VERTEX, PFP3::MAP>(attrNames[0]);
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

void Volume_Import_Plugin::importFromFileDialog()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(m_schnapps, "Import volumes", m_schnapps->getAppPath(), "Volume mesh Files (*.msh *.vtu *.nas *.vbgz *.tetmesh *.node *.ts *.off *.tet)");
	QStringList::Iterator it = fileNames.begin();
	while(it != fileNames.end()) {
		importFromFile(*it);
		++it;
	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(Volume_Import_Plugin, Volume_Import_Plugin)
#else
Q_EXPORT_PLUGIN2(Volume_Import_PluginD, Volume_Import_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
