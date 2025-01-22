/**
 * 
 * @author 
 *
 */
public class EquationODERoll  extends EquationODE {
/**
 * 
 * @param pParametres
 */
	public EquationODERoll(double pParametres[]) {
		super(pParametres);
	}
/**
 * 
 */
	public double[] getEvaluation(double s, double q[] ){
		/**
		 * @author 
		 * @param
		 */
		double vx 			= q[1];		// = dxdt en m/s
		double vy 			= q[3];		// = dydt en m/s
		double vz 			= q[5];		// = dzdt en m/s
		double getRhoGreen	= getParamEq(4);
		double getG			= getParamEq(8);
		double dQ[] 		= new double[q.length];

		dQ[0] = vx;																		//
		dQ[1] = -(5.0/7.0)*getRhoGreen*getG * vx/Math.sqrt(vx*vx+vy*vy+vz*vz);			// ax longueur
		dQ[2] = vy;																		//
		dQ[3] = -(5.0/7.0)*getRhoGreen*getG * vy/Math.sqrt(vx*vx+vy*vy+vz*vz); 			// ay largeur
		dQ[4] = 0.0 * vz/Math.sqrt(vx*vx+vy*vy+vz*vz);									//
		dQ[5] = 0.0 * vz/Math.sqrt(vx*vx+vy*vy+vz*vz); 									// acceleration hauteur nulle

		return dQ;
	} // fin  getEvaluation()
}