import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;


public class ClubVFocus implements FocusListener {
	private SaisieData fenetre;

	public ClubVFocus(SaisieData saisieData){
		this.fenetre = saisieData;
	}
	public void focusGained(FocusEvent e) {
	}

	public void focusLost(FocusEvent e) {
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setClubV0Kmh(Double.parseDouble(this.fenetre.getClubV0Field()));
	}

}
