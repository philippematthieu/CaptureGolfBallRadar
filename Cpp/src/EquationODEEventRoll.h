/*
 * EquationODEEventRoll.h
 *
 *  Created on: 28 sept. 2016
 *      Author:
 */

#include "EquationODE.h"

#ifndef EQUATIONODEEVENTROLL_H_
#define EQUATIONODEEVENTROLL_H_

class EquationODEEventRoll  : public EquationODE  {
public:
	EquationODEEventRoll();
	EquationODEEventRoll(std::vector<double> pParametres);
	virtual ~EquationODEEventRoll();
	virtual std::vector<double>  getEvaluation(double s, std::vector<double> q ) ;

private :
	std::vector<double>  qRes;
};

#endif /* EQUATIONODEEVENTROLL_H_ */
