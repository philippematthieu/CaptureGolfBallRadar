/**
 * 
 * @author 
 *
 */
public abstract class EquationODE {
	
	private double[] parametres = new double[9];

	public EquationODE(double pParametres[]) {
		parametres = pParametres;
	}

	abstract double[] getEvaluation(double s, double q[] );
	
	public void setParamEq(double value, int index) {
		parametres[index] = value;
	}
	
	public double getParamEq(int index) {
		return parametres[index];
	}
}
