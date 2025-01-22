public class SolverODE
{
	private int j;
	private int numEqns;
	private double q[]		= new double[numEqns];
	private double dq1[] 	= new double[numEqns];
	private double dq2[] 	= new double[numEqns];
	private double dq3[] 	= new double[numEqns];
	private double dq4[] 	= new double[numEqns];
	private double ds;
	private double sCurrent;
	private EquationODE eqn;
	private boolean zeroCrossing;
	private boolean underMinDs;

	// Constructor
	/**
	 * @author 
	 * {@literal} Description : constructeur du solver Runge Kutta
	 * @param (String pMarque, int pNbAlveoles, int pNbPieces, double pTemperature, GolfClub pGolfClub, double pdt, double pTimeMax, double pdt)
	 */
	public SolverODE(EquationODE pEqn, double ps, double pds, double[] pq) {
		/**
		 * @param double ps, double pds, double[] pq
		 * 
		 */
		eqn 		= pEqn;
		sCurrent 	= ps;					// temps intiale
		ds 			= pds;					// step temporel
		q 			= pq;					// parametres depedants
		numEqns 	= q.length ; 			// nombre d'equations
		zeroCrossing = false;
		underMinDs = false;
	}	


	public double getCurrentS() {
		return sCurrent;
	}
	public void setCurrentS(double s) {
		sCurrent = s;
	}
	public double getQ(int index ) {
		return q[index];
	}	
	public void setQ(double value, int index ) {
		q[index] = value;
	}	
	public double[] getAllQ() {
		return q;
	}	
	public void setAllQ(double[] allQ) {
		q = allQ;
	}	
	public boolean getZeroCrossing() {
		return zeroCrossing;
	}	
	public void resetZeroCrossing() {
		zeroCrossing = false;
	}	
	public boolean isUnderMinDs() {
		return underMinDs;
	}	
	public void resetUnderMinDs() {
		underMinDs = false;
	}	
	public EquationODE getEquationODE()
	{
		return eqn;
	}

	/**
	 *  Fourth-order Runge-Kutta ODE solver.
	 */
	public void rungeKutta4() {
		double dqnew[] = new double[numEqns];

		// dZ1 = v(tn, zn)*dt
		dq1 = eqn.getEvaluation(getCurrentS()		, q);
		for(j=0; j < numEqns; ++j) {
			dq1[j] = dq1[j]*ds;
		}

		// dZ2 = v(tn+0.5*dt , zn+0.5*dz1)*dt
		for(j=0; j < numEqns; ++j) {
			dqnew[j] = q[j] + 0.5*dq1[j];
		}
		dq2 = eqn.getEvaluation(getCurrentS() + 0.5 * ds, dqnew);
		for(j=0; j < numEqns; ++j) {
			dq2[j] = dq2[j]*ds;
		}

		// dZ3 = v(tn+0.5*dt , zn+0.5*dz2)*dt
		for(j=0; j < numEqns; ++j) {
			dqnew[j] = q[j] + 0.5*dq2[j];
		}		
		dq3 = eqn.getEvaluation(getCurrentS() + 0.5 * ds, dqnew);
		for(j=0; j < numEqns; ++j) {
			dq3[j] = dq3[j]*ds;
		}

		// dZ4 = v(tn+dt , zn+dz3)*dt
		for(j=0; j < numEqns; ++j) {
			dqnew[j] = q[j] + dq3[j];
		}	
		dq4 = eqn.getEvaluation(getCurrentS() + ds	, dqnew);
		for(j=0; j < numEqns; ++j) {
			dq4[j] = dq4[j]*ds;
		}	

		for(j=0; j < numEqns; ++j) {
			q[j] = q[j] + (dq1[j] + 2.0*dq2[j] + 2.0*dq3[j] + dq4[j])/6.0;
		}
		setCurrentS(getCurrentS() + ds); 			// evolution du temps au step suivant
		return;
	}

	/**
	 * ZeroCrossing Solver using Runge Kutta.
	 * @param event
	 * @param precision
	 */
	public void zeroCrossing(EquationODE event, double precision, double minTimeStep) {
		double sCurrentOrg;						// temps intiale
		double dsOrg;							// step temporel
		double[] qOrg	= new double[numEqns];	// parametres depedants
		double[] qRes	= new double[numEqns];	// parametres depedants
		boolean iter	= true;

		// initialisation a la valeur avant calcul
		sCurrentOrg = getCurrentS();
		dsOrg 		= ds;
		qOrg 		= (double[]) getAllQ().clone();

		rungeKutta4();
		// si les valeurs de qRes sont positives, on renvoie le q de rungeKutta en sortant
		// si l'une des valeurs de retour qRes est < (-precision) on reprend le pas precedent positif avec un calcul � ds/2.
		// si les valeurs de retour qRes sont comprises entre (-precision) < qRes < 0 on arrete car on a traverse le zero a la precision.
		iter 	= true;
		zeroCrossing = false;
		underMinDs = (ds < minTimeStep);
		while (iter){ // iteration sur un pas de temps à t-1 plus petit / 2,  pour trouver une solution sans zero crossing
			setAllQ((double[]) qOrg.clone());		// on remet la valeur avant le pas si on itere. Si c'est la premiere iteration, alors les valeurs n'ont pas changees
			setCurrentS(sCurrentOrg); 				// on remet la valeur avant le pas
			rungeKutta4(); 							// on resoud l'equation
			qRes 	= event.getEvaluation(getCurrentS(), (double[]) getAllQ()); // appel de l'evenement pour verifier si passage zerocrossing
			iter 	= false; 						// pas defaut, il n'y a pas besoin d'iterer.
			underMinDs = (ds < minTimeStep); 		// on verifie si le step temporel mini est atteind
			for (int i=0;i< qRes.length;i++) {
				//iter = ( ((!underMinDs) && (Math.abs(qRes[i]) < precision)) || iter);	// si l'une des valeurs est qRes(i) < - precision on reprend le pas positif precedent avec ds/2 (on itere pour trouver le zerocrossing
				// si qRes[i] != 0 faire
				if (qRes[i] != 0) {
					// on cherche si zerocrossing pour iterer sur un pas plus petit ==> 
					// changement de signe ==> zerorossing = (abs(position(n) - position(n-1)) > abs(abs(position(n)) - abs(position(n-1))))
					zeroCrossing = (Math.abs(qRes[i] - qOrg[i]) > Math.abs(Math.abs(qRes[i]) - Math.abs(qOrg[i])));
					iter = (iter || zeroCrossing ) &&
							(!underMinDs) &&
							(Math.abs(qRes[i]) > precision );
				}
			}
			ds =ds/2.0;								// on diminue le pas de temps pour eviter le zerocrossing dans le cas d'une iteration
		}
		underMinDs = (ds < minTimeStep);			// on verifie si le pas de temps minimum est franchi
		ds = dsOrg;									// on reprend le pas de temps intial
	} 
}