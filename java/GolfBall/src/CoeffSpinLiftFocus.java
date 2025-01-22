import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;


public class CoeffSpinLiftFocus implements FocusListener {
	private SaisieData fenetre;

	public CoeffSpinLiftFocus(SaisieData saisieData){
		this.fenetre = saisieData;
	}
	public void focusGained(FocusEvent e) {
	}

	public void focusLost(FocusEvent e) {
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setCoeffSpinLift(Double.parseDouble(this.fenetre.getCoeffSpinLiftField()));
	}

}
