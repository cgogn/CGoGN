/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include <iostream>
#include "Geometry/bounding_box.h"
#include "Geometry/plane_3d.h"
#include "Algo/BooleanOperator/mergeVertices.h"
#include "Container/fakeAttribute.h"
#include <limits>

namespace CGoGN
{

namespace Algo
{

namespace Import
{

inline bool checkXmlNode(xmlNodePtr node, const std::string& name)
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

template <typename VEC3>
void getPolygonFromSVG(std::string allcoords, std::vector<VEC3>& curPoly, bool& closedPoly)
{
	closedPoly=false;
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

					VEC3 previous = (curPoly)[(curPoly).size()-1];
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

			VEC3 previous;

			if(curPoly.size()>0)
				previous = (curPoly)[(curPoly).size()-1];

			if(relative)
			{
				x += previous[0];
				y += previous[1];
			}

//			std::cout << "coord " << x << " " << y << std::endl;

			if(curPoly.size()==0 || (curPoly.size()>0 && (x!=previous[0] || y!= previous[1])))
				curPoly.push_back(VEC3(x,y,0));
		}
	}
}

template <typename PFP>
bool importSVG(typename PFP::MAP& map, const std::string& filename, typename PFP::TVEC3& position, CellMarker& polygons, CellMarker& polygonsFaces)
{
	typedef typename PFP::VEC3 VEC3;
	typedef std::vector<VEC3 > POLYGON;

	xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
	xmlNodePtr map_node = xmlDocGetRootElement(doc);

	if (!checkXmlNode(map_node,"svg"))
	{
		CGoGNerr << "Wrong xml format: Root node != svg"<< CGoGNendl;
		return false;
	}

	std::vector<POLYGON> allPoly;
	std::vector<POLYGON> allBrokenLines;
	std::vector<float> allBrokenLinesWidth;
	bool closedPoly;

	for (xmlNode* cur_node = map_node->children; cur_node; cur_node = cur_node->next)
	{
		// for each layer
		if (checkXmlNode(cur_node, "g"))
		{
//			CGoGNout << "----load layer----"<< CGoGNendl;

			for (xmlNode* cur_path = cur_node->children ; cur_path; cur_path = cur_path->next)
			{
				if (checkXmlNode(cur_path, "path"))
				{
					POLYGON curPoly;
//					CGoGNout << "--load a path--"<< CGoGNendl;
					xmlChar* prop = xmlGetProp(cur_path, BAD_CAST "d");
					std::string allcoords((reinterpret_cast<const char*>(prop)));
					getPolygonFromSVG(allcoords,curPoly,closedPoly);

					//check orientation : set in CCW
					if(curPoly.size()>2)
					{
						VEC3 v1(curPoly[1]-curPoly[0]);
						VEC3 v2(curPoly[2]-curPoly[1]);
						if((v1^v2)[2]>0)
						{
							std::reverse(curPoly.begin(), curPoly.end());
						}
					}

					if(closedPoly)
						allPoly.push_back(curPoly);
					else
					{
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
								allBrokenLinesWidth.push_back(sizeOfLine);
							}
						}
					}
				}
			}
		}
		else
		{
			xmlNode* cur_path = cur_node;
			if (checkXmlNode(cur_path, "path"))
			{
				POLYGON curPoly;
//				CGoGNout << "--load a path--"<< CGoGNendl;
				xmlChar* prop = xmlGetProp(cur_path, BAD_CAST "d");
				std::string allcoords((reinterpret_cast<const char*>(prop)));
				getPolygonFromSVG(allcoords,curPoly,closedPoly);

				//check orientation : set in CCW
				if(curPoly.size()>2)
				{
					VEC3 v1(curPoly[1]-curPoly[0]);
					VEC3 v2(curPoly[2]-curPoly[1]);
					if((v1^v2)[2]>0)
					{
						std::reverse(curPoly.begin(), curPoly.end());
					}
				}

				if(closedPoly)
					allPoly.push_back(curPoly);
				else
				{
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

	xmlFreeDoc(doc);

	Geom::BoundingBox<typename PFP::VEC3> * bb;
	if(allBrokenLines.size()>0)
		bb = new Geom::BoundingBox<typename PFP::VEC3>(*(allBrokenLines.begin()->begin()));
	else if(allPoly.size()>0)
		bb = new Geom::BoundingBox<typename PFP::VEC3>(*(allPoly.begin()->begin()));
	else
	{
		std::cerr << " no usable data in svg file " << std::endl;
		return false;
	}

	CellMarker brokenMark(map,EDGE);
	AttributeHandler<float> edgeWidth = map.template addAttribute<float>(EDGE, "width");
	AttributeHandler<NoMathAttribute<Geom::Plane3D<typename PFP::REAL> > > edgePlanes = map.template addAttribute<NoMathAttribute<Geom::Plane3D<typename PFP::REAL> > >(EDGE, "planes");

	/////////////////////////////////////////////////////////////////////////////////////////////
	//create broken lines
	DartMarker brokenL(map);

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

			polygonsFaces.mark(d);

			Dart d1=d;
			Dart d_1=map.phi_1(d);
			for(unsigned int i = 0; i<faceDegree/2 ; ++i)
			{
				map.sewFaces(d1,d_1);

				edgeWidth[d1] = *itW;

				brokenL.mark(d1);
				brokenL.mark(d_1);

				d1 = map.phi1(d1);
				d_1 = map.phi_1(d_1);
			}

			Dart dd = d;
			for(typename POLYGON::iterator emb = it->begin(); emb != it->end() ; emb++)
			{
				bb->addPoint(*emb);
				position[dd] = *emb;
				dd = map.phi1(dd);
			}

//			do
//			{
//				bb->addPoint(*emb);
//				position[dd] = *emb;
//				std::cout << "emb " << *emb << std::endl;
//				emb++;
//				dd = map.phi1(dd);
//			} while(dd!=map.phi_1(d));

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
//			std::cout << "newFace1 " << it->size()-1 << std::endl;
			polygonsFaces.mark(d);

			Dart dd = d;
			typename POLYGON::iterator emb = it->begin();
			do
			{
				bb->addPoint(*emb);
				position[dd] = *emb;
				emb++;
				dd = map.phi1(dd);
			} while(dd!=d);
		}
	}

	for(Dart d = map.begin();d != map.end(); map.next(d))
	{
		if(position[d][0] == position[map.phi1(d)][0] && position[d][1] == position[map.phi1(d)][1])
			std::cout << "prob d " << d << std::endl;
	}

	DartMarker inside(map);

	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		polygons.mark(d);
		inside.mark(d);
	}

