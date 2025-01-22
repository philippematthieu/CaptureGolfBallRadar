import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class AlphaClubPathAction implements ActionListener   {
	private SaisieData fenetre;
	
	public AlphaClubPathAction(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setAlphaClubPathDegre(Double.parseDouble(this.fenetre.getAlphaClubPathField()));
	}
}
