//============================================================================
// Name        : TestCPP.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
/**
*
* Simulates a flight of a golf ball, shot with a golf club.</br></br>
* <B>Bibliographie :</B>
* <OL>
* <li>A new aerodynamic model of a golf ball in flight 1994, Smits and Smith (1994)
* <li>Golf Ball Flight Dynamics Brett Burglund Ryan Street
* <li>The physics of golf: The optimum loft of a driver, A. Raymond Penner, Physics Department, Malaspina University-College, Nanaimo, British Columbia, Canada, Received 27 March 2000; accepted 9 November 2000!
* <li>The run of a golf ball, A. Raymond Penner
* <li>The physics of golf, A Raymond Penner, Physics Department, Malaspina University-College, Nanaimo, British Columbia, V9R 5S5, Canada, Received 31 October 2002, Published 20 December 2002, Online at stacks.iop.org/RoPP/66/131
* <li>Impact of the clubhead of a driver : The physics of golf: The convex face of a driver A. Raymond Penner
* <li>The physics of putting. Canadian Journal of Physics, 80 :83–96, 2002. A.R. Penner.
* <li>Physics for game programmers, Apress, 2005G. Palmer
* <li>GOLF BALL AERODYNAMICS P W BEARMAN & J K HARVEY (1976) Imperial College Of Science and Technolgy, Journal of The AERONAUTICAL QUARTERLY V27-Issue 112-P 122
* <li>Golf Ball Landing, Bounce and Roll on Turf. Woo-Jin Roha, Chong-Won Leeb
* <li>Notion de force de traînée – exercices corrigés (Daniel Huilier) LICENCE L3S5 2011-2011 Mécanique des Fluides TD –trainee-Corrigé Dany Huilier – début novembre 2010
* <li>Concours Centrale / SupElec 2012 : Physique du golf
* <li>Golf Ball Flight Dynamics, Brett Burglund Ryan Street 5-13-2011 1
* <li>L'effet Magnus,  Gilbert Gastebois
* <li>Golf Ball Flight Dynamics, Brett Burglund, Ryan Street, 5-13-2011
* </OL>
* <B>Volle de la Balle de Golf : Sum(Force) = - G + dragForce + liftForce</B></br>
* ball : 0.04593 grams et diametre 0.04267m, rayon 0.021335m</br>
* r = 1.225 kg/m^3 air density</br>
* Rho Air = 1.292*273.15/(273.15+ Temperature)</br>
* formule de la résistance de l''air en fonction de la vitesse²</br>
* R =1/2 . p . Cd . S . V^2</br>
* R = résistance de l''air</br>
* p = masse volumique de l''air</br>
* S = Air de la section droite perpendiculaire au mouvement (m²)</br>
* <B>La force de traction (Drag Force):</B><br>
* Fd(x) = 0.5 * Cd * rho_air * A * sqrt(V(x)^2+V(y)^2+V(z)^2) * Vhat(x))</br>
* 	   Vhat est le vecteur unitaire de la vitesse dans les 3 dimensions.</br>
*     Re = rho*UD/µ = VitesseBalle*Diametre/1.5e-5 </br>
*     µ = 1.5x10^-6 m^2/s</br>
*     Re = Vm*diam / 1.5e-5; // Nombre de Reynolds</br>
*     Cd = le coefficient de résistance aérodynamique</br>
*     C_d = 0.36 + 24/Re + 6/(1+sqrt(Re)); // relation de White</br>
* </br>
* <B>La force de Magnus (généré par le Back Spin et le Side Spin)</br></B>
* Coefficient Magnus en kg/rad; 1 rad/sec = 9.54929659643 RPM</br>
* liftCoeff		= getCl1 * getRayon * Math.abs(Math.pow(normeSpinIn  / normeVitesseIn, 0.4));	// Cl Smits and Smiths ""A new aerodynamic model of a golf ball in flight 1994"</br>
* liftForce		= 0.5 * RhoAir * BallSectionArea * Rayon * liftCoeff   * normeVitesseIn *  [<VEC>ω</VEC>x<VECT>V</VECT>]</br>
* V0Ballbfnms    	= V0Clubms * cos(thetaLoft)*(1+e)/(1+m/M) ;  * Conversion Vitesse Club en vitesse balle</br>
* V0Ballbfpms    	= -V0Clubms * sin(thetaLoft)/(1+m/M + 5/2);  * Conversion Vitesse Club en vitesse balle</br>
* V0Ballms       	= sqrt(V0Ballbfnms^2 + V0Ballbfpms^2)* (1 - 0.3556*miss);</br>
* LaunchAngle    	= thetaLoft + atan(V0Ballbfpms/V0Ballbfnms);</br>
* LaunchAngledeg 	= LaunchAngle*180/%pi;                       * en radian</br>
* here miss is the amount (in cm) by which the sweet spot is missed</br>
* e = The Coefficient of Restitution (COR),</br>
* e=0.78 pour les club</br>
* e=0.83 pour les Drives</br>
* m = Mass of the ball (typically 46 grams or 1.62 ounces).</br>
* M = Mass of the clubhead (typically 200 grams or 7 ounces for driver)</br>
* D = 0.043 m</br>
* A 10% increase in clubhead speed with no change in clubhead weight increases ball velocity 10%.</br>
* A 10% increase in clubhead weight with no change in clubhead speed increases ball velocity only 1.7%.</br>
* </br>
* <B>spin rate en rpm </B></br>
* approximation Spin = Coef * V0 * sin(thetaLoft)</br>
* on considerera que la décélération de W est negligeable sur le temps de vol.</br>
* On recalculera le spin au contact du sol.</br>
* SpinX = 0;//V0initms(6) * Sac.coeffSpinLift(Type == Club) * sin(gamaFacePath);// Le SPIN sur l'axe X n'est pas une composante dans l'axe du chemin de club. Le spin est appliqué sur le plan du sol
* SpinY = V0initms(4) * sin(gamaFacePath)* Sac.coeffSpinLift(Type == Club) ;//
* SpinZ = V0initms(2) * sin(thetaLoft)   * Sac.coeffSpin(Type == Club);//
*</br>

* V= C*(1-Femise/Fpercue)/cos(theta)</br>
* theta = angle de propagation</br>
* </br>
* <B>Rebond de la balle de golf sur le green / Fairway, The run of a golf ball A. Raymond Penner donne :</br></B>
*     ImpactAngle = tan-1(|vix/viy|); // prendre vix = sqrt(vx^2+vz^2))</br>
*     ThetaRebond = 15.4*(|vi|/18.6)*(ImpactAngle/44.4); // en m/s et degre</br>
*     |vixprime| = vix * cos(ThetaRebond) - |viy| * sin(ThetaRebond)</br>
*     |viyprime|= vix * sin(ThetaRebond) + |viy| * cos(ThetaRebond)    </br>
*     eball = 0.510 - 0.0375 * |viyprime| + 0.000903 * |viyprime|^2; // si |viyprime| < 20 m/s</br>
*     eball = 0.120; // si |viyprime| >= 20 m/s</br>
*     muFriction = 0.40; // mu limite, mu < muc alors la balle va glisser, si mu > muc alors la balle va rouler sur la collision</br>
*     pour un green plus ferme, on peut prendre un mu inférieur, par exemple 0.25 sur les tournoie pro?</br>
*     il faut alors aussi ajuster muFrictionCritic</br>
*     muFrictionCritic = 2*(|vixprime| + r*|wi|) / (7*(1+e)*|viyprime|)</br>
*    </br>
*     Vitesse apres rebond glisse  mu < muc</br>
*     vrxprime = vixprime - muFriction * |viyprime| *(1 + eball)</br>
*     vryprime = eball * |viyprime|</br>
*     ωr = ωi - (5*muFriction * 2r) * |viyprime| * (1 + eball)</br>
*    </br>
*     Vitesse apres rebond roule mu > muc </br>
*     vrxprime = (5/7)*vixprime - (2/7)*r*wi</br>
*     vryprime = eball * |viyprime|</br>
*     wr = -vrxprime/r</br>
*    </br>
*     vrx = vrxprime * cos(ThetaRebond) - vryprime * sin(ThetaRebond)</br>
*     vry = vrxprime * sin(ThetaRebond) + vryprime * cos(ThetaRebond)</br>
*    </br>
*     <B>Le roulement est admis si le rebond est inferieur a 10mm</br></B>
*     alors la deceleration est donnee par :</br>
*     rhog = 0.131;</br>
*     a_green = -(5/7)*rhog*g; // ou rhog = 0.131 coeff de frottement pour le green et plus grand pour le fairway [0.065 0.196]</br>
*     F = m * vxz - m*a_green</br></br>
*     Type	v km/h		Loft	Ball Speed (km/h)	meter		Lie		Max Height	Vertical Launch	attack angle	ShaftLean	BackSpin rpm</br>
* 		D	170,6		11		223,698816			201,168		59		22,86		14				3				6			2628</br>
* 		B5	141,6		18		205,996032			169,164		59		23,7744		12,2			-1,8			-3			4501</br>
* 		H3	136,8		18		197,949312			163,6776	59		22,86		12,7			-3				-2.5		4693</br>
* 		5	127,1		23		180,246528			147,2184	61		21,0312		14,8			-1,9			-5			5081</br>
* 		6	125,5		26		175,418496			138,9888	61,5	22,86		17,1			-2,3			-5			5943</br>
* 		7	122,3		30		167,371776			128,9304	62		23,7744		19				-2,3			-6			6699</br>
* 		8	119,1		34,5	160,9344			118,872		62,5	22,86		20,8			-3,1			-9			7494</br>
* 		9	115,9		39		149,668992			108,8136	63		23,7744		23,9			-3,1			-9,1		7589</br>
* 		PW	112,7		44		138,403584			97,8408		63,5	21,0312		25,6			-2,8			-11,5		8403</br>
* 		Gap	112,7		52		128,74752			91,44		63,5	18,288		27				-2,8			-17,5		9000</br>
* 		SW	104,6		54		120,7008			73,152		63,5	17,3736		30				-3				-19			9300</br>
* 		LW	96,6		60		120					33,8328		63,5	20			30				-3,3			-20			5500</br>
* @author Matthieu PHILIPPE
* @version  1.0
* @category A
* @see other http://jmathtools.sourceforge.net/doc/jmathplot/html/classorg_1_1math_1_1plot_1_1PlotPanel.html
* @param Golf.jar Dr 170.6 0.0 0.0 6.0 20.0
* @return N/A
* 
* parametre de debugage Projet/Propriétés de Golf...Propriétés de configuration/Debogage
*/
#include <iostream>
#include <algorithm> // for copy
#include <string>
#include <iterator> // for ostream_iterator
#include <vector>

