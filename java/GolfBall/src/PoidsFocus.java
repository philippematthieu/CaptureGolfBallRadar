import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;


public class PoidsFocus implements FocusListener {
	private SaisieData fenetre;
	
	public PoidsFocus(SaisieData saisieData){
		this.fenetre = saisieData;
	}
	public void focusGained(FocusEvent e) {
	}
	public void focusLost(FocusEvent e) {
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setPoids(Double.parseDouble(this.fenetre.getPoidsField()));
	}
}
