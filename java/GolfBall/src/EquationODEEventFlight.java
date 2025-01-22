/**
 * 
 * @author 
 *
 */
public class EquationODEEventFlight  extends EquationODE {
	/**
	 * 
	 * @param pParametres
	 */
	public EquationODEEventFlight(double pParametres[]) {
		super(pParametres);
	}
	/**
	 * 
	 */
	public double[] getEvaluation(double s, double q[] ){
		double qRes[] 	= new double[q.length];
		
		qRes[0] = 0.0;
		qRes[1] = 0.0;
		qRes[2] = 0.0;
		qRes[3] = 0.0;
		qRes[4] = q[4]; // je cherche si Z passe sous zero
		qRes[5] = 0.0;
		return qRes;
	} // fin  getEvaluation()
}
