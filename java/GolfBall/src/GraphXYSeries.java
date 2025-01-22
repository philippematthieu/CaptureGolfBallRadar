import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.util.StringTokenizer;
import java.util.Vector;
//import java.util.Collections;
//import java.util.Arrays;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.ui.RefineryUtilities;
import org.math.plot.Plot3DPanel;
import org.math.plot.*;

public class GraphXYSeries  extends JFrame {
	/**
	 * 
	 */
	private XYSeries series = new XYSeries("Hauteur");
	private XYSeries series2 = new XYSeries("Largeur");
	final Plot3DPanel	plot3d	 = new Plot3DPanel("SOUTH");
	final Plot2DPanel 	plot2d1 = new Plot2DPanel(); // ajout pour utilisation mathplot
	final Plot2DPanel 	plot2d2 = new Plot2DPanel(); // ajout pour utilisation mathplot

	private static final long serialVersionUID = 1L;

	public GraphXYSeries(final String title,String xTitle,String yTitle, Vector<double[]> pMatriceFlight, String pLegend) {

		super(title);
		int 	i=0;
		final XYSeriesCollection data = new XYSeriesCollection(series);
		final XYSeriesCollection data2 = new XYSeriesCollection(series2);

		double[] x = new double[pMatriceFlight.size()];
		double[] y = new double[pMatriceFlight.size()];
		double[] z = new double[pMatriceFlight.size()];
		i = 0;
		for (double[] j : pMatriceFlight) {
			x[i] = (double)j[0];
			y[i] = (double)j[2];
			z[i] = (double)j[4];
			i++;
		}

		JPanel 		panel1 		= new JPanel();
		JPanel 		panel2 		= new JPanel();
		final JFreeChart chart = ChartFactory.createXYLineChart(
				"",
				"", 
				"Hauteur", 
				data,
				PlotOrientation.VERTICAL,
				false,
				true,
				false
				);
		final JFreeChart chart2 = ChartFactory.createXYLineChart(
				"",
				xTitle, 
				"Largeur", 
				data2,
				PlotOrientation.VERTICAL,
				false,
				true,
				false
				);

		plot2d1.addLegend("SOUTH");			 // ajout pour utilisation mathplot
		plot2d2.addLegend("SOUTH");			 // ajout pour utilisation mathplot
		plot2d1.setAxisLabel(0, "Longueur");
		plot2d1.setAxisLabel(1, "Hauteur");
		plot2d2.setAxisLabel(0, "Longueur");
		plot2d2.setAxisLabel(1, "Largeur");
		final ChartPanel chartPanel  = new ChartPanel(chart);
		final ChartPanel chartPanel2 = new ChartPanel(chart2);

		chartPanel.setMouseWheelEnabled(true);
		chartPanel2.setMouseWheelEnabled(true);

		this.setLayout(new GridBagLayout());
		GridBagConstraints c = new GridBagConstraints();
		StringTokenizer multiTokenizer = new StringTokenizer(pLegend, "|");

		panel1.setLayout(new GridLayout(7,2));
		panel2.setLayout(new GridLayout(2,2));

		i = 1;
		while (multiTokenizer.hasMoreTokens())
		{
			i++;
			panel1.add(new JButton(multiTokenizer.nextToken()));
		}

		// panel2.add(chartPanel);
		panel2.add(plot2d1);// ajout pour utilisation mathplot
		panel2.add(plot3d);
		// panel2.add(chartPanel2);
		panel2.add(plot2d2);// ajout pour utilisation mathplot
		panel2.add(panel1);

		c.gridx = 1;
		c.gridy = 0;
		c.fill = GridBagConstraints.BOTH;
		c.gridwidth = 1;
		c.weightx = 1;
		c.weighty = 1;
		this.getContentPane().add(panel2,c);

		this.pack();
		this.getContentPane();
		RefineryUtilities.centerFrameOnScreen(this);
		/**
		 * 
		 */
		for (double[] j : pMatriceFlight) {
			//System.out.println((float)j[0]+" ; "+(float)j[1]+" ; "+(float)j[2]+" ; "+(float)j[3]+" ; "+(float)j[4]+" ; "+(float)j[5]);
			this.addSeries((float)j[0], (float)j[4]);	// Ajoute les nouvelles coordonnees
			this.addSeries2((float)j[0], (float)j[2]);	// Ajoute les nouvelles coordonnees
		}
		// add a line plot3d
		this.addLinePlot2d("Ball Flight", pMatriceFlight);
		this.addLinePlot3d("Ball Flight", pMatriceFlight);
		this.setVisible(true);

	}
	public void setKey(String pLegend)
	{
		this.series2.setKey(pLegend);
	}
	public XYSeries getSeries()
	{
		return series;
	}
	public XYSeries getSeries2()
	{
		return series2;
	}
	public void addSeries(double X, double Y)
	{
		this.series.add(X, Y);
	}
	public void addSeries2(double X, double Y)
	{
		this.series2.add(X, Y);
	}	
	public void removeSeries(int i)
	{
		this.series.remove(i);
	}
	public void removeSeries2(int i)
	{
		this.series2.remove(i);
	}

