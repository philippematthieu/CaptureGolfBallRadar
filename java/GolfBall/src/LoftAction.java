import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class LoftAction implements ActionListener   {
	private SaisieData fenetre;
	
	public LoftAction(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setLoftDegre(Double.parseDouble(this.fenetre.getLoftField()));
	}
}
