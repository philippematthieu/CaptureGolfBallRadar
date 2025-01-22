/*
 * Club.cpp
 *
 *  Created on: 19 sept. 2016
 *      Author: matthieu
 */
#include <string>
#include "Club.h"

using namespace std;

Club::Club():
				Type("Dr"), Poids(0.300), temperature(20.0), Loft(11.0), Ecoeff(0.738), CoeffBackSpin(200.00), CoeffSpinLift(1500.0),
				Cl1(0.64), clubV0kmh(170.6), alphaClubPath(0.0), gamaFacePath(0.0), shaftLeanImp(6.0){};

Club::Club(const std::string pType, double pPoids, double pTemperature, double pLoft, double pEcoeff, double pCoeffBackSpin,
		double pCoeffSpinLift, double pCl1, double pClubV0, double pAlphaClubPath, double pGamaFacePath,
		double pShaftLeanImp):
		Type(pType), Poids(pPoids), temperature(pTemperature), Loft(pLoft), Ecoeff(pEcoeff), CoeffBackSpin(pCoeffBackSpin), CoeffSpinLift(pCoeffSpinLift),
		Cl1(pCl1), clubV0kmh(pClubV0), alphaClubPath(pAlphaClubPath*M_PI/180.0), gamaFacePath(pGamaFacePath*M_PI/180.0), shaftLeanImp(pShaftLeanImp),miss(0) {}
Club::~Club() {
}

std::string Club::getType() {
	return Type;
}
double Club::getTemperature() {
	return temperature;
}
void Club::setTemperature(double temp) {
	temperature = temp;
}
double Club::getPoids() {
	return Poids;
}
void Club::setPoids(double poids) {
	Poids = poids;
}
double Club::getEcoeff() {
	return Ecoeff;
}
void Club::setECoeff(double pEcoeff) {
	Ecoeff = pEcoeff;
}
double Club::getCoeffBackSpin() {
	return CoeffBackSpin;
}
void Club::setCoeffBackSpin(double pCoeffBackSpin) {
	CoeffBackSpin = pCoeffBackSpin;
}
double Club::getCoeffSpinLift() {
	return CoeffSpinLift;
}
void Club::setCoeffSpinLift(double pCoeffSpinLift) {
	CoeffSpinLift = pCoeffSpinLift;
}
double Club::getCl1() {
	return Cl1;
}
void Club::setCl1(double pCl1) {
	Cl1 = pCl1;
}
double Club::getLoft() {
	return Loft;
}
void Club::setLoftDegre(double pLoft) {
	Loft = pLoft;
}
double Club::getDynamiqueLoftRadian() {
	return (getLoft() + getShaftLeanImp())*M_PI/180.0;
}
double Club::getDynamiqueLoftDegre() {
	return getDynamiqueLoftRadian()*180.0/M_PI;
}
double Club::getShaftLeanImp() {
	return shaftLeanImp;
}
void Club::setShaftLeanImpDegre(double pShaftLean) {
	shaftLeanImp = pShaftLean;
}
void Club::setClubV0Kmh(double vitesse) {
	clubV0kmh	= vitesse;
}
double Club::getClubV0kmh() {
	return clubV0kmh;
}
double Club::getClubV0ms() {
	return getClubV0kmh() * 10.0 / 36.0;
}
double Club::getMiss() {
	return miss;
}
double Club::getGamaFacePathRadian() {
	return gamaFacePath;
}
void Club::setGamaFacePathDegre(double pGamaFacePath) {
	gamaFacePath = pGamaFacePath*M_PI/180.0;
}
double Club::getAlphaClubPath() {
	return alphaClubPath;
}
double Club::getAlphaClubPathRadian() {
	return alphaClubPath;
}
void Club::setAlphaClubPathDegre(double pAlphaClubPath) {
	alphaClubPath = pAlphaClubPath*M_PI/180.0;
}


