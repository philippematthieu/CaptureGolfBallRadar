import java.awt.GridLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class SaisieData extends JFrame  {
	/**
	 * Launch the input data window.
	 * @author Matthieu PHILIPPE
	 * @version  1.0
	 * @category A
	 * @see 
	 * @param 
	 * @return N/A	 
	 */
	private static final long serialVersionUID = 2L;
	private JComboBox<Object> listClub;
	private Club[] sac;
	private JTextField poidsField;
	private JTextField clubFieldV0;
	private JTextField loftField;
	private JTextField ecoeffField;
	private JTextField coeffBackSpinField;
	private JTextField coeffSpinLiftField;
	private JTextField cl1Field;
	private JTextField alphaClubPathField;
	private JTextField gamaFacePathField;
	private JTextField shaftLeanImpField;
	private JTextField temperatureField;
	private Club club;
	private JCheckBox boutonCheck;

	public SaisieData(Club[] pSac, int indiceDefaut){
		super("Simu");

		JPanel panel = new JPanel();
		panel.setLayout(new GridLayout(13,2));
		sac 	= pSac;
		club 	= sac[indiceDefaut]; // initialisation avec le club par défaut

		// creation des champs
		//Type,  Poids , Loft , Ecoeff, CoeffBackSpin,	CoeffSpinLift, 	Cl1,  	pClubV0, pAlphaClubPath, 	pGamaFacePath,	pShaftLeanImp
		JLabel clubText 					= new JLabel("Club");
		JLabel poidsText 					= new JLabel("Poids (kg)");
		JLabel temperatureText 				= new JLabel("Temperatuer (°C)");
		JLabel loftText 					= new JLabel("Loft (°)");
		JLabel ecoeffText 					= new JLabel("E coeff");
		JLabel backSpinText 				= new JLabel("Coeff BackSpin");
		JLabel spinLiftText 				= new JLabel("Coeff SideSpin");
		JLabel cl1Text 						= new JLabel("Cl1 (Smits and Smith)");
		JLabel vClubText 					= new JLabel("Vitesse Club (km/h)");
		JLabel clubPathText 				= new JLabel("Alpha ClubPath (°)");
		JLabel gamaPathText 				= new JLabel("Gama FacePath (°)");
		JLabel shaftLeanText 				= new JLabel("ShaftLean Imp (°)");
		JButton boutonRun;

		listClub 		= new JComboBox<Object>();
		listClub.addActionListener(new ListClubAction(this,"List"));
		listClub.addFocusListener(new ListClubFocus(this));

		poidsField				= new JTextField(Double.toString(club.getPoids()));
		poidsField.addActionListener(new PoidsAction(this,"Poids"));
		poidsField.addFocusListener(new PoidsFocus(this));
		setPoidsField(Double.toString(club.getPoids()));

		temperatureField				= new JTextField(Double.toString(20.0));
		temperatureField.addActionListener(new TemperatureAction(this,"Poids"));
		temperatureField.addFocusListener(new TemperatureFocus(this));
		setTemperatureField(Double.toString(club.getTemperature()));

		loftField 				= new JTextField(Double.toString(club.getLoft()));
		loftField.addActionListener(new LoftAction(this,"Loft"));
		loftField.addFocusListener(new LoftFocus(this));
		setLoftField(Double.toString(club.getLoft()));

		ecoeffField 			= new JTextField(Double.toString(club.getEcoeff()));
		ecoeffField.addActionListener(new ECoeffAction(this,"ECoeff"));
		ecoeffField.addFocusListener(new ECoeffFocus(this));
		setECoeffField(Double.toString(club.getEcoeff()));

		coeffBackSpinField 		= new JTextField(Double.toString(club.getCoeffBackSpin()));
		coeffBackSpinField.addActionListener(new CoeffBackSpinAction(this,"BackSpin"));
		coeffBackSpinField.addFocusListener(new CoeffBackSpinFocus(this));
		setCoeffBackSpinField(Double.toString(club.getCoeffBackSpin()));

		coeffSpinLiftField 		= new JTextField(Double.toString(club.getCoeffSpinLift()));
		coeffSpinLiftField.addActionListener(new CoeffSpinLiftAction(this,"Lift"));
		coeffSpinLiftField.addFocusListener(new CoeffSpinLiftFocus(this));
		setCoeffSpinLiftField(Double.toString(club.getCoeffSpinLift()));

		cl1Field 				= new JTextField(Double.toString(club.getCl1()));
		cl1Field.addActionListener(new Cl1Action(this,"CL1"));
		cl1Field.addFocusListener(new Cl1Focus(this));
		setCl1Field(Double.toString(club.getCl1()));

		clubFieldV0 			= new JTextField(Double.toString(club.getClubV0ms()*3.6));
		clubFieldV0.addActionListener(new ClubVAction(this,"V0Club"));
		clubFieldV0.addFocusListener(new ClubVFocus(this));
		setClubV0Field(Double.toString(club.getClubV0ms()*3.6));

		alphaClubPathField 		= new JTextField(Double.toString(club.getAlphaClubPathRadian()*180/Math.PI));
		alphaClubPathField.addActionListener(new AlphaClubPathAction(this,"Alpha"));
		alphaClubPathField.addFocusListener(new AlphaClubPathFocus(this));
		setAlphaClubPathField(Double.toString(club.getAlphaClubPathRadian()*180/Math.PI));

		gamaFacePathField 		= new JTextField(Double.toString(club.getGamaFacePathRadian()*180/Math.PI));
		gamaFacePathField.addActionListener(new GamaFacePathFieldAction(this,"Gama"));
		gamaFacePathField.addFocusListener(new GamaFacePathFieldFocus(this));
		setGamaFacePathField(Double.toString(club.getGamaFacePathRadian()*180/Math.PI));

		shaftLeanImpField 		= new JTextField(Double.toString(club.getShaftLeanImp()));
		shaftLeanImpField.addActionListener(new shaftLeanImpAction(this,"ShaftLean"));
		shaftLeanImpField.addFocusListener(new shaftLeanImpFocus(this));
		setShaftLeanImpField(Double.toString(club.getShaftLeanImp()));

		boutonRun 		= new JButton( new RunAction(this,"Swing"));
		boutonCheck 	= new JCheckBox( "Same Frame", false);

		for (Club j : pSac){
			listClub.addItem(j.getType());
		}
		setListClubField(indiceDefaut);

		panel.add(clubText);
		panel.add(listClub);
		panel.add(poidsText);
		panel.add(poidsField);
		panel.add(temperatureText);
		panel.add(temperatureField);
		panel.add(loftText);
		panel.add(loftField);
		panel.add(ecoeffText);
		panel.add(ecoeffField);
		panel.add(backSpinText);
		panel.add(coeffBackSpinField);
		panel.add(spinLiftText);
		panel.add(coeffSpinLiftField);
		panel.add(cl1Text);
		panel.add(cl1Field);
		panel.add(vClubText);
		panel.add(clubFieldV0);
		panel.add(clubPathText);
		panel.add(alphaClubPathField);
		panel.add(gamaPathText);
		panel.add(gamaFacePathField);
		panel.add(shaftLeanText);
		panel.add(shaftLeanImpField);
		panel.add(boutonRun);
		panel.add(boutonCheck);

		this.getContentPane().add(panel);
		this.pack();
		this.setVisible(true);
	}	
	/**
	 * fin du constructeur
	 */
	public JComboBox<Object> getListClub(){
		return listClub;
	}
	public String getPoidsField(){
		return poidsField.getText();
	}
	public void setPoidsField(String poids){
		poidsField.setText(poids);
	}
	public String getECoeffField(){
		return ecoeffField.getText();
	}
	public void setECoeffField(String poids){
		ecoeffField.setText(poids);
	}
	public String getCoeffBackSpinField(){
		return coeffBackSpinField.getText();
	}
	public void setCoeffBackSpinField(String poids){
		coeffBackSpinField.setText(poids);
	}
	public String getCoeffSpinLiftField(){
		return coeffSpinLiftField.getText();
	}
	public void setCoeffSpinLiftField(String poids){
		coeffSpinLiftField.setText(poids);
	}	
	public void setCl1Field(String loft){
		cl1Field.setText(loft);
	}
	public String getCl1Field(){
		return cl1Field.getText();
	}
	public void setLoftField(String loft){
		loftField.setText(loft);
	}
	public String getLoftField(){
		return loftField.getText();
	}
	public String getClubV0Field(){
		return clubFieldV0.getText();
	}	
	public void setClubV0Field(String ClubV0){
		clubFieldV0.setText(ClubV0);
	}
	public int getSelectedIndex(){
		return listClub.getSelectedIndex();
	}
	public Club getClub(){
		return club;
	}
	public void setClub(int index){
		club = sac[index];
	}
	public String getAlphaClubPathField(){
		return alphaClubPathField.getText();
	}
	public void setAlphaClubPathField(String alphaClubPath){
		alphaClubPathField.setText(alphaClubPath);
	}
	public String getGamaFacePathField(){
		return gamaFacePathField.getText();
	}
	public void setGamaFacePathField(String gamaFacePath){
		gamaFacePathField.setText(gamaFacePath);
	}
	public String getShaftLeanImpField(){
		return shaftLeanImpField.getText();
	}	
	public void setShaftLeanImpField(String shaftLean){
		shaftLeanImpField.setText(shaftLean);
	}
	public String getTemperatureField(){
		return temperatureField.getText();
	}		
	public void setTemperatureField(String Temp){
		temperatureField.setText(Temp);
	}	
	public Boolean getPlotInSameFrame(){
		return boutonCheck.isSelected();
	}
	public void setListClubField(int indice){
		listClub.setSelectedIndex(indice);
	}
}
