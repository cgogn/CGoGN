namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

/***********************************************************
 * class VoronoiDiagram
 ***********************************************************/

template <typename PFP>
VoronoiDiagram<PFP>::VoronoiDiagram (typename PFP::MAP& m, const EdgeAttribute<REAL>& p, VertexAttribute<unsigned int>& r) : map(m), edgeCost (p), regions (r), vmReached(m)
{
	vertexInfo = map.template addAttribute<VertexInfo, VERTEX>("vertexInfo");
}

template <typename PFP>
VoronoiDiagram<PFP>::~VoronoiDiagram ()
{
	map.removeAttribute(vertexInfo);
}

template <typename PFP>
void VoronoiDiagram<PFP>::clear ()
{
	regions.setAllValues(0);
	border.clear();
	front.clear();
	vmReached.unmarkAll();
}

template <typename PFP>
void VoronoiDiagram<PFP>::setSeeds (const std::vector<Dart>& s)
{
	seeds.clear();
//	clear();
	seeds = s;
}

template <typename PFP>
void VoronoiDiagram<PFP>::setRandomSeeds (unsigned int nseeds)
{
	seeds.clear();
//	clear();
	vmReached.unmarkAll();

	srand ( time(NULL) );
	unsigned int n = nseeds;
	while (n > 0)
	{ // TODO : correct this random init which assumes contiguous Dart table
		Dart d = rand() % map.getNbDarts() ;
		if (! vmReached.isMarked(d))
		{
			vmReached.mark(d);
			seeds.push_back(d);
			n--;
		}
	}
}

template <typename PFP>
void VoronoiDiagram<PFP>::initFrontWithSeeds ()
{
//	vmReached.unmarkAll();
	clear();
	for (unsigned int i = 0; i < seeds.size(); i++)
	{
		Dart d = seeds[i];
		vmReached.mark(d);
		vertexInfo[d].it = front.insert(std::pair<float,Dart>(0.0, d));
		vertexInfo[d].valid = true;
		regions[d] = i;
		vertexInfo[d].pathOrigin = d;
	}
}

template <typename PFP>
void VoronoiDiagram<PFP>::setCost (const EdgeAttribute<typename PFP::REAL>& c){
	edgeCost = c;
}

template <typename PFP>
void VoronoiDiagram<PFP>::collectVertexFromFront(Dart e){
	regions[e] = regions[vertexInfo[e].pathOrigin];
	front.erase(vertexInfo[e].it);
	vertexInfo[e].valid=false;
}

template <typename PFP>
void VoronoiDiagram<PFP>::addVertexToFront(Dart f, float d){
	VertexInfo& vi (vertexInfo[f]);
	vi.it = front.insert(std::pair<float,Dart>(d + edgeCost[f], f));
	vi.valid=true;
	vi.pathOrigin = map.phi2(f);
	vmReached.mark(f);
}

template <typename PFP>
void VoronoiDiagram<PFP>::updateVertexInFront(Dart f, float d){
	VertexInfo& vi (vertexInfo[f]);
	float dist = d + edgeCost[f];
	if (dist < vi.it->first)
	{
		front.erase(vi.it);
		vi.it = front.insert(std::pair<float,Dart>(dist, f));
		vi.pathOrigin = map.phi2(f);
	}
}

template <typename PFP>
void VoronoiDiagram<PFP>::computeDiagram ()
{
	initFrontWithSeeds();

	while ( !front.empty() )
	{
		Dart e = front.begin()->second;
		float d = front.begin()->first;

		collectVertexFromFront(e);

		Traversor2VVaE<typename PFP::MAP> tv (map, e);
		for (Dart f = tv.begin(); f != tv.end(); f=tv.next())
		{
			if (vmReached.isMarked(f))
			{ // f has been reached
				if (vertexInfo[f].valid) // f is in the front : update
					updateVertexInFront(f,d);
				else // f is not in the front any more (already collected) : detect a border edge
					if ( regions[f] != regions[e] ) border.push_back(f);
			}
			else
			{ // f has not been reached : add it to the front
				addVertexToFront(f,d);
			}
		}
	}
}

