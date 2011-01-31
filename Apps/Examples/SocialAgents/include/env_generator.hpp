namespace CGoGN
{

namespace CityGenerator
{

template <typename PFP>
void generateAbsolutSpiralOfDeath(typename PFP::MAP& map,typename PFP::TVEC3& position, DartMarker& closeMark, unsigned int side, float rMin, float rMax,float nbTurns)
{
	Algo::Modelisation::Polyhedron<PFP> prim(map,position);
	prim.grid_topo(side,1);

// 	prim.embedCircle(rMin,rMax);
	prim.embedHelicoid(rMin,rMax,nbTurns*20.0f,nbTurns);
// 		prim.embedGrid(100,100);

	CellMarker traite(map,FACE_CELL);
	for(Dart d = map.begin(); d!=map.end() ; map.next(d)) {
		if(!traite.isMarked(d)) {
			traite.mark(d);
			map.splitFace(d,map.phi1(map.phi1(d)));
		}
	}

	map.closeMap(closeMark) ;

	map.initOrbitEmbedding(FACE_ORBIT);
}

template <typename PFP>
void generateToboggan(typename PFP::MAP& map,typename PFP::TVEC3& position, DartMarker& closeMark, unsigned int side, float rMin, float rMax,float nbTurns)
{
	//the stairs
	Algo::Modelisation::Polyhedron<PFP> prim(map,position);
	prim.grid_topo(side,3);
	//the slide
	Algo::Modelisation::Polyhedron<PFP> prim2(map,position);
	prim2.grid_topo(side/4,3);

	float height=150;

	prim2.embedHelicoid(rMin*4,rMax,-1*height,0.5f,-1);

// 	typename PFP::VEC3 transl(0,2*rMax,height);
	typename PFP::VEC3 transl(-rMax/2,0,height);
	CellMarker m(map, VERTEX_CELL) ;
	for(Dart d=map.begin();d!=map.end();map.next(d)) {
		if(!m.isMarked(d)) {
			m.mark(d);
			position[d] += transl;
			position[d][0] *= 2.0f;
		}
	}

	prim.embedHelicoid(rMin,rMax,height,nbTurns-0.5f);

	Dart dStairsDown = map.phi_1(prim.getDart());
	Dart dStairsUp = dStairsDown;
	do {
		closeMark.markOrbit(FACE_ORBIT,dStairsUp);
		closeMark.markOrbit(FACE_ORBIT,map.phi2(map.phi1(map.phi1(map.phi2(map.phi_1(dStairsUp))))));
		dStairsUp= map.phi2(map.phi1(map.phi1(dStairsUp)));
	} while(map.phi2(dStairsUp)!=dStairsUp);

	Dart dSlideUp = map.phi_1(prim2.getDart());
	Dart dSlideDown = dSlideUp;
	do {
		closeMark.markOrbit(FACE_ORBIT,dSlideDown);
		closeMark.markOrbit(FACE_ORBIT,map.phi2(map.phi1(map.phi1(map.phi2(map.phi_1(dSlideDown))))));
		dSlideDown= map.phi2(map.phi1(map.phi1(dSlideDown)));
	} while(map.phi2(dSlideDown)!=dSlideDown);


	Algo::Modelisation::Polyhedron<PFP> primPathUp(map,position);
	primPathUp.grid_topo(1,3);
	Dart dPathUp = map.phi_1(primPathUp.getDart());

	Algo::Modelisation::Polyhedron<PFP> primPathDown(map,position);
	primPathDown.grid_topo(1,3);
	Dart dPathDown = map.phi_1(primPathDown.getDart());

	closeMark.markOrbit(FACE_ORBIT,dPathUp);
	closeMark.markOrbit(FACE_ORBIT,dPathDown);

	map.sewFaces(dStairsUp,dPathUp);
	map.sewFaces(dSlideUp,map.phi1(map.phi1(dPathUp)));
	map.sewFaces(dSlideDown,dPathDown);
	map.sewFaces(dStairsDown,map.phi1(map.phi1(dPathDown)));

	dPathUp = map.phi_1(map.alpha1(dPathUp));
	dPathDown = map.phi_1(map.alpha1(dPathDown));

	dSlideUp = map.phi_1(map.alpha1(dSlideUp));
	dSlideDown =map.alpha_1(map.phi1(dSlideDown));
	dStairsUp = map.alpha_1(map.phi1(dStairsUp));
	dStairsDown = map.phi_1(map.alpha1(dStairsDown));

// 	Dart dPathUp = map.newOrientedFace(4);
// 	Dart dPathDown = map.newOrientedFace(4);

	map.sewFaces(dStairsUp,dPathUp);
	map.sewFaces(dSlideUp,map.phi1(map.phi1(dPathUp)));
	map.sewFaces(dSlideDown,dPathDown);
	map.sewFaces(dStairsDown,map.phi1(map.phi1(dPathDown)));

	dPathUp = map.phi_1(map.alpha1(dPathUp));
	dPathDown = map.phi_1(map.alpha1(dPathDown));
	dSlideUp = map.phi_1(map.alpha1(dSlideUp));
	dStairsUp = map.alpha_1(map.phi1(dStairsUp));
	dSlideDown =map.alpha_1(map.phi1(dSlideDown));
	dStairsDown = map.phi_1(map.alpha1(dStairsDown));

	closeMark.markOrbit(FACE_ORBIT,dPathUp);
	closeMark.markOrbit(FACE_ORBIT,dPathDown);

	map.sewFaces(dStairsUp,dPathUp);
	map.sewFaces(dSlideUp,map.phi1(map.phi1(dPathUp)));
	map.sewFaces(dSlideDown,dPathDown);
	map.sewFaces(dStairsDown,map.phi1(map.phi1(dPathDown)));

// 	map.sewFaces(dStairsDown,dPathDown);
// 	map.sewFaces(dSlideDown,map.phi1(map.phi1(dPathDown)));

// 	map.sewFaces(dStairs,dSlide);

	map.closeMap(closeMark) ;
	CellMarker traite(map,FACE_CELL);
	for(Dart d = map.begin(); d!=map.end() ; map.next(d)) {
		if(!traite.isMarked(d)) {
			traite.mark(d);
			map.splitFace(d,map.phi1(map.phi1(d)));
		}
	}

	for(Dart d = map.begin(); d!=map.end() ; map.next(d)) {
		if(closeMark.isMarked(d)) {
			closeMark.markOrbit(FACE_ORBIT,d);
		}
	}

	map.initOrbitEmbedding(FACE_ORBIT);
}

template <typename PFP, typename EMBV>
void generateSmallCity(typename PFP::MAP& map, EMBV& position, DartMarker& closeMark, float sideSize)
{
	DartMarkerStore obj(map);
// 	sideSize *= 0.2f;
	unsigned int nbBuilding=1000;
	float height = sideSize/2.0f;
	unsigned int side = 20;
	generateGrid<PFP,EMBV>(map,position,side,side,sideSize,closeMark);

	Dart dEnd = map.end();
	for(Dart d = map.begin(); d!=dEnd && nbBuilding>0; map.next(d)) {
		if(!obj.isMarked(d) && (rand()%20==0) && notEightAdjacentToAnObstacle<PFP,EMBV>(map,d,closeMark)) {
			typename PFP::VEC3 n1 =  Algo::Geometry::faceNormal<PFP>(map,d,position);
			typename PFP::VEC3 n2 =  Algo::Geometry::faceNormal<PFP>(map,map.phi2(d),position);
			if(position[d][2]==0.0f && position[map.phi1(d)][2]==0.0f && position[map.phi_1(d)][2]==0.0f && n1[0]==0.0f && n1[1]==0.0f && n1[2]==1.0f && n1[0]==n2[0] && n1[1]==n2[1] && n1[2]==n2[2] && map.faceDegree(d)==4) {
				obj.markOrbit(FACE_ORBIT,d);
				generateBuilding<PFP,EMBV>(map,position,d,obj,(1+(rand()%3))*height/2.0f,side,rand()%2,closeMark);
				--nbBuilding;
			}
		}
	}

	for(Dart d = map.begin(); d!=map.end(); map.next(d)) {
		if(closeMark.isMarked(d) && map.faceDegree(d)>4) {
			Dart dd=d;
			do {
				closeMark.markOrbit(DART_ORBIT,dd);
				dd = map.phi1(dd);
			} while(position[dd][2]==0.0f && position[map.phi1(dd)][2]==0.0f && dd!=d);

			dd=d;
			do {
				closeMark.markOrbit(DART_ORBIT,dd);
				dd = map.phi_1(dd);
			} while(position[dd][2]==0.0f && position[map.phi1(dd)][2]==0.0f && dd!=d);
		}
	}

	map.closeMap(closeMark);	
}

template <typename PFP, typename EMBV>
void generateCity(typename PFP::MAP& map, EMBV& position, DartMarker& closeMark, float sideSize)
{
	DartMarkerStore obj(map);
	unsigned int nbBuilding=100;
// 	unsigned int nbBuilding=20;
	float height = sideSize;
// 	unsigned int side = 50;
	unsigned int side = 40;
// 	unsigned int side = 20;
// 	unsigned int side = 10;
	generateGrid<PFP,EMBV>(map,position,side,side,height,closeMark);
	Dart dEnd = map.end();
	for(Dart d = map.begin(); d!=dEnd && nbBuilding>0; map.next(d)) {
		if(!obj.isMarked(d) && (rand()%20==0) && notEightAdjacentToAnObstacle<PFP,EMBV>(map,d,closeMark)) {
			typename PFP::VEC3 n1 =  Algo::Geometry::faceNormal<PFP>(map,d,position);
			typename PFP::VEC3 n2 =  Algo::Geometry::faceNormal<PFP>(map,map.phi2(d),position);
			if(position[d][2]==0.0f && position[map.phi1(d)][2]==0.0f && position[map.phi_1(d)][2]==0.0f && n1[0]==0.0f && n1[1]==0.0f && n1[2]==1.0f && n1[0]==n2[0] && n1[1]==n2[1] && n1[2]==n2[2] && map.faceDegree(d)==4) {
				obj.markOrbit(FACE_ORBIT,d);
				generateBuilding<PFP,EMBV>(map,position,d,obj,(1+(rand()%3))*height/2.0f,side,rand()%4,closeMark);
				--nbBuilding;
			}
		}
	}

	for(Dart d = map.begin(); d!=map.end(); map.next(d)) {
		if(closeMark.isMarked(d) && map.faceDegree(d)>4) {
			Dart dd=d;
			do {
				closeMark.markOrbit(DART_ORBIT,dd);
				dd = map.phi1(dd);
			} while(position[dd][2]==0.0f && position[map.phi1(dd)][2]==0.0f && dd!=d);

			dd=d;
			do {
				closeMark.markOrbit(DART_ORBIT,dd);
				dd = map.phi_1(dd);
			} while(position[dd][2]==0.0f && position[map.phi1(dd)][2]==0.0f && dd!=d);
		}
	}

	map.closeMap(closeMark);
}

template <typename PFP, typename EMBV, typename EMBAGENT>
void animateCity(typename PFP::MAP& map, EMBV& position, EMBAGENT& agents, DartMarker& closeMark, std::vector<Dart>& newBuildings)
{
	unsigned int state=rand()%10;
	if(state<3) {
		//generate new building
		bool found=false;
		Dart d;
		for(d = map.begin() ;!found && d!=map.end() ; map.next(d)) {
			if(!closeMark.isMarked(d) && agents[d].size()==0 && notEightAdjacentToAnObstacle<PFP,EMBV>(map,d,closeMark)) {
				typename PFP::VEC3 n1 =  Algo::Geometry::faceNormal<PFP>(map,d,position);
				typename PFP::VEC3 n2 =  Algo::Geometry::faceNormal<PFP>(map,map.phi2(d),position);

				bool groundZero=true;
				Dart dd = d;
				do {
					if(position[dd][2]!=0.0f)
						groundZero=false;
					dd = map.phi1(dd);
				} while(groundZero && dd!=d);

				if(groundZero && n1[0]==0.0f && n1[1]==0.0f && n1[2]==1.0f && n1[0]==n2[0] && n1[1]==n2[1] && n1[2]==n2[2]) {
					found=true;
					break;
				}
			}
		}

		if(found) {
			if(map.faceDegree(d)>16) {

			}
			closeMark.markOrbit(FACE_ORBIT,d);
			d = Algo::Modelisation::extrudeFace<PFP>(map,position,d,2.0f);
			newBuildings.push_back(d);
		}
	}
	else if(newBuildings.size()>0) {
	//take a newBuilding and make it higher
		state = rand()%newBuildings.size();
		Dart d = newBuildings[state];
		//decide if we create a new floor, or just make it higher
		unsigned int typeOfUpdate = rand()%10;
		if(typeOfUpdate<9) {
			Dart dd=d;
			do {
				position[dd][2] += 2.0f;
				dd = map.phi1(dd);
			} while(dd!=d);

			if(position[dd][2]>((10+rand()%10)*10.0f))
				newBuildings.erase(newBuildings.begin()+state);
		}
		else {
			typename PFP::VEC3 c = Algo::Geometry::faceCentroid<PFP>(map,d,position);
			Dart dRoofSmall = Algo::Modelisation::extrudeFace<PFP>(map,position,d,0.0f);
			Dart dd = dRoofSmall;
			do {
				position[dd] = position[dd] + (c-position[dd])/3.0f;
				dd = map.phi1(dd);
			} while (dd!=dRoofSmall);
			Dart dRoof = Algo::Modelisation::extrudeFace<PFP>(map,position,dRoofSmall,2.0f);
			
			newBuildings.erase(newBuildings.begin()+state);
			newBuildings.push_back(dRoof);
		}

	}
}

template <typename PFP, typename EMBV>
bool notEightAdjacentToAnObstacle(typename PFP::MAP& map, Dart d, DartMarker& closeMark)
{
	Dart dd = d;	
	do {
		if(closeMark.isMarked(map.alpha1(map.alpha1(dd))))
			return false;
		dd = map.phi1(dd);
	}while(dd!=d);

	return true;	
}

template <typename PFP, typename EMBV>
void generateGrid(typename PFP::MAP& map, EMBV& position, unsigned int cX,unsigned int cY,float sideLength, DartMarker& closeMark)
{
	Algo::Modelisation::Polyhedron<PFP> prim(map,position);
	prim.grid_topo(cX,cY);
	map.closeMap(closeMark) ;

	prim.embedGrid(sideLength*cX,sideLength*cY);

	map.initOrbitEmbedding(FACE_ORBIT);
}

template <typename PFP, typename EMBV>
void markObstacleSteepFaces(typename PFP::MAP& map, EMBV& position, float steepness, DartMarker& closeMark)
{
	DartMarkerStore treated(map);
	for(Dart d=map.begin();d!=map.end(); map.next(d)) {
		if(!closeMark.isMarked(d) && !treated.isMarked(d)) {
			treated.markOrbit(FACE_ORBIT,d);
			typename PFP::VEC3 n = Algo::Geometry::faceNormal<PFP>(map,d,position);
			if(fabs(n*typename PFP::VEC3(1,1,0))>steepness && !(n[2]<-0.5f)) {
				closeMark.markOrbit(FACE_ORBIT,d);
			}
		}
		if(closeMark.isMarked(d) && closeMark.isMarked(map.phi2(d))) {
		 closeMark.unmarkOrbit(EDGE_ORBIT,d);
		}
	}
}

//void EnvMap::generateStairs(Dart d, float stepSize, float maxHeight)
//{
//	for(float currentHeight=0.0f; currentHeight<maxHeight ; currentHeight += stepSize) {
//		//create one step
//		Dart dStep = Algo::Modelisation::extrudeFace<PFP>(map,position,d,stepSize);
//		typename PFP::VEC3 vecStep = position[map.phi1(map.phi1(d))]-position[map.phi1(d)];
//		vecStep.normalize();
//		vecStep *= stepSize;
//		map.cutEdge(map.phi1(dStep));
//		map.cutEdge(map.phi_1(dStep));
//		position[map.phi1(map.phi1(dStep))] = position[map.phi1(dStep)] + vecStep;
//		position[map.phi_1(dStep)] = position[dStep] + vecStep;
//		map.splitFace(map.phi_1(dStep),map.phi1(map.phi1(dStep)));
//		d = map.phi2(map.phi1(map.phi1(dStep)));
//	}
//}

template <typename PFP, typename EMBV>
void generatePathToUpperStair(typename PFP::MAP& map, EMBV& position, Dart dLower,Dart dUpper)
{
	if(dLower!= map.phi2(dLower) || dUpper!=map.phi2(dUpper)) {
		std::cout << "generatePathToUpperStair : lower and upper stair darts must be fixpoint in phi2" << std::endl;
		return;
	}

	//create the path
	Dart dPathUp = map.newOrientedFace(4);
	Dart dInBetween = map.newOrientedFace(4);
	map.cutEdge(dInBetween);
	map.cutEdge(dInBetween);
	Dart dPathDown = map.newOrientedFace(4);

	//sew the path
	map.sewFaces(dLower,dPathUp);
	map.sewFaces(map.phi1(map.phi1(dPathUp)),map.phi1(map.phi1(dInBetween)));
	map.sewFaces(map.phi1(map.phi1(dPathDown)),dInBetween);
	map.sewFaces(dUpper,dPathDown);

	//embed the inBetween floor
	float z = (position[dLower][2]+position[dUpper][2])/2.0f;

	//from lower path
	position[dInBetween] = position[dLower] + (position[dLower]-position[map.phi_1(dLower)]);
	position[map.phi1(dInBetween)] = position[map.phi1(dLower)] + (position[map.phi1(dLower)]-position[map.phi1(map.phi1(dLower))]);

	//from upper path
	position[map.phi1(map.phi1(dInBetween))] =  position[dUpper] + (position[dUpper]-position[map.phi_1(dUpper)]);
	position[map.phi1(map.phi1(map.phi1(dInBetween)))] = position[map.phi1(dUpper)] + (position[map.phi1(dUpper)]-position[map.phi1(map.phi1(dUpper))]);

	//position[map.phi_1(dInBetween)] = 	
	std::cout << "un brin a plonger !!" << std::endl;

	Dart dd = dInBetween;
	do {
		position[dd][2] = z;
		dd = map.phi1(dd);
	} while(dd!=dInBetween);
}

template <typename PFP, typename EMBV>
void generateBuilding(typename PFP::MAP& map, EMBV& position, Dart d, DartMarkerStore& build, float height, float side, unsigned int buildingType, DartMarker& closeMark)
{
	Dart dRoof;
	if(buildingType<3) {
		closeMark.markOrbit(FACE_ORBIT,d);

		dRoof = Algo::Modelisation::extrudeFace<PFP>(map,position,d,height);
	}

// 	switch(buildingType) {
// 	case 0 : {
// 		dRoof = Algo::Modelisation::extrudeFace<PFP>(map,position,dRoof,height/3);
// 		Dart dNext = map.phi1(dRoof);
// 		Dart dPrev = map.phi2(map.phi_1(dRoof));
// 		map.collapseEdge(dNext);
// 		map.collapseEdge(dPrev);
// 		}
// 		break;
// 	case 1 : {
// 		dRoof = Algo::Modelisation::extrudeFace<PFP>(map,position,dRoof,height/3);
// 		Dart dNext = map.phi1(dRoof);
// 		Dart dPrev = map.phi2(map.phi_1(dRoof));
// 		typename PFP::VEC3 mid1 = (position[dNext]+position[map.phi1(dNext)])/2.0f;
// 		typename PFP::VEC3 mid2 = (position[dPrev]+position[map.phi1(dPrev)])/2.0f;
// 		map.collapseEdge(dNext);
// 		map.collapseEdge(dPrev);
// 		position[dRoof] = mid2;
// 		position[map.phi1(dRoof)] = mid1;	
// 		}
// 		break;
// 	case 2 : {
// 		unsigned int nbStairs=rand()%5;
// 		for(unsigned int i = 0;i<nbStairs;++i) {
// 			typename PFP::VEC3 c = Algo::Geometry::faceCentroid<PFP>(map,dRoof,position);
// 			Dart dRoofSmall = Algo::Modelisation::extrudeFace<PFP>(map,position,dRoof,0.0f);
// 			Dart dd = dRoofSmall;
// 			do {
// 				position[dd] = position[dd] + (c-position[dd])/3.0f;
// 				dd = map.phi1(dd);
// 			} while (dd!=dRoofSmall);
// 			dRoof = Algo::Modelisation::extrudeFace<PFP>(map,position,dRoofSmall,height/2.0f);
// 		}
// 		bool spike=(rand() % 2);
// 		if(spike) {
// 			typename PFP::VEC3 c = Algo::Geometry::faceCentroid<PFP>(map,dRoof,position);
// 			c[2] += height/1.5f;
// // 			Dart dRoofSmall = Algo::Modelisation::extrudeFace<PFP>(map,position,dRoof,height/1.5f);
// 			Dart dRoofSmall = Algo::Modelisation::trianguleFace<PFP>(map,dRoof);
// 
// 			position[dRoofSmall] = c;
// 		}
// 	}
// 		break;
// 	case 3 : {
// 		typename PFP::VEC3 c = Algo::Geometry::faceCentroid<PFP>(map,d,position);
// 		Dart dd=d;
// 		do {
// 			Dart f = map.phi1(dd);
// 			map.cutEdge(dd);
// 			Dart e = map.phi1(dd);
// 			position[e] = position[dd];
// 			position[e] += position[f];
// 			position[e] *= 0.5;
// 			dd=map.phi1(map.phi1(dd));
// 		} while(dd!=d);
// 		Dart dMid = Algo::Modelisation::quadranguleFace<PFP>(map,d);
// 		position[dMid] = c;
// 
// 		dd = dMid;
// 		do {
// 			Dart ddd=map.phi1(map.phi1(dd));
// 
// 				Dart f = map.phi1(ddd);
// 				map.cutEdge(ddd);
// 				Dart e = map.phi1(ddd);
// 				position[e] = position[ddd];
// 				position[e] += position[f];
// 				position[e] *= 0.5;
// 
// 			ddd=map.phi1(dd);
// 				f = map.phi1(ddd);
// 				map.cutEdge(ddd);
// 				e = map.phi1(ddd);
// 				position[e] = position[ddd];
// 				position[e] += position[f];
// 				position[e] *= 0.5;
// 
// 			ddd=dd;
// 				f = map.phi1(ddd);
// 				map.cutEdge(ddd);
// 				e = map.phi1(ddd);
// 				position[e] = position[ddd];
// 				position[e] += position[f];
// 				position[e] *= 0.5;
// 
// 			build.markOrbit(FACE_ORBIT,dd);
// 			dd=map.alpha1(dd);
// 		} while(dd!=dMid);
// 
// 		dd=dMid;
// 		do {
// 			c = Algo::Geometry::faceCentroid<PFP>(map,dd,position);
// 			Dart ddMid = Algo::Modelisation::quadranguleFace<PFP>(map,dd);
// 			Dart ddd=dMid;
// 			do {
// 				build.markOrbit(FACE_ORBIT,ddMid);
// 				ddd = map.alpha1(ddd);
// 			}while(ddd!=dMid);
// 			position[ddMid] = c;
// 			dd=map.alpha1(dd);
// 		} while(dd!=dMid);
// 
// 		float bridgeHeight=20.0f;
// 		Dart dStart = d;
// 		Dart dStop=dStart;
// 		for(unsigned int i=0;i<3;++i)
// 			dStop =  map.phi2(map.phi1(map.phi1(dStop)));
// 
// 		dStop = map.phi1(map.phi1(dStop));
// 
// 		Dart dUnderStart = map.phi2(map.phi1(map.phi1(dStart)));
// 		Dart dUnderStop = map.phi2(map.phi1(map.phi1(dStop)));
// 
// 		Dart f = map.phi1(map.phi1(dUnderStart));
// 		map.cutEdge(map.phi1(dUnderStart));
// 		Dart e = map.phi1(map.phi1(dUnderStart));
// 		position[e] = (position[f]+position[map.phi1(dUnderStart)])*0.5f;
// 
// 		f = map.phi_1(dUnderStart);
// 		map.cutEdge(f);
// 		e = map.phi_1(dUnderStart);
// 		position[e] = (position[f]+position[dUnderStart])*0.5f;
// 
// 		map.splitFace(map.phi1(map.phi1(dUnderStart)),map.phi_1(dUnderStart));
// 		closeMark.markOrbit(FACE_ORBIT, dUnderStart);
// 
// 		f = map.phi1(map.phi1(dUnderStop));
// 		map.cutEdge(map.phi1(dUnderStop));
// 		e = map.phi1(map.phi1(dUnderStop));
// 		position[e] = (position[f]+position[map.phi1(dUnderStop)])*0.5f;
// 
// 		f = map.phi_1(dUnderStop);
// 		map.cutEdge(f);
// 		e = map.phi_1(dUnderStop);
// 		position[e] = (position[f]+position[dUnderStop])*0.5f;
// 
// 		map.splitFace(map.phi1(map.phi1(dUnderStop)),map.phi_1(dUnderStop));
// 		closeMark.markOrbit(FACE_ORBIT, dUnderStop);
// 
// 		generateBridge<PFP,EMBV>(map,position,dStart,dStop,bridgeHeight,closeMark);
// 	}
// 	break;
// 	default :
// 		break;
// 	}
}

template <typename PFP, typename EMBV>
void generateBridge(typename PFP::MAP& map, EMBV& position, Dart dStart, Dart dStop, float bridgeHeight, DartMarker& closeMark)
{
	typename PFP::VEC3 cStart = Algo::Geometry::faceCentroid<PFP>(map,dStart,position);
	typename PFP::VEC3 cStop = Algo::Geometry::faceCentroid<PFP>(map,dStop,position);

	Dart ddStart = Algo::Modelisation::extrudeFace<PFP>(map,position,dStart,bridgeHeight);
	Dart ddStop = Algo::Modelisation::extrudeFace<PFP>(map,position,dStop,bridgeHeight/2.0f);
	position[ddStop][2] += bridgeHeight/2.0f;
	typename PFP::VEC3 transl = position[map.phi_1(ddStop)]-position[ddStop];
	position[ddStop] += transl *0.5f;
	position[map.phi1(ddStop)][2] += bridgeHeight/2.0f;
	transl = position[map.phi1(map.phi1(ddStop))]-position[map.phi1(ddStop)];
	position[map.phi1(ddStop)] += transl *0.5f;
	transl = Algo::Geometry::faceCentroid<PFP>(map,ddStop,position);
	Dart dLoop = ddStop;
	do {
		position[dLoop] += (transl-position[dLoop])*0.1f;
		if(position[dLoop][2]>(bridgeHeight/2.0f+bridgeHeight/10.0f)) {
			position[dLoop] += (transl-position[dLoop])*0.4f;
		}
		position[dLoop] += (cStart-cStop)/4.0f;
		dLoop = map.phi1(dLoop);
	} while(dLoop !=ddStop);

	Dart dddStop = Algo::Modelisation::extrudeFace<PFP>(map,position,ddStop,0.0f);
	dLoop = dddStop;

	transl = (cStart-cStop)/3.0f;
	do {
		position[dLoop] += (transl)*position[dLoop][2]/bridgeHeight;
		dLoop = map.phi1(dLoop);
	} while(dLoop!=dddStop);

	Dart dBef = map.phi2(dddStop);
	Dart dBef2 = map.phi2(ddStop);
	map.mergeVolumes(dddStop,ddStart);

	closeMark.markOrbit(FACE_ORBIT,map.alpha1(map.alpha1(map.phi2(dBef))));
	closeMark.markOrbit(FACE_ORBIT,map.phi2(map.alpha1(map.alpha1(map.phi2(dBef)))));
	closeMark.markOrbit(FACE_ORBIT,map.alpha1(map.alpha1(dBef)));
	closeMark.markOrbit(FACE_ORBIT,map.alpha1(map.alpha1(map.phi2(dBef2))));
	closeMark.markOrbit(FACE_ORBIT,map.alpha1(map.alpha1(dBef2)));
	closeMark.markOrbit(FACE_ORBIT,map.phi2(map.alpha1(map.alpha1(dBef2))));

// 	map.mergeFaces(map.phi1(ddStart));
// 	map.mergeFaces(map.phi_1(ddStart));

// 	map.mergeFaces(map.alpha1(map.alpha1(map.phi2(dBef))));
// 	map.mergeFaces(map.alpha1(map.alpha1(dBef)));
// 	map.mergeFaces(map.alpha1(map.alpha1(map.phi2(dBef2))));
// 	map.mergeFaces(map.alpha1(map.alpha1(dBef2)));

// 	map.mergeFaces(map.alpha_1(dBef));
}

template <typename PFP, typename EMBV>
void generateBridge(typename PFP::MAP& map, EMBV& position, unsigned int cX, unsigned int cY,float sideLength, DartMarker& closeMark)
{
	std::cout << "Warning : current version allows only one bridge.." << std::endl;

	//define a ground grid
	Algo::Modelisation::Polyhedron<PFP> ground(map,position);
        ground.grid_topo(5,3);

    ground.embedGrid(sideLength*5,sideLength*3);

	std::cout << "embedded" << std::endl;

	//get the insertion darts of the bridge
	Dart d = ground.getDart();
	Dart dStart = map.phi1(map.phi2(map.phi1(map.phi1(d))));
	Dart dStop=dStart;
	for(unsigned int i=0;i<3;++i) {
		dStop =  map.phi1(map.phi1(map.phi2(dStop)));
	}
	dStop = map.phi2(dStop);


	std::cout << "ground S : " << dStart << " stop " << dStop << std::endl;
	std::cout << "ground S : " << map.phi2(dStart) << " stop " << map.phi2(dStop) << std::endl;

	dStart = map.phi2(dStart);
	Dart ddStart = Algo::Modelisation::extrudeFace<PFP>(map,position,dStart,100.0f);

	dStop = map.phi2(dStop);
	Dart ddStop = Algo::Modelisation::extrudeFace<PFP>(map,position,dStop,50.0f);
	position[ddStop][2] += 50.0f;
	typename PFP::VEC3 transl = position[map.phi_1(ddStop)]-position[ddStop];
	position[ddStop] += transl *0.5f;
	position[map.phi1(ddStop)][2] += 50.0f;
	transl = position[map.phi1(map.phi1(ddStop))]-position[map.phi1(ddStop)];
	position[map.phi1(ddStop)] += transl *0.5f;
	transl = Algo::Geometry::faceCentroid<PFP>(map,ddStop,position);
	Dart dLoop = ddStop;
	do {
		position[dLoop] += (transl-position[dLoop])*0.1f;
		if(position[dLoop][2]>70.0f) {
			std::cout << "changement de plan supérieur" << std::endl;
			position[dLoop] += (transl-position[dLoop])*0.1f;
		}
		dLoop = map.phi1(dLoop);
	} while(dLoop !=ddStop);

	Dart dddStop = Algo::Modelisation::extrudeFace<PFP>(map,position,ddStop,0.0f);
	dLoop = dddStop;
	transl = typename PFP::VEC3(-sideLength*2.5f,0,0);
	do {
		position[dLoop] += (transl)*position[dLoop][2]/100.0f;
		if(position[dLoop][2]==100.0f) {
			
		}
		dLoop = map.phi1(dLoop);
	} while(dLoop!=dddStop);

	map.mergeVolumes(dddStop,ddStart);

	map.closeMap(closeMark);

	map.check();
	std::cout << "bridge built" << std::endl;
}

}

}
