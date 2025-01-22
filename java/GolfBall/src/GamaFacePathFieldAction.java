import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class GamaFacePathFieldAction implements ActionListener   {
	private SaisieData fenetre;
	
	
	
	
	public GamaFacePathFieldAction(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setGamaFacePathDegre(Double.parseDouble(this.fenetre.getGamaFacePathField()));
	}
}
