/*
 * Club.h
 *
 *  Created on: 19 sept. 2016
 *      Author: matthieu
 */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef CLUB_H_
#define CLUB_H_

class Club {
public:
	Club();
	Club(const std::string pType, double pPoids, double pTemperature, double pLoft, double pEcoeff, double pCoeffBackSpin,
			double pCoeffSpinLift, double pCl1, double pClubV0, double pAlphaClubPath, double pGamaFacePath,
			double pShaftLeanImp);
	std::string getType();
	double getTemperature();
	void setTemperature(double temp);
	double getPoids();
	void setPoids(double poids);
	double getEcoeff();
	void setECoeff(double pEcoeff);
	double getCoeffBackSpin();
	void setCoeffBackSpin(double pCoeffBackSpin);
	double getCoeffSpinLift();
	void setCoeffSpinLift(double pCoeffSpinLift);
	double getCl1();
	void setCl1(double pCl1);
	double getLoft();
	void setLoftDegre(double pLoft);
	double getDynamiqueLoftRadian();
	double getDynamiqueLoftDegre();
	void setShafLeanImpDegre(double pShafLeanImp);
	double getShaftLeanImp();
	void setShaftLeanImpDegre(double pShaftLean);
	void setClubV0Kmh(double vitesse);
	double getClubV0kmh();
	double getClubV0ms();
	double getMiss();
	double getGamaFacePathRadian();
	void setGamaFacePathDegre(double pGamaFacePath);
	double getAlphaClubPath();
	double getAlphaClubPathRadian();
	void setAlphaClubPathDegre(double pAlphaClubPath);

	virtual ~Club();

private:
	std::string Type;
	double Poids;
	double temperature;	 		// Temperature de l'aire en °C
	double Loft;
	double Ecoeff;
	double CoeffBackSpin;
	double CoeffSpinLift;
	double Cl1;					// A new aerodynamic model of a golf ball in flight (1994); Alexander J Smits
	double clubV0kmh;			// vitesse en km/h dde la tete de club
	double alphaClubPath;		// angle du chemin de club a l'impact
	double gamaFacePath;		// angle de la tete de club a l'impact
	double shaftLeanImp;		// angle du shaft a l'impact en degre
	double miss;			// centre du club loupe en cm
};


#endif /* CLUB_H_ */
