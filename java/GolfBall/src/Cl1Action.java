import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Cl1Action implements ActionListener   {
	private SaisieData fenetre;
	
	public Cl1Action(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setCl1(Double.parseDouble(this.fenetre.getCl1Field()));
	}
}
