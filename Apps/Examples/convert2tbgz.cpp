/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
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
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <iostream>


#include "Algo/Import/import.h"
#include "Algo/Import/export.h"
#include "map_dual/mapd2_xpt.h"
#include "Algo/Render/map_glRender.h"
#include "point3d.h"



#include <gmtl/Output.h>



gmtl::Vec3f gMax;
gmtl::Vec3f gMin;

struct PFP
{
	typedef DartObj<DP::DefaultDartParameters> DART;
	typedef e0mapd2<DART> MAP;
	typedef Point3D EMB;
	static const unsigned id0=0;
};

PFP::MAP myMap;


int getFileType(char* filenameIN)
{
	std::string fstr(filenameIN);
	if (fstr.rfind(".trianbgz")!=std::string::npos)
		return 6;
	if (fstr.rfind(".trianb")!=std::string::npos)
		return 5;
	if (fstr.rfind(".trian")!=std::string::npos)
		return 1;
	if (fstr.rfind(".ply")!=std::string::npos)
		return 2;
	if (fstr.rfind(".off")!=std::string::npos)
		return 3;
	if (fstr.rfind(".obj")!=std::string::npos)
		return 4;
	return -1;
}

int main(int argc, char **argv)
{

	gmtl::Vec3f gMax;
	gmtl::Vec3f gMin;

	char* filenameIN = argv[1];
	char* filenameOUT = argv[2];

	if (argc < 3)
	{
		std::cerr << argv[0]<<" fich_in fich_out"<<std::endl;
		return 1;
	}

	if ( std::string(filenameIN) == std::string(filenameOUT) )
	{
		std::cerr << "Meme nom entree/sotie non autorise"<<std::endl;
		return 1;
	}

	int ft = getFileType(filenameIN);
	std::cout << "Type entree: "<<ft<<std::endl;
	switch (ft)
	{
	case 1:	Import::importTrian<PFP>(myMap,filenameIN);
			break;
	case 2:	Import::importPly<PFP>(myMap,filenameIN);
			break;
	case 3:	Import::importOff<PFP>(myMap,filenameIN);
			break;
	case 4:	Import::importObj<PFP>(myMap,filenameIN);
			break;
	case 5:	Import::importTrianBin<PFP>(myMap,filenameIN);
			break;
	case 6:	Import::importTrianBinGz<PFP>(myMap,filenameIN);
			break;

	default: std:: cerr << "type fichier non reconnu" << std::endl;
			 return 1;
			 break;
	}
	std::cout<<  "objet importe ..."<<std::endl;


	ft = getFileType(filenameOUT);
	std::cout << "Type sortie: "<<ft<<std::endl;
	switch (ft)
	{
	case 1:	Import::computeNeighbourOptimized<PFP>(myMap,gMin,gMax);
			Export::exportTrian<PFP>(myMap,filenameOUT);
			break;
	case 2:	Export::exportPly<PFP>(myMap,filenameOUT);
			break;
	case 3:	Export::exportOff<PFP>(myMap,filenameOUT);
			break;
	case 4:	Export::exportObj<PFP>(myMap,filenameOUT);
			break;
	case 5:	Import::computeNeighbourOptimized<PFP>(myMap,gMin,gMax);
			Export::exportTrianBin<PFP>(myMap,filenameOUT);
			break;
	case 6:	Import::computeNeighbourOptimized<PFP>(myMap,gMin,gMax);
			Export::exportTrianBinGz<PFP>(myMap,filenameOUT);
			break;

	default: std:: cerr << "type fichier non reconnu" << std::endl;
			 return 1;
			 break;
	}
	std::cout<<  "... et exporte"<<std::endl;

	return 0;
}



