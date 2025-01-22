/*
 * EquationODEEventRoll.cpp
 *
 *  Created on: 28 sept. 2016
 *      Author: f009770
 */

#include "EquationODEEventRoll.h"
#include <vector>

using namespace std;

EquationODEEventRoll::EquationODEEventRoll() {
}

EquationODEEventRoll::EquationODEEventRoll(std::vector<double> pParametres):  EquationODE(pParametres), qRes(6,0){
}

EquationODEEventRoll::~EquationODEEventRoll() {
}

std::vector<double>  EquationODEEventRoll::getEvaluation(double s, std::vector<double>  q)  {
		qRes[0] = 0.0;
		qRes[1] = q[1]; // je cherche si VX passe sous 0.01ms;
		qRes[2] = 0.0;
		qRes[3] = 0.0;
		qRes[4] = 0.0;
		qRes[5] = 0.0;
		return qRes;
	} // fin  getEvaluation()
