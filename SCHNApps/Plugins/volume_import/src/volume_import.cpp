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
	//	magic line that init static variables of GenericMap in the plugins
	GenericMap::copyAllStatics(m_schnapps->getStaticPointers());

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
		pythonRecording("importFromFile", fi.baseName(), fileName);
		MapHandlerGen* mhg = m_schnapps->addMap(fi.baseName(), 3);
		if(mhg)
		{
			MapHandler<PFP3>* mh = static_cast<MapHandler<PFP3>*>(mhg);
			PFP3::MAP* map = mh->getMap();

			std::vector<std::string> attrNames ;
			Algo::Volume::Import::importMesh<PFP3>(*map, fileName.toStdString(), attrNames);

			for (unsigned int orbit = VERTEX; orbit <= VOLUME; orbit++)
			{
				AttributeContainer& cont = map->getAttributeContainer(orbit);
				std::vector<std::string> names;
				std::vector<std::string> types;
				cont.getAttributesNames(names);
				cont.getAttributesTypes(types);
				for(unsigned int i = 0; i < names.size(); ++i)
					mhg->registerAttribute(orbit, QString::fromStdString(names[i]), QString::fromStdString(types[i]));
			}
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

#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#else
	Q_EXPORT_PLUGIN2(Volume_Import_Plugin, Volume_Import_Plugin)
#endif


} // namespace SCHNApps

} // namespace CGoGN
