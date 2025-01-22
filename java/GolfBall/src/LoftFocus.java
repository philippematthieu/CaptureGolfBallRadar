import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;


public class LoftFocus implements FocusListener {
	private SaisieData fenetre;

	public LoftFocus(SaisieData saisieData){
		this.fenetre = saisieData;
	}
	public void focusGained(FocusEvent e) {
	}

	public void focusLost(FocusEvent e) {
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setLoftDegre(Double.parseDouble(this.fenetre.getLoftField()));
	}

}
