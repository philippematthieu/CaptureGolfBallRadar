/*
* SolverODE.h
*
*  Created on: 27 sept. 2016
*      Author:
*/

#ifndef SOLVERODE_H_
#define SOLVERODE_H_
#include "EquationODE.h"


class SolverODE {
public:
	SolverODE();
	SolverODE(EquationODE *pEqn, double ps, double pds, std::vector<double>  pq);
	virtual 			~SolverODE();

	double 				getCurrentS() ;
	void 				setCurrentS(double s);

	double 				getQ(int index ) ;
	void 				setQ(double value, int index );

	std::vector<double>	getAllQ();
	void 				setAllQ(std::vector<double> allQ) ;

	virtual EquationODE			*getEquationODE();
	void 				setEquationODE(EquationODE *pEqn);

	bool 				getZeroCrossing() ;
	void 				resetZeroCrossing();
	void 				zeroCrossing(EquationODE *event, double precision, double minTimeStep);
	bool isUnderMinDs();
	void resetUnderMinDs();
	void 				rungeKutta4();

private :
	int numEqns;
	std::vector<double>  q;
	double ds;
	double sCurrent;
	EquationODE *eqn;
	bool bzeroCrossing;
	bool underMinDs;
};

#endif /* SOLVERODE_H_ */