template <typename PFP>
void VoronoiDiagram<PFP>::computeDistancesWithinRegion (Dart seed)
{ // TODO : this algo has not yet been tested
	// init
	front.clear();
	vmReached.unmarkAll();

	vmReached.mark(seed);
	vertexInfo[seed].it = front.insert(std::pair<float,Dart>(0.0, seed));
	vertexInfo[seed].valid = true;
	vertexInfo[seed].pathOrigin = seed;

	//compute
	while ( !front.empty() )
	{
		Dart e = front.begin()->second;
		float d = front.begin()->first;

		collectVertexFromFront(e);

		Traversor2VVaE<typename PFP::MAP> tv (map, e);
		for (Dart f = tv.begin(); f != tv.end(); f=tv.next())
		{
			if (vmReached.isMarked(f))
			{ // f has been reached
				if (vertexInfo[f].valid) updateVertexInFront(f,d); // f is in the front : update
			}
			else
			{ // f has not been reached : add it to the front
				if ( regions[f] == regions[e] ) addVertexToFront(f,d);
			}
		}
	}
}

/***********************************************************
 * class CentroidalVoronoiDiagram
 ***********************************************************/

template <typename PFP>
CentroidalVoronoiDiagram<PFP>::CentroidalVoronoiDiagram (typename PFP::MAP& m, const EdgeAttribute<REAL>& c, VertexAttribute<unsigned int>& r, VertexAttribute<REAL>& d, VertexAttribute<Dart>& o, VertexAttribute<REAL>& a) :
	VoronoiDiagram<PFP>(m,c,r), distances(d), pathOrigins(o), areaElts(a)
{
}

template <typename PFP>
CentroidalVoronoiDiagram<PFP>::~CentroidalVoronoiDiagram ()
{
}

template <typename PFP>
void CentroidalVoronoiDiagram<PFP>::clear ()
{
	VoronoiDiagram<PFP>::clear();
	distances.setAllValues(0.0);
}

template <typename PFP>
void CentroidalVoronoiDiagram<PFP>::collectVertexFromFront(Dart e){
	distances[e] = this->vertexInfo[e].it->first;
	pathOrigins[e] = this->vertexInfo[e].pathOrigin;

	VoronoiDiagram<PFP>::collectVertexFromFront(e);
}



template <typename PFP>
void CentroidalVoronoiDiagram<PFP>::cumulateEnergyOnPaths(){
	globalEnergy = 0.0;
	for (unsigned int i = 0; i < this->seeds.size(); i++)
	{
		cumulateEnergyFromRoot(this->seeds[i]);
		globalEnergy += distances[this->seeds[i]];
	}
}

template <typename PFP>
unsigned int CentroidalVoronoiDiagram<PFP>::moveSeeds(){
	unsigned int m = 0;
	globalEnergy = 0.0;
	for (unsigned int i = 0; i < this->seeds.size(); i++)
	{
		Dart oldSeed = this->seeds[i];
		m += moveSeed(i);
		globalEnergy += distances[oldSeed];
	}
	return m;
}

template <typename PFP>
unsigned int CentroidalVoronoiDiagram<PFP>::moveSeeds2(){
	unsigned int m = 0;
	globalEnergy = 0.0;
	for (unsigned int i = 0; i < this->seeds.size(); i++)
	{
		Dart oldSeed = this->seeds[i];
		int r = moveSeed(i);
		Dart newSeed = this->seeds[i];
		globalEnergy += distances[oldSeed];
		REAL regionEnergy = distances[oldSeed];
		if (r==1)
		{
			this->computeDistancesWithinRegion(newSeed);
			cumulateEnergyFromRoot(newSeed);
			if (distances[newSeed] < regionEnergy)
				m+=1;
			else
				this->seeds[i] = oldSeed;
		}
	}
	return m;
}

