/*
 * Ball.cpp
 *
 *  Created on: 20 sept. 2016
 *      Author:
 */

#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator

#include "Club.h"
#include "EquationODEFlight.h"
#include "EquationODEEventFlight.h"
#include "EquationODERoll.h"
#include "EquationODEEventRoll.h"
#include "Ball.h"
#include "SolverODE.h"

using namespace std;

Ball::Ball() {}
Ball::~Ball() {}
Ball::Ball(string pMarque, int pNbAlveoles, int pNbPieces, Club pGolfClub, double pTimeMax, double pdt):
													marque(pMarque),	nbAlveoles(pNbAlveoles),	nbPieces(pNbPieces),
													temperature(pGolfClub.getTemperature()),	masse(0.04545),	rayon(0.0215),
													area(M_PI*rayon*rayon),	rhoAir(1.292*273.15/(temperature + 273)),	rhoGreen(0.131),
													g(9.81),	timeMax(pTimeMax),	dt(pdt),	alphaClubPath(pGolfClub.getAlphaClubPathRadian()),
													v0Initms(6),paramEqn(9,0)
{
	numEqns = 6;
	verticalLand = 0.0;
	//std::vector<double> paramEqn(9);// wx, wy,wz, getRayon, getRhoAir, getBallArea, getCl1, getMasse, getG
	//std::vector<double> matriceFlight ; // la dimension est faite par le .clone() plus loin
	area = M_PI*rayon*rayon;
	indexChute = 0;
	Club clubToTest = pGolfClub;

	double v0Ballbfnms =  clubToTest.getClubV0ms() * cos(clubToTest.getDynamiqueLoftRadian())*(1.0 + clubToTest.getEcoeff())/(1+masse/clubToTest.getPoids()) ;// Vitesse longitudinale dans le referentiel de decollage apres impact (ref: The physics of golf: The optimum loft of a driverA. Raymond Penner)
	double v0Ballbfpms = -clubToTest.getClubV0ms() * sin(clubToTest.getDynamiqueLoftRadian())/(1.0 + masse/clubToTest.getPoids() + 2.5);     // Vitesse perpendiculaire dans le referentiel de decollage apres impact

	// Calcule de l'angle de decollage de la balle
	launchAngle		= clubToTest.getDynamiqueLoftRadian() + atan(v0Ballbfpms/v0Ballbfnms);    // angle de decollage de la balle

	// Calcule de la vitesse initial de la ball
	v0BallInitms	= sqrt(v0Ballbfnms*v0Ballbfnms + v0Ballbfpms*v0Ballbfpms)* (1.0 - 0.3556 * clubToTest.getMiss()); // Vitesse de decollage de la balle
	v0Initms[1] 	=  v0BallInitms * cos(launchAngle) * cos(0.0);     // Vx
	v0Initms[3] 	= -v0BallInitms * cos(launchAngle) * sin(0.0);     // Vy
	v0Initms[5] 	=  v0BallInitms * sin(launchAngle);                // Vz

	// initialisation de la position
	v0Initms[0] = 0.0;	// X = dVx.dt
	v0Initms[2] = 0.0;	// Y = dVy.dt
	v0Initms[4] = 0.0;	// Z = dVz.dt
	v0Currentms = v0Initms; // copie des valeurs initiales v0Initms

	// Calcul des SPINs qui sont des parametre de l'ODE
	paramEqn[0] = 0; 																									// wx Le SPIN sur l'axe X n'est pas une composante dans l'axe du chemin de club. Le spin est appliqu� sur le plan du sol
	paramEqn[1] = v0Initms[1] * sin(clubToTest.getDynamiqueLoftRadian())    * clubToTest.getCoeffBackSpin() / 9.54929659643;	// wy Spin dans l'axe largeur backspin
	paramEqn[2] = v0Initms[5] * sin(clubToTest.getGamaFacePathRadian()) * clubToTest.getCoeffSpinLift() / 9.54929659643;	// wz Spin dans l'axe hauteur lift
	spinYOrgrpm = paramEqn[1]*9.54929659643; // sauvegarde des Spin d'origine
	spinZOrgrpm = paramEqn[2]*9.54929659643;

	paramEqn[3] = getRayon();
	paramEqn[4] = getRhoAir();
	paramEqn[5] = getBallArea();
	paramEqn[6] = clubToTest.getCl1();
	paramEqn[7] = getMasse();
	paramEqn[8] = getG();

	cout << "RhoAir: " << getRhoAir() << endl;
	cout << "BallAreaSection: " << getBallArea() << endl;
	cout << "Cl1: " << clubToTest.getCl1() << endl;
	cout << "Masse: " << getMasse() << endl;
	cout << "Rayon: " << getRayon() << endl;
	cout << "G: " << getG() << endl;
	cout << "paramEqn Size: " << paramEqn.size() << endl;

	// declaration des instances pour le vol de la balle
	eqnVolBalle	= new EquationODEFlight(paramEqn);
	eventFlight = new EquationODEEventFlight(paramEqn);
	solveFlight = SolverODE(eqnVolBalle, 0.0, getdt(), v0Initms);

	// declaration des instances de roullage de la balle
	eqnRoulBalle= new EquationODERoll(paramEqn);
	eventRoll	= new EquationODEEventRoll(paramEqn);
	solveRoll 	= SolverODE(eqnRoulBalle, 0.0, getdt(), v0Initms);

	unsigned i=0;
	cout << "getV0Initms: "  << v0Initms[0] << " ; " <<3.6* v0Initms[1]   << " ; " << v0Initms[2]   << " ; " << 3.6*v0Initms[3]   << " ; " << v0Initms[4] <<" ; " << 3.6*v0Initms[5]   << endl;
}
// methodes pour constantes et variables du systeme
int Ball::getNumEqns() {
	return numEqns;
}
string Ball::getMarque() {
	return marque;
}
int Ball::getNbAlveoles() {
	return nbAlveoles;
}
int Ball::getNbPieces() {
	return nbPieces;
}
double Ball::getTemperature() {
	return temperature;
}
void Ball::setTemperature(double temp) {
	temperature = temp;
}
double Ball::getMasse() {
	return masse;
}
double Ball::getBallArea() {
	return area;
}
double Ball::getRhoAir() {
	return rhoAir;
}
double Ball::getRhoG() {
	return rhoGreen;
}
double Ball::getRayon() {
	return rayon;
}
double Ball::getG() {
	return g;
}
double Ball::getV0BallInitms() {
	return v0BallInitms;
}
double Ball::getLaunchAngle() {
	return launchAngle;
}
double Ball::getImpactAngle() {
	return verticalLand;
}
std::vector<double>  Ball::getV0Initms() {
	return v0Initms;
}
std::vector<double>  Ball::getVCurrentms() {
	return v0Currentms;
}
void Ball::setVCurrentms(double valeur ,int index ) {
	v0Currentms[index] = valeur;
	return;
}
void Ball::setVx(double vx) {
	v0Currentms[1] = vx;
	return;
}
double Ball::getVx() {
	return v0Currentms[1];
}
void Ball::setVy(double vy) {
	v0Currentms[3] = vy;
	return;
}
double Ball::getVy() {
	return v0Currentms[3];
}
void Ball::setVz(double vz) {
	v0Currentms[5] = vz;
	return;
}
double Ball::getVz() {
	return v0Currentms[5];
}
void Ball::setX(double x) {
	v0Currentms[0] = x;
	return;
}
double Ball::getX() {
	return v0Currentms[0];
}
void Ball::setY(double y) {
	v0Currentms[2] = y;
	return;
}
double Ball::getY() {
	return v0Currentms[2];
}
void Ball::setZ(double z) {
	v0Currentms[4] = z;
	return;
}
double Ball::getZ() {
	return v0Currentms[4];
}
double Ball::getSpinX() {
	return paramEqn[0];
}
void Ball::setSpinX(double wx) {
	paramEqn[0] = wx;
	return;
}
double Ball::getSpinY() {
	return paramEqn[1];
}
double Ball::getSpinYOrgrpm() {
	return spinYOrgrpm;
}
void Ball::setSpinY(double wy) {
	paramEqn[1] = wy;
	return;
}
double Ball::getSpinZ() {
	return paramEqn[2];
}
double Ball::getSpinZOrgrpm() {
	return spinZOrgrpm;
}
void Ball::setSpinZ(double wz) {
	paramEqn[2] = wz;
	return;
}
double Ball::getTimeMax() {
	return timeMax;
}
double Ball::getdt() {
	return dt;
}
double Ball::getIndexChute() {
	return indexChute-1;
}
double Ball::getTempsTotal() {
	return tempsTotal;
}
std::vector< std::vector<double> >  Ball::getMatriceFlight()
{
	return matriceFlight;
}
double Ball::getMaxHeight()
{
	return maxHeight;
}

