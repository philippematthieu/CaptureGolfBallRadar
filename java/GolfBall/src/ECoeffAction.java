import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class ECoeffAction implements ActionListener   {
	private SaisieData fenetre;
	
	public ECoeffAction(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setECoeff(Double.parseDouble(this.fenetre.getECoeffField()));
	}
}
