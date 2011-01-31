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

#include "viewer.h"
#include <string>

/**********************************************************************************************
 *                                 AntTweakBar CALLBACKS                                      *
 **********************************************************************************************/

/**********************************************************************************************
 *                                 MyGlutWin IMPLEMENTATION                                   *
 **********************************************************************************************/

MyGlutWin::MyGlutWin(int* argc, char **argv, int winX, int winY) :
	GlutWin_ATB(argc, argv, winX, winY)
{
	if(this->shaderOk)
	{
// 		shaders[0].loadShaders("phong_vs.txt", "phong_ps.txt") ;
// 		shaders[1].loadShaders("flat_vs.txt", "flat_ps.txt", "flat_gs.txt") ;
	}
}


void MyGlutWin::updateUI()
{
}

void MyGlutWin::init()
{
	posToReach = PFP::VEC3(0,0,0);
	

	b_animate = false;
	render_anim = false;
	nbGenerated=0;
	sim = new Simulator();

	displayFps=false;
	nextUpdate = clock() + 1 * CLOCKS_PER_SEC;

// 	topo_render = new Algo::Render::VBO::topo_VBORenderMapD() ;

// 	m_render = new Algo::Render::VBO::MapRender_VBO<PFP>(sim->envMap.map, allDarts) ;
// 	m_render->initPrimitives(Algo::Render::VBO::TRIANGLES) ;
// 	m_render->updateData(Algo::Render::VBO::POSITIONS, sim->envMap.position) ;

// 	Algo::Geometry::computeNormalFaces<PFP>(sim->envMap.map, sim->envMap.position, sim->envMap.normal) ;
// 	m_render->updateData(Algo::Render::VBO::NORMALS, sim->envMap.normal) ;;

// 	m_render->initPrimitives(Algo::Render::VBO::TRIANGLES) ;
// 	m_render->initPrimitives(Algo::Render::VBO::LINES) ;
}

void MyGlutWin::initGUI()
{
// 	viewer_bar = TwNewBar("SocialAgents") ;
// 	TwDefine("SocialAgents position='16 16' size='350 50' valueswidth='80'") ;
// 	TwAddSeparator(viewer_bar, "", "") ;

	glEnable( GL_POINT_SMOOTH );

	visu=0;
}

bool MyGlutWin::reachedGoal(Simulator* sim)
{
	if((int(sim->globalTime_)%1000)==0)
		std::random_shuffle( sim->goals.begin(), sim->goals.end() );
//   /* Check if all agents have reached their goals. */
//   for (size_t i = 0; i < sim->getNumAgents(); ++i) {
// //     if (absSq(sim->getAgentPosition(i) - posToReach) > sqr(40.0f)) {
// 	if (absSq(sim->getAgentPosition(i) - sim->goals[sim->agents_[i]->goalNo_]) > 20.0f * 20.0f) 
//       return false;
//   }
// 
//   return true;

  /* Check if all agents have reached their goals. */
//   for (size_t i = 0; i < sim->getNumAgents(); ++i) {
//     if (absSq(sim->getAgentPosition(i) - posToReach) > sqr(40.0f)) {
// 	if (absSq(sim->getAgentPosition(i) - sim->goals[sim->agents_[i]->goalNo_]) < 10000.0f) {
// 		sim->agents_[i]->setGoalNo(rand()%sim->goals.size());
// 		sim->agents_[i]->goalNo_ = (sim->agents_[i]->goalNo_+1)%sim->goals.size();
// 		std::cout << "changing" << std::endl;
// 		for(std::vector<VEC3>::iterator it = sim->goals.begin() ; it!= sim->goals.end() ; ++it )
// 			std::cout << *it << std::endl;
// 		std::random_shuffle(sim->goals.begin(),sim->goals.end());
// 		return false;
// 	}
//   }
	

  return false;
}

void glCircle3i(GLint x, GLint y, GLint radius) {
    float angle; 
//     glPushMatrix();
//     glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 5; i++) { 
        angle = i*2*M_PI/5; 
        glVertex2f(x + (cos(angle) * radius), y + (sin(angle) * radius)); 
    } 
    glEnd(); 
//     glEnable(GL_TEXTURE_2D);
//     glPopMatrix();
}

