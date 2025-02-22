
# Golf Ball Simulator in Scilab (java and cpp)
<img src="sci/img/Pasted image 20250120171635.png"  width="560" height="380">

## Flight Analyzer captured by little Radars
<img src="sci/img/Pasted image 20250122082849.png"  width="280" height="190"> <img src="sci/img/Pasted image 20250122082902.png"  width="280" height="190"> <img src="sci/img/Pasted image 20250122082913.png"  width="280" height="190">
<img src="sci/img/Pasted image 20250122082924.png"  width="280" height="190"> <img src="sci/img/Pasted image 20250122082939.png"  width="280" height="190"><img src="sci/img/Pasted image 20250122082958.png"  width="280" height="190"> <img src="sci/img/Pasted image 20250122083320.png"  width="280" height="190">
<img src="sci/img/Pasted image 20250122083456.png"  width="280" height="190"> <img src="sci/img/Pasted image 20250122083851.png"  width="280" height="190">

# Find backspin

<img src="sci/img/Pasted image 20250120172502.png"  width="560" height="380">

## Data catched with SkyTrak

Club : iron 7

Club Speed : 111kh

Ball Speed : 150kmh

Length : 123m

Backspin : 5274trm

Sidespin : 210trm

## Backspin estimated with the radar
Given d = 0.04267m

 V = F / 19,48
 
rpm = 1000*(F_sommet - F_balle)/(19.48\*60\*%pi*d)

As mesured and as seen above, we can see that the ball speed is around 160km/h and the maximum echo is 191km/h.
So that the backspin could be estimated as 5097trm.

## The HW
### The suitecase
<img src="sci/img/Pasted image 20250123184355.png"  width="380" height="560">

### Internal with Arduino, Radar, Amplifier, Audio Boards
<img src="sci/img/20250123_132345.jpg"  width="190" height="280"> <img src="sci/img/20250123_132310.jpg"  width="190" height="280"> <img src="sci/img/20250123_132304.jpg"  width="190" height="280">

### Aluminium Tape
Place aluminum tape to increase the surface area of ​​the radar echo.

<img src="sci/img/PatchedBall.png"  width="345" height="191">

### Arduino Tennsy 3.6

<img src="sci/img/IMG_2427.jpeg"  width="400" height="114">

see here : [Arduino Tennsy 3.6](https://www.pjrc.com/store/teensy36.html)

### Radar HB100

<img src="sci/img/IMG_2429.png"  width="400" height="400">

see here : [Radar HB100](https://www.otronic.nl/fr/capteur-radar-doppler-hyperfrequence.html)

### Amplification Stage Outputs

<img src="sci/img/PCB_Back_s.jpg"  width="350" height="244">

see here : [Amplification Stage Outputs](https://www.limpkin.fr/index.php?post/2013/08/09/Making-the-electronics-for-a-%247-USD-doppler-motion-sensor)

### Audio Board

<img src="sci/img/IMG_2428.jpeg"  width="485" height="364">

see here : [Audio Board](https://www.pjrc.com/store/teensy3_audio.html)

## Simples Scilab Commands
exec('GolfBall-master\sci\comet3d2.sci', -1)
exec('GolfBall-master\sci\FFT_Mat.sce', -1)
exec('GolfBall-master\sci\Golfball.sci', -1)

fichier = uigetfile("*.wav", "Golf\Radar\");[x,Fs,bits]=wavread(fichier);
[x,Fs,bits]=wavread(fichier); 

Or 

[x,Fs,bits]=wavread('2019-02-24-F7-123m-150kmh-5274trm-210trm-cs111.wav');

Then

xg = x(1,1:$);
xd = x(2,1:$);

[xgCentre, xgf] = plotFFT(xg,Fs);
[xdCentre, xdf] = plotFFT(xd,Fs);

[tt5,f5,M5]=animFFT(xgf,44100, 128 ,2 , 2,0);
[tt5,f5,M5]=animFFT(xdf,44100, 128 ,2 , 2,0);

[M5,tt5,f5]=animDensite(xgf,44100, 512 ,10 , 1, 0.0010,1);
[vBall, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info(M5,f5,Club)

[M5,tt5,f5]=animDensite(xdf,44100, 512 ,10 , 1, 0.0010,1);
[vBall, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info(M5,f5,Club)

[M5,tt5,f5]=animDensite3(xgf,44100, 512 ,10 , 1, 100,1);

[vClub, vBall, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info2((xgf),'7',44100)
[vClub, vBall, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info2((xdf),'7',44100)

alphaClubPath=0;
gamaFacePath=0;
[t,VOL,Res] = Golfball(18, vClub, '7', alphaClubPath,gamaFacePath,ShafLeanImp ,1);
