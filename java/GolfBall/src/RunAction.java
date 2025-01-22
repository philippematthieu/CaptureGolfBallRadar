import java.awt.event.ActionEvent;
import java.util.Vector;
import javax.swing.AbstractAction;


public class RunAction extends AbstractAction  {
	/**
	 * Runs the ball's simulator and launch the graphic result window.
	 * @author Matthieu PHILIPPE
	 * @version  1.0
	 * @category A
	 * @see 
	 * @param 
	 * @return N/A	 
	 */
	private static final long serialVersionUID = 2L;
	private SaisieData fenetre;
	private	GraphXYSeries graphLongHaut = null;

	public RunAction(SaisieData saisieData, String texte){
		super(texte);
		this.fenetre = saisieData;
	}
	public SaisieData getFenetre() {
		return this.fenetre;
	}
	public void actionPerformed(ActionEvent e) {
		Vector<double[]> matriceFlight;
		/**
		 * Nouvelle balle lancee
		 */
		Ball theBall = new Ball("Décathlon 520 Soft", 350, 2, this.fenetre.getClub(), 18.0, 0.01);

		

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 * Lancement de l'enregistrement sonore (pour tests)		
		 */
		
			    final long RECORD_TIME = 1000;  // 1/1000s	temps d'enregistrement		
		        final RecordSwing recorder = new RecordSwing(RECORD_TIME);
		        byte[] 					samples;
		        // creates a new thread that waits for a specified
		        // of time before stopping
		        Thread stopper = new Thread(new Runnable() {
		            public void run() {
		                try {
		                    Thread.sleep(RECORD_TIME);
		                } catch (InterruptedException ex) {
		                    ex.printStackTrace();
		                }
		                recorder.finish();
		            }
		        });
		 
		        stopper.start();
		 
		        // start recording
		        recorder.start();
		        samples = recorder.getWavSignal();
		        recorder.finish();
		
		/* Unit tests the {@code FFT} class.
		 *
		 * @param args the command-line arguments
		 */
		int 		n 		= 8001;		// 
		Complex[] 	x 		= new Complex[n];
		// original data
		for (int i = 0; i < n; i++) {
			x[i] = new Complex(i, 0.0);
			x[i] = new Complex(Math.sin(3.0*2.0*Math.PI*(double)i/50.0), 0.0); // cela devrait donner 134kmh et 1.24 de smashfactor
		}

		int 				nbEchantillon = 512;
		int 				pas			= 50;
		int					Fs			= 44100;
		int					b			= x.length % nbEchantillon;// b=modulo(N,nbEchantillon);
		Complex[] 			xCentre		= new Complex[x.length + nbEchantillon - b]; // correction de la taille de l'échantillon
		double				tau			= 1 / (double)Fs;
		Complex				mean		= new Complex(0.0,0.0);
		double[]			t			= new double[xCentre.length];
		double[]			f			= new double[nbEchantillon/2]; // on ne prend que les F/2
		FFT 				maFft 		= new FFT();
		Complex[]			Fenetre_x 	= new Complex[nbEchantillon];
		double[]			s2_densite	= new double[nbEchantillon];
		double[]			s2_densite2	= new double[nbEchantillon/2];
		double[]			s2_densite2_sqr	= new double[nbEchantillon/2];
		double 				s2_densite_max = 0.0;
		int					max_k 		= 0;
		double				s2_densite2_sqr_max = 0;
		Vector<Double>		tt			= new Vector<Double>();
		Vector<double[]> 	M2 			= new Vector<double[]>();

		for (int i=0;i < x.length ; i++){				// xCentre = x - mean(x);
			mean.plus(x[i]);
		}
		mean = mean.divides(new Complex((double)x.length,0.0));
		for (int i=0;i < x.length ; i++){				// recentrage du signal autour de l'axe abscice.
			xCentre[i] = x[i];							// initialisation de xCentré
			xCentre[i].minus(mean);						// centrage de x
		}
		for (int i=x.length;i < xCentre.length ; i++){	// xCentre=[xCentre, zeros(nbEchantillon - b,1)'];
			xCentre[i] = new Complex(0.0,0.0);			
		}            
		for (int i = 0; i< xCentre.length; i++){		// t = (0:N - 1) * tau; // construction du vecteur temps
			t[i] = (double)i * tau; 					// construction du vecteur temps
		}
		for (int i = 0; i < nbEchantillon/2; i++){		// f=Fs*(0:(nbEchantillon)-1)/nbEchantillon;            
			f[i] = (double) (Fs *  i) / nbEchantillon;
		}
		for (int i  = 0; i < xCentre.length - nbEchantillon; i=i+pas) {//          for ii=1:pas:(size(s2Centre,2) - nbEchantillon),
			for (int j = 0;j < nbEchantillon; j++) {	// densite spectrale // s2_fft = (fft(s2Centre(1,ii:ii +  nbEchantillon)));
				Fenetre_x[j] = xCentre[i+j];
			}
			s2_densite = maFft.densityRe(Fenetre_x);	// conjuguee_s2 = conj(s2_fft);// s2_densite = abs(real(s2_fft.*conjuguee_s2));
			s2_densite_max = 0.0; 						// init du max ; max(s2_densite);
			for(int k = 0; k < s2_densite.length; k++){
				s2_densite_max = Math.max(s2_densite[k],s2_densite_max);
			}
			for(int k = 0; k < s2_densite.length/2; k++){// s2_densite / max(s2_densite);
				s2_densite2[k] = s2_densite[k] / s2_densite_max;
			}
			for(int k = 0; k < s2_densite.length/2; k++){// sqr
				s2_densite2_sqr[k] = s2_densite2_sqr[k] + s2_densite[k]*s2_densite[k]; // somme des carre des amplitudes sur totue la période
			}
			tt.add(t[i]);								// tt = [tt;t(ii)];
			M2.add(s2_densite2.clone());				// M1 = [M1;s2_densite/max(s2_densite)]; contien 154 pas colonnes et 256 lignes (nb echantilons decoupes)
		}
		M2.add(s2_densite2.clone());				// M1 = [M1;s2_densite/max(s2_densite)]; contien 154 lignes (pas, elements) et 256 colonnes (nb echantilons decoupes)
		for (int k = 0; k < s2_densite2_sqr.length; k++){
			if (s2_densite2_sqr_max < s2_densite2_sqr[k]){
				s2_densite2_sqr_max = s2_densite2_sqr[k];
				max_k = k;
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		
		/**
		 * Lancement de la simulation du vol
		 */		
		theBall.runSimu();

		/**
		 * Recuperation des donnees x, y z du vol
		 */
		matriceFlight = theBall.getMatriceFlight(); // recuperation des donnees x, y z du vol

		/**
		 * affichage des données dans les graphiques
		 */
		if (this.fenetre.getPlotInSameFrame()){
			try {
				graphLongHaut.addLinePlot3d("Ball Flight", matriceFlight) ;
				graphLongHaut.addLinePlot2d("", matriceFlight);
			}
			finally {	
			}
		}
		else {
			graphLongHaut = new GraphXYSeries("Ball Flight","Longueur", "Hauteur",matriceFlight, String.format(
					String.format("<HTML><CENTER><b>Club : </b>" + this.fenetre.getClub().getType() + "</CENTER></HTML>|")
					+String.format("<HTML><CENTER><b>Masse : </b>" + this.fenetre.getClub().getPoids() + "kg</CENTER></HTML>|")
					+String.format("<HTML><CENTER><b>VClub</b><BR> %4.2f km/h</CENTER></HTML>|",this.fenetre.getClub().getClubV0ms()*3.6)
					+String.format("<HTML><CENTER><b>ClubPath</b><BR> %4.2f°</CENTER></HTML>|",this.fenetre.getClub().getAlphaClubPathRadian()*180/Math.PI)
					+String.format("<HTML><CENTER><b>Loft</b><BR> %4.2f°</CENTER></HTML>|",this.fenetre.getClub().getLoft())
					+String.format("<HTML><CENTER><b>VBall</b><BR> %4.2f km/h</CENTER></HTML>|",theBall.getV0BallInitms()*3.6)
					+String.format("<HTML><CENTER><b>ClubAngle</b><BR> %4.2f°</CENTER></HTML>|",this.fenetre.getClub().getGamaFacePathRadian()*180/Math.PI)
					+String.format("<HTML><CENTER><b>ShaftLean</b><BR> %4.2f°</CENTER></HTML>|",this.fenetre.getClub().getShaftLeanImp())
					+String.format("<HTML><CENTER><b>BackSpin</b><BR> %4.0f rpm</CENTER></HTML>|",theBall.getSpinYOrgrpm())
					+String.format("<HTML><CENTER><b>launchAngle</b><BR> %4.2f°</CENTER></HTML>|",theBall.getLaunchAngle()*180/Math.PI)
					+String.format("<HTML><CENTER><b>DynamicLoft</b><BR> %4.2f°</CENTER></HTML>|",this.fenetre.getClub().getDynamiqueLoftDegre())
					+String.format("<HTML><CENTER><b>SideSpin</b><BR> %4.0f rpm</CENTER></HTML>|",theBall.getSpinZOrgrpm())
					+String.format("<HTML><CENTER><b>impactAngle</b><BR> %4.2f°</CENTER></HTML>|",theBall.getImpactAngle()*180/Math.PI)
					+String.format("<HTML><CENTER><b>XFall</b><BR> %4.2fm</CENTER></HTML>|",matriceFlight.get((int) theBall.getIndexChute())[0])
					+String.format("<HTML><CENTER><b>XTotal</b><BR> %4.2fm</CENTER></HTML>|",matriceFlight.get(matriceFlight.size()-1)[0])
					+String.format("<HTML><CENTER><b>Temps Total</b><BR> %4.2fs</CENTER></HTML>|",theBall.getTempsTotal())
					+String.format("<HTML><CENTER><b>YFall</b><BR> %4.2fm</CENTER></HTML>|",matriceFlight.get((int) theBall.getIndexChute())[2])
					+String.format("<HTML><CENTER><b>YTotal</b><BR> %4.2fm</CENTER></HTML>|",matriceFlight.get(matriceFlight.size()-1)[2])
					+String.format("<HTML><CENTER><b>Temperature</b><BR> %4.2f°C</CENTER></HTML>|",this.fenetre.getClub().getTemperature())
					+String.format("<HTML><CENTER><b>Max Height</b><BR> %4.2fm</CENTER></HTML>|",theBall.getMaxHeight())
					+String.format("<HTML><CENTER><b>Smash Factor</b><BR> %4.2f</CENTER></HTML>|", (theBall.getV0BallInitms()) / (this.fenetre.getClub().getClubV0ms()))
					));		
/////////////////////////////////////////////////////////////////////////////////////////////
			graphLongHaut.addLinePlot2d(f,s2_densite2);
////////////////////////////////////////////////////////////////////////////////////////////
		}
	}
} 

