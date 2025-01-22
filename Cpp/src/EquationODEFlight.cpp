/*
 * EquationODEFlight.cpp
 *
 *  Created on: 21 sept. 2016
 *      Author:
 */

#include "EquationODEFlight.h"
#include <cmath>

EquationODEFlight::EquationODEFlight(){
}

EquationODEFlight::EquationODEFlight(std::vector<double> pParametres):  EquationODE(pParametres){
}

EquationODEFlight::~EquationODEFlight(){
	}

std::vector<double>  EquationODEFlight::getEvaluation(double s, std::vector<double>  q ) {
	/**
	 * @author
	 * @param
	 */
	double normeVitesseIn;	// en m/s
	double normeSpinIn; 	// en rad/s
	double nbReynolds;		//
	double dragCoeff;		//
	double liftCoeff;		//
	double dragForce;		//
	double dragForceX;		// sur la Longueur
	double dragForceY;		// sur la hauteur
	double dragForceZ;		// sur la Largeur

	double liftForce;		//
	double liftForceX;		// sur la Longueur
	double liftForceY;		// sur la Hauteur
	double liftForceZ;		// sur la Largeur

	double vx 			= q[1];		// = dxdt en m/s
	double vy 			= q[3];		// = dydt en m/s
	double vz 			= q[5];		// = dzdt en m/s

	double wx			= getParamEq(0);
	double wy			= getParamEq(1);
	double wz			= getParamEq(2) ;
	double getRayon		= getParamEq(3);
	double getRhoAir	= getParamEq(4);
	double getBallArea	= getParamEq(5);
	double getCl1		= getParamEq(6);
	double getMasse		= getParamEq(7);
	double getG			= getParamEq(8);

	std::vector<double> dQ(q);

	normeVitesseIn	= sqrt(vx*vx + vy*vy + vz*vz) + 1.0e-8;		// calcul de la norme de la vitesse de balle instantanee
	normeSpinIn		= sqrt(wx*wx + wy*wy + wz*wz);					// calcul de la norme du Spin de balle instantanee
	nbReynolds		= normeVitesseIn * (getRayon*2) / 1.5e-5;

	dragCoeff		= 0.36 + 24/nbReynolds + 6/(1+sqrt(nbReynolds)); 										// relation de White de Cm
	dragForce 		= 0.5 * getRhoAir * getBallArea * dragCoeff * normeVitesseIn * normeVitesseIn; 						// calcul de la force de resistance de l'air.

	dragForceX		= -(dragForce)*vx / normeVitesseIn ;      													// Drag Force sur Longueur
	dragForceY		= -(dragForce)*vy / normeVitesseIn ;  														// Drag Force sur Hauteur
	dragForceZ		= -(dragForce)*vz / normeVitesseIn ;      													// Drag Force sur largeur

	liftCoeff		= getCl1 * getRayon * std::abs(pow(normeSpinIn  / normeVitesseIn, 0.4));					// Cl Smits and Smiths ""A new aerodynamic model of a golf ball in flight 1994"
	liftForce		= 0.5 * getRhoAir * getBallArea * getRayon * liftCoeff   * normeVitesseIn * normeVitesseIn;		// calcul de la force de Magnus de l'air.

	liftForceX 	= - liftForce * (wy*vz - wz*vy) / normeVitesseIn  ;
	liftForceY 	= - liftForce * (wz*vx - wx*vz) / normeVitesseIn  ;
	liftForceZ	= - liftForce * (wx*vy - wy*vx) / normeVitesseIn  ;

	dQ[0] = vx;
	dQ[1] = (dragForceX + liftForceX)/getMasse;				// vx longueur
	dQ[2] = vy;
	dQ[3] = (dragForceY + liftForceY)/getMasse; 			// vy largeur
	dQ[4] = vz;
	dQ[5] = - getG + (dragForceZ + liftForceZ) /getMasse; 	// vz hauteur

	return dQ;
} // fin  getEvaluation()

