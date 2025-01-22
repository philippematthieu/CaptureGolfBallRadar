import javax.sound.sampled.*;
import java.io.*;

public class RecordSwing {
	/**
	 * Launch the record data from sound card
	 * @author http://www.codejava.net/coding/capture-and-record-sound-into-wav-file-with-java-sound-api
	 * @version  1.0
	 * @category A
	 * @see 
	 * @param 
	 * @return N/A	 
	 */
	private long 					RECORD_TIME; // in ms
	private TargetDataLine 			line;
	private AudioFileFormat.Type	fileType;
	private File 					wavFile;
	private AudioFormat 			format;
	private byte[] 					samples;
	// Constructor.
	public RecordSwing(long pRECORD_TIME){
		/**
		 * @author 
		 * @param 
		 */
		RECORD_TIME = pRECORD_TIME;
		// path of the wav file
		wavFile = new File("C:/Users/matthieu/Desktop/Tiro/RecordAudio.wav");
		// format of audio file
		fileType = AudioFileFormat.Type.WAVE;
		float sampleRate = 44100;
		int sampleSizeInBits = 16;
		int channels = 2;
		boolean signed = true;
		boolean bigEndian = true;
		format = new AudioFormat(sampleRate, sampleSizeInBits, channels, signed, bigEndian);
		
	}

	/**
	 * Captures the sound and record into a WAV file
	 */
	public void start() {
		try {
			DataLine.Info info = new DataLine.Info(TargetDataLine.class, this.getAudioFormat());
			// checks if system supports the data line
			if (!AudioSystem.isLineSupported(info)) {
				System.out.println("Line Audio unavailable");
				System.exit(0);
			}
			line = (TargetDataLine) AudioSystem.getLine(info);
			line.open(this.getAudioFormat());
			line.start();   // start capturing

			System.out.println("Start capturing...");

			AudioInputStream ais = new AudioInputStream(line);
			//DataInputStream dis = new DataInputStream(ais);
			System.out.println("Start recording...");
			// start recording
			AudioSystem.write(ais, this.getFileType(), this.getWavFile());
		    int length = (int) (88);//ais.getFrameLength() * ais.getFormat().getFrameSize());			
			byte[] samples = new byte[length];
			//dis.readFully(samples);
			ais.read(samples);
			

		} catch (LineUnavailableException ex) {
			ex.printStackTrace();
		} catch (IOException ioe) {
			ioe.printStackTrace();
		}
	}
	/**
	 * Closes the target data line to finish capturing and recording
	 */
	public void finish() {
		line.stop();
		line.close();
		System.out.println("Finished");
	}
	/**
	 * Defines an audio format
	 */
	public AudioFormat getAudioFormat() {		
		return format;
	}
	public long getRecordTime(){
		return RECORD_TIME;
	}
	public File getWavFile(){
		return wavFile;
	}
	public AudioFileFormat.Type getFileType(){
		return fileType;
	}

	public byte[] getWavSignal() {
		return samples;
	}
}