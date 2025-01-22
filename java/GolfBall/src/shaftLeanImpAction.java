import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class shaftLeanImpAction implements ActionListener   {
	private SaisieData fenetre;
	
	public shaftLeanImpAction(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setShafLeanImpDegre(Double.parseDouble(this.fenetre.getShaftLeanImpField()));
	}
}