	public void addLinePlot2d(String pLegend, Vector<double[]> pMatriceFlight) {

		double[] x = new double[pMatriceFlight.size()];
		double[] y = new double[pMatriceFlight.size()];
		double[] z = new double[pMatriceFlight.size()];
		int i = 0;
		
		for (double[] j : pMatriceFlight) {
			x[i] = (double)j[0];
			y[i] = (double)j[2];
			z[i] = (double)j[4];
			i++;
		}
		plot2d1.addLinePlot("", x, z);// ajout pour utilisation mathplot
		plot2d2.addLinePlot("", x, y);// ajout pour utilisation mathplot
	}

	public void addLinePlot2d(double x[], double y[]) {
		plot2d2.addLinePlot("", x, y);// ajout pour utilisation mathplot
	}
	
	public void addLinePlot3d(String pLegend, Vector<double[]> pMatriceFlight) {

		double[] x = new double[pMatriceFlight.size()];
		double[] y = new double[pMatriceFlight.size()];
		double[] z = new double[pMatriceFlight.size()];
		int i = 0;
		for (double[] j : pMatriceFlight) {
			x[i] = (double)j[0];
			y[i] = (double)j[2];
			z[i] = (double)j[4];
			i++;
		}

		double[] min =  new double[3];
		double[] max =  new double[3];
		min[1] = y[0];
		max[1] = y[0];
		max[2] = z[0];
		for (i = 0;i<x.length;i++) {
			min[1] = Math.min(min[1], y[i]); // minY
			max[1] = Math.max(max[1], y[i]); // maxY
			max[2] = Math.max(max[2], z[i]); // maxZ
		}
		min[1] = -Math.max(Math.abs(min[1]-1), max[1]+1);// minY
		max[1] =  Math.max(Math.abs(min[1]-1), max[1]+1);// maxY
		max[0] = x[x.length-1] ; // maxX = dernier element
		min[2] = 0; // minZ
		min[0] = 0; // minX

		/**
		 * Afichage de la direction du drapeau
		 */
		double[] xD = {0.0 , max[0]};
		double[] doubleZero = new double[2]; // Z ligne de drapeau
		double[] sizedZero = new double[z.length];
		this.plot3d.addLinePlot("To Flag", xD , doubleZero , doubleZero);
		/**
		 * Affichage de la trajectoire de la balle sur le sol
		 */
		this.plot3d.addLinePlot("Ground", x , y , sizedZero);

		/**
		 * Affichage de la trajectoire de la balle
		 */
		this.plot3d.addLinePlot(pLegend, x, y, z);
		this.plot3d.setFixedBounds(min, max);
	}
}