	DartMarker close(map);
	map.closeMap(close);

	Algo::BooleanOperator::mergeVertices<PFP>(map,position);

	/////////////////////////////////////////////////////////////////////////////////////////////
	//add bounding box

	CellMarker boundingBox(map,VERTEX);
//	Dart dBorder = map.newFace(4);
//
//	VEC3 bbCenter = bb->center();
//	VEC3 bmin = bb->min();
//	bmin += 0.3f*(bmin-bbCenter);
//	VEC3 bmax = bb->max();
//	bmax -= 0.3f*(bmin-bbCenter);
//
//	position[dBorder] = bmin;
//	position[map.phi1(dBorder)] = VEC3(bmax[0],bmin[1],0);
//	position[map.phi1(map.phi1(dBorder))] = VEC3(bmax[0],bmax[1],0);
//	position[map.phi_1(dBorder)] = VEC3(bmin[0],bmax[1],0);
//
//	Dart d = dBorder;
//	do
//	{
//		boundingBox.mark(d);
//		d = map.phi1(d);
//	} while(d!=dBorder);

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

	/////////////////////////////////////////////////////////////////////////////////////////////
	//process broken lines
	CellMarker eMTreated(map,EDGE);
	for(Dart d = map.begin();d != map.end(); map.next(d))
	{
		if(brokenL.isMarked(d) && !eMTreated.isMarked(d))
		{
			eMTreated.mark(d);
			//insert a quadrangular face in the broken line
			// -> we convert broken lines to faces to represent their width
			// -> the intersection are then closed

			Dart d1 = d;
			Dart d2 = map.phi2(d1);
			VEC3 p1 = position[d1];
			VEC3 p2 = position[d2];
			float width = edgeWidth[d1]/2.0f;
			if(width==0)
				std::cout << "importSVG : error width of path is equal to zero" << std::endl;

			VEC3 v = p2-p1;
			map.unsewFaces(d1);

			Dart dN = map.newFace(4);

			//take the orthogonal direction to the path to apply width afterward
			VEC3 ortho = v^VEC3(0,0,1);
			ortho.normalize();
			v.normalize();

			//if the valence of one of the vertex is equal to one
			//cut the edge to insert the quadrangular face
			if(map.phi2_1(d1)==d1)
			{
				map.cutEdge(d2);

				brokenL.mark(map.phi1(d2));
				eMTreated.mark(map.phi1(d2));
				map.sewFaces(map.phi_1(d1),map.phi1(dN));

				position[map.phi_1(d1)]=p1;
				edgePlanes[map.phi_1(d1)] = Geom::Plane3D<typename PFP::REAL>(v,p1);
			}

			if(map.phi2_1(d2)==d2)
			{
				map.cutEdge(d1);
				brokenL.mark(map.phi1(d1));
				eMTreated.mark(map.phi1(d1));

				map.sewFaces(map.phi_1(d2), map.phi_1(dN));

				position[map.phi_1(d2)]=p2;
				edgePlanes[map.phi_1(d2)] = Geom::Plane3D<typename PFP::REAL>(-1.0f*v, p2);
			}

			map.sewFaces(d1, dN);
			map.sewFaces(d2, map.phi1(map.phi1(dN)));

			edgePlanes[d1] = Geom::Plane3D<typename PFP::REAL>(ortho, p1-(width*ortho));
			edgePlanes[d2] = Geom::Plane3D<typename PFP::REAL>(-1.0f*ortho, p2+(width*ortho));
		}
	}

