import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;


public class ListClubFocus implements FocusListener {
	private SaisieData fenetre;

	public ListClubFocus(SaisieData saisieData){
		this.fenetre = saisieData;
	}
	public void focusGained(FocusEvent e) {
	}

	public void focusLost(FocusEvent e) {
		/**
		 * mise à jour de la donnee du club utilise
		 * En changeant de club il faut remettre a jour toutes les informations
		 * du club en cours de la fenetre de saisie
		 * String pType, double pPoids, double pLoft, double pEcoeff, double pCoeffBackSpin, double pCoeffSpinLift, double pCm
		 */
		this.fenetre.setClub(this.fenetre.getSelectedIndex());
		this.fenetre.setPoidsField(String.format("%4.3f",this.fenetre.getClub().getPoids()));
		this.fenetre.setLoftField(String.format("%4.1f",this.fenetre.getClub().getLoft()));
		this.fenetre.setECoeffField(String.format("%4.2f",this.fenetre.getClub().getEcoeff()));
		this.fenetre.setCoeffBackSpinField(String.format("%4.2f",this.fenetre.getClub().getCoeffBackSpin()));
		this.fenetre.setCoeffSpinLiftField(String.format("%4.2f",this.fenetre.getClub().getCoeffSpinLift()));
		this.fenetre.setCl1Field(String.format("%4.2f",this.fenetre.getClub().getCl1()));
		// mise à jour du alphaClubPath du club depuis la fenetre de saisie
		this.fenetre.getClub().setAlphaClubPathDegre(Double.parseDouble(this.fenetre.getAlphaClubPathField()));
		//  mise à jour du gamaFacePath du club depuis la fenetre de saisie
		this.fenetre.getClub().setGamaFacePathDegre(Double.parseDouble(this.fenetre.getGamaFacePathField()));
		//  mise à jour du V0 Club du club depuis la fenetre de saisie
		this.fenetre.getClub().setClubV0Kmh(Double.parseDouble(this.fenetre.getClubV0Field()));
		//  mise à jour du ShaftLean Club du club depuis la fenetre de saisie
		this.fenetre.getClub().setShafLeanImpDegre(Double.parseDouble(this.fenetre.getShaftLeanImpField()));
	}

}