#include "Club.h"
#include "Ball.h"
// Include CImg library header.
#include "CImg.h"
using namespace cimg_library;


using namespace std;

const unsigned char
    red[]   = { 255,0,0 },
    green[] = { 0,255,0 },
    blue [] = { 0,0,255 },
    black[] = { 0,0,0 };

int main(int argc, char *argv[]) {
	vector<Club> sac(13);
	unsigned j=0;

	//Ball theBall;
	//				Type, Poids, Temp, Loft, Ecoeff, CoeffBackSpin, CoeffSpinLift,		Cl1,	ClubV0, AlphaClubPath,		GamaFacePath,	ShaftLeanImp
	sac[0]  = Club("Dr",  0.300 , 20.0, 11.0 , 0.738,  200.00 ,			1500.0,			0.64,	170.6,	-8.0,				2.0,			6.0);
	sac[1]  = Club("B5",  0.280 , 20.0, 18   , 0.78 ,  281.64 , 		500.0 , 		0.65, 	151.6, 	 5.0,	 			-5.0,			-3.0);
	sac[2]  = Club("H3",  0.250 , 20.0, 18   , 0.80 ,  322.00 , 		500.0 , 		0.54, 	136.8, 	 0.0,	 			0.0,			-2.5);
	sac[3]  = Club("F5",  0.200 , 20.0, 23   , 0.85 ,  335.00 , 		420.0 , 		0.54, 	127.1, 	 0.0,	 			0.0,			-5.0);
	sac[4]  = Club("F6",  0.200 , 20.0, 26   , 0.83 ,  355.00 , 		420.0 , 		0.54, 	125.5, 	 0.0,	 			0.0,			-5.0);
	sac[5]  = Club("F7",  0.200 , 20.0, 30   , 0.83 ,  380.00 , 		420.0 , 		0.54, 	122.3, 	 0.0,	 			0.0,			-6.5);
	sac[6]  = Club("F8",  0.200 , 20.0, 34.5 , 0.83 ,  340.00 , 		420.0 , 		0.53, 	120.0, 	 0.0,	 			0.0,			-8.0);
	sac[7]  = Club("F9",  0.200 , 20.0, 39   , 0.83 ,  400.99 , 		308.0 , 		0.53, 	115.9, 	 0.0,	 			0.0,			-8.0);
	sac[8]  = Club("PW",  0.200 , 20.0, 44   , 0.78 ,  438.24 , 		280.0 , 		0.52,	112.7, 	 0.0,	 			0.0,			-12.0);
	sac[9]  = Club("AW",  0.200 , 20.0, 52   , 0.78 ,  470.09 , 		308.0 , 		0.52, 	112.7, 	 0.0,	 			0.0,			-17.0);
	sac[10] = Club("SW",  0.200 , 20.0, 54   , 0.71 ,  520.00 , 		308.0 , 		0.15,	104.6, 	 0.0,	 			0.0,			-15.0);
	sac[11] = Club("LW",  0.200 , 20.0, 60   , 0.40 ,  500.00 , 		308.0 , 		0.15, 	100.0, 	 0.0,	 			-3.0,			-12.0);
	sac[12] = Club("Pt",  0.300 , 20.0, 03   , 0.68 ,  550.24 , 		308.0 , 		0.00, 	10.00, 	 0.0,	 			0.0,			 1.0);

	// 5 parametres a prendre en compte Type,ClubV0,AlphaClubPath,GamaFacePath,ShaftLeanImp
	if (argc != 6) {
		cout << "Il faut 0 ou 5 argument en entree: "<< argc << endl;
		j = 0;
	} 
	else {
		for (unsigned i=0;i<sac.size();i++)
			if (!((sac[i].getType()).compare(argv[1])))
				j=i;
	}
	// mise à jour des données d club en remplacement de celles par defaut de demo.
	//Menu Projet/Propriétés de Tests2.. Débugage
	cout << "setClubV0Kmh: "				 << strtod(argv[2],NULL)		<< endl;
	sac[j].setClubV0Kmh(			strtod(argv[2],NULL));
	cout << "setAlphaClubPathDegre: "		 << strtod(argv[3],NULL)		<< endl;
	sac[j].setAlphaClubPathDegre(	strtod(argv[3],NULL));
	cout << "setGamaFacePathDegre: "		 << strtod(argv[4],NULL)		<< endl;
	sac[j].setGamaFacePathDegre(	strtod(argv[4],NULL));
	cout << "setShaftLeanImpDegre: "		 << strtod(argv[5],NULL)		<< endl;
	sac[j].setShaftLeanImpDegre(	strtod(argv[5],NULL));

	Ball theBall = Ball("Decathlon", 320, 3, sac[j], 18.0, 0.01);

	cout << "Type Club: "		 << sac[j].getType()		<< endl;
	cout << "Vitesse Club: "	 << sac[j].getClubV0ms()	<< endl;
	cout << "Loft: " 	 		 << sac[j].getLoft()		<< endl;
	cout << "getV0Initms Dim: "  << (theBall.getV0Initms()).size() << endl;
	for(unsigned  i = 0; i < (theBall.getV0Initms()).size();  ++i) {
		cout << "getV0Initms: "  << (theBall.getV0Initms())[i]   << endl;
		cout << "getVCurrentms: "<< (theBall.getVCurrentms())[i] << endl;
	}
	theBall.runSimu();

	/////////////////////// debut exemple CImage ///////////////////
	// Read command line argument.
	cimg_usage("Swing Resultat");
	const char *const formula = cimg_option("-f","x","Formula to plot");
	const float x0 = cimg_option("-x0",0.0f,"Minimal X-value");
	const float x1 = cimg_option("-x1",200.0f,"Maximal X-value");
	const int resolution = cimg_option("-r",1024,"Plot resolution");
	const unsigned int nresolution = resolution>1?resolution:1024;
	const unsigned int plot_type = cimg_option("-p",1,"Plot type");
	const unsigned int vertex_type = cimg_option("-v",1,"Vertex type");

	// Create plot data pour Longueur / Hauteur.
	CImg<double> valuesX(4,nresolution,1,1,0);
	CImg<double> valuesZ(4,nresolution,1,1,0);
	const unsigned int r = nresolution - 1;
	std::vector< std::vector<double> > matriceFlight = theBall.getMatriceFlight();
	// Longueur
	cimg_forY(valuesX,X) valuesX(0,X) = matriceFlight.at(X)[4];
	// Largeur
	cimg_forZ(valuesZ,X) valuesZ(0,X) = matriceFlight.at(X)[2];

	//std::vector<double> v0Currentms;
	//v0Currentms[0];

	cimg::eval(formula,valuesX).move_to(valuesX);
	cimg::eval(formula,valuesZ).move_to(valuesZ);

	// Display interactive plot window.
	valuesX.display_graph(formula,plot_type,vertex_type,"X-axis",x0,x1,"Y-axis");
	valuesZ.display_graph(formula,plot_type,vertex_type,"X-axis",x0,x1,"Y-axis");
	/////////////////////// fin exemple CImage ///////////////////

	CImgDisplay draw_disp(500,400,"Color profile of the X-axis",0), draw_disp2(500,400,"Color profile of the Z-axis",0);

	CImg<unsigned char>(draw_disp.width(),	draw_disp.height()).
		display(draw_disp);

		CImg<unsigned char>(draw_disp2.width(),	draw_disp2.height()).
		display(draw_disp2);
	return 0;
}