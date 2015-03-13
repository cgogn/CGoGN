#ifndef SPHERICALFUNCTIONINTEGRATORCARTESIAN_H
#define SPHERICALFUNCTIONINTEGRATORCARTESIAN_H

#include "sphere_lebedev_rule.h"

typedef double (*CartesianFunction)(double x, double y, double z, void* userData);		// Prototype for the function to be evaluated
typedef bool (*CartesianDomain)(double x, double y, double z, void* userData);			// Prototype for the domain definition (true: inside, false: outside)

class SphericalFunctionIntegratorCartesian
{
public:
	SphericalFunctionIntegratorCartesian();
	~SphericalFunctionIntegratorCartesian();

	static const unsigned int maxRuleId = 65;						// Span of rule id (inclusive)
	static inline bool RuleAvailable(unsigned int ruleId);			// States that the quadrature rule is available
	static inline unsigned int RuleOrder(unsigned int ruleId);		// Number of points used in the quadrature
	static inline unsigned int RulePrecision(unsigned int ruleId);	// Max degree of exactly integrated polynomial

	void Init(unsigned int ruleId);									// Rule to use for subsequent integration - allocates quadrature samples and weights
	void Release();													// Release cached informations

	/*
	 *	Integrates a function over a user-specified domain of the full 2D-sphere
	 *
	 *	outIntegral: resulting value
	 *	outArea: area of the integration domain, as specified by the dom function
	 *	f: function to integrate
	 *	userDataFunction: user callback value passed to f during evaluation
	 *	dom: Implicit domain definition (true when inside, false outside)
	 *	userDataDomain: user callback value passed to dom during evaluation
	 *	
	 */
	void Compute(double* outIntegral, double* outArea, CartesianFunction f, void* userDataFunction, CartesianDomain dom, void* userDataDomain) const;

protected:
	unsigned int rId;
	unsigned int rOrder;
	double* quadValues;		// [x_i] then [y_i], then [z_i], then [w_i] values
};

bool SphericalFunctionIntegratorCartesian::RuleAvailable(unsigned int ruleId)
{
	return available_table(ruleId) == 1;
}

unsigned int SphericalFunctionIntegratorCartesian::RuleOrder(unsigned int ruleId)
{
	return order_table(ruleId);
}

unsigned int SphericalFunctionIntegratorCartesian::RulePrecision(unsigned int ruleId)
{
	return precision_table(ruleId);
}

#endif
