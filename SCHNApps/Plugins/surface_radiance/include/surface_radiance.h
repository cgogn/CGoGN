#ifndef _SURFACE_RADIANCE_PLUGIN_H_
#define _SURFACE_RADIANCE_PLUGIN_H_

#include "plugin_interaction.h"
#include "surface_radiance_dockTab.h"

#include "Utils/sphericalHarmonics.h"
#include "Utils/Shaders/shaderRadiancePerVertex.h"

#include "Algo/Import/import.h"
#include "Algo/Import/import2tables.h"

namespace CGoGN
{

namespace SCHNApps
{

typedef SphericalHarmonics<PFP2::REAL, PFP2::VEC3> SH_TYPE;

struct MapParameters
{
	MapParameters() :
		positionVBO(NULL),
		normalVBO(NULL),
		radianceTexture(NULL),
		paramVBO(NULL)
	{}

	Utils::VBO* positionVBO;
	Utils::VBO* normalVBO;

	CGoGN::Utils::ShaderRadiancePerVertex* radiancePerVertexShader;

	VertexAttribute<SH_TYPE, PFP2::MAP> radiance;
	Utils::Texture<2, Geom::Vec3f>* radianceTexture;
	VertexAttribute<Geom::Vec2i, PFP2::MAP> param;
	Utils::VBO* paramVBO;
};

class Surface_Radiance_Plugin : public PluginInteraction
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

	friend class Surface_Radiance_DockTab;

public:
	Surface_Radiance_Plugin()
	{}

	~Surface_Radiance_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

	virtual void draw(View *view) {}
	virtual void drawMap(View* view, MapHandlerGen* map);

	virtual void keyPress(View* view, QKeyEvent* event) {}
	virtual void keyRelease(View* view, QKeyEvent* event) {}
	virtual void mousePress(View* view, QMouseEvent* event) {}
	virtual void mouseRelease(View* view, QMouseEvent* event) {}
	virtual void mouseMove(View* view, QMouseEvent* event) {}
	virtual void wheelEvent(View* view, QWheelEvent* event) {}

	virtual void viewLinked(View *view) {}
	virtual void viewUnlinked(View *view) {}

private slots:
	// slots called from SCHNApps signals
	void selectedViewChanged(View* prev, View* cur);
	void selectedMapChanged(MapHandlerGen* prev, MapHandlerGen* cur);
	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);

	// slots called from MapHandler signals
	void vboAdded(Utils::VBO* vbo);
	void vboRemoved(Utils::VBO* vbo);
	void attributeModified(unsigned int orbit, QString nameAttr);

	void importFromFileDialog();

public slots:
	// slots for Python calls
	void changePositionVBO(const QString& map, const QString& vbo);
	void changeNormalVBO(const QString& map, const QString& vbo);
	MapHandlerGen* importFromFile(const QString& fileName);

protected:
	Surface_Radiance_DockTab* m_dockTab;
	QHash<MapHandlerGen*, MapParameters> h_mapParameterSet;

	QAction* m_importAction;
};


class MeshTablesSurface_Radiance : public CGoGN::Algo::Surface::Import::MeshTablesSurface<PFP2>
{
public:
	MeshTablesSurface_Radiance(MAP& m) : MeshTablesSurface<PFP2>(m)
	{}

