#include "SphericalFunctionIntegratorCartesian.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdlib.h>

SphericalFunctionIntegratorCartesian::SphericalFunctionIntegratorCartesian()
{
	quadValues = NULL;
}

SphericalFunctionIntegratorCartesian::~SphericalFunctionIntegratorCartesian()
{
	delete[] quadValues;
}

void SphericalFunctionIntegratorCartesian::Init(unsigned int ruleId)
{
	delete[] quadValues;

	rId = ruleId;
	rOrder = order_table(rId);

	quadValues = new double[4 * rOrder];
	ld_by_order(rOrder, quadValues,
						quadValues + rOrder,
						quadValues + 2 * rOrder,
						quadValues + 3 * rOrder);
}

void SphericalFunctionIntegratorCartesian::Release()
{
	delete[] quadValues;
	quadValues = NULL;
}

void SphericalFunctionIntegratorCartesian::Compute(double* outIntegral, double* outArea, CartesianFunction f, void* userDataFunction, CartesianDomain dom, void* userDataDomain) const
{
	double intVal = 0.0;
	double areaVal = 0.0;

	double* px = quadValues;
	double* py = quadValues + rOrder;
	double* pz = quadValues + 2 * rOrder;
	double* pw = quadValues + 3 * rOrder;

	for(unsigned i = 0 ; i < rOrder ; i++)
	{
		const double x = *px++;
		const double y = *py++;
		const double z = *pz++;
		const double w = *pw++;

		bool valid = dom(x, y, z, userDataDomain);

		if(valid)
		{
			const double fVal = f(x, y, z, userDataFunction);
			intVal += w * fVal;

			areaVal += w;		
		}
	}

	*outIntegral = intVal * 4.0 * M_PI;
	*outArea = areaVal * 4.0 * M_PI;
}
