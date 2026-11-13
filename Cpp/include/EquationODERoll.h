/*
 * EquationODERoll.h
 *
 *  Created on: 28 sept. 2016
 *      Author:
 */

#include "EquationODE.h"

#ifndef EQUATIONODEROLL_H_
#define EQUATIONODEROLL_H_

class EquationODERoll  : public EquationODE  {
public:
	EquationODERoll();
	EquationODERoll(std::vector<double> pParametres);
	virtual ~EquationODERoll();
	virtual std::vector<double>  getEvaluation(double s, std::vector<double> q ) ;
};

#endif /* EQUATIONODEROLL_H_ */
