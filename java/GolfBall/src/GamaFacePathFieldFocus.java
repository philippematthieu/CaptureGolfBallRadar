import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;

public class GamaFacePathFieldFocus implements FocusListener {
	private SaisieData fenetre;

	public GamaFacePathFieldFocus(SaisieData saisieData){
		this.fenetre = saisieData;
	}
	public void focusGained(FocusEvent e) {
	}

	public void focusLost(FocusEvent e) {
		/**
		 * mise à jour de la donnee du club utilise
		 */
		this.fenetre.getClub().setGamaFacePathDegre(Double.parseDouble(this.fenetre.getGamaFacePathField()));
	}

}
