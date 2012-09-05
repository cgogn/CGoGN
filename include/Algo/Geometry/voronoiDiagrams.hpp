namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

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
	border.clear();
	regions.setAllValues(0);
	border.clear();
	seeds.clear();
	front.clear();
	vmReached.unmarkAll();
}

template <typename PFP>
void VoronoiDiagram<PFP>::setSeeds (const std::vector<Dart>& s)
{
	clear();
	seeds = s;
}

template <typename PFP>
void VoronoiDiagram<PFP>::setRandomSeeds (unsigned int nseeds)
{
	clear();

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
	vmReached.unmarkAll();
	for (unsigned int i = 0; i < seeds.size(); i++)
	{
		Dart d = seeds[i];
		vmReached.mark(d);
		vertexInfo[d].it = front.insert(std::pair<float,Dart>(0.0, d));
		vertexInfo[d].valid = true;
//		vertexInfo[d].region = i;
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
	front.erase(vertexInfo[e].it);
	vertexInfo[e].valid=false;
//	regions[e] = vertexInfo[e].region;
	regions[e] = regions[vertexInfo[e].pathOrigin];
}

template <typename PFP>
void VoronoiDiagram<PFP>::addVertexToFront(Dart f, float d){
	VertexInfo& vi (vertexInfo[f]);
	vi.it = front.insert(std::pair<float,Dart>(d + edgeCost[f], f));
	vi.valid=true;
//	vi.region = regions[map.phi2(f)];
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
//		vi.region = regions[map.phi2(f)];
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

}// end namespace Geometry
}// end namespace Algo
}// end namespace CGoGN
