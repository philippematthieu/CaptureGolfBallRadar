import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class ClubVAction implements ActionListener   {
	private SaisieData fenetre;
	
	public ClubVAction(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setClubV0Kmh(Double.parseDouble(this.fenetre.getClubV0Field()));
	}
}
