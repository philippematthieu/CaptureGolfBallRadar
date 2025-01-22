import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class CoeffSpinLiftAction implements ActionListener   {
	private SaisieData fenetre;
	
	public CoeffSpinLiftAction(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setCoeffSpinLift(Double.parseDouble(this.fenetre.getCoeffSpinLiftField()));
	}
}
