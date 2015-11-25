#ifndef _MESHTABLESURFACE_RADIANCE_P_H_
#define _MESHTABLESURFACE_RADIANCE_P_H_

#include "Algo/Import/import.h"
#include "Algo/Import/import2tables.h"

namespace CGoGN
{

namespace SCHNApps
{

class MeshTablesSurface_Radiance_P : public CGoGN::Algo::Surface::Import::MeshTablesSurface<PFP2>
{
public:
	MeshTablesSurface_Radiance_P(MAP& m) : MeshTablesSurface<PFP2>(m)
	{}

	/**
	 * @brief importPLY
	 * @param filename: the ply file to be imported. This file is supposed to have a three position, nine frame and at least three radiance scalars per vertex.
	 * @return succeeded
	 */
	template <typename P_TYPE>
	bool importPLY(const std::string& filename);

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
	template <typename REAL, typename P_TYPE>
	unsigned int readVerticesPLY(
		std::ifstream& fp,	bool binary,
		const unsigned int& nbVertices, const unsigned int& nbProps, const unsigned int& propSize,
		VertexAttribute<PFP2::VEC3, PFP2::MAP>& positions,
		VertexAttribute<PFP2::VEC3, PFP2::MAP>& tangents,
		VertexAttribute<PFP2::VEC3, PFP2::MAP>& normals,
		VertexAttribute<PFP2::VEC3, PFP2::MAP>& binormals,
		VertexAttribute<P_TYPE, PFP2::MAP>& radiances,
		std::vector<unsigned int>& verticesID
	);
};

} // namespace SCHNApps

} // namespace CGoGN

#include "meshTableSurfaceRadiance_P.hpp"

#endif
