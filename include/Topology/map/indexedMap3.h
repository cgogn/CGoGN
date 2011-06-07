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

#ifndef __INDEXEDMAP3_H__
#define __INDEXEDMAP3_H__

#include "Topology/map/map3.h"
#include "indexEmb.h"

class DP_IndMap3 : public DefaultDartParameters
{
public:
	static const int nbInvolutions=2;
	static const int nbEmbeddings=4;
};

typedef tMap3< DartObjDualMap<DP_IndMap3> > IMAP3;


template<typename EMBV>
class IndexedMapd3Topology: public IMAP3
{
protected:
	/**
	* Table of darts per vertex
	*/
	std::vector<Dart> m_tableVertexDart;

	/**
	* Table of darts per edge
	*/
	std::vector<Dart> m_tableEdgeDart;

	/**
	* Table of darts per face
	*/
	std::vector<Dart> m_tableFaceDart;

	/**
	* Table of darts per volume 
	*/
	std::vector<Dart> m_tableVolumeDart;


	int nbPoints;

	vector< Vector3 > seqPoints;


	/**
	* Table of edges indices
	*/
	SeqEdges m_edgesTopoIndices;

	/**
	* Table of face indices
	*/
	SeqTriangles m_facesTopoIndices;

	/**
	* Table of volume indices
	*/
	SeqTriangles m_volumesTopoIndices;
	
	Data< std::string > filename;

public:
	const unsigned BAD_INDEX = 0xffffffff;

	/**
	* Embedding  methods
	*/
	IndexT getVertexIndex(Dart d) { IndexEmb* ie = reinterpret_cast<IndexEmb*>(d->getEmb(0));
									if (ie != NULL) return ie->getVal(); 
									return BAD_INDEX;}

	IndexT getEdgeIndex(Dart d) { IndexEmb* ie = reinterpret_cast<IndexEmb*>(d->getEmb(1));
									if (ie != NULL) return ie->getVal();
									return BAD_INDEX;}

	IndexT getFaceIndex(Dart d) { IndexEmb* ie = reinterpret_cast<IndexEmb*>(d->getEmb(2));
									if (ie != NULL) return ie->getVal();
									return BAD_INDEX;}

	IndexT getVolumeIndex(Dart d) { IndexEmb* ie = reinterpret_cast<IndexEmb*>(d->getEmb(3));
									if (ie != NULL) return ie->getVal();
									return BAD_INDEX;}


	void setVertexEmb(Dart d, IndexEmb* e) { embedOrbit(0,d,0,e);}

	void setEdgeEmb(Dart d, IndexEmb* e) { embedOrbit(1,d,1,e);}

	void setFaceEmb(Dart d, IndexEmb* e) { embedOrbit(2,d,2,e);}

	void setVolumeEmb(Dart d, IndexEmb* e) { embedOrbit(3,d,3,e);}


	void setVertexIndex(Dart d, IndexT i ) {IndexEmb* e = IndexEmb::create(i); embedOrbit(0,d,0,e);}

	void setEdgeIndex(Dart d, IndexT i) {IndexEmb* e = IndexEmb::create(i); embedOrbit(1,d,1,e);}

	void setFaceIndex(Dart d, IndexT i) {IndexEmb* e = IndexEmb::create(i); embedOrbit(2,d,2,e);}

	void setVolumeIndex(Dart d, IndexT i) {IndexEmb* e = IndexEmb::create(i); embedOrbit(3,d,3,e);}


	// constructor
	IndexedMapd3Topology();
	
	virtual void clear();
	
	virtual bool load(const char* filename);
	
	virtual int getNbPoints() const;
	
	// Complete sequence accessors
	
	virtual const SeqEdges& getEdges();
	virtual const SeqTriangles& getTriangles();
	virtual const SeqQuads& getQuads();
	virtual const SeqTetras& getTetras();
	virtual const SeqHexas& getHexas();
	
    // Points accessors (not always available)

	virtual bool hasPos() const;
	virtual double getPX(int i) const;
	virtual double getPY(int i) const;
	virtual double getPZ(int i) const;
	virtual std::string getFilename() const{return filename.getValue();}
	
    void parse(core::objectmodel::BaseObjectDescription* arg)
    {
	if (arg->getAttribute("filename")) {
            filename.setValue( arg->getAttribute("filename") );
            this->load(arg->getAttribute("filename"));
	} 
	arg->removeAttribute("filename");
        this->core::componentmodel::topology::Topology::parse(arg);
    }
};



#endif