	//close the intersections
	for(Dart d = map.begin();d != map.end(); map.next(d))
	{
		if(d==map.phi2(d))
			 map.closeHole(d);
	}

	//embed the path
	for(Dart d = map.begin();d != map.end(); map.next(d))
	{
		if(brokenL.isMarked(d))
		{
			Geom::Plane3D<typename PFP::REAL> pl = edgePlanes[d];

			VEC3 pos = position[d];
			pl.project(pos);
			pl = edgePlanes[map.phi_1(d)];

			pl.project(pos);
			position[d] = pos;
		}
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
//				bool noIntersect=true;
//				CellMarker edgeM(map,EDGE);
//				edgeM.mark(d);
//				edgeM.mark(dMin);
////				edgeM.mark(map.phi1(d));
////				edgeM.mark(map.phi_1(d));
////				edgeM.mark(map.phi1(dMin));
////				edgeM.mark(map.phi_1(dMin));
//				for(Dart dd = map.begin();noIntersect &&  dd != map.end(); map.next(dd))
//				{
//					if(!edgeM.isMarked(dd))
//					{
//						edgeM.mark(dd);
//						VEC3 inter;
//						Geom::Intersection eeI = Geom::intersection2DSegmentSegment(position[dd],position[map.phi1(dd)],position[dMin],position[d],inter);
//						if(eeI ==Geom::EDGE_INTERSECTION)
//						{
//							noIntersect=false;
////							std::cout << "dd " << dd << " d " << d << " dMin " << dMin << std::endl;
//							std::cout << " pos " << position[dd] << " ; " << position[map.phi1(dd)] << std::endl;
//						}
//					}
//				}
//
//				if(noIntersect)
//				{
//					if(!map.sameFace(dMin,d)) {
//						connected.mark(d);
//						connected.mark(dMin);
//						map.linkVertices(dMin,d);
//					}
//					else
//					{
//						map.splitFace(dMin,d);
//						connected.mark(d);
//						connected.mark(dMin);
//					}
//				}
//			}
//		}
//	}

	return true ;
}

} // namespace Import

} // namespace Algo

} // namespace CGoGN
