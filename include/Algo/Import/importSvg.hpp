#include <iostream>
#include "Geometry/bounding_box.h"

namespace CGoGN
{

namespace Algo
{

namespace Import
{

inline bool chechXmlNode(xmlNodePtr node, const std::string& name)
{
	return (strcmp((char*)(node->name),(char*)(name.c_str())) == 0);
}

template<typename T>
inline bool valueOf(const std::string &s, T &obj)
{
  std::istringstream is(s);
  return is >> obj;
}

template <typename PFP>
bool importSVG(typename PFP::MAP& map, const std::string& filename, typename PFP::TVEC3& position, CellMarker& polygons)
{
	xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
	xmlNodePtr map_node = xmlDocGetRootElement(doc);

	if (!chechXmlNode(map_node,"svg"))
	{
		CGoGNerr << "Wrong xml format: Root node != svg"<< CGoGNendl;
		return false;
	}

	typedef std::vector<typename PFP::VEC3 > POLYGON;
	std::vector<POLYGON> allPoly;

	for (xmlNode* cur_node = map_node->children; cur_node; cur_node = cur_node->next)
	{
		// for each layer
		if (chechXmlNode(cur_node, "g"))
		{
			CGoGNout << "----load layer----"<< CGoGNendl;

			for (xmlNode* cur_path = cur_node->children; cur_path; cur_path = cur_path->next)
			{
				if (chechXmlNode(cur_path, "path"))
				{
					allPoly.push_back(POLYGON());
					CGoGNout << "--load a path--"<< CGoGNendl;
					xmlChar* prop = xmlGetProp(cur_path, BAD_CAST "d");
//					CGoGNout << "path "<< prop << CGoGNendl;
					std::string allcoords((reinterpret_cast<const char*>(prop)));
					std::stringstream is(allcoords);
					bool relative;
					bool push_point;
					std::string coord;
					int mode = -1;

					while ( std::getline( is, coord, ' ' ) )
					{
						float x,y;
						push_point=false;

						if(coord[0]=='m' || coord[0]=='l' || coord[0]=='t') //start point, line or quadratic bezier curve
						{
							mode = 0;
//							std::cout << "relative coordinates" << std::endl;
							relative=true;
						}
						else if(coord[0]=='M' || coord[0] == 'L' || coord[0]=='T') //same in absolute coordinates
						{
//							std::cout << "absolute coordinates" << std::endl;
							mode = 1;
							relative=false;
						}
						else if(coord[0]=='h' || coord[0] == 'H') //horizontal line
						{
							mode = 2;
							relative=(coord[0]=='h');
						}
						else if(coord[0]=='v' || coord[0] == 'V') //vertical line
						{
//							std::cout << "vertical line" << std::endl;
							mode = 3;
							relative=(coord[0]=='v');
						}
						else if(coord[0]=='c' || coord[0] == 'C') //bezier curve
						{
//							std::cout << "bezier line" << std::endl;
							mode = 4;
							relative=(coord[0]=='c');
						}
						else if(coord[0]=='s' || coord[0] == 'S' || coord[0]=='q' || coord[0] == 'Q') //bezier curve 2
						{
//							std::cout << "bezier line 2" << std::endl;
							mode = 5;
							relative= ((coord[0]=='s') || (coord[0]=='q'));
						}
						else if(coord[0]=='a' || coord[0] == 'A') //elliptic arc
						{
//							std::cout << "elliptic arc" << std::endl;
							mode =6;
							relative= (coord[0]=='a');
						}
						else if(coord[0]=='z')
						{
//							std::cout << "the end" << std::endl;
						}
						else //coordinates
						{
							switch(mode)
							{
							case 0 : //relative
							break;
							case 1 : //absolute
							break;
							case 2 : //horizontal
							{
								std::stringstream streamCoord(coord);
								std::string xS;
								std::getline(streamCoord, xS, ',' );

								valueOf(xS,x);

								POLYGON curPoly = allPoly[allPoly.size()-1];
								typename PFP::VEC3 previous = (curPoly)[(curPoly).size()-1];
								y = previous[1];

								push_point=true;
							}
							break;
							case 3 : //vertical
							{
								std::stringstream streamCoord(coord);
								std::string yS;
								std::getline(streamCoord, yS, ',' );

								valueOf(yS,y);

								POLYGON curPoly = allPoly[allPoly.size()-1];
								typename PFP::VEC3 previous = (curPoly)[(curPoly).size()-1];
								x = previous[0];

								push_point=true;
							}
							break;
							case 4 : //bezier
							{
								std::getline( is, coord, ' ' ); //ignore first control point
								std::getline( is, coord, ' ' ); //ignore second control point
							}
							break;
							case 5 : //bezier 2
								std::getline( is, coord, ' ' ); //ignore control point
							break;
							case 6 : //elliptic
								std::getline( is, coord, ' ' ); //ignore rx
								std::getline( is, coord, ' ' ); //ignore ry
								std::getline( is, coord, ' ' ); //ignore x-rotation
								std::getline( is, coord, ' ' ); //ignore large arc flag
								std::getline( is, coord, ' ' ); //ignore sweep flag
							break;
							}
							std::stringstream streamCoord(coord);
							std::string xS,yS;
							std::getline(streamCoord, xS, ',' );
							std::getline(streamCoord, yS, ',' );

							valueOf(xS,x);
							valueOf(yS,y);

							push_point = true;
						}

						//if there is a point to push
						if(push_point)
						{
							POLYGON& curPoly = allPoly[allPoly.size()-1];

							if(relative && curPoly.size()>0)
							{
								typename PFP::VEC3 previous = (curPoly)[(curPoly).size()-1];
								x += previous[0];
								y += previous[1];
							}

//							std::cout << "coord " << x << " " << y << std::endl;
							curPoly.push_back(typename PFP::VEC3(x,y,0));
						}
					}
				}
			}
		}
	}

	xmlFreeDoc(doc);

	Geom::BoundingBox<typename PFP::VEC3> bb(*(allPoly.begin()->begin()));

	typename std::vector<POLYGON >::iterator it;
	for(it = allPoly.begin() ; it != allPoly.end() ; ++it)
	{

		if(it->size()<3)
		{
			it = allPoly.erase(it);
		}
		else
		{
			Dart d = map.newFace(it->size()-1);
			for(typename POLYGON::iterator emb = it->begin() ; emb != it->end() ; emb++)
			{
				position[d] = *emb;
				bb.addPoint(*emb);
				d = map.phi1(d);
			}
		}
	}

	Dart dBorder = map.newFace(4);
	typename PFP::VEC3 bmin = bb.min();
	typename PFP::VEC3 bmax = bb.max();
	position[dBorder] = bmin-0.1f*bmin;
	position[map.phi1(dBorder)] = VEC3(bmax[0],bmin[1],0) + 0.1f*VEC3(bmax[0],-bmin[1],0);
	position[map.phi1(map.phi1(dBorder))] = VEC3(bmax[0],bmax[1],0) + 0.1f*bmax;
	position[map.phi_1(dBorder)] = VEC3(bmin[0],bmax[1],0) + 0.1f*VEC3(-bmin[0],bmax[1],0);;

	DartMarker inside(map);

	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(map.phi2(d) == d)
		{
			polygons.mark(d);
			inside.mark(d);
		}
	}

	DartMarker close(map);
	map.closeMap(close);

	CellMarker linked(map,FACE);
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!linked.isMarked(d) && !inside.isMarked(d))
		{
			linked.mark(d);
			Dart dMin;
			float distMin = (bb.max()-bb.min()).norm2();
			for(Dart dd = map.begin(); dd != map.end(); map.next(dd))
			{
				if(!map.sameFace(d,dd))
				{
					float dist = (position[dd]-position[d]).norm2();
					if(dist<distMin)
					{
						distMin = dist;
						dMin = dd;
					}
				}
			}

//			map.splitFace(d,d);
//			map.splitFace(dMin,dMin);

//			map.sewFaces(map.phi_1(d),map.phi_1(dMin));
//			map.mergeFaces(map.phi_1(d));
		}
	}

	return true ;
}

} //import

} //algo

} //cgogn
