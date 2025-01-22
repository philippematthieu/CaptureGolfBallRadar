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
	double wx			= getParamEq(0);
	double wy			= getParamEq(1);
	double wz			= getParamEq(2);
	double getRayon		= getParamEq(3);
	double getBallArea	= getParamEq(5);
	double getCl1		= getParamEq(6);
	double getMasse		= getParamEq(7);

	double vx 			= q[1];		// = dxdt en m/s
	double vy 			= q[3];		// = dydt en m/s
	double vz 			= q[5];		// = dzdt en m/s
	double getRhoGreen	= getParamEq(4);
	double getG			= getParamEq(8);
	std::vector<double> dQ(q);

	dQ[0] = vx;																		//
	dQ[1] = -(5.0/7.0)*getRhoGreen*getG * vx/sqrt(vx*vx+vy*vy+vz*vz);			// ax longueur
	dQ[2] = vy;																		//
	dQ[3] = -(5.0/7.0)*getRhoGreen*getG * vy/sqrt(vx*vx+vy*vy+vz*vz); 			// ay largeur
	dQ[4] = 0.0 * vz/sqrt(vx*vx+vy*vy+vz*vz);									//
	dQ[5] = 0.0 * vz/sqrt(vx*vx+vy*vy+vz*vz); 									// acceleration hauteur nulle

	return dQ;
} // fin  getEvaluation()
