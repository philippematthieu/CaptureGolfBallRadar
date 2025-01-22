import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class CoeffBackSpinAction implements ActionListener   {
	private SaisieData fenetre;
	
	public CoeffBackSpinAction(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setCoeffBackSpin(Double.parseDouble(this.fenetre.getCoeffBackSpinField()));
	}
}