template <typename PFP>
typename PFP::REAL CentroidalVoronoiDiagram<PFP>::cumulateEnergyFromRoot(Dart e){
	REAL distArea = areaElts[e] * distances[e];
	distances[e] = areaElts[e] * distances[e] * distances[e];

	Traversor2VVaE<typename PFP::MAP> tv (this->map, e);
	for (Dart f = tv.begin(); f != tv.end(); f=tv.next())
	{
		if ( pathOrigins[f] == this->map.phi2(f))
		{
			distArea += cumulateEnergyFromRoot(f);
			distances[e] += distances[f];
		}
	}
	return distArea;
}

template <typename PFP>
unsigned int CentroidalVoronoiDiagram<PFP>::moveSeed(unsigned int numSeed){
	Dart e = this->seeds[numSeed];
	unsigned int seedMoved = 0;

	std::vector<Dart> v;
	v.reserve(8);

	std::vector<float> da;
	da.reserve(8);

	distances[e] = 0.0;

	Traversor2VVaE<typename PFP::MAP> tv (this->map, e);
	for (Dart f = tv.begin(); f != tv.end(); f=tv.next())
	{
		if ( pathOrigins[f] == this->map.phi2(f))
		{
			float distArea = cumulateEnergyFromRoot(f);
			da.push_back(distArea);
			distances[e] += distances[f];
			v.push_back(f);
		}
	}

	/* second attempt */
	// TODO : check if the computation of grad and proj is still valid for other edgeCost than geodesic distances
	VEC3 grad (0.0);
	const VertexAttribute<VEC3>& pos = this->map.template getAttribute<VEC3,VERTEX>("position");

	// compute the gradient
	for (unsigned int j = 0; j<v.size(); ++j)
	{
		Dart f = v[j];
		VEC3 edgeV = pos[f] - pos[this->map.phi2(f)];
		edgeV.normalize();
		grad += da[j] * edgeV;
	}
	grad /= 2.0;

	float maxProj = 0.0;
//	float memoForTest = 0.0;
	for (unsigned int j = 0; j<v.size(); ++j)
	{
		Dart f = v[j];
		VEC3 edgeV = pos[f] - pos[this->map.phi2(f)];
//		edgeV.normalize();
		float proj = edgeV * grad;
//		proj -= areaElts[e] * this->edgeCost[f] * this->edgeCost[f];
		if (proj > maxProj)
		{
//			if (numSeed==1) memoForTest = (edgeV * grad) / (areaElts[e] * this->edgeCost[f] * this->edgeCost[f]);
//				CGoGNout << (edgeV * grad) / (areaElts[e] * this->edgeCost[f] * this->edgeCost[f]) * this->seeds.size() << CGoGNendl;
//				CGoGNout << edgeV * grad << "\t - \t" << areaElts[e] * this->edgeCost[f] * this->edgeCost[f] << CGoGNendl;
			maxProj = proj;
			seedMoved = 1;
			this->seeds[numSeed] = v[j];
		}
	}

//	if (numSeed==1 && seedMoved==1)
//		CGoGNout << memoForTest * this->seeds.size() << CGoGNendl;

	/* end second attempt */

/* first attempt by approximating the energy change
	float minDE = 0.0;
	for (unsigned int j = 0; j<v.size(); ++j)
	{
		float c = this->edgeCost[v[j]];
		float de = regionArea * c * c;
		de += 2 * c *( distances[e] - 2*distances[v[j]] );
		if (de < minDE)
		{
			minDE = de;
			seedMoved = 1;
			this->seeds[numSeed] = v[j];
		}
	}
*/
	return seedMoved;
}

}// end namespace Geometry
}// end namespace Algo
}// end namespace CGoGN