void MyGlutWin::myRedraw()
{
// 	GLfloat black[4]= {0.0f,0.0f,0.0f,1.0f};
// 	GLfloat amb[4]= {0.2f,0.1f,0.1f,1.0f};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	if(b_animate) {
		animate();
		if(reachedGoal(sim)) {
			b_animate=false;
		}
	}

	glColor3f(0.5f,0.5f,0.5f);
	glBegin(GL_LINES);
		glVertex3f(0.,0.,0.);
		glVertex3f(1.,0.,0.);

		glVertex3f(0.,0.,0.);
		glVertex3f(0.,1.,0.);

		glVertex3f(0.,0.,0.);
		glVertex3f(0.,0.,1.);
	glEnd();

// 	SelectorMarked<PFP::MAP> markDarts(sim->envMap.map,sim->envMap.closeMark);
	SelectorTrue markDarts;

// 	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHTING);

	glColor3f(1.0f,0.0f,0.0f);

// 	glBegin(GL_POINTS);
// 		glVertex3fv(&posToReach[0]);
// 		for(Dart d = sim->envMap.map.begin() ; d != sim->envMap.map.end() ; sim->envMap.map.next(d))
// 			glVertex3fv(&sim->envMap.getPosition(d)[0]);
// 	glEnd();

// 	glBegin(GL_LINES);
// 	for(std::vector<Dart>::iterator it = sim->path.begin() ; it != sim->path.end() ; ++ it) {
// 		VEC3 c = Algo::Geometry::faceCentroid<PFP>(sim->envMap.map,*it,sim->envMap.position);
// 		glVertex3fv(&c[0]);
// 	}
// 	glEnd();

//	glBegin(GL_POINTS);
//	for(unsigned int i = 0; i<4;++i) {
//		for(std::vector<VEC3>::iterator it=(*sim->pathToFollow[0]).begin() ; it!=(*sim->pathToFollow[0]).end() ; ++it) {
//			glVertex3fv(&(*it)[0]);
//		}
//	}
//	glEnd();

	unsigned int i=0;
	for(std::vector<Agent * >::iterator it = sim->agents_.begin() ; it != sim->agents_.end() ; ++ it, ++i) {
// 		glColor3f(i%int(2.0f/3.0f*v.size())/(v.size()/3.0f),i%int(1.0f/3.0f*v.size())/(v.size()/3.0f),i/(v.size()/3.0f));
//		glCircle3i(v[i][0],v[i][1],1.5f);
		glPushMatrix();
//			Geom::Plane3D<PFP::REAL> pl = Algo::Geometry::trianglePlane<PFP>(sim->envMap.map,sim->agents_[i]->nearestDart,sim->envMap.position);
			VEC3 pBase(sim->agents_[i]->position_);
			VEC3 posR = pBase;
//			pl.normal() = -1.0f*pl.normal();

// 			VEC3 v(sim->envMap.position[sim->agents_[i]->nearestDart]-pBase);
// 			float scal = v*pl.normal();
// 			posR = pBase + scal*pl.normal();

//			VEC3 dir(0.0f,0.0f,-1.0f);
//			pBase[2]=100000.0f;
//			Geom::intersectPlaneRay<VEC3,Geom::Plane3D<PFP::REAL> >(pl,pBase,dir,posR);

// 			VEC3 posR(sim->agents_[i]->position_[0],sim->agents_[i]->position_[1],0);
			glTranslatef(posR[0],posR[1],posR[2]);
// 			glutSolidSphere(1.5f,5,5);
// 			if(i==0)
// 			  glColor3f(1.0f,1.0f,0.0f);
// 			else
			  glColor3f(1.0f,0.0f,0.0f);
// 			glVertex3f(posR[0],posR[1],posR[2]);
			glCircle3i(0,0,sim->agents_[i]->radius_);
  		glPopMatrix();
// 	glEnd();

	glColor3f(1.0f,1.0f,1.0f);

// 		//draw all containing faces
// 		glColor3f(1,1,0);
// 		for(std::vector<Dart>::iterator incF = (*it)->includingFaces.begin(); incF != (*it)->includingFaces.end(); ++incF) {
// 			renderFace(sim->envMap,*incF);
// 		}


		if(visu==1) {
			//draw containing center face
	 		glColor3f(0,0,1);
	 		glLineWidth(4.0f);
	 		renderFace(sim->envMap,(*it)->part->d);
	 		glLineWidth(1.0f);

	// 		//draw displacement
	// 		glBegin(GL_LINES);
	// 		glVertex3fv(&(*it)->position_[0]);
	// 		glVertex3fv(&(*it)->part->prevPos[0]);
	// 		glEnd();

	// 		//and prediction triangle
 			glColor3f(((*it)->part->state)/3.0f,((*it)->part->state%2),0);
 			glLineWidth(5.0f);
 			renderPredictionTriangle(sim->envMap,(*it)->part->d,/*(*it)->part->m_position*/posR);
 			glLineWidth(1.0f);

			//draw next goal
//			glBegin(GL_LINES);
//				glVertex3f((*it)->part->m_position[0],(*it)->part->m_position[1],(*it)->part->m_position[2]);
//				glVertex3f((*sim->pathToFollow[sim->pathPointer[i].first])[sim->pathPointer[i].second][0],
//							(*sim->pathToFollow[sim->pathPointer[i].first])[sim->pathPointer[i].second][1],
//							(*sim->pathToFollow[sim->pathPointer[i].first])[sim->pathPointer[i].second][2]);
//			glEnd();
		}

// 		glCircle3i((*it)->position_[0],(*it)->position_[1],(*it)->neighborDist_);

// 		glCircle3i((*it)->position_[0],(*it)->position_[1],(*it)->timeHorizonObst_ * (*it)->maxSpeed_ + (*it)->radius_);

		for(std::vector<std::pair<float, Dart> >::iterator 	obst = (*it)->obstacleNeighbors_.begin();
															obst !=(*it)->obstacleNeighbors_.end(); ++obst) {
			glColor3f(0,1,1);
			glLineWidth(10.0f);
			renderDart(sim->envMap,(*obst).second);
			glLineWidth(1.0f);
		}
	}

