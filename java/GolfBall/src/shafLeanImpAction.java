import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class shafLeanImpAction implements ActionListener   {
	private SaisieData fenetre;
	
	public shafLeanImpAction(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setShafLeanImpDegre(Double.parseDouble(this.fenetre.getShaftLeanImpField()));
	}
}
