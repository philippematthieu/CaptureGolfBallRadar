import java.util.Vector;


/**
 * Calculates the golf ball's flight, with the initial club velocity and others parameters, given by the golf club. The solver Runge Kutta is called to solve the flight's ordinary differential equation.  
 * @author Matthieu PHILIPPE
 * @param (String pMarque, int pNbAlveoles, int pNbPieces, double pTemperature, GolfClub pGolfClub, double pdt, double pTimeMax, double pdt)
 * @return Vector &lsaquo;double[]&rsaquo; matriceFlight; initial ball's velocity ; Back Spin; Side Spin; Launch Angle; Impact Angle; Dynamic Loft; Fall point; Total Range
 * 
 */
public class Ball {
	private String				marque; 			// marque de la balle de golf
	private double				masse; 				// masse de la ball de golf 
	private int					nbAlveoles;	 		// nombre d'alveole de la balle de golf
	private int					nbPieces; 			// nb de pieces de la bale de golf
	private double				area; 				// aire de la balle de golf
	private double				temperature;	 	// Temperature de l'aire en °C
	private double				rayon; 				// rayon de la balle de golf
	private double				rhoAir; 			// densite de l'aire
	private double				g; 					// acceleration terrestre en m/s2
	private double				rhoGreen;			// densite de green
	private double				launchAngle;		// angle de decollage de la balle en radian
	private double[]			v0Initms 	= new double[6]; // {x, vx, y, vy, z, vy } = q init
	private double[]			v0Currentms	= new double[6]; // {x, vx, y, vy, z, vy } = q courrant
	private double				v0BallInitms;		// Vitesse initiale de la balle 
	private int					numEqns 	= 6;	// nombre d'equations
	private double				timeMax;				// temps max de l'ODE
	private double				dt;					// pas de temps
	private SolverODE			solveFlight;
	private EquationODEFlight	eqnVolBalle;
	private SolverODE			solveRoll;
	private EquationODERoll		eqnRoulBalle;
	private EquationODEEventFlight 	event;
	private EquationODEEventRoll 	eventRoll;
	private double				alphaClubPath;
	private double 				verticalLand = 0.0;
	private double[] 			paramEqn = new double[9];// wx, wy,wz, getRayon, getRhoAir, getBallArea, getCl1, getMasse, getG
	private Vector<double[]> 	matriceFlight = new Vector<double[]>(); // la dimension est faite par le .clone() plus loin
	private	int 				indexChute = 0;
	private double 				spinYOrgrpm, spinZOrgrpm;
	private double 				tempsTotal;
	private double 				maxHeight;
	// Constructor
	public Ball(String pMarque, int pNbAlveoles, int pNbPieces, Club pGolfClub, double pTimeMax, double pdt){
		/**
		 * @author 
		 * @param String pMarque, int pNbAlveoles, int pNbPieces, double pTemperature
		 */
		marque 			= pMarque;
		nbAlveoles 		= pNbAlveoles;
		nbPieces 		= pNbPieces;
		temperature 	= pGolfClub.getTemperature(); 		// en °C
		masse 			= 0.04545; 							// en kg
		rayon			= 0.0215; 							// en m
		area 			= Math.PI*rayon*rayon;				// air de la section en m^2
		rhoAir		 	= 1.292*273.15/(temperature + 273); // en kg/m^3
		rhoGreen	 	= 0.131; 							// en kg/m^3
		g 				= 9.81; 							// m/s^2
		timeMax 		= pTimeMax;
		dt 				= pdt;
		alphaClubPath	= pGolfClub.getAlphaClubPathRadian();

		double v0Ballbfnms =  pGolfClub.getClubV0ms() * Math.cos(pGolfClub.getDynamiqueLoftRadian())*(1.0 + pGolfClub.getEcoeff())/(1+masse/pGolfClub.getPoids()) ;// Vitesse longitudinale dans le referentiel de decollage apres impact (ref: The physics of golf: The optimum loft of a driverA. Raymond Penner)
		double v0Ballbfpms = -pGolfClub.getClubV0ms() * Math.sin(pGolfClub.getDynamiqueLoftRadian())/(1.0 + masse/pGolfClub.getPoids() + 2.5);     // Vitesse perpendiculaire dans le referentiel de decollage apres impact

		// Calcul de l'angle de decollage de la balle
		launchAngle		= pGolfClub.getDynamiqueLoftRadian() + Math.atan(v0Ballbfpms/v0Ballbfnms);    // angle de decollage de la balle

		// Calcul de la vitesse initiale de la balle
		v0BallInitms	= Math.sqrt(v0Ballbfnms*v0Ballbfnms + v0Ballbfpms*v0Ballbfpms)* (1.0 - 0.3556 * pGolfClub.getMiss()); // Vitesse de decollage de la balle
		v0Initms[1] 	=  v0BallInitms * Math.cos(launchAngle) * Math.cos(0.0);     // Vx
		v0Initms[3] 	= -v0BallInitms * Math.cos(launchAngle) * Math.sin(0.0);     // Vy 
		v0Initms[5] 	=  v0BallInitms * Math.sin(launchAngle);                   // Vz

		// initialisation de la position
		v0Initms[0] = 0.0;	// X = dVx.dt
		v0Initms[2] = 0.0;	// Y = dVy.dt 
		v0Initms[4] = 0.0;	// Z = dVz.dt
		v0Currentms = (double[]) v0Initms.clone(); // copie des valeurs initiales v0Initms

		// Calcul des SPINs qui sont des parametre de l'ODE
		paramEqn[0] = 0; 																									// wx Le SPIN sur l'axe X n'est pas une composante dans l'axe du chemin de club. Le spin est appliqu� sur le plan du sol
		paramEqn[1] = v0Initms[1] * Math.sin(pGolfClub.getDynamiqueLoftRadian())    * pGolfClub.getCoeffBackSpin() / 9.54929659643;	// wy Spin dans l'axe largeur backspin
		paramEqn[2] = v0Initms[5] * Math.sin(pGolfClub.getGamaFacePathRadian()) * pGolfClub.getCoeffSpinLift() / 9.54929659643;	// wz Spin dans l'axe hauteur lift
		spinYOrgrpm = paramEqn[1]*9.54929659643; // sauvegarde des Spin d'origine
		spinZOrgrpm = paramEqn[2]*9.54929659643;

		paramEqn[3] = this.getRayon();
		paramEqn[4] = this.getRhoAir();
		paramEqn[5] = this.getBallArea();
		paramEqn[6] = pGolfClub.getCl1();
		paramEqn[7] = this.getMasse();
		paramEqn[8] = this.getG();

		// declaration des instances pour le vol de la balle
		eqnVolBalle	= new EquationODEFlight(paramEqn);
		event		= new EquationODEEventFlight(paramEqn);
		solveFlight = new SolverODE(eqnVolBalle, 0.0, this.getdt(), v0Initms);

		// declaration des instances de roullage de la balle
		eqnRoulBalle= new EquationODERoll(paramEqn);
		eventRoll	= new EquationODEEventRoll(paramEqn);
		solveRoll 	= new SolverODE(eqnRoulBalle, 0.0, this.getdt(), v0Initms);
	}
	// methodes pour constantes et variables du systeme
	public int getNumEqns() {
		return numEqns;
	}	
	public String getMarque() {
		return marque;
	}	
	public int getNbAlveoles() {
		return nbAlveoles;
	}	
	public int getNbPieces() {
		return nbPieces;
	}	
	public double getTemperature() {
		return temperature;
	}
	public void setTemperature(double temp) {
		temperature = temp;
	}	
	public double getMasse() {
		return masse;
	}	
	public double getBallArea() {
		return area;
	}	
	public double getRhoAir() {
		return rhoAir;
	}	
	public double getRhoG() {
		return rhoGreen;
	}	
	public double getRayon() {
		return rayon;
	}	
	public double getG() {
		return g;
	}	
	public double getV0BallInitms() {
		return v0BallInitms;
	}	
	public double getLaunchAngle() {
		return launchAngle;
	}	
	public double getImpactAngle() {
		return verticalLand;
	}	
	public double[] getV0Initms() {
		return v0Initms;
	}	
	//methodes pour vitesses Vx, Vy, Vz
	public double[] getVCurrentms() {
		return v0Currentms;
	}	
	public void setVCurrentms(double valeur ,int index ) {
		v0Currentms[index] = valeur;
		return;
	}	
	public void setVx(double vx) {
		v0Currentms[1] = vx;
		return;
	}	
	public double getVx() {
		return v0Currentms[1];
	}	
	public void setVy(double vy) {
		v0Currentms[3] = vy;
		return;
	}	
	public double getVy() {
		return v0Currentms[3];
	}	
	public void setVz(double vz) {
		v0Currentms[5] = vz;
		return;
	}	
	public double getVz() {
		return v0Currentms[5];
	}	
	// methodes pour position x, y ,z
	public void setX(double x) {
		v0Currentms[0] = x;
		return;
	}	
	public double getX() {
		return v0Currentms[0];
	}	
	public void setY(double y) {
		v0Currentms[2] = y;
		return;
	}	
	public double getY() {
		return v0Currentms[2];
	}	
	public void setZ(double z) {
		v0Currentms[4] = z;
		return;
	}
	public double getZ() {
		return v0Currentms[4];
	}	
	// methodes pour SPIN
	public double getSpinX() {
		return paramEqn[0];
	}	
	public void setSpinX(double wx) {
		paramEqn[0] = wx;
		return;
	}	
	public double getSpinY() {
		return paramEqn[1];
	}	
	public double getSpinYOrgrpm() {
		return spinYOrgrpm;
	}	
	public void setSpinY(double wy) {
		paramEqn[1] = wy;
		return;
	}	
	public double getSpinZ() {
		return paramEqn[2];
	}	
	public double getSpinZOrgrpm() {
		return spinZOrgrpm;
	}	
	public void setSpinZ(double wz) {
		paramEqn[2] = wz;
		return;
	}
	/** gestion du temps maximum pour le solver
	 * @return timeMax : temps maximum du solver
	 */
	public double getTimeMax() {
		return timeMax;
	}	
	/** gestion du pas de temps pour le solver
	 * @return dt : pas de temps du solver
	 */
	public double getdt() {
		return dt;
	}	
	public double getIndexChute() {
		return indexChute-1;
	}		
	public double getTempsTotal() {
		return tempsTotal;
	}	
	public Vector<double[]> getMatriceFlight()
	{
		return matriceFlight;
	}
	public double getMaxHeight()
	{
		return maxHeight;
	}
	/**
	 * Debut de runSimu()
	 * author Matthieu PHILIPPE
	 **/
	public void runSimu() {
		Vector<double[]> matrice = new Vector<double[]>(); // la dimension est faite par le .clone() plus loin

		double vi, thetaRebond, vixprime, vrxprime, vizprime, vrzprime, eBall, muFrictionCritic, wr, vr, vrx, vry, vrz, impactAngle; 
		final  double muFriction = 0.40;
		for (int i = 0;i < 3;i++) {
			/**
			 * Calcul de Runge ZeroCrossingODE
			 */
			while ( (solveFlight.getCurrentS() < this.getTimeMax()) && (! solveFlight.getZeroCrossing()) && !solveFlight.isUnderMinDs()   )  {
				solveFlight.zeroCrossing(event,  1e-2, 6e-4); // si pas de zero crossing une iteration, sinon, event est true
				matrice.add(solveFlight.getAllQ().clone());
			}
			solveFlight.resetZeroCrossing(); // reset du zero crossing pour les boucles suivantes.
			solveFlight.resetUnderMinDs();
			// sortie de la boucle while
			/**
			 *  calcul du vecteur vitesse et spin apres rebond 
			 *  @param : double muFriction = 0.40;
			 *  @param : longueur Vx = getQ(1), lageur Vy = getQ(3), hauteur Vz = getQ(5),
			 *  @pram : rayon, getSpinY()
			 */
			vi			= Math.sqrt(solveFlight.getQ(1)*solveFlight.getQ(1)+solveFlight.getQ(3)*solveFlight.getQ(3)); // calcul de la nouvelle norme de la vitesse de la balle dans le plan vertical de lancement ==> Vy = 0 car on considere que la balle apres rebond continue dans l'axe X (pour le moment)
			impactAngle = Math.atan(Math.abs(vi/solveFlight.getQ(5)));
			thetaRebond = 0.2687807 * Math.sqrt(vi*vi+solveFlight.getQ(5)*solveFlight.getQ(5))/18.6*(impactAngle/0.7749262 ); // en m/s et degre 
			vixprime 	= vi * Math.cos(thetaRebond) - Math.abs(solveFlight.getQ(5)) * Math.sin(thetaRebond);
			vizprime 	= vi * Math.sin(thetaRebond) + Math.abs(solveFlight.getQ(5)) * Math.cos(thetaRebond); 
			if (Math.abs(vizprime) < 20.0) {
				eBall 	= 0.510 - 0.0375 * Math.abs(vizprime) + 0.000903 * Math.abs(vizprime*vizprime);
			}
			else {
				eBall 	= 0.120;
			}
			muFrictionCritic = 2.0 * (vixprime + getRayon() * this.getSpinY()) / (7.0*(1+eBall) * Math.abs(vizprime));

			if (muFriction < muFrictionCritic) {
				vrxprime 	= vixprime - muFriction * Math.abs(vizprime) *(1.0 + eBall);
				vrzprime 	= eBall * Math.abs(vizprime);
				wr 			= this.getSpinY() - (5.0 * muFriction * 2.0 * getRayon()) * Math.abs(vizprime) * (1.0 + eBall); // nouveau spinY
			}
			else {
				vrxprime 	= 5.0/7.0*vixprime - 2.0/7.0*getRayon()*this.getSpinY();
				vrzprime 	= eBall * Math.abs(vizprime);
				wr 			= -vrxprime / getRayon(); 											// nouveau spinY
			}
			vr 		= vrxprime * Math.cos(thetaRebond) - vrzprime * Math.sin(thetaRebond); 	// nouvelle norme de la vitesse de la balle
			vrz 	= vrxprime * Math.sin(thetaRebond) + vrzprime * Math.cos(thetaRebond); 	// Hauteur
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
			solveFlight.getEquationODE().setParamEq(0,0);	// wx et le this.SpinX
			solveFlight.getEquationODE().setParamEq(wr,1);	// wy et le this.SpinY
			solveFlight.getEquationODE().setParamEq(0,2);	// wz et le this.SpinZ
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
		eqnRoulBalle.setParamEq(this.getRhoG(), 4);	// positionne le parametre de densite de green.

		solveRoll.setQ(solveFlight.getQ(0), 0); 	// initialisation des positions vitesse au sol pour le roulage 
		solveRoll.setQ(solveFlight.getQ(1), 1);
		solveRoll.setQ(solveFlight.getQ(2), 2);
		solveRoll.setQ(solveFlight.getQ(3), 3);
		solveRoll.setQ(solveFlight.getQ(4), 4);
		solveRoll.setQ(solveFlight.getQ(5), 5);
		/**
		 * Calcul de Runge Kutta
		 */
		while ( (solveRoll.getCurrentS() < this.getTimeMax()) && (! solveRoll.getZeroCrossing())  && !solveFlight.isUnderMinDs())  {
			solveRoll.zeroCrossing(eventRoll, 1e-2, 6e-4); // si pas de zero crossing une iteration, sinon, event est true
			// mise a jour des donnees courrantes
			matrice.add(solveRoll.getAllQ().clone());
		}
		solveRoll.resetZeroCrossing(); // reset du zero crossing pour les boucles suivantes.
		solveRoll.resetUnderMinDs(); // reset du zero crossing pour les boucles suivantes.		
		/**
		 * fin du roullage
		 */
		for (double[] j : matrice) {
			/**
			 * mise a jour des donnees courrantes et rotation vers nouveau referentiel prenant en compte la direction initiale du club
			 */
			this.setX( j[0]  * Math.cos(alphaClubPath) + j[2] * Math.sin(alphaClubPath));
			this.setVx(j[1]  * Math.cos(alphaClubPath) + j[3] * Math.sin(alphaClubPath));
			this.setY(-j[0]  * Math.sin(alphaClubPath) + j[2] * Math.cos(alphaClubPath));
			this.setVy(-j[1] * Math.sin(alphaClubPath) + j[3] * Math.cos(alphaClubPath));
			this.setZ( j[4]);
			this.setVz(j[5]);				
			matriceFlight.add(this.getVCurrentms().clone());
			maxHeight = Math.max(maxHeight, j[4]); // sauvegarde de la hauteur max atteinte
		}
		tempsTotal = solveRoll.getCurrentS() + solveFlight.getCurrentS();
		return;
	}
	/**
	 * Fin de runSimu()
	 *  
	 **/
} 
/**
 *  Fin de class GolfBall
 */