	/**
	 * @brief importPLY
	 * @param filename: the ply file to be imported. This file is supposed to have a three position, three normal and at least three radiance scalars per vertex.
	 * @return succeeded
	 */
	template <typename SH_TYPE>
	bool importPLY(const std::string& filename)
	{
		// Open file
		std::ifstream fp(filename, std::ios::in | std::ios::binary) ;
		if (!fp.good())
		{
			CGoGNerr << "Unable to open file " << filename << CGoGNendl ;
			return false ;
		}

		// Read quantities : #vertices, #faces, #properties, degree of polynomials
		std::string tag ;

		fp >> tag;
		if (tag != std::string("ply")) // verify file type
		{
			CGoGNerr << filename << " is not a ply file !" <<  CGoGNout ;
			return false ;
		}

		do // go to "format"
		{
			fp >> tag ;
		} while (tag != std::string("format")) ;

		fp >> tag ;
		bool binary = (tag != "ascii") ;

		do // go to #vertices
		{
			fp >> tag ;
		} while (tag != std::string("vertex")) ;
		unsigned int nbVertices ;
		fp >> nbVertices ; // Read #vertices

		bool position = false ;
		bool normal = false ;
		bool radiance = false ;

		unsigned int propSize = 0 ; // for binary read
		unsigned int nbProps = 0 ;		// # properties
		unsigned int nbCoefs = 0 ; 	// # coefficients
		do // go to #faces and count #properties
		{
			fp >> tag ;
			if (tag == std::string("property"))
			{
				++nbProps ;
			}
			else if (tag == std::string("int8") || tag == std::string("uint8"))
			{
				if (propSize < 2)
				{
					propSize = 1 ;
					std::cerr << "only float32 of float64 is yet handled" << std::endl ;
					assert(!"only float32 or float64 is yet handled") ;
				}
				else
				{
					std::cerr << "only float32 of float64 is yet handled" << std::endl ;
					assert(!"only float32 or float64 is yet handled") ;
				}
			}
			else if (tag == std::string("int16") || tag == std::string("uint16"))
			{
				if (propSize == 0 || propSize == 2)
				{
					propSize = 2 ;
					std::cerr << "only float32 of float64 is yet handled" << std::endl ;
					assert(!"only float32 or float64 is yet handled") ;
				}
				else
				{
					std::cerr << "only float32 of float64 is yet handled" << std::endl ;
					assert(!"only float32 or float64 is yet handled") ;
				}
			}
			else if (tag == std::string("int32") || tag == std::string("float32") || tag == std::string("uint32"))
			{
				if (propSize == 0 || propSize == 4)
				{
					propSize = 4 ;
				}
				else
				{
					std::cerr << "only float32 of float64 is yet handled" << std::endl ;
					assert(!"only float32 or float64 is yet handled") ;
				}
			}
			else if (tag == std::string("int64") || tag == std::string("float64"))
			{
				if (propSize == 0 || propSize == 8)
				{
					propSize = 8 ;
				}
				else
				{
					std::cerr << "only float32 of float64 is yet handled" << std::endl ;
					assert(!"only float32 or float64 is yet handled") ;
				}
			}
			else if (tag == std::string("x") || tag == std::string("y") || tag == std::string("z"))
				position = true ;
			else if (tag == std::string("nx") || tag == std::string("ny") || tag == std::string("nz"))
				normal = true ;
			else if (tag.substr(0,6) == std::string("SHcoef"))
			{
				radiance = true ;
				++nbCoefs ;
			}
		} while (tag != std::string("face")) ;

		assert((nbCoefs % 3) == 0 || !"Import only supports RGB SphericalHarmonics (i.e. Tcoef==VEC3)") ;
		nbCoefs /= 3 ;
		SH_TYPE::set_nb_coefs(nbCoefs) ;

		fp >> this->m_nbFaces ; // Read #vertices

		do // go to end of header
		{
			fp >> tag ;
		} while (tag != std::string("end_header")) ;

		if (binary)
		{
			char* endline = new char[1] ;
			fp.read(endline, sizeof(char)) ;
			if (*endline == '\r') // for windows
				fp.read(endline, sizeof(char)) ;
			assert(*endline == '\n') ;
			delete[] endline ;
		}

		// Define containers
		assert((position && normal && radiance) || !"Import: position, normal and radiance attributs should be provided") ;
		if (!(position && normal && radiance))
			return false ;

		VertexAttribute<VEC3, MAP> positions =  this->m_map.template checkAttribute<VEC3, VERTEX, MAP>("position") ;
		VertexAttribute<VEC3, MAP> normals =  this->m_map.template checkAttribute<VEC3, VERTEX, MAP>("normal") ;
		VertexAttribute<SH_TYPE, MAP> radiances = this->m_map.template checkAttribute<SH_TYPE, VERTEX, MAP>("radiance") ;

		// Read vertices
		std::vector<unsigned int> verticesID ;
		if (propSize == 4)
		{
			this->m_nbVertices = readVerticesPLY<float, SH_TYPE>(fp, binary, nbVertices, nbProps, propSize, positions, normals, radiances, verticesID) ;
		}
		else if (propSize == 8)
		{
			this->m_nbVertices = readVerticesPLY<double, SH_TYPE>(fp, binary, nbVertices, nbProps, propSize, positions, normals, radiances, verticesID) ;
		}

		// Read faces index
		this->m_nbEdges.reserve(this->m_nbFaces) ;
		this->m_emb.reserve(3 * this->m_nbFaces) ;
		for (unsigned int i = 0 ; i < this->m_nbFaces ; ++i)
		{
			// read the indices of vertices for current face
			unsigned int nbEdgesForFace ;
			if (binary)
			{
				unsigned char tmp ;
				fp.read((char*)&(tmp), sizeof(unsigned char)) ;
				nbEdgesForFace = tmp ;
			}
			else
				fp >> nbEdgesForFace ;
			this->m_nbEdges.push_back(nbEdgesForFace);

			unsigned int vertexID ;
			if (binary)
			{
				for (unsigned int j=0 ; j < nbEdgesForFace ; ++j)
				{
					fp.read((char*)&vertexID, sizeof(unsigned int)) ;
					this->m_emb.push_back(verticesID[vertexID]);
				}
			}
			else
			{
				for (unsigned int j=0 ; j < nbEdgesForFace ; ++j)
				{
					fp >> vertexID ;
					this->m_emb.push_back(verticesID[vertexID]);
				}
			}
		}

		// Close file
		fp.close() ;

		return true ;
	}

private:
	/**
	 * \brief reads the list of vertices in a radiance PLY file. The function is template of the type of arithmetic (float, double) used in the file to be read
	 * \param fp IN: the file pointer descriptor
	 * \param binary IN: binary mode for reading fp
	 * \param nbVertices IN: the number of vertices to read
	 * \param nbProps IN: the number of properties according to the header
	 * \param positions IN/OUT: the positions container
	 * \param normals IN/OUT: the normals container
	 * \param radiances IN/OUT: the radiances container
	 * \param verticesID IN/OUT: the vector of vertex IDs
	 * \return number of vertices readVerticesPLY
	 */
	template <typename REAL, typename SH_TYPE>
	unsigned int readVerticesPLY(
		std::ifstream& fp,	bool binary,
		const unsigned int& nbVertices, const unsigned int& nbProps, const unsigned int& propSize,
		VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions,
		VertexAttribute<PFP2::VEC3, PFP2::MAP>& normals,
		VertexAttribute<SH_TYPE, PFP2::MAP>& radiances,
		std::vector<unsigned int>& verticesID
	)
	{
		verticesID.reserve(nbVertices) ;

		REAL* properties = new REAL[nbProps] ;
		AttributeContainer& container = this->m_map.template getAttributeContainer<VERTEX>() ;
		for (unsigned int i = 0 ; i < nbVertices ; ++i) // Read and store properties for current vertex
		{
			unsigned int id = container.insertLine() ;
			verticesID.push_back(id) ;

			if (binary)
			{
				fp.read((char*)properties,nbProps * propSize) ;
			}
			else
			{
				for (unsigned int j = 0 ; j < nbProps ; ++j) // get all properties
					fp >> properties[j] ;
			}

			positions[id] = VEC3(properties[0],properties[1],properties[2]) ; // position
			normals[id] = VEC3(properties[3],properties[4],properties[5]) ; // normal
			int coefno = 6 ;
			for (int l = 0 ; l <= SH_TYPE::get_resolution() ; ++l) // radiance
				for (int m = -l ; m <= l ; ++m)
				{
					radiances[id].get_coef(l,m) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
					coefno += 3 ;
				}
		}
		this->m_nbVertices = verticesID.size() ;
		delete[] properties ;

		return verticesID.size() ;
	}
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
