namespace CGoGN
{

namespace SCHNApps
{

template <typename P_TYPE>
bool MeshTablesSurface_Radiance_P::importPLY(const std::string& filename)
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
	bool frame = false ;
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
		else if (tag == std::string("x"))
			position = true ;
		else if (tag == std::string("frameT_0"))
			frame = true ;
		else if (tag.substr(0,6) == std::string("PBcoef"))
		{
			radiance = true ;
			++nbCoefs ;
		}
	} while (tag != std::string("face")) ;

	assert((nbCoefs % 3) == 0 || !"Import only supports RGB Polynomials (i.e. Tcoef==VEC3)") ;
	nbCoefs /= 3 ;
	assert(nbCoefs == 15 || !"Import only supports bi-degree 4 polynomials");

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
	assert((position && frame && radiance) || !"Import: position, normal and radiance attributes should be provided") ;
	if (!(position && frame && radiance))
		return false ;

	VertexAttribute<VEC3, MAP> positions = this->m_map.template checkAttribute<VEC3, VERTEX, MAP>("position") ;
	VertexAttribute<VEC3, MAP> tangents = this->m_map.template checkAttribute<VEC3, VERTEX, MAP>("tangent") ;
	VertexAttribute<VEC3, MAP> normals = this->m_map.template checkAttribute<VEC3, VERTEX, MAP>("normal") ;
	VertexAttribute<VEC3, MAP> binormals = this->m_map.template checkAttribute<VEC3, VERTEX, MAP>("binormal") ;
	VertexAttribute<P_TYPE, MAP> radiances = this->m_map.template checkAttribute<P_TYPE, VERTEX, MAP>("radiance") ;

	// Read vertices
	std::vector<unsigned int> verticesID ;
	if (propSize == 4)
	{
		this->m_nbVertices = readVerticesPLY<float, P_TYPE>(fp, binary, nbVertices, nbProps, propSize, positions, tangents, normals, binormals, radiances, verticesID) ;
	}
	else if (propSize == 8)
	{
		this->m_nbVertices = readVerticesPLY<double, P_TYPE>(fp, binary, nbVertices, nbProps, propSize, positions, tangents, normals, binormals, radiances, verticesID) ;
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

template <typename REAL, typename P_TYPE>
unsigned int MeshTablesSurface_Radiance_P::readVerticesPLY(
	std::ifstream& fp,	bool binary,
	const unsigned int& nbVertices, const unsigned int& nbProps, const unsigned int& propSize,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& tangents,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& normals,
	VertexAttribute<PFP2::VEC3, PFP2::MAP>& binormals,
	VertexAttribute<P_TYPE, PFP2::MAP>& radiances,
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
			fp.read((char*)properties, nbProps * propSize) ;
		}
		else
		{
			for (unsigned int j = 0 ; j < nbProps ; ++j) // get all properties
				fp >> properties[j] ;
		}

		positions[id] = VEC3(properties[0], properties[1], properties[2]) ; // position
		tangents[id] = VEC3(properties[3], properties[4], properties[5]) ; // tangent
		binormals[id] = VEC3(properties[6], properties[7], properties[8]) ; // binormal
		normals[id] = VEC3(properties[9], properties[10], properties[11]) ; // normal

		unsigned int coefno = 12;
		radiances[id].get_coef(0,0) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(0,1) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(1,0) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(1,1) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(0,2) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(2,0) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(1,2) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(2,1) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(0,3) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(3,0) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(2,2) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(1,3) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(3,1) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(0,4) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
		coefno += 3;
		radiances[id].get_coef(4,0) = VEC3(properties[coefno],properties[coefno+1],properties[coefno+2]) ;
	}
	this->m_nbVertices = (unsigned int)(verticesID.size());
	delete[] properties ;

	return (unsigned int)(verticesID.size()) ;
}

} // namespace SCHNApps

} // namespace CGoGN
