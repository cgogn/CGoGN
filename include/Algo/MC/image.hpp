/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <math.h>
#include <typeinfo>

#include "Utils/img3D_IO.h"
#include "Zinrimage.h"

namespace CGoGN
{

namespace Algo
{

namespace MC
{

template< typename  DataType >
Image<DataType>::Image():
	m_Data	(NULL),
	m_OX	(0),
	m_OY	(0),
	m_OZ	(0)
{
}



template< typename  DataType >
Image<DataType>::Image(DataType *data, int32 wx, int32 wy, int32 wz, float sx, float sy, float sz, bool copy ):
	m_WX   (wx),
	m_WY   (wy),
	m_WZ   (wz),
	m_OX   (0),
	m_OY   (0),
	m_OZ   (0),
	m_SX   (sx),
	m_SY   (sy),
	m_SZ   (sz)
{
	if ( copy )
	{
		m_Data = new DataType[wx*wy*wz];
		memcpy( m_Data, data, wx*wy*wz*sizeof(DataType) );
		m_Alloc=true;
	}
	else
	{
		m_Data = data;
		m_Alloc=false;
	}

	m_WXY = m_WX * m_WY;
}



template< typename  DataType >
void Image<DataType>::loadRaw(char *filename)
{
	std::ifstream fp( filename, std::ios::in|std::ios::binary);
	if (!fp.good())
	{
		CGoGNerr << "Mesh_Base::loadRaw: Unable to open file " << CGoGNendl;
		exit(0);
	}

	// read size
	fp.read(reinterpret_cast<char*>(&m_WX),sizeof(int32));
	fp.read(reinterpret_cast<char*>(&m_WY),sizeof(int32));
	fp.read(reinterpret_cast<char*>(&m_WZ),sizeof(int32));

	m_WXY = m_WX * m_WY;

	int32 total = m_WXY * m_WZ;

	m_Data = new DataType[total];
	// read data
	fp.read( reinterpret_cast<char*>(m_Data),total*sizeof(DataType) );

	m_SX = 1.0;
	m_SY = 1.0;
	m_SZ = 1.0;

	m_Alloc=true;

}



template< typename  DataType >
void Image<DataType>::loadVox(char *filename)
{
	std::ifstream in(filename);
	if (!in)
	{
		CGoGNerr << "Mesh_Base::loadVox: Unable to open file " << CGoGNendl;
		exit(0);
	}

	m_SX = 1.0 ;
	m_SY = 1.0 ;
	m_SZ = 1.0 ;

	// read encoding
	while(in.good())
	{
		char line[1024] ;
		in.getline(line, 1024) ;
		std::istringstream line_input(line) ;
		std::string keyword ;
		line_input >> keyword ;

		if(keyword == "encoding") {
			std::string encoding_str ;
			line_input >> encoding_str ;
			if(encoding_str != "GRAY") {
				CGoGNerr << "loadVox : invalid encoding: \'" << encoding_str << "\'" << CGoGNendl ;
				exit(0) ;
			}
		} else if(keyword == "nu") {
			line_input >> m_WX ;
		} else if(keyword == "nv") {
			line_input >> m_WY ;
		} else if(keyword == "nw") {
			line_input >> m_WZ ;
		} else if(keyword == "scale_u") {
			line_input >> m_SX ;
		} else if(keyword == "scale_v") {
			line_input >> m_SY ;
		} else if(keyword == "scale_w") {
			line_input >> m_SZ ;
		}
	}

	m_WXY = m_WX * m_WY;
	int32 total = m_WXY * m_WZ;

	m_Data = new DataType[total];

	int filename_s = strlen(filename) ;
	char datafile[filename_s] ;
	strcpy(datafile, filename) ;
	datafile[filename_s-3] = 'r' ;
	datafile[filename_s-2] = 'a' ;
	datafile[filename_s-1] = 'w' ;

	std::ifstream fp(datafile, std::ios::in|std::ios::binary);
	fp.read(reinterpret_cast<char*>(m_Data), total*sizeof(DataType));

	m_Alloc=true;
}

template< typename  DataType >
bool Image<DataType>::loadPNG3D(const char* filename)
{
	CGoGN::Utils::Img3D_IO::initIO();

	int tag;
	//en fonction de DataType utiliser la bonne fonction de chargement,
	if ( (typeid(DataType)==typeid(char)) ||  (typeid(DataType)==typeid(unsigned char)) )
		m_Data = reinterpret_cast<DataType*>(CGoGN::Utils::Img3D_IO::loadVal_8(const_cast<char*>(filename), m_WX,m_WY,m_WZ, m_SX,m_SY,m_SZ,tag));

	m_WXY = m_WX * m_WY;


	if (m_Data==NULL)
	{
		CGoGNerr << " ERROR : try to load an image of type " << typeid(DataType).name() << " although file ";
		return false;
	}

	m_Alloc=true;

	return true;
}


template< typename  DataType >
bool Image<DataType>::loadInrgz(const char* filename)
{
		mImage = readZInrimage(filename);

		// A modifier en verifiant le type de donne
		// contenu dans l'image.

		m_Data = (DataType*)mImage->data;
		m_Alloc = false;
		m_WX = mImage->ncols;
		m_WY = mImage->nrows;
		m_WZ = mImage->nplanes;


		m_WXY = m_WX*m_WY;

		if (m_Data==NULL)
		{
			CGoGNerr << "problem loading image" << CGoGNendl;
			return false;
		}

		m_SX = 1.0;
		m_SY = 1.0;
		m_SZ = 1.0;

		return true;
}




template< typename  DataType >
bool Image<DataType>::loadIPB(const char* filename)
{
	// chargement fichier

	// taille de l'image en X
	m_WX= 256;// A MODIFIER
	// taille de l'image en Y
	m_WY= 256;// A MODIFIER
	// taille de l'image en Z
	m_WZ= 256;// A MODIFIER
// taille d'une coupe XY de l'image
	m_WXY = m_WX * m_WY;

	// taille des voxels en
	m_SX= 1.0f;// A MODIFIER
	m_SY= 1.0f;// A MODIFIER
	m_SZ= 1.0f;// A MODIFIER

	// pointeur sur les donnees
	m_Data =  NULL;// A MODIFIER
	return true;
}




template< typename  DataType >
Image<DataType>::~Image()
{
	// ATTENTION A MODIFIER PEUT-ETRE SI m_data EST ALLOUE AILLEURS !!

	if ( m_Alloc && (m_Data != NULL))
	{
		delete[] m_Data;
	}
}


template< typename  DataType >
DataType Image<DataType>::getVoxel(int _lX, int _lY, int _lZ)
{
  return m_Data[_lX + m_WX*_lY + m_WXY*_lZ];
}


template< typename  DataType >
const DataType* Image<DataType>::getVoxelPtr(int lX, int lY, int lZ) const
{
	return m_Data + lX + m_WX*lY + m_WXY*lZ;
}


template< typename  DataType >
DataType* Image<DataType>::getVoxelPtr(int lX, int lY, int  lZ)
{
	return m_Data + lX + m_WX*lY + m_WXY*lZ;
}



/*
*  add a frame of Zero to the image
*/
template< typename  DataType >
Image<DataType>* Image<DataType>::addFrame(int frameMax)
{

	float minVS = std::max(m_SX, std::max(m_SY, m_SZ));
	float realFS = static_cast<float>(frameMax) * minVS;

    // real frame size for anisotropic images
    int32 lFX = static_cast<int32>( ceilf( realFS / m_SX) );
	int32 lFY = static_cast<int32>( ceilf( realFS / m_SY) );
	int32 lFZ = static_cast<int32>( ceilf( realFS / m_SZ) );

	int lTx = m_WX+2*lFX;
	int lTy = m_WY+2*lFY;
	int lTz = m_WZ+2*lFZ;
	int lTxy = lTx*lTy;

	// define Zero
	DataType Zero = DataType();

	// allocate new data
	DataType *newData = new DataType[lTxy*lTz];

	// get pointer to original data
	DataType *original = m_Data;


	DataType *data = newData;
	int32 sizeFrameZ = lTxy * lFZ;

	// frame Z upper
	for(int32 i=0; i<sizeFrameZ; i++)
	{
		*data++ = Zero;
	}

	int32 nbsl = lTz - 2*lFZ;
	for(int32 i=0; i<nbsl; i++)
	{
		int32 sizeFrameY = lTx*lFY;
		// frame Y upper
		for(int32 j=0; j<sizeFrameY; j++)
		{
			*data++ = Zero;
		}

		int32 nbrow = lTy - 2*lFY;
		for(int32 k=0; k<nbrow; k++)
		{
			// frame X upper
			for(int32 l=0; l<	lFX; l++)
			{
				*data++ = Zero;
			}

			// copy original Data
			int32 nbcol = lTx - 2*lFX;
			for(int32 l=0; l<nbcol; l++)
			{
				*data++ = *original++;
			}

			// frame X lower
			for(int32 l=0; l<	lFX; l++)
			{
				*data++ = Zero;
			}

		}

		// frame Y upper
		for(int32 j=0; j<sizeFrameY; j++)
		{
			*data++ = Zero;
		}
	}

	// frame Z lower
	for(int32 i=0; i<sizeFrameZ; i++)
	{
		*data++ = Zero;
	}

	Image<DataType>* newImg = new Image<DataType>(newData,lTx,lTy,lTz,getVoxSizeX(),getVoxSizeY(),getVoxSizeZ());

	// set origin of real data in image
	newImg->setOrigin(m_OX+lFX, m_OY+lFY, m_OZ+lFZ);

	return newImg;
}


template< typename  DataType >
template< typename Windowing >
float Image<DataType>::computeVolume(const Windowing& wind) const
{
	int32 nbv = getWidthXY()*getWidthZ();

	const DataType *data = getData();

	// volume in number of voxel
	int32 vol=0;

	for(int32 i=0; i<nbv; i++)
	{
		if (wind.inside(*data))
		{
			vol++;
		}
		data++;
	}


	return float(vol);
}

template< typename  DataType >
Image<DataType>* Image<DataType>::Blur3()
{

	int txm = m_WX-1;
	int tym = m_WY-1;
	int tzm = m_WZ-1;

	DataType* data2 = new DataType[m_WX*m_WY*m_WZ];
	Image<DataType>* newImg = new Image<DataType>(data2,m_WX,m_WY,m_WZ,getVoxSizeX(),getVoxSizeY(),getVoxSizeZ());
	newImg->m_Alloc=true;
	// set origin of real data in image ??

	// for frame
	for(int y=0; y<=tym; ++y)
		for(int x=0; x<=txm; ++x)
			*(newImg->getVoxelPtr(x,y,0)) = *(getVoxelPtr(x,y,0));

	for(int z=1; z<tzm; ++z)
	{
		for(int x=0; x<=txm; ++x)
			*(newImg->getVoxelPtr(x,0,z)) = *(getVoxelPtr(x,0,z));
		for(int y=1; y<tym; ++y)
		{
			*(newImg->getVoxelPtr(0,y,z)) = *(getVoxelPtr(0,y,z));
			#pragma omp parallel for // OpenMP
			for(int x=1; x<txm; ++x)
			{
				DataType* ori = getVoxelPtr(x,y,z);
				DataType* dest = newImg->getVoxelPtr(x,y,z);
				DataType* ptr = ori - m_WXY - m_WX -1;
				double val=0.0;
				for (int i=0; i<3;++i)
				{
					val += (*ptr++);
					val += (*ptr++);
					val += (*ptr);
					ptr += m_WX;
					val += (*ptr--);
					val += (*ptr--);
					val += (*ptr);
					ptr += m_WX;
					val += (*ptr++);
					val += (*ptr++);
					val += (*ptr);
					ptr += m_WXY -( 2+m_WX*2);
				}
				val += 3.0 * (*ori);
				val /= (27.0 + 3.0);
				DataType res(val);
				*dest= res;
			}
			*(newImg->getVoxelPtr(txm,y,z)) = *(getVoxelPtr(txm,y,z));
		}
		for(int x=0; x<=txm; ++x)
			*(newImg->getVoxelPtr(x,tym,z)) = *(getVoxelPtr(x,tym,z));

	}
	for(int y=0; y<=tym; ++y)
		for(int x=1; x<txm; ++x)
			*(newImg->getVoxelPtr(x,y,tzm)) = *(getVoxelPtr(x,y,tzm));


	return newImg;

}

template<typename DataType>
void Image<DataType>::createMaskOffsetSphere(std::vector<int>& table, int _i32radius)
{
	// compute the width of the sphere for memory allocation
	int i32Width = 2*_i32radius + 1;
	// squared radius
    float fRad2 = (float)(_i32radius*_i32radius);

	// memory allocation
	// difficult to know how many voxels before computing,
	// so the reserve for the BB
	table.reserve(i32Width*i32Width*i32Width);
	table.clear();

	// scan all the BB of the sphere
	for (int z = -_i32radius;  z<=_i32radius; z++)
	{
		for (int y = -_i32radius;  y<=_i32radius; y++)
		{
			for (int x = -_i32radius;  x<=_i32radius; x++)
			{
				Geom::Vec3f v((float)x,(float)y,(float)z);
				float fLength =  v.norm2();
				// if inside the sphere
				if (fLength<=fRad2)
				{
					// the the index of the voxel
					int index = z * m_WXY + y * m_WX + x;
					table.push_back(index);
				}
			}
		}
	}
}

template<typename DataType>
float Image<DataType>::computeCurvatureCount(const DataType *ptrVox, const std::vector<int>& sphere, DataType val)
{
	int noir = 0;
	int blanc = 0;


	for (std::vector<int>::const_iterator it=sphere.begin(); it!=sphere.end();it++)
	{
		const DataType *data = ptrVox + *it;
		if (*data != val)
		{
			blanc++;
		}
		else
		{
			noir++;
		}
	}

	if (blanc >= noir)
	{
		return 1.0f - ((float)noir) / ((float)blanc);
	}
	else
	{
		return 1.0f - ((float)blanc)/((float)noir);
	}
}


} // end namespace
} // end namespace
} // end namespace

