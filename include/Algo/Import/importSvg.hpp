#include <iostream>

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
bool importSVG(typename PFP::MAP& map, const std::string& filename, std::vector<std::string>& attrNames)
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
					CGoGNout << "path "<< prop << CGoGNendl;
					std::string allcoords((reinterpret_cast<const char*>(prop)));
					std::stringstream is(allcoords);
					bool relative;
					bool push_point;
					std::string coord;

					while ( std::getline( is, coord, ' ' ) )
					{
						float x,y;
						push_point=false;

						if(coord[0]=='m' || coord[0]=='l' || coord[0]=='t') //start point, line or quadratic bezier curve
						{
//							std::cout << "relative coordinates" << std::endl;
							relative=true;
						}
						else if(coord[0]=='M' || coord[0] == 'L' || coord[0]=='T') //same in absolute coordinates
						{
//							std::cout << "absolute coordinates" << std::endl;
							relative=false;
						}
						else if(coord[0]=='h' || coord[0] == 'H') //horizontal line
						{
//							std::cout << "horizontal line" << std::endl;
							relative=(coord[0]=='h');

							std::stringstream streamCoord(coord);
							std::string xS;
							std::getline(streamCoord, xS, ',' );

							valueOf(xS,x);

							POLYGON curPoly = allPoly[allPoly.size()-1];
							typename PFP::VEC3 previous = (curPoly)[(curPoly).size()-1];
							y = previous[1];

							push_point=true;
						}
						else if(coord[0]=='v' || coord[0] == 'V') //vertical line
						{
//							std::cout << "horizontal line" << std::endl;
							relative=(coord[0]=='v');

							std::stringstream streamCoord(coord);
							std::string yS;
							std::getline(streamCoord, yS, ',' );

							valueOf(yS,y);

							POLYGON curPoly = allPoly[allPoly.size()-1];
							typename PFP::VEC3 previous = (curPoly)[(curPoly).size()-1];
							x = previous[0];

							push_point=true;
						}
						else if(coord[0]=='c' || coord[0] == 'C') //bezier curve
						{
//							std::cout << "bezier line" << std::endl;
							relative=(coord[0]=='c');

							std::getline( is, coord, ' ' ); //ignore first control point
							std::getline( is, coord, ' ' ); //ignore second control point
						}
						else if(coord[0]=='s' || coord[0] == 'S' || coord[0]=='q' || coord[0] == 'Q') //bezier curve 2
						{
//							std::cout << "bezier line 2" << std::endl;
							relative= ((coord[0]=='s') || (coord[0]=='q'));

							std::getline( is, coord, ' ' ); //ignore control point
						}
						else if(coord[0]=='a' || coord[0] == 'A') //elliptic arc
						{
//							std::cout << "elliptic arc" << std::endl;
							relative= (coord[0]=='a');

							std::getline( is, coord, ' ' ); //ignore rx
							std::getline( is, coord, ' ' ); //ignore ry
							std::getline( is, coord, ' ' ); //ignore x-rotation
							std::getline( is, coord, ' ' ); //ignore large arc flag
							std::getline( is, coord, ' ' ); //ignore sweep flag
						}
						else if(coord[0]=='z')
						{
//							std::cout << "the end" << std::endl;
						}
						else //coordinates
						{
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

	return true ;
}

} //import

} //algo

} //cgogn
