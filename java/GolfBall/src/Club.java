/**
 * @author 
 * {@literal} Description : Class de definition d'un Club
 */
public class Club {
	private String Type;
	private double Poids;
	private double Loft;
	private double Ecoeff;
	private double CoeffBackSpin;
	private double CoeffSpinLift;
	private double Cl1;					// A new aerodynamic model of a golf ball in flight (1994); Alexander J Smits
	private double alphaClubPath;		// angle du chemin de club a l'impact
	private double gamaFacePath;		// angle de la tete de club a l'impact
	private double shaftLeanImp;			// angle du shaft a l'impact en degre
	private double clubV0kmh;			// vitesse en km/h dde la tete de club
	private double miss;				// centre du club loupe en cm
	private double temperature;	 	// Temperature de l'aire en °C

	// Constructor 
	public Club(String pType, double pPoids, double pTemperature, double pLoft, double pEcoeff, double pCoeffBackSpin, double pCoeffSpinLift, double pCl1, double pClubV0, double pAlphaClubPath, double pGamaFacePath, double pShaftLeanImp){
		/**
		 * @author 
		 * @param String pType, double pPoids, double pLoft, double pEcoeff, double pCoeffBackSpin, double pCoeffSpinLift, double pCm)
		 * 
		 */
	    System.out.println("Création d\'un Club : " + pType);
		Type 			= pType;
		Poids 			= pPoids;
		Loft 			= pLoft; // en degre
		Ecoeff 			= pEcoeff;
		CoeffBackSpin	= pCoeffBackSpin;
		CoeffSpinLift 	= pCoeffSpinLift;
		Cl1 			= pCl1;
		alphaClubPath 	= pAlphaClubPath*Math.PI/180; // en radian
		gamaFacePath 	= pGamaFacePath*Math.PI/180;  // en radian
		shaftLeanImp 	= pShaftLeanImp;
		clubV0kmh		= pClubV0;
		temperature 	= pTemperature;
		
	} // fin constructor public GolfClub()
	
	public String getType() {
		return Type;
	}	
	public double getTemperature() {
		return temperature;
	}
	public void setTemperature(double temp) {
		temperature = temp;
	}
	public double getPoids() {
		return Poids;
	}
	public void setPoids(double poids) {
		Poids = poids;
	}
	public double getEcoeff() {
		return Ecoeff;
	}
	public void setECoeff(double pEcoeff) {
		Ecoeff = pEcoeff;
	}
	public double getCoeffBackSpin() {
		return CoeffBackSpin;
	}
	public void setCoeffBackSpin(double pCoeffBackSpin) {
		CoeffBackSpin = pCoeffBackSpin;
	}
	public double getCoeffSpinLift() {
		return CoeffSpinLift;
	}
	public void setCoeffSpinLift(double pCoeffSpinLift) {
		CoeffSpinLift = pCoeffSpinLift;
	}
	public double getCl1() {
		return Cl1;
	}
	public void setCl1(double pCl1) {
		Cl1 = pCl1;
	}
	public double getLoft() {
		return Loft;
	}
	public void setLoftDegre(double pLoft) {
		Loft = pLoft;
	}
	public double getDynamiqueLoftRadian() {
		return (getLoft() + getShaftLeanImp())*Math.PI/180;
	}
	public double getDynamiqueLoftDegre() {
		return getDynamiqueLoftRadian()*180/Math.PI;
	}
	public void setShafLeanImpDegre(double pShafLeanImp) {
		shaftLeanImp = pShafLeanImp;
	}
	public double getShaftLeanImp() {
		return shaftLeanImp;
	}
	public void setShaftLeanImpDegre(double pShaftLean) {
		shaftLeanImp = pShaftLean;
	}
	public void setClubV0Kmh(double vitesse) {
		clubV0kmh	= vitesse;
	}
	public double getClubV0kmh() {
		return clubV0kmh;
	}	
	public double getClubV0ms() {
		return getClubV0kmh() * 10 / 36;
	}
	public double getMiss() {
		return miss;
	}
	public double getGamaFacePathRadian() {
		return gamaFacePath;
	}
	public void setGamaFacePathDegre(double pGamaFacePath) {
		gamaFacePath = pGamaFacePath*Math.PI/180;
	}
	public double getAlphaClubPathRadian() {
		return alphaClubPath;
	}
	public void setAlphaClubPathDegre(double pAlphaClubPath) {
		alphaClubPath = pAlphaClubPath*Math.PI/180;
	}
} // fin public class GolfClub



