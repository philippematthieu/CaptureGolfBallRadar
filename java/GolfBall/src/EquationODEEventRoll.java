/**
 * 
 * @author 
 *
 */
public class EquationODEEventRoll  extends EquationODE {
	/**
	 * 
	 * @param pParametres
	 */
	public EquationODEEventRoll(double pParametres[]) {
		super(pParametres);
	}
	/**
	 * 
	 */
	public double[] getEvaluation(double s, double q[] ){
		double qRes[] 	= new double[q.length];
		
		qRes[0] = 0.0;
		qRes[1] = q[1]; // je cherche si VX passe sous 0.01ms;
		qRes[2] = 0.0;
		qRes[3] = 0.0;
		qRes[4] = 0.0;
		qRes[5] = 0.0;
		return qRes;
	} // fin  getEvaluation()
}
