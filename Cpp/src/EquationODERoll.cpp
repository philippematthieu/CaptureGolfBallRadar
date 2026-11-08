/*
 * EquationODERoll.cpp
 *
 *  Created on: 28 sept. 2016
 *      Author:
 */

#include "EquationODERoll.h"
#include <cmath>

EquationODERoll::EquationODERoll() {
}

EquationODERoll::EquationODERoll(std::vector<double> pParametres) :  EquationODE(pParametres) {
	}

EquationODERoll::~EquationODERoll() {
}

std::vector<double>  EquationODERoll::getEvaluation(double s, std::vector<double>  q ){
	/**
	 * @author
	 * @param
	 */

	double vx 			= q[1];		// = dxdt en m/s
	double vy 			= q[3];		// = dydt en m/s
	double vz 			= q[5];		// = dzdt en m/s
	double getRhoGreen	= getParamEq(4);
	double getG			= getParamEq(8);
	std::vector<double> dQ(q);

	double normeVitesse = sqrt(vx*vx+vy*vy+vz*vz) + 1.0e-8; // garde anti-division par zero (porte depuis Golfball.sci/Python, commit 73ef2d8)
	dQ[0] = vx;																		//
	dQ[1] = -(5.0/7.0)*getRhoGreen*getG * vx/normeVitesse;							// ax longueur
	dQ[2] = vy;																		//
	dQ[3] = -(5.0/7.0)*getRhoGreen*getG * vy/normeVitesse; 							// ay largeur
	dQ[4] = 0.0 * vz/normeVitesse;												//
	dQ[5] = 0.0 * vz/normeVitesse; 													// acceleration hauteur nulle

	return dQ;
} // fin  getEvaluation()
