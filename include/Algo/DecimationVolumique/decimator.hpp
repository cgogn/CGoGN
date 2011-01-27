namespace CGoGN
{

namespace Algo
{

namespace DecimationVolumique
{
template <typename PFP>
void decimate(
	typename PFP::MAP& map, SelectorType s, ApproximatorType a,
	typename PFP::TVEC3& position, double nbWanted//, const FunctorSelect& selected
)
{
	Approximator<PFP>* approximator = NULL ;
	Selector<PFP>* selector = NULL ;

	//choose the Approximator
	switch(a)
	{
		case A_Centroid :
			approximator = new Approximator_Centroid<PFP>(map, position) ;
			break ;

		default :
			std::cout << "not yet implemented" << std::endl;
			break;
	}

	//choose the Selector
	switch(s)
	{
		case S_MapOrder :
			selector = new EdgeSelector_MapOrder<PFP>(map, position) ;
			break ;
		case S_Random :
			selector = new EdgeSelector_Random<PFP>(map, position) ;
			break ;

		default:
			std::cout << "not yet implemented" << std::endl;
			break;
	}

	//Init both
	approximator->init(selector);
	selector->init(approximator);

	//Init nbCell in the mesh
	unsigned int nbCell = map.getNbOrbits(VOLUME_ORBIT);

	std::cout << "nbCell = " << nbCell << std::endl;

	bool finished = false ;
	while(!finished)
	{
		//Next Operator to perform
		Operator<PFP> *op;

		if(!selector->nextOperator()) //a changer de nom
			break;

		 switch(selector->nextOperatorType())
		 {
		  	 case O_CEdge :
		  	 {
		  	 	 op = new CollapseEdgeOperator<PFP>(selector->nextCell(), approximator);
		  	 	 break;
		  	 }
		  	 case O_CFace :
		  	 {
		  		 break;
		  	 }
		  	 case O_CVolume:
		  	 {
		  		 break;
		  	 }

		 }

		//Compute an approximated position in function of the current operator
		approximator->approximate(op);

		//Update the selector before performing operation
		selector->updateBeforeOperation(op);

		//Perform the topological operation and
		//compute the number of cells resulting
		nbCell -= op->perform(map, position);

		//Update the embedded position and
		//search the next operation to perform
		selector->updateAfterOperation(op);

		if(nbCell <= nbWanted)
			finished = true ;

		delete op;
	}

	selector->finish() ;

	delete selector ;
	delete approximator ;
}

} //namespace DecimationVolumique

} //namespace Algo

} //namespace CGoGN
