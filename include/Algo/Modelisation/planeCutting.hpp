namespace CGoGN
{

namespace Algo
{

namespace Surface
{

namespace Modelisation
{

/*
template <typename PFP>
void planeCut(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3>& position, const Geom::Plane3D<typename PFP::REAL>& plane,
			  CellMarker<FACE>& cmf_over, bool keepTriangles=false, bool with_unsew = true)
{
	typedef typename PFP::REAL REAL;

	//marker for vertices on the plane
	CellMarker<VERTEX> cmv(map);

	// marker for vertices over the plane
	CellMarker<VERTEX> cmv_over(map);


	TraversorV<typename PFP::MAP> traVert(map);
	for (Dart d=traVert.begin(); d!=traVert.end();d=traVert.next())
	{
		Geom::Orientation3D or1 = plane.orient(position[d]);
		if (or1 == Geom::ON)
			cmv.mark(d);
		if (or1 == Geom::OVER)
			cmv_over.mark(d);
	}



	TraversorE<typename PFP::MAP> traEdg(map);
	for (Dart d=traEdg.begin(); d!=traEdg.end();d=traEdg.next())
	{
		Dart dd = map.phi1(d);


		if (!cmv.isMarked(d) && !cmv.isMarked(dd) && (cmv_over.isMarked(d) != cmv_over.isMarked(dd)))
		{
			Dart x = map.cutEdge(d);
			REAL dist1 = plane.distance(position[d]);
			REAL dist2 = plane.distance(position[dd]);

			if (dist1<0.0)
				dist1 = -dist1;
			if (dist2<0.0)			// no abs() to avoid type problem with REAL template
				dist2 = -dist2;

			position[x] = (position[d]*dist2 + position[dd]*dist1)/(dist1+dist2);

			traEdg.skip(x);
			traEdg.skip(map.phi_1(x));
			cmv.mark(x);
		}
	}

	Algo::Surface::Modelisation::EarTriangulation<PFP>* triangulator=NULL;
	if (keepTriangles)	// triangule faces if needed
	{
		triangulator = new Algo::Surface::Modelisation::EarTriangulation<PFP>(map);
	}

	TraversorF<typename PFP::MAP> traFac(map);
	for (Dart d=traFac.begin(); d!=traFac.end();d=traFac.next())
	{
		// turn in the face to search if there are 2 vertices marked as on the plane
		Traversor2FV<typename PFP::MAP> traV(map,d);
		Dart e=traV.begin();
		while ((e!=traV.end())&&(!cmv.isMarked(e)))
			e=traV.next();

		Dart V1=NIL;
		if (e!=traV.end())
			V1 = e;

		e=traV.next();
		while ((e!=traV.end())&&(!cmv.isMarked(e)))
			e=traV.next();

		Dart V2=NIL;
		if (e!=traV.end())
			V2 = e;

		// is there 2 vertices in the plane (but not consecutive)
		if ((V1!=NIL) && (V2!=NIL) && (V2!=map.phi1(V1)) && (V1!=map.phi1(V2)))
		{
			map.splitFace(V1,V2);
			if (with_unsew)
				map.unsewFaces(map.phi_1(V1)); // ne marche pas !

			// ensure to not scan this two new faces
			traFac.skip(V1);
			traFac.skip(V2);
			// mark face of V1 if it is over
			if (cmv_over.isMarked(map.phi1(V1)))
				cmf_over.mark(V1);
			// mark face of V2 if it is over
			if (cmv_over.isMarked(map.phi1(V2)))
				cmf_over.mark(V2);

			if (keepTriangles)	// triangule faces if needed
			{
				triangulator->trianguleFace(V1);
				triangulator->trianguleFace(V2);
			}
		}
		else
		{
			// find the first vertex not on the plane
			Dart e = d;
			while (cmv.isMarked(e))
				e = map.phi1(e);
			// face is all on same side than vertex
			if (cmv_over.isMarked(e))
				cmf_over.mark(e);
		}
	}

	if (triangulator != NULL)
		delete triangulator;
}
*/

template <typename PFP>
void planeCut(typename PFP::MAP& map, VertexAttribute<typename PFP::VEC3>& position, const Geom::Plane3D<typename PFP::REAL>& plane,
			  CellMarker<FACE>& cmf_over, bool keepTriangles=false, bool with_unsew = true)
{
	typedef typename PFP::REAL REAL;

	//marker for vertices on the plane
	CellMarker<VERTEX> cmv(map);
	// marker for vertices over the plane
	CellMarker<VERTEX> cmv_over(map);

	TraversorE<typename PFP::MAP> traEdg(map);
	for (Dart d=traEdg.begin(); d!=traEdg.end();d=traEdg.next())
	{
		Dart dd = map.phi1(d);

		REAL dist1;
		REAL dist2;

		Geom::Orientation3D or1 = plane.orient(position[d],dist1);
		Geom::Orientation3D or2 = plane.orient(position[dd],dist2);

		if (or1 == Geom::ON)
			cmv.mark(d);

		if (or2 == Geom::ON)
			cmv.mark(dd);

		if ((or1!=Geom::ON) && (or2!=Geom::ON) && (or1 != or2))
		{
			Dart x = map.cutEdge(d);

			if (dist1<0.0)
				dist1 = -dist1;
			if (dist2<0.0)			// no abs() to avoid type problem with REAL template
				dist2 = -dist2;

			position[x] = (position[d]*dist2 + position[dd]*dist1)/(dist1+dist2);

			traEdg.skip(x);
			traEdg.skip(map.phi_1(x));
			cmv.mark(x);

			if (or1 == Geom::OVER)
				cmv_over.mark(d);
			else
				cmv_over.mark(dd);
		}
		else
		{
			if (or1 == Geom::OVER)
			{
				cmv_over.mark(d);
				cmv_over.mark(dd);
			}
		}
	}

	Algo::Surface::Modelisation::EarTriangulation<PFP>* triangulator=NULL;
	if (keepTriangles)	// triangule faces if needed
	{
		triangulator = new Algo::Surface::Modelisation::EarTriangulation<PFP>(map);
	}

	TraversorF<typename PFP::MAP> traFac(map);
	for (Dart d=traFac.begin(); d!=traFac.end();d=traFac.next())
	{
		// turn in the face to search if there are 2 vertices marked as on the plane
		Traversor2FV<typename PFP::MAP> traV(map,d);
		Dart e=traV.begin();
		while ((e!=traV.end())&&(!cmv.isMarked(e)))
			e=traV.next();

		Dart V1=NIL;
		if (e!=traV.end())
			V1 = e;

		e=traV.next();
		while ((e!=traV.end())&&(!cmv.isMarked(e)))
			e=traV.next();

		Dart V2=NIL;
		if (e!=traV.end())
			V2 = e;

		// is there 2 vertices in the plane (but not consecutive)
		if ((V1!=NIL) && (V2!=NIL) && (V2!=map.phi1(V1)) && (V1!=map.phi1(V2)))
		{
			map.splitFace(V1,V2);
			if (with_unsew)
				map.unsewFaces(map.phi_1(V1)); // ne marche pas !

			// ensure to not scan this two new faces
			traFac.skip(V1);
			traFac.skip(V2);
			// mark face of V1 if it is over
			if (cmv_over.isMarked(map.phi1(V1)))
				cmf_over.mark(V1);
			// mark face of V2 if it is over
			if (cmv_over.isMarked(map.phi1(V2)))
				cmf_over.mark(V2);

			if (keepTriangles)	// triangule faces if needed
			{
				triangulator->trianguleFace(V1);
				triangulator->trianguleFace(V2);
			}
		}
		else
		{
			// find the first vertex not on the plane
			Dart e = d;
			while (cmv.isMarked(e))
				e = map.phi1(e);
			// face is all on same side than vertex
			if (cmv_over.isMarked(e))
				cmf_over.mark(e);
		}
	}

	if (triangulator != NULL)
		delete triangulator;
}


} // namespace Modelisation

} // namespace Surface

} // namespace Algo

} // namespace CGoGN
