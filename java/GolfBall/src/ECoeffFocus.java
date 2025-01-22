import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;


public class ECoeffFocus implements FocusListener {
	private SaisieData fenetre;

	public ECoeffFocus(SaisieData saisieData){
		this.fenetre = saisieData;
	}
	public void focusGained(FocusEvent e) {
	}

	public void focusLost(FocusEvent e) {
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setECoeff(Double.parseDouble(this.fenetre.getECoeffField()));
	}

}
