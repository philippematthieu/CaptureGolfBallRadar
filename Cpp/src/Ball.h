/*
 * Ball.h
 *
 *  Created on: 20 sept. 2016
 *      Author:
 */

//#ifndef M_PI
//#define M_PI 3.14159265358979323846
//#endif

#ifndef BALL_H_
#define BALL_H_
#include <vector>
#include "Club.h"
#include "EquationODEFlight.h"
#include "EquationODEEventFlight.h"
#include "EquationODERoll.h"
#include "EquationODEEventRoll.h"
#include "SolverODE.h"

using namespace std;

class Ball {
public:
	Ball();

	Ball(std::string  pMarque, int pNbAlveoles, int pNbPieces, Club pGolfClub, double pTimeMax, double pdt);
	int getNumEqns();
	std::string getMarque();
	int getNbAlveoles();
	int getNbPieces();
	double getTemperature();
	void setTemperature(double temp);
	double getMasse();
	double getBallArea();
	double getRhoAir();
	double getRhoG();
	double getRayon();
	double getG();
	double getV0BallInitms();
	double getLaunchAngle();
	double getImpactAngle();
	std::vector<double>  getV0Initms();
	std::vector<double>  getVCurrentms();
	void setVCurrentms(double valeur ,int index );
	void setVx(double vx);
	double getVx();
	void setVy(double vy);
	double getVy();
	void setVz(double vz);
	double getVz();
	void setX(double x);
	double getX();
	void setY(double y);
	double getY();
	void setZ(double z);
	double getZ();
	double getSpinX();
	void setSpinX(double wx);
	double getSpinY();
	double getSpinYOrgrpm();
	void setSpinY(double wy);
	double getSpinZ();
	double getSpinZOrgrpm();
	void setSpinZ(double wz);
	double getTimeMax();
	double getdt();
	double getIndexChute();
	double getTempsTotal();
	std::vector< std::vector<double> >  getMatriceFlight();
	double getMaxHeight();
	void runSimu();

	virtual ~Ball();

private :
	std::string			marque; 			// marque de la balle de golf
	double				masse; 				// masse de la ball de golf
	int					nbAlveoles;	 		// nombre d'alveole de la balle de golf
	int					nbPieces; 			// nb de pieces de la bale de golf
	double				area; 				// aire de la balle de golf
	double				temperature;	 	// Temperature de l'aire en °C
	double				rayon; 				// rayon de la balle de golf
	double				rhoAir; 			// densite de l'aire
	double				g; 					// acceleration terrestre en m/s2
	double				rhoGreen;			// densite de green
	double				launchAngle;		// angle de decollage de la balle en radian
	std::vector<double> v0Initms; 			// {x, vx, y, vy, z, vy } = q init
	std::vector<double> v0Currentms; 		// {x, vx, y, vy, z, vy } = q courrant
	double				v0BallInitms;		// Vitesse initiale de la balle
	int					numEqns;			// nombre d'equations
	double				timeMax;			// temps max de l'ODE
	double				dt;					// pas de temps
	EquationODEFlight	*eqnVolBalle;
	SolverODE			solveFlight;
	SolverODE			solveRoll;
	EquationODERoll		*eqnRoulBalle;
	EquationODEEventFlight 	*eventFlight;
	EquationODEEventRoll 	*eventRoll;
	double				alphaClubPath;
	double 				verticalLand;
	std::vector<double> paramEqn;// wx, wy,wz, getRayon, getRhoAir, getBallArea, getCl1, getMasse, getG
	std::vector< std::vector<double> > matriceFlight; // la dimension est faite par le .clone() plus loin
	int 				indexChute;
	double 				spinYOrgrpm, spinZOrgrpm;
	double 				tempsTotal;
	double 				maxHeight;
};

#endif /* BALL_H_ */
