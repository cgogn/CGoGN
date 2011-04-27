#ifndef EXPORT_POV_H
#define EXPORT_POV

#include "Topology/generic/attributeHandler.h"
#include "Utils/cgognStream.h"

namespace CGoGN
{

namespace Algo
{

namespace ExportPov
{

template <typename PFP>
void exportTriangleWire(std::ofstream& out,typename PFP::VEC3& p1,typename PFP::VEC3& p2,typename PFP::VEC3& p3)
{
		CGoGNout << "cylinder {" << CGoGNendl;
		CGoGNout << "<" << p1[0] << "," << p1[1] << "," << p1[2] << ">, <" << p2[0] << "," << p2[1] << "," << p2[2] << ">, 0.5" << CGoGNendl;
		CGoGNout << "}" << CGoGNendl;

		CGoGNout << "cylinder {" << CGoGNendl;
		CGoGNout << "<" << p1[0] << "," << p1[1] << "," << p1[2] << ">, <" << p3[0] << "," << p3[1] << "," << p3[2] << ">, 0.5" << CGoGNendl;
		CGoGNout << "}" << CGoGNendl;

		CGoGNout << "cylinder {" << CGoGNendl;
		CGoGNout << "<" << p3[0] << "," << p3[1] << "," << p3[2] << ">, <" << p2[0] << "," << p2[1] << "," << p2[2] << ">, 0.5" << CGoGNendl;
		CGoGNout << "}" << CGoGNendl;
}

template <typename PFP>
void exportTrianglePlain(std::ofstream& out,typename PFP::VEC3& p1,typename PFP::VEC3& p2,typename PFP::VEC3& p3)
{
		CGoGNout << "triangle {" << CGoGNendl;
		CGoGNout << "<" << p1[0] << "," << p1[1] << "," << p1[2] << ">," << CGoGNendl;
		CGoGNout << "<" << p2[0] << "," << p2[1] << "," << p2[2] << ">, " << CGoGNendl;
		CGoGNout << "<" << p3[0] << "," << p3[1] << "," << p3[2] << "> " << CGoGNendl;
		CGoGNout << "}" << CGoGNendl;
}

template <typename PFP>
void exportMeshPlain(std::ofstream& out, typename PFP::MAP& map, typename PFP::TVEC3& position, const std::string& meshName, const FunctorSelect& good = SelectorTrue())
{
	CGoGNout << "#declare " << meshName << "= union {" << CGoGNendl;

	DartMarkerStore traite(map);

	for(Dart d = map.begin() ; d!= map.end() ; map.next(d)) {
		if(good(d) && !traite.isMarked(d)) {
			unsigned int nb=0;
			Dart dd=d;
			do {
				traite.markOrbit(DART_ORBIT,dd);
				dd = map.phi1(dd);
				nb++;
			} while(dd!=d);

			if(nb==3) {
				Algo::ExportPov::exportTrianglePlain<PFP>(out,position[dd],position[map.phi1(dd)],position[map.phi1(map.phi1(dd))]);
			}
			else {
					CGoGNout << "polygon{ " << nb+1 << CGoGNendl;
					dd=d;
					do {
						CGoGNout << "<" << position[dd][0] << "," << position[dd][1] << "," << position[dd][2] << ">," << CGoGNendl;
						dd = map.phi1(dd);
					}while(dd!=d);
					CGoGNout << "<" << position[d][0] << "," << position[d][1] << "," << position[d][2] << ">" << CGoGNendl;
					CGoGNout << "}" << CGoGNendl;
			}
		}
	}

	CGoGNout << "}" << CGoGNendl;
}

template <typename PFP>
void exportMeshWire(std::ofstream& out, typename PFP::MAP& map, typename PFP::TVEC3& position, const std::string& meshName, const FunctorSelect& good = SelectorTrue())
{
	CGoGNout << "#declare " << meshName << "= union {" << CGoGNendl;

	DartMarkerStore traite(map);

	for(Dart d = map.begin() ; d!= map.end() ; map.next(d)) {
		if(good(d) && !traite.isMarked(d)) {
			unsigned int nb=0;
			Dart dd=d;
			do {
				traite.markOrbit(DART_ORBIT,dd);
				dd = map.phi1(dd);
				nb++;
			} while(dd!=d);

			if(nb==3) {
				Algo::ExportPov::exportTriangleWire<PFP>(out,position[dd],position[map.phi1(dd)],position[map.phi1(map.phi1(dd))]);
			}
			else {
					dd=d;
					do {
						if(position[dd][0]!=position[map.phi1(dd)][0] || position[dd][1]!=position[map.phi1(dd)][1] || position[dd][2]!=position[map.phi1(dd)][2]) {
							CGoGNout << "cylinder{ " << CGoGNendl;
							CGoGNout << "<" << position[dd][0] << "," << position[dd][1] << "," << position[dd][2] << ">," << CGoGNendl;
							CGoGNout << "<" << position[map.phi1(dd)][0] << "," << position[map.phi1(dd)][1] << "," << position[map.phi1(dd)][2] << ">, 0.5" << CGoGNendl;
							CGoGNout << "}" << CGoGNendl;
						}
						dd = map.phi1(dd);
					}while(dd!=d);
			}
		}
	}

	CGoGNout << "}" << CGoGNendl;
}

template <typename PFP>
bool exportScenePov(typename PFP::MAP& map, typename PFP::TVEC3& position, const std::string& filename, typename PFP::VEC3 cameraPos, typename PFP::VEC3 cameraLook, typename PFP::VEC3 translate, float angle_X, float angle_Y, float angle_Z,const FunctorSelect& good = SelectorTrue())
{
	std::ofstream out(filename.c_str(), std::ios::out);
	if (!out.good()) {
		CGoGNerr << "(export) Unable to open file " << filename << CGoGNendl;
		return false;
	}

	float angleX = angle_X;
	float angleY = angle_Y;
	float angleZ = angle_Z;

	//define the camera position
	CGoGNout << "camera { location <" << cameraPos[0] << "," << cameraPos[1] << "," << cameraPos[2] << "> look_at <" << cameraLook[0] << "," << cameraLook[1] << "," << cameraLook[2] <<">}" << CGoGNendl;;
	//set a "infinite" plane 
// 	CGoGNout << "plane { y, -1 pigment { color rgb 1 } }" << CGoGNendl;
	//set a sky sphere
	CGoGNout << "sphere { <0, 0, 0>, 5000";
	CGoGNout << "texture{ pigment { color rgb <1, 1, 1>}	finish { ambient 1 diffuse 0 } } }" << CGoGNendl;
	//put some lights
	CGoGNout << "light_source { <" << cameraPos[0] << "," << cameraPos[1] << "," << cameraPos[2] << "> color rgb 0.45}" << CGoGNendl;
// 	CGoGNout << "light_source { <-120, -300, -10> color rgb 0.25 }"<< CGoGNendl;
	//set a high quality rendering
	CGoGNout << "global_settings {" << CGoGNendl;
	CGoGNout << "radiosity {" << CGoGNendl;
	CGoGNout << "pretrace_start 0.08 pretrace_end 0.04" << CGoGNendl;
	CGoGNout << "count 100 nearest_count 10 error_bound 0.15 recursion_limit 1 low_error_factor 0.2 gray_threshold 0.0 minimum_reuse 0.015 brightness 1 adc_bailout 0.01/2 normal off media off} max_trace_level 255}" << CGoGNendl;

	Algo::ExportPov::exportMeshPlain<PFP>(out,map,position,"myMesh",good);

	CGoGNout << "object {myMesh" << CGoGNendl;
 	CGoGNout << "translate <" << translate[0] << "," << translate[1] << "," << translate[2] << ">" << CGoGNendl;
 	CGoGNout << "rotate <" << angleX << "," << angleY << "," << angleZ << "> " << CGoGNendl;
 	CGoGNout << "texture{ pigment{ color rgb<1.0,1.0,1>} finish { ambient rgb 0.05 brilliance 0.5 } } }" << CGoGNendl;

	out.close();
	return true;
}

}

}

}

#endif
