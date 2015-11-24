#include "surface_import.h"

#include "schnapps.h"
#include "mapHandler.h"

#include "Algo/Import/import.h"
#include "Algo/Tiling/Surface/square.h"

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

	importMeshAction = new QAction("import mesh", this);
	m_schnapps->addMenuAction(this, "Surface;Import Mesh", importMeshAction);
	connect(importMeshAction, SIGNAL(triggered()), this, SLOT(importMeshFromFileDialog()));

	importImageAction = new QAction("import image", this);
	m_schnapps->addMenuAction(this, "Surface;Import Image", importImageAction);
	connect(importImageAction, SIGNAL(triggered()), this, SLOT(importImageFromFileDialog()));

	return true;
}

MapHandlerGen* Surface_Import_Plugin::importMeshFromFile(const QString& nameOfFile)
{
	QString fileName = SCHNApps::noBackSlash(nameOfFile);

	QFileInfo fi(fileName);
	if(fi.exists())
	{
		MapHandlerGen* mhg = m_schnapps->addMap(SCHNApps::forceASCII(fi.baseName()), 2);
		if(mhg)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
			PFP2::MAP* map = mh->getMap();

			std::vector<std::string> attrNames;
			Algo::Surface::Import::importMesh<PFP2>(*map, SCHNApps::niceStdString(fileName), attrNames);

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
		pythonRecording("importMeshFromFile", mhg->getName(), fileName);
		return mhg;
	}
	else
		return NULL;
}

void Surface_Import_Plugin::importMeshFromFileDialog()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(m_schnapps, "Import surface meshes", m_schnapps->getAppPath(), "Surface mesh Files (*.ply *.off *.trian)");
	QStringList::Iterator it = fileNames.begin();
	while(it != fileNames.end())
	{
		importMeshFromFile(*it);
		++it;
	}
}

MapHandlerGen* Surface_Import_Plugin::importImageFromFile(const QString& nameOfFile)
{
	QString fileName = SCHNApps::noBackSlash(nameOfFile);

	QFileInfo fi(fileName);
	if(fi.exists())
	{
		MapHandlerGen* mhg = m_schnapps->addMap(SCHNApps::forceASCII(fi.baseName()), 2);
		if(mhg)
		{
			MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
			PFP2::MAP* map = mh->getMap();

			VertexAttribute<PFP2::VEC3, PFP2::MAP> position = map->addAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>("position");
			VertexAttribute<PFP2::VEC3, PFP2::MAP> color = map->addAttribute<PFP2::VEC3, VERTEX, PFP2::MAP>("color");

			QImage image;
			QString extension = fi.suffix();
			extension.toUpper();
			if(!image.load(fileName, extension.toUtf8().constData()))
			{
				CGoGNout << "Image [" << fileName.toStdString() << "] has not been loaded correctly" << CGoGNendl;
				return NULL;
			}

			int imageX = image.width();
			int imageY = image.height();
			Algo::Surface::Tilings::Square::Grid<PFP2> grid(*map, imageX - 1, imageY - 1);
			grid.embedIntoGrid(position, imageX - 1, imageY - 1);
			const std::vector<Dart>& vDarts = grid.getVertexDarts();
			QRgb pixel;
			for(int i = 0; i < imageX; ++i)
			{
				for(int j = 0; j < imageY; ++j)
				{
					pixel = image.pixel(i, (imageY-j) - 1);
					color[vDarts[j*imageX + i]] = PFP2::VEC3(qRed(pixel)/255.f, qGreen(pixel)/255.f, qBlue(pixel)/255.f);
				}
			}

			mh->registerAttribute(position);
			mh->registerAttribute(color);

		}
		pythonRecording("importImageFromFile", mhg->getName(), fileName);
		return mhg;
	}
	else
		return NULL;
}

void Surface_Import_Plugin::importImageFromFileDialog()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(m_schnapps, "Import images", m_schnapps->getAppPath(), "Surface mesh Files (*.png *.jpg *.jpeg)");
	QStringList::Iterator it = fileNames.begin();
	while(it != fileNames.end())
	{
		importImageFromFile(*it);
		++it;
	}
}

#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#else
	Q_EXPORT_PLUGIN2(Surface_Import_Plugin, Surface_Import_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
