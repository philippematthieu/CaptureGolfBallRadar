import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class TemperatureAction implements ActionListener   {
	private SaisieData fenetre;
	
	public TemperatureAction(SaisieData saisieData, String texte){
		this.fenetre = saisieData;
	}
	public void actionPerformed(ActionEvent e) { 
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setTemperature(Double.parseDouble(this.fenetre.getTemperatureField()));
	}
}
