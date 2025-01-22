import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;


public class TemperatureFocus implements FocusListener {
	private SaisieData fenetre;
	
	public TemperatureFocus(SaisieData saisieData){
		this.fenetre = saisieData;
	}
	public void focusGained(FocusEvent e) {
	}
	public void focusLost(FocusEvent e) {
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setTemperature(Double.parseDouble(this.fenetre.getTemperatureField()));
	}
}