// 	glBegin(GL_POINTS);
// 		glVertex3f(11.8162,-2.77888,0);
// 	glEnd();
// 	glBegin(GL_LINES);
// 		glVertex3f(4.6252,4.66903,0);
// 		glVertex3f(4,5,0);
// 	glEnd();
// 	glLineWidth(10.0f);
// 	Dart d = sim->envMap.getDart(608);
// 	renderDart(sim->envMap,d);
// 	glLineWidth(1.0f);

// 	glBegin(GL_LINES);
// 	glVertex3f(-7.00001,269,0);
// 	glVertex3f(-7.28877,268.963,0);
// 	glVertex3f(0,168,0);
// 	glVertex3f(-14,168,0);
// 	glEnd();
// 	glBegin(GL_POINTS);
// 	glVertex3f(-798.772,168,0);
// 	glEnd();


	//draw the environment
	glColor3f(1,1,1);
// 	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHTING);

	if(visu==0) {
		SelectorUnmarked sum(sim->envMap.closeMark);

// 		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		Algo::Render::Direct::renderTriQuadPoly<PFP>(sim->envMap.map,Algo::Render::Direct::FLAT,1.0,sim->envMap.position,sim->envMap.normal,sum);
// 		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
// 		m_render->draw(Algo::Render::VBO::TRIANGLES) ;

// 		glColor3f(0,0,0);
// 		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
// 		Algo::Render::Direct::renderTriQuadPoly<PFP>(sim->envMap.map,Algo::Render::Direct::LINE,1.0,sim->envMap.position,sim->envMap.normal);
// 		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	}
	else if(visu==1) {
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		SelectorMarked sm(sim->envMap.closeMark);
		Algo::Render::Direct::renderTriQuadPoly<PFP>(sim->envMap.map,Algo::Render::Direct::LINE,1.0,sim->envMap.position,sim->envMap.normal,sm);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
// 		for(Dart d = sim->envMap.map.begin() ; d != sim->envMap.map.end() ; sim->envMap.map.next(d)) {
// 			if( sim->envMap.closeMark.isMarked(d)) {
// 				renderDart(sim->envMap,d);
// 			}
// 		}
	}
	else if(visu==2) {

// 		renderDart(sim->envMap,sim->envMap.getDart(15332));
// 		renderDart(sim->envMap,sim->envMap.getDart(15336));
// 		renderDart(sim->envMap,sim->envMap.getDart(15325));
// 		renderDart(sim->envMap,sim->envMap.getDart(15323));
// 		renderDart(sim->envMap,sim->envMap.getDart(15329));
// 		renderDart(sim->envMap,sim->envMap.getDart(15314));
// 		renderDart(sim->envMap,sim->envMap.getDart(15334));
// 		renderDart(sim->envMap,sim->envMap.getDart(53));
// 		renderDart(sim->envMap,sim->envMap.getDart(50));
// 		renderDart(sim->envMap,sim->envMap.getDart(49));
// 		renderDart(sim->envMap,sim->envMap.getDart(46));
		glDisable(GL_LIGHTING);
		SelectorTrue alldarts;
// 		topo_render = new Algo::Render::VBO::topo_MD2_VBORender<PFP>(sim->envMap.map,alldarts) ;
// 		topo_render->updateData<PFP>(sim->envMap.map,alldarts,sim->envMap.position, 0.9f, 0.9f);
//   		topo_render->drawTopo();
		Algo::Render::Direct::renderTopoMD2<PFP>(sim->envMap.map,sim->envMap.position,true,true,0.9f,0.9f);
	}
	else if(visu==3) {
// 		renderDart(sim->envMap,sim->envMap.getDart(33701));
// 		renderDart(sim->envMap,sim->envMap.getDart(452));
// 		renderCellOfDim(sim->envMap,sim->envMap.getDart(1257),2);
// 		glBegin(GL_POINTS);
// 		glVertex3f(-494.518,-672,0);
// 		glEnd();
	}

	//count fps
	if(displayFps) {
		++frames;
		clock_t overtime = clock() - nextUpdate;
		if (overtime > 0) {
			float fps = frames / (float)(1+ (float)overtime/(float)CLOCKS_PER_SEC);
			frames = 0;
			nextUpdate = clock() + 1 * CLOCKS_PER_SEC;
			std::cout << "fps : " << fps << std::endl;
		}
	}


	glPopMatrix();
}

