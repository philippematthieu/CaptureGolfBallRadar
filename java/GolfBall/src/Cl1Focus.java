import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;


public class Cl1Focus implements FocusListener {
	private SaisieData fenetre;

	public Cl1Focus(SaisieData saisieData){
		this.fenetre = saisieData;
	}
	public void focusGained(FocusEvent e) {
	}

	public void focusLost(FocusEvent e) {
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setCl1(Double.parseDouble(this.fenetre.getCl1Field()));
	}

}
