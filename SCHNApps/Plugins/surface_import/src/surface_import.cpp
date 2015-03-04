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
//	magic line that init static variables of GenericMap in the plugins
	GenericMap::copyAllStatics(m_schnapps->getStaticPointers());

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

			AttributeContainer& cont = map->getAttributeContainer<VERTEX>();
			std::vector<std::string> names;
			std::vector<std::string> types;
			cont.getAttributesNames(names);
			cont.getAttributesTypes(types);
			for(unsigned int i = 0; i < names.size(); ++i)
				mhg->registerAttribute(VERTEX, QString::fromStdString(names[i]), QString::fromStdString(types[i]));
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

Q_EXPORT_PLUGIN2(Surface_Import_Plugin, Surface_Import_Plugin)

} // namespace SCHNApps

} // namespace CGoGN