void updateVisualization(Simulator* sim)
{
  /* Output the current global time. */
  std::cout << sim->globalTime_ << std::endl;

  /* Output the current position of all the agents. */
//   for (size_t i = 0; i < sim->getNumAgents(); ++i) {
// //     std::cout << sim->getAgentPosition(i) << std::endl;
// 	VEC3 v = sim->getAgentPosition(i);
// 	std::cout << "(" << v[0] << "," << v[1] << ")" << std::endl;
//   }

//   std::cout << std::endl;
}

void MyGlutWin::animate(void)
{
// 	if(int(sim->globalTime_)%2)
// 	CGoGN::CityGenerator::animateCity<PFP,PFP::TVEC3,PFP::TAB_AGENTVECT>(sim->envMap.map,sim->envMap.position,sim->envMap.agentvect,sim->envMap.closeMark,sim->envMap.newBuildings);
// 	sim->envMap.map.check();
// 	posToReach[0] += sin(rand());
// 	posToReach[1] += cos(rand());
// 	std::cout << "pos to reach : " << posToReach << std::endl;
// 	std::cout << sim->getGlobalTime() << " " << std::endl;
	updateVisualization(sim);
	sim->setPreferredVelocities(posToReach);
//	sim->setPreferredPathVelocities();
// 	sim->setPreferredNextCellVelocities();
	sim->doStep();
// 	sim->envMap.simplify();
	glutPostRedisplay();

// 	for(Dart d = sim->envMap.map.begin() ; d != sim->envMap.map.end() ; sim->envMap.map.next(d)) {
// 		if(sim->envMap.closeMark.isMarked(d) && !sim->envMap.closeMark.isMarked(sim->envMap.map.phi1(d))) {
// 			std::cout << "probleme" << std::endl;
// 		}
// 	}

	if(render_anim) {
		std::ostringstream oss;
		std::ostringstream tmpNb;
		tmpNb << std::setfill('0') << std::setw(4) << nbGenerated;
		nbGenerated++;
		oss << "./helicoid3/exportTransZoomHelicoid" << tmpNb.str() << ".pov";
		std::string chaine = oss.str();
// 		exportScenePov(sim->envMap.map,sim->envMap.position,chaine,VEC3(43,762,65),VEC3(0,762,0),VEC3(1.0f,0,0),0,0,0);
		exportScenePov(sim->envMap.map,sim->envMap.position,chaine,VEC3(43,762,65+(1500.0f*float(nbGenerated)/400.0f)),VEC3(0,762,0),VEC3(1.0f,0,0),0,0,0);

		if(nbGenerated==400) {
			std::cout << "enough .pov generated" << std::endl;
			exit(0);
		}
	}
}

