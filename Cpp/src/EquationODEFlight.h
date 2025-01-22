/*
 * EquationODEFlight.h
 *
 *  Created on: 21 sept. 2016
 *  Author:
 */

#include "EquationODE.h"

#ifndef EQUATIONODEFLIGHT_H_
#define EQUATIONODEFLIGHT_H_

class EquationODEFlight  : public EquationODE {
public:
	EquationODEFlight();
	EquationODEFlight(std::vector<double> pParametres);
	virtual ~EquationODEFlight();
	virtual std::vector<double>  getEvaluation(double s, std::vector<double>  q ) ;
};

#endif /* EQUATIONODEFLIGHT_H_ */
