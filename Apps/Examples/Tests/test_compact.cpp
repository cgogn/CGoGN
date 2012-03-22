#include <iostream>
#include <Container/attributeContainer.h>

#include <iostream>

#include "Topology/generic/parameters.h"
#include "Topology/map/embeddedMap2.h"
//#include "Geometry/vector_gen.h"
#include "Algo/Modelisation/polyhedron.h"

using namespace CGoGN;


struct PFP: public PFP_STANDARD
{
	// definition of the map
	typedef EmbeddedMap2 MAP ;
};


void dump(AttributeContainer& cont, AttributeMultiVector<int>* amv)
{
	std::cout << "==============================================="<<std::endl;
	int j=0;
	for (unsigned int it = cont.begin(); it != cont.end(); cont.next(it))
	{
		if (j%10 == 0)
			std::cout << std::endl << j<< " : ";
		std::cout << amv->operator [](it) << " / ";
		j++;
	}
	std::cout << std::endl <<"------------------------------------------------"<<std::endl;
}

void test_compact_container()
{
#define NB 200
	AttributeContainer cont;

	AttributeMultiVector<int>* amv = cont.addAttribute<int>("entiers");
	for (int i =0; i< NB; ++i)
	{
		unsigned int l = cont.insertLine();
		amv->operator [](l) = i;
	}

	dump(cont,amv);
	
	for (int i =0; i< NB; i+=3)
	{
		cont.removeLine(i);
	}

	dump(cont,amv);

	std::vector<unsigned int> oldnew;
	cont.compact(oldnew);

	dump(cont,amv);

	std::cout << "Size = "<< oldnew.size()<< std::endl;
	int k=0;
	for (std::vector<unsigned int>::iterator it =oldnew.begin(); it != oldnew.end() ;++it)
	{
		std::cout <<k << "->";
		if (*it != 0xffffffff)
			std::cout << *it;
		else
			std::cout << "X";
		std::cout <<"/";
		++k;

		if (k%10 ==0)
			std::cout << std::endl;
	}
	std::cout << std::endl;
#undef NB
}



void test_compact_map(const std::string& name)
{

	EmbeddedMap2 myMap;
	PFP::TVEC3 position = myMap.addAttribute<PFP::VEC3>(VERTEX, "position") ;

#define NB 256
	Algo::Modelisation::Polyhedron<PFP> prim1(myMap, position);
	prim1.cylinder_topo(256, 256, false, false);
	prim1.embedCylinder(30.0f, 10.0f, 70.0f);
#undef NB
	DartMarker dm(myMap);
	std::vector<Dart> verts;

	int i=0;
	for (Dart d = myMap.begin(); d!= myMap.end(); myMap.next(d))
	{
		if (!dm.isMarked(d) && (!myMap.isBoundaryEdge(d))&& (i%11 ==0))
		{
			verts.push_back(d);
			Dart e = d;
			do						// mark all edges incident to vertex;
			{
				dm.markOrbit( EDGE, e);
				e = myMap.phi2_1(e);
			}while (e!=d);
		}
		i++;
	}

	for (std::vector<Dart>::iterator it=verts.begin(); it != verts.end(); ++it)
	{
		myMap.deleteVertex(*it);
	}


	myMap.saveMapBin(name+"_full.map");
	myMap.compact();
	myMap.saveMapBin(name+"_compact.map");
}

int main(int argc, char** argv)
{
	if (argc>1)
		test_compact_map(std::string(argv[1]));
	else
		test_compact_container();

	return 0;
}
