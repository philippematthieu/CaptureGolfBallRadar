/*
 * EquationODE.h
 *
 *  Created on: 21 sept. 2016
 *  Author:
 */

#ifndef EQUATIONODE_H_
#define EQUATIONODE_H_

#include <vector>
#include <iostream>

class EquationODE {
public:
	EquationODE();
	EquationODE(std::vector<double> pParametres);

	virtual std::vector<double>  getEvaluation(double s, std::vector<double>  q) = 0 ;

	void 	setParamEq(double value, int index);
	double 	getParamEq(int index);
	int 	getSizeParamEq();
	virtual ~EquationODE();

protected:
	std::vector<double> parametres;
};

#endif /* EQUATIONODE_H_ */
/**
 *
 * @author
 *
 */
