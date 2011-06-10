#include <iostream>
#include "Geometry/bounding_box.h"
#include <limits>

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

template <typename VEC>
bool posSort(const std::pair<VEC, Dart>& a1, const std::pair<VEC, Dart>& a2)
{
	VEC v1 = a1.first;
	VEC v2 = a2.first;
	return v1[0] < v2[0] || (v1[0] == v2[0] && v1[1] < v2[1]);
}

template <typename PFP>
bool importSVG(typename PFP::MAP& map, const std::string& filename, typename PFP::TVEC3& position, CellMarker& polygons, CellMarker& polygonsFaces)
{
	typedef typename PFP::VEC3 VEC3;
	typedef std::vector<VEC3 > POLYGON;

	xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
	xmlNodePtr map_node = xmlDocGetRootElement(doc);

	if (!chechXmlNode(map_node,"svg"))
	{
		CGoGNerr << "Wrong xml format: Root node != svg"<< CGoGNendl;
		return false;
	}


	std::vector<POLYGON> allPoly;
	std::vector<POLYGON> allBrokenLines;
	std::vector<float> allBrokenLinesWidth;

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
					bool closedPoly=false;
					POLYGON curPoly;
					CGoGNout << "--load a path--"<< CGoGNendl;
					xmlChar* prop = xmlGetProp(cur_path, BAD_CAST "d");
//					CGoGNout << "path "<< prop << CGoGNendl;
					std::string allcoords((reinterpret_cast<const char*>(prop)));
					std::stringstream is(allcoords);
					bool relative=false;
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
							closedPoly = true;
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

									VEC3 previous = (curPoly)[(curPoly).size()-1];
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
								{
									std::getline( is, coord, ' ' ); //ignore control point

								}
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

							if(relative && curPoly.size()>0)
							{
								VEC3 previous = (curPoly)[(curPoly).size()-1];
								x += previous[0];
								y += previous[1];
							}

							std::cout << "coord " << x << " " << y << std::endl;
							curPoly.push_back(VEC3(x,y,0));
						}
					}

					//check orientation : set in CCW
					if(curPoly.size()>2)
					{
						VEC3 v1(curPoly[1]-curPoly[0]);
						VEC3 v2(curPoly[2]-curPoly[1]);
						if((v1^v2)[2]<0)
						{
							std::cout << "reverse !" << std::endl;
							std::reverse(curPoly.begin(), curPoly.end());
						}
					}

					if(closedPoly)
						allPoly.push_back(curPoly);
					else {
						allBrokenLines.push_back(curPoly);
						xmlChar* prop = xmlGetProp(cur_path, BAD_CAST "style");
						std::string allstyle((reinterpret_cast<const char*>(prop)));
						std::stringstream is(allstyle);
						std::string style;
						while ( std::getline( is, style, ';' ) )
						{
							if(style.find("stroke-width:")!=std::string::npos)
							{
								std::stringstream isSize(style);
								std::getline( isSize, style, ':' );
								float sizeOfLine;
								isSize >> sizeOfLine;
								std::cout << "sizeOfLine : " << sizeOfLine << std::endl;
								allBrokenLinesWidth.push_back(sizeOfLine);
							}
						}

					}

				}
			}
		}
	}

	xmlFreeDoc(doc);

	Geom::BoundingBox<typename PFP::VEC3> bb(*(allBrokenLines.begin()->begin()));

	CellMarker brokenMark(map,EDGE);
	AttributeHandler<float> edgeWidth = map.template addAttribute<float>(FACE, "width");

	/////////////////////////////////////////////////////////////////////////////////////////////
	//create broken lines
	std::vector<float >::iterator itW = allBrokenLinesWidth.begin();
	for(typename std::vector<POLYGON >::iterator it = allBrokenLines.begin() ; it != allBrokenLines.end() ; ++it)
	{
		if(it->size()<2)
		{
			it = allBrokenLines.erase(it);
			itW = allBrokenLinesWidth.erase(itW);
		}
		else
		{
			unsigned int faceDegree = it->size()*2-2;
			Dart d = map.newFace(faceDegree);
			edgeWidth[d] = *itW;
			std::cout << "newFace " << faceDegree << std::endl;

			polygonsFaces.mark(d);

			Dart d1=d;
			Dart d_1=map.phi_1(d);
			for(unsigned int i = 0; i<faceDegree/2 ; ++i)
			{
				map.sewFaces(d1,d_1);
				d1 = map.phi1(d1);
				d_1 = map.phi_1(d_1);
			}


			for(typename POLYGON::iterator emb = it->begin() ; emb != it->end() ; emb++)
			{
				position[d] = *emb;
				bb.addPoint(*emb);
				d = map.phi1(d);
			}
		}

		itW++;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//create polygons
	typename std::vector<POLYGON >::iterator it;
	for(it = allPoly.begin() ; it != allPoly.end() ; ++it)
	{

		if(it->size()<4)
		{
			it = allPoly.erase(it);
		}
		else
		{
			Dart d = map.newFace(it->size()-1);
			std::cout << "newFace " << it->size()-1 << std::endl;
			polygonsFaces.mark(d);
			for(typename POLYGON::iterator emb = it->begin() ; emb != it->end() ; emb++)
			{
				position[d] = *emb;
				bb.addPoint(*emb);
				d = map.phi1(d);
			}
		}
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	//add bounding box

	CellMarker boundingBox(map,VERTEX);
	Dart dBorder = map.newFace(4);

	VEC3 bbCenter = bb.center();
	VEC3 bmin = bb.min();
	bmin += 0.3f*(bmin-bbCenter);
	VEC3 bmax = bb.max();
	bmax -= 0.3f*(bmin-bbCenter);

	position[dBorder] = bmin;
	position[map.phi1(dBorder)] = VEC3(bmax[0],bmin[1],0);
	position[map.phi1(map.phi1(dBorder))] = VEC3(bmax[0],bmax[1],0);
	position[map.phi_1(dBorder)] = VEC3(bmin[0],bmax[1],0);

	Dart d = dBorder;
	do
	{
		boundingBox.mark(d);
		d = map.phi1(d);
	} while(d!=dBorder);

	/////////////////////////////////////////////////////////////////////////////////////////////
	//cut the edges to have a more regular sampling
//	float maxDist=60.0f;
//	CellMarker treated(map,EDGE);
//	for(Dart d = map.begin(); d != map.end(); map.next(d))
//	{
//		if(!treated.isMarked(d))
//		{
//			treated.mark(d);
//			VEC3 p1 =position[d];
//			VEC3 p2 =position[map.phi1(d)];
//
//			if((p1-p2).norm()>maxDist)
//			{
//				unsigned int nbSeg = ((p1-p2).norm())/int(maxDist);
//				for(unsigned int i=0;i<nbSeg-1;++i)
//				{
//					map.cutEdge(d);
//
//					if(boundingBox.isMarked(d))
//						boundingBox.mark(map.phi1(d));
//				}
//
//				Dart dd = map.phi1(d);
//				VEC3 interv(p2-p1);
//				interv /= nbSeg;
//
//				for(unsigned int i=1;i<nbSeg;++i)
//				{
//					position[dd] = p1+interv*i;
//					dd = map.phi1(dd);
//				}
//			}
//		}
//	}

	DartMarker inside(map);

	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		polygons.mark(d);
		inside.mark(d);
	}

	DartMarker close(map);
	map.closeMap(close);

	/////////////////////////////////////////////////////////////////////////////////////////////
	//process broken lines
	for(Dart d = map.begin();d != map.end(); map.next(d))
	{
		if(map.phi2(d) != d && map.sameFace(map.phi2(d),d) && !boundingBox.isMarked(d) && inside.isMarked(d))
		{
			std::cout << "add face" << std::endl;

			Dart d1 = d;
			Dart d2 = map.phi2(d);
			VEC3 p1 = position[d1];
			VEC3 p2 = position[d2];
			float width = edgeWidth[d1];

			VEC3 v = p2-p1;
			map.unsewFaces(d);

			Dart dN = map.newFace(4);

			VEC3 ortho = v^VEC3(0,0,1);
			ortho.normalize();

			if(map.alpha1(d1)==d1)
			{
				std::cout << "cut1 " << std::endl;
				map.cutEdge(map.phi_1(d1));

				position[map.phi_1(d1)]=p1-width*ortho;
				position[d1]=p1+width*ortho;
			}
			else
			{
				Dart d_1 = map.phi_1(d1);
				VEC3 v2 = p1-position[d_1];

				VEC3 ortho2 = v2^VEC3(0,0,1);
				ortho2.normalize();
				ortho += ortho2;
				ortho.normalize();
				position[d1]=p1+width*ortho;
			}

			if(map.alpha1(d2)==d2)
			{
				std::cout << "cut2 " << std::endl;
				map.cutEdge(map.phi_1(d2));

				position[map.phi_1(d2)]=p2+width*ortho;
				position[d2]=p2-width*ortho;
			}
			else
			{
				Dart d_1 = map.phi_1(d2);
				VEC3 v2 = p2-position[d_1];

				VEC3 ortho2 = v2^VEC3(0,0,1);
				ortho2.normalize();
				ortho -= ortho2;
				ortho.normalize();
				position[d2]=p2-2.0f*width*ortho;
			}

			map.sewFaces(d1,dN);
			map.sewFaces(d2,map.phi1(map.phi1(dN)));

		}
	}

	for(Dart d = map.begin();d != map.end(); map.next(d))
	{
		if(d==map.phi2(d))
			map.closeHole(d);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//process polygons


//	std::vector<std::pair<VEC3,Dart> > toConnect;
//	CellMarker connected(map,VERTEX);
//	for(Dart d = map.begin(); d != map.end(); map.next(d))
//	{
//		if( ((!boundingBox.isMarked(d) && !inside.isMarked(d)) || (boundingBox.isMarked(d) && inside.isMarked(d)))
//			&& !connected.isMarked(d)
//		  )
//		{
//			connected.mark(d);
//			toConnect.push_back(std::make_pair(position[d],d));
//		}
//	}
//
//	std::sort(toConnect.begin(), toConnect.end(),posSort<VEC3>);
//
//	for(typename std::vector<std::pair<VEC3 ,Dart > >::iterator it = toConnect.begin(); it != toConnect.end() ; ++it)
//	{
//		Dart d = it->second;
//		typename std::vector<std::pair<VEC3,Dart> >::iterator it2 = it+1;
//		if(it2!= toConnect.end())
//		{
//			Dart dd = it2->second;
//			if(!map.sameFace(map.phi2(d),map.phi2(dd)))
//			{
//				if(!map.sameFace(dd,d)) {
//					std::cout << "link" << std::endl;
//					map.linkVertices(dd,d);
//				}
//				else
//				{
//					map.splitFace(dd,d);
//					std::cout << "split" << std::endl;
//				}
//			}
//		}
//
//	}

//	CellMarker connected(map,VERTEX);
//	unsigned int i=0;
//	for(Dart d = map.begin();d != map.end(); map.next(d))
//	{
//		if(!connected.isMarked(d) && !boundingBox.isMarked(d) && !inside.isMarked(d))
//		{
//			i++;
//			Dart dMin=map.end();
//			float distMin = std::numeric_limits<float>::max();
//			for(Dart dd = map.begin(); dd != map.end(); map.next(dd))
//			{
//				if(    (
//						(boundingBox.isMarked(dd) && inside.isMarked(dd))
//					|| (!boundingBox.isMarked(dd) && !inside.isMarked(dd))
//					   )
//						&& !map.sameFace(map.phi2(d),map.phi2(dd)))
//				{
//					if(Geom::testOrientation2D(position[dd], position[d], position[map.phi1(d)]) == Geom::LEFT
//							&& Geom::testOrientation2D(position[dd], position[d], position[map.phi_1(d)]) == Geom::RIGHT)
//					{
//						float dist = (position[dd]-position[d]).norm();
//						if(dist<distMin)
//						{
//							distMin = dist;
//							dMin = dd;
//						}
//					}
//				}
//			}
//
//			if(dMin!=map.end())
//			{
//				if(!map.sameFace(dMin,d)) {
//					std::cout << "link" << std::endl;
//					connected.mark(d);
//					connected.mark(dMin);
//					map.linkVertices(dMin,d);
//				}
//				else
//				{
//					map.splitFace(dMin,d);
//					connected.mark(d);
//					connected.mark(dMin);
//					std::cout << "split" << std::endl;
//				}
//			}
//		}
//	}

	return true ;
}

} //import

} //algo

} //cgogn