bool MyGlutWin::exportScenePov(PFP::MAP& map, PFP::TVEC3& position, const std::string& filename, PFP::VEC3 cameraPos, PFP::VEC3 cameraLook, PFP::VEC3 translate, float angle_X, float angle_Y, float angle_Z,const FunctorSelect& good)
{
	static const bool highlightAgent=false;
	static const bool wireDisplay=false;
	
	uint highlightAgentNo=0;
  
	std::ofstream out(filename.c_str(), std::ios::out);
	if (!out.good()) {
		std::cerr << "(export) Unable to open file " << filename << std::endl;
		return false;
	}

	float angleX = angle_X;
	float angleY = angle_Y;
	float angleZ = angle_Z;
	
	if(highlightAgent) {
	  cameraLook = sim->agents_[highlightAgentNo]->position_;
	   VEC3 goalV = (sim->goals[highlightAgentNo] - sim->agents_[highlightAgentNo]->position_);
	   goalV.normalize();
	  cameraPos = cameraLook - goalV*50.0f;
// 	  cameraPos[2]+= 100.0f;
	  float tmp = cameraPos[1];
	  cameraPos[1] = cameraPos[2]+400.0f;
	  cameraPos[2] = tmp;
	  tmp = cameraLook[1];
	  cameraLook[2] = cameraLook[1];
	  cameraLook[1] = tmp;
	}

	//define the camera position
	out << "camera { location <" << cameraPos[0] << "," << cameraPos[1] << "," << cameraPos[2]+100.0f << "> look_at <" << cameraLook[0] << "," << cameraLook[1] << "," << cameraLook[2] <<"> ";
// 	if(highlightAgent) {
// 	  out << "translate <" << translate[0] << "," << translate[1] << "," << translate[2] << "> " << std::endl;
// 	  out << "rotate <" << angleX << "," << angleY << "," << angleZ << "> }" << std::endl;
// 	}
	out << "}" << std::endl;
	
	//set a "infinite" plane 
// 	out << "plane { y, -1 pigment { color rgb 1 } }" << std::endl;
	//set a sky sphere
	out << "sphere { <0, 0, 0>, 5000";
	out << "texture{ pigment { color rgb <1, 1, 1>}	finish { ambient 1 diffuse 0 } } }" << std::endl;
	//put some lights
	out << "light_source { <" << cameraPos[0] << "," << cameraPos[1]+200 << "," << cameraPos[2] << "> color rgb 0.45}" << std::endl;
// 	out << "light_source { <-800, 800, -800> color rgb 0.25 }"<< std::endl;
	//set a high quality rendering
	out << "global_settings {" << std::endl;
	out << "radiosity {" << std::endl;
	out << "pretrace_start 0.08 pretrace_end 0.04" << std::endl;
	out << "count 500 nearest_count 10 error_bound 0.15 recursion_limit 1 low_error_factor 0.2 gray_threshold 0.0 minimum_reuse 0.015 brightness 1 adc_bailout 0.01/2 normal off media off} max_trace_level 255}" << std::endl;

	std::vector<PFP::VEC3> v = sim->getAgentsPosition();
	for(unsigned int i = 0; i< sim->agents_.size() ; ++i) {
		out << "cylinder {" << std::endl;
			Geom::Plane3D<PFP::REAL> pl = Algo::Geometry::trianglePlane<PFP>(sim->envMap.map,sim->agents_[i]->nearestDart,sim->envMap.position);
			VEC3 pBase(sim->agents_[i]->position_);
			VEC3 posR;
			pl.normal() = -1.0f*pl.normal();
			VEC3 dir(0.0f,0.0f,-1.0f);
			pBase[2]=100000.0f;
			Geom::intersectPlaneRay<VEC3,Geom::Plane3D<PFP::REAL> >(pl,pBase,dir,posR);
// 			VEC3 posR(sim->agents_[i]->position_[0],sim->agents_[i]->position_[1],0);
			out << "<" << posR[0] << "," << posR[2]+0.05 << "," << posR[1] << ">, " << std::endl;
			out << "<" << posR[0] << "," << posR[2]+(sim->agents_[i]->radius_*5.0f) << "," << posR[1] << ">, " << std::endl;
			out << sim->agents_[i]->radius_ << std::endl;
			
			if(highlightAgent) {
			  if(i==highlightAgentNo) {
			    out << "texture{ pigment{ color rgb<0.5,1.0,0.5>} finish { ambient rgb 0.1 brilliance 0.5 } }" << std::endl;
			  }
			  else {
			    Agent * ag = sim->agents_[highlightAgentNo];
			    bool found = false;
			    for(std::vector<std::pair<float, size_t> >::iterator it = ag->agentNeighbors_.begin();!found && it != ag->agentNeighbors_.end();++it) {
				if((*it).second==i)
				    found=true;
			    }
			    if(found) {
// 				std::cout << "trouve voisin" << std::endl;
				out << "texture{ pigment{ color rgb<0.5.0,0.5,0.0>} finish { ambient rgb 0.1 brilliance 0.5 } }" << std::endl;
			    }
			    else {
				  out << "texture{ pigment{ color rgb<0.5,0.5.0,0.5>} finish { ambient rgb 0.1 brilliance 0.5 } }" << std::endl;
			    }
			  }
			}
			else {
				if(i<(sim->agents_.size()/2))
					out << "texture{ pigment{ color rgb<0.5,1.0,0.5>} finish { ambient rgb 0.1 brilliance 0.5 } }" << std::endl;
				else
					out << "texture{ pigment{ color rgb<0.5,0.5,1.0>} finish { ambient rgb 0.1 brilliance 0.5 } }" << std::endl;

			}
			
 			out << "translate <" << translate[0] << "," << translate[1] << "," << translate[2] << ">" << std::endl;
 			out << "rotate <" << angleX << "," << angleY << "," << angleZ << "> " << std::endl;
		out << "}" << std::endl;
	}

	if(highlightAgent) {
	  DartMarkerStore highlight(map);
	  Agent * ag = sim->agents_[highlightAgentNo];
	  for(Dart d=map.begin();d!=map.end();map.next(d)) {
	    if(!highlight.isMarked(d)) {
	      PFP::AGENTS listAgentInCell  = sim->envMap.agentvect[d];
	      if(std::find(listAgentInCell.begin(), listAgentInCell.end(), ag)!=listAgentInCell.end()) {
		highlight.markOrbit(FACE_ORBIT,d);
	      }
	    }
	  }
	  
	  SelectorMarked sm(highlight);
	  SelectorUnmarked sum(highlight);
	  Algo::ExportPov::exportMeshPlain<PFP>(out,map,position,"facesHighlighted",sm);
	  Algo::ExportPov::exportMeshPlain<PFP>(out,map,position,"myMesh",sum);

	  out << "object {facesHighlighted" << std::endl;
	  out << "translate <" << translate[0] << "," << translate[1] << "," << translate[2] << ">" << std::endl;
	  out << "rotate <" << angleX << "," << angleY << "," << angleZ << "> " << std::endl;
	  out << "texture{ pigment{ color rgb<0.4667,0.709,0.996>} finish { ambient rgb 0.35 brilliance 0.5 } } }" << std::endl;
	}
	else {
	  Algo::ExportPov::exportMeshPlain<PFP>(out,map,position,"myMesh",good);
	}

	if(wireDisplay) {
		Algo::ExportPov::exportMeshWire<PFP>(out,map,position,"myMeshWire",good);
		out << "object {myMeshWire" << std::endl;
		out << "translate <" << translate[0] << "," << translate[1] << "," << translate[2] << ">" << std::endl;
		out << "rotate <" << angleX << "," << angleY << "," << angleZ << "> " << std::endl;
		out << "texture{ pigment{ color rgb<0,0,0>} } }" << std::endl;
	}
	
	out << "object {myMesh" << std::endl;
 	out << "translate <" << translate[0] << "," << translate[1] << "," << translate[2] << ">" << std::endl;
 	out << "rotate <" << angleX << "," << angleY << "," << angleZ << "> " << std::endl;
 	out << "texture{ pigment{ color rgbf<1.0,1.0,1,"<< 1.0f*(float(nbGenerated)/400.0f) <<">} finish { ambient rgb 0.3 brilliance 0.5 } } }" << std::endl;
//  	out << "texture{ pigment{ color rgb<1.0,1.0,1>} finish { ambient rgb 0.05 brilliance 0.5 } } }" << std::endl;

	out.close();
// 	for(Dart d = sim->envMap.map.begin() ; d != sim->envMap.map.end() ; sim->envMap.map.next(d)) {
// 		if(sim->envMap.closeMark.isMarked(d) && !sim->envMap.closeMark.isMarked(sim->envMap.map.phi1(d))) {
// 			std::cout << "probleme" << std::endl;
// 		}
// 	}
	return true;
}

