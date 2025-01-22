import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;


public class AlphaClubPathFocus implements FocusListener {
	private SaisieData fenetre;

	public AlphaClubPathFocus(SaisieData saisieData){
		this.fenetre = saisieData;
	}
	public void focusGained(FocusEvent e) {
	}

	public void focusLost(FocusEvent e) {
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setAlphaClubPathDegre(Double.parseDouble(this.fenetre.getAlphaClubPathField()));
	}

}
