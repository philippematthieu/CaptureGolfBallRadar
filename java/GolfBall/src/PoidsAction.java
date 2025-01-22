import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class PoidsAction implements ActionListener   {
	private SaisieData fenetre;
	
	public PoidsAction(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setPoids(Double.parseDouble(this.fenetre.getPoidsField()));
	}
}