void MyGlutWin::myKeyboard(unsigned char keycode, int x, int y)
{
	switch (keycode) {

		case 'a': {
			std::cout << ".." << std::endl;
			sim->envMap.map.check();
			b_animate = !b_animate;
		}
		break;
		case 'f': {
			displayFps = ! displayFps;
		}
		break;
		case 'p' : {
			std::cout << "display agents positions : " << std::endl;
			for(std::vector<Agent * >::iterator it = sim->agents_.begin() ; it != sim->agents_.end() ; ++ it) {
			      std::cout << (*it)->position_ << std::endl;
			}
		}
		break;
		case 'r' : {
// 			exportScenePov(sim->envMap.map,sim->envMap.position,"exportScene0002.pov",VEC3(1500,500,1500),VEC3(0,0,0),VEC3(1.0f,0,0),0,0,0);
			exportScenePov(sim->envMap.map,sim->envMap.position,"exportScene0002.pov",VEC3(43,762,1565),VEC3(0,762,0),VEC3(1.0f,0,0),0,0,0);
// 			exportScenePov(sim->envMap.map,sim->envMap.position,"exportScene0002.pov",VEC3(43,762,65),VEC3(0,762,0),VEC3(1.0f,0,0),0,0,0);
		}
		break;
		case 's': {
			animate();
		}
		break;
		case 't' : {
			for(std::vector<Agent * >::iterator it = sim->agents_.begin() ; it != sim->agents_.end() ; ++ it) {
				std::cout << "(" << (*it)->position_[0] << "," << (*it)->position_[1] << ")" << std::endl;
			}
		}
		break;
		case 'v': {
			++visu;
			visu = visu % 4;
		}
		break;
		case '9': {
			render_anim=!render_anim;
		}
		break;
		case 'c': {
			sim->envMap.map.check();
		}
		break;
	}

	updateUI();
	glutPostRedisplay();
}