/**
 * Debut de runSimu()
 * author Matthieu PHILIPPE
 **/
void Ball::runSimu() {
	std::vector< std::vector<double> > matrice; // la dimension est faite par le .clone() plus loin
	double vi, thetaRebond, vixprime, vrxprime, vizprime, vrzprime, eBall, muFrictionCritic, wr, vr, vrx, vry, vrz, impactAngle;
	double muFriction = 0.40;

	for (int i = 0;i < 3;i++) {
		/**
		 * Calcul de Runge ZeroCrossingODE
		 */
		while ( (solveFlight.getCurrentS() < getTimeMax()) && (! solveFlight.getZeroCrossing()) && !solveFlight.isUnderMinDs() )  {
			solveFlight.zeroCrossing(eventFlight, -1e-2, 6e-4); // si aucun zero-crossing et temps < temps max, alors on continue, sinon on boucle pour trouver un pas de calcul au dessus du zero-crossing.
			matrice.push_back(solveFlight.getAllQ());
		}
		solveFlight.resetZeroCrossing(); // reset du zero crossing pour les boucles suivantes.
		solveFlight.resetUnderMinDs(); // reset du zero crossing pour les boucles suivantes.
		// sortie de la boucle while
		/**
		 *  calcul du vecteur vitesse et spin apres rebond
		 *  @param : double muFriction = 0.40;
		 *  @param : longueur Vx = getQ(1), lageur Vy = getQ(3), hauteur Vz = getQ(5),
		 *  @pram : rayon, getSpinY()
		 */
		vi			= sqrt(solveFlight.getQ(1)*solveFlight.getQ(1)+solveFlight.getQ(3)*solveFlight.getQ(3)); // calcul de la nouvelle norme de la vitesse de la balle dans le plan vertical de lancement ==> Vy = 0 car on considere que la balle apres rebond continue dans l'axe X (pour le moment)
		impactAngle = atan(abs(vi/solveFlight.getQ(5)));
		thetaRebond = 0.2687807 * sqrt(vi*vi+solveFlight.getQ(5)*solveFlight.getQ(5))/18.6*(impactAngle/0.7749262 ); // en m/s et degre
		vixprime 	= vi * cos(thetaRebond) - abs(solveFlight.getQ(5)) * sin(thetaRebond);
		vizprime 	= vi * sin(thetaRebond) + abs(solveFlight.getQ(5)) * cos(thetaRebond);
		if (abs(vizprime) < 20.0) {
			eBall 	= 0.510 - 0.0375 * abs(vizprime) + 0.000903 * abs(vizprime*vizprime);
		}
		else {
			eBall 	= 0.120;
		}
		muFrictionCritic = 2.0 * (vixprime + getRayon() * getSpinY()) / (7.0*(1+eBall) * abs(vizprime));

		if (muFriction < muFrictionCritic) {
			vrxprime 	= vixprime - muFriction * abs(vizprime) *(1.0 + eBall);
			vrzprime 	= eBall * abs(vizprime);
			wr 			= getSpinY() - (5.0 * muFriction * 2.0 * getRayon()) * abs(vizprime) * (1.0 + eBall); // nouveau spinY
		}
		else {
			vrxprime 	= 5.0/7.0*vixprime - 2.0/7.0*getRayon()*getSpinY();
			vrzprime 	= eBall * abs(vizprime);
			wr 			= -vrxprime / getRayon(); 											// nouveau spinY
		}
		vr 		= vrxprime * cos(thetaRebond) - vrzprime * sin(thetaRebond); 	// nouvelle norme de la vitesse de la balle
		vrz 	= vrxprime * sin(thetaRebond) + vrzprime * cos(thetaRebond); 	// Hauteur
		vrx 	= vr / vi * solveFlight.getQ(1);										// longueur
		vry		= vr / vi * solveFlight.getQ(3);										// largeur
		/**
		 * fin calcul du vecteur vitesse et spin apres rebond
		 */
		/**
		 *  mise a jour des nouvelles donnees apres rebond pour la resolution suivante.
		 */
		solveFlight.setQ(vrx, 1); 						// positionne les nouvelles valeurs  dt+1
		solveFlight.setQ(vry, 3);
		solveFlight.setQ(vrz, 5);
		solveFlight.setQ(0, 4);							// La nouvelle position de la hauteur est 0
		// mise a jour du spin
		(solveFlight.getEquationODE())->setParamEq(0,0);	// wx et le this.SpinX dans les parametres d'equation
		(solveFlight.getEquationODE())->setParamEq(wr,1);	// wy et le this.SpinY dans les parametres d'equation
		(solveFlight.getEquationODE())->setParamEq(0,2);	// wz et le this.SpinZ dans les parametres d'equation
		setSpinX(0);	// wx et le this.SpinX
		setSpinY(wr);	// wy et le this.SpinY
		setSpinZ(0);	// wz et le this.SpinZ
		// enregistrement du point de chutte
		if (i == 0) {
			indexChute = matrice.size();
			verticalLand = impactAngle;
		}
	} // fin de la boucle for 3 rebonds

	/**
	 *  Resolution du roullage avec nouvelle equation
	 *  @param : longueur Vx = getQ(1), lageur Vy = getQ(3), hauteur Vz = getQ(5),
	 */
	eqnRoulBalle->setParamEq(getRhoG(), 4);	// positionne le parametre de densite de green.

	solveRoll.setQ(solveFlight.getQ(0), 0); 	// initialisation des positions vitesse au sol pour le roulage
	solveRoll.setQ(solveFlight.getQ(1), 1);
	solveRoll.setQ(solveFlight.getQ(2), 2);
	solveRoll.setQ(solveFlight.getQ(3), 3);
	solveRoll.setQ(solveFlight.getQ(4), 4);
	solveRoll.setQ(solveFlight.getQ(5), 5);
	/**
	 * Calcul de Runge Kutta
	 */
	while ( (solveRoll.getCurrentS() < getTimeMax()) && (! solveRoll.getZeroCrossing()) && !solveFlight.isUnderMinDs() )  {
		solveRoll.zeroCrossing(eventRoll, -1e-2, 6e-4); // si pas de zero crossing une iteration, sinon, event est true
		// mise a jour des donnees courrantes
		matrice.push_back(solveRoll.getAllQ());
	}
	solveRoll.resetZeroCrossing(); // reset du zero crossing pour les boucles suivantes.
	solveFlight.resetUnderMinDs(); // reset du zero crossing pour les boucles suivantes.
	/**
	 * fin du roullage
	 */
	for (unsigned  i=0; i < matrice.size(); i++) {
		/**
		 * mise a jour des donnees courrantes et rotation vers nouveau referentiel prenant en compte la direction initiale du club
		 */
		setX( matrice[i][0]  * cos(alphaClubPath) + matrice[i][2] * sin(alphaClubPath));
		setVx(matrice[i][1]  * cos(alphaClubPath) + matrice[i][3] * sin(alphaClubPath));
		setY(-matrice[i][0]  * sin(alphaClubPath) + matrice[i][2] * cos(alphaClubPath));
		setVy(-matrice[i][1] * sin(alphaClubPath) + matrice[i][3] * cos(alphaClubPath));
		setZ( matrice[i][4]);
		setVz(matrice[i][5]);
		matriceFlight.push_back(getVCurrentms());
		maxHeight = std::max(maxHeight, matrice[i][4]); // sauvegarde de la hauteur max atteinte
		//cout << getVx() << " ; " << getVy() << " ; " <<  getVz()<< " ; " <<  getX() << " ; " << getY() << " ; " <<  getZ()<< endl;
	}
	tempsTotal = solveRoll.getCurrentS() + solveFlight.getCurrentS();
	cout << "Temps Total : " <<  tempsTotal << endl;
	return;
}
/**
 * Fin de runSimu()
 *
 **/

/**
 *  Fin de class GolfBall
 */