float MyGlutWin::computeSelectRadius(int x, int y, int pixelRadius)
{
   GLint viewport[4];
   GLdouble modelview[16];
   GLdouble projection[16];
   GLfloat winX, winY, winZ;
   GLdouble posX, posY, posZ;

   glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
   glGetDoublev( GL_PROJECTION_MATRIX, projection );
   glGetIntegerv( GL_VIEWPORT, viewport );

   // get depth
   glReadPixels( x, viewport[3]-y , 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

   // if we have click on background, consider that we are in the middle of space
   // to avoid too big radius
   if (winZ == 1.0f)    // depth vary in [0-1]
       winZ = 0.5f;

   winX = (float)x;
   winY = (float)viewport[3] - (float)y;

//    get first point in object space
   gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
   PFP::VEC3 p(posX,posY,posZ);

//    get second point in object space
   gluUnProject( winX+pixelRadius, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
   PFP::VEC3 q(posX,posY,posZ);

   // compute & return distance
   q -= p;
   return float(q.norm());
}

void MyGlutWin::myMouse(int button, int state, int x, int y)
{
	glutPostRedisplay();
}

/**********************************************************************************************
 *                                      MAIN FUNCTION                                         *
 **********************************************************************************************/

int main(int argc, char** argv)
{
	MyGlutWin* mgw = new MyGlutWin(&argc, argv, 1200, 800) ;

	mgw->init() ;

	if(argc==2) {
// 		std::cout << "do some step" << std::endl;
// 		char c;
// 		do {
// 			mgw->animate();
// 			c = std::getchar();	
// 		} while(c!='x');

//		GLint t1 = glutGet(GLUT_ELAPSED_TIME);
 		do {
 			mgw->animate();
 		} while (!mgw->reachedGoal(mgw->sim));
//		for(unsigned int i =0;i<300;++i) {
//			mgw->animate();
//		}
//		GLint t2 = glutGet(GLUT_ELAPSED_TIME);
//		GLfloat seconds = (t2 - t1) / 1000.0f;
//		std::cout << "animation : "<< seconds << "sec" << std::endl;
	}
	else {
		mgw->initGUI() ;
		mgw->mainLoop() ;
	}
	return 0 ;
}
