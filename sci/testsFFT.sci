function toto()
    
x=[1;2,5;
pause;
abort;
pause;
pause;
// Copyright (C) 2017 - mm - Author
//
// About your license if you have any
//
// Date of creation: 30 oct. 2017
//
// Dimensions d, w, and h
// The separation distance (d) between the antenna element and the back-reflection plane should be by 
// approximately λc ~= 19 mm = 0.7 in.) This will enhance frequencies close to 4 GHz.
// The width (w) of the reflection plane should, in theory, be at least 15 cm (6 in.), providing 
// a full wavelength of surface area margin on each side of the antenna element in the azimuth (horizontal) 
// plane. Even wider back-reflectors will provide even better performance at wide angles, but in practice anything 
// wider than 20 cm (9 in.) produces rapidly diminishing returns.
// The reflector height (h) isn’t as critical due to the dipole nature of the element. However, the reflector should have at 
// least ¼λc = 19 mm = 0.7 in. of top and bottom margin, especially if the reflected antenna can tilt forward/back or the 
// relative elevation angle of the other radio(s) can be large (more than +/-45 o.) In this case a wider top/bottom margin is recommended. Note: one can always
//    
//
////fichier = '400_TrMin.wav';
//fichier = '1150_TrMin.wav';
//fichier = 'BalleRoule3_smooth.wav';
//fichier = 'BalleRoule4_smooth.wav';
//fichier = 'BalleRoule5_smooth.wav';
//fichier = 'BalleRoule2_dimpled.wav';
//fichier = 'BalleRoule5_smooth';
//fichier = 'Driver5.wav';[z,Fs,bits]=wavread(fichier);s2 = z(2,:);s2test = s2(11300:14200);[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'Driver4.wav';
//fichier = 'Driver3.wav';[z,Fs,bits]=wavread(fichier);s2 = z;s2test = s2(10000:14500);[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'Driver2.wav';
//fichier = 'Driver1.wav';
fichier = 'Fer7_5.wav';[z,Fs,bits]=wavread(fichier);s2 = z(2,:);s2test = s2(6500:11200);[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'Fer7_4.wav';[z,Fs,bits]=wavread(fichier);s2 = z(2,:);s2test = s2(16950:21000);[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'Fer7_3.wav';
//fichier = 'Fer7_2.wav';
//fichier = 'Fer7_1.wav';
//fichier = 'fer7Radar.wav';[z,Fs,bits]=wavread(fichier);s2 = z(2,:);s2test = s2(1:4500);[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = '1150_TrMin_Bruit.wav';
//fichier = '1150_TrMin.wav';
//fichier = 'BalleRoule1_dimpled.wav';
//fichier = '400_TrMinbruit.wav';
//fichier = 'roulage1.wav';
//fichier = 'BalleRoule3.wav';
//fichier = 'voiture1.wav';
//fichier = 'testRadar.wav';[z,Fs,bits]=wavread(fichier);s2 = z;s2test = s2(20000:60000);[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'testRadar2.wav';[z,Fs,bits]=wavread(fichier);s2 = z;s2test = s2(20000:70000);[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'testRadar3.wav';[z,Fs,bits]=wavread(fichier);s2 = z;s2test = s2(1:10000);[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'testRadar4.wav';[z,Fs,bits]=wavread(fichier);s2 = z;s2test = s2(1:10000);[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'roulage2.wav';
//fichier = 'lancer.wav';
//fichier = 'BalleFil.wav';
//fichier = 'BalleFil4Tourne.wav';[z,Fs,bits]=wavread(fichier);s2 = z(2,:);s2test = s2(1:10000);[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'BalleFil5Tombe.wav';
//fichier = 'Fer7_seul_1.wav';[z,Fs,bits]=wavread(fichier);s2 = z(2,:);s2test = s2(58000:60600);[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'Fer7_seul_2.wav';[z,Fs,bits]=wavread(fichier);s2 = z(2,:);s2test = s2(52000:55500);
//fichier = 'Fer7_seul_3.wav';[z,Fs,bits]=wavread(fichier);s2 = z(2,:);s2test = s2(52000:55500);
//fichier = 'Fer7_seul_4.wav';[z,Fs,bits]=wavread(fichier);s2 = z(2,:);s2test = s2(52000:55500);
//fichier = 'RecordAudio.wav';;[z,Fs,bits]=wavread(fichier);s2=z(2,14000:24000);s2test = s2;[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'test2_1_dehors_metal.wav';[z,Fs,bits]=wavread(fichier);s2=z(2,:);s2test = s2;[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'test2_2_dehors_metal.wav';[z,Fs,bits]=wavread(fichier);s2=z(2,:);s2test = s2;[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'test2_3_dehors_metal.wav';[z,Fs,bits]=wavread(fichier);s2=z(2,:);s2test = s2;[s2Centre, s2f] = plotFFT(s2test,Fs);
//fichier = 'test2_4_dehors_metal.wav';[z,Fs,bits]=wavread(fichier);s2=z(2,:);s2test = s2(1000:9000);[s2Centre, s2f] = plotFFT(s2test,Fs);
[z,Fs,bits]=wavread(fichier); // lecture du fichier wav
s2 = z(2,:);// prise en compte uniquement du canal 2, car le 1 n'est pas utilise dans mon appli
[s2Centre, s2f] = plotFFT(s2,Fs);

// pour le fer7_5
s2test = s2( 6700:7600);// swing
s2test = s2( 7600:8400);// swing
s2test = s2( 8500:10000);// swing
s2test = s2(10000:11000);// 
s2test = s2(11000:12000);// 

s2test = s2(6800:12500);// 


[s2Centre, s2f] = plotFFT(s2test,Fs);
figure();plot(s2Centre);

s2backswing = s2test(1:2100);
s2swing = s2test(2100:4000);
s2ball = s2test(4000:$);
figure();plot(s2test);
figure();plot(s2backswing);
figure();plot(s2swing);
figure();plot(s2ball);
plotFFT(s2test,Fs);
plotFFT(s2backswing,Fs);
plotFFT(s2swing,Fs);
plotFFT(s2ball,Fs);

[T,X1,H1,k,M,tt, f_y]=splitSignal(s2Centre,Fs);
figure();xset("colormap",hotcolormap(64));plot3d1(M(:,1:$/4));
[xx,yy,zz]=genfac3d(tt,f_y,M);
figure();xset("colormap",hotcolormap(64));plot3d1(xx,yy,zz);

///////////////////////////////////////////////////////////////////////////////////


[s2Centre, s2f] = plotFFT(s2test,Fs);

s2f =  filtrageVitesse(s2Centre);
//s2f =  filtrageVitesseSpin(s2Centre);
//s2f =  filtrage(s2Centre,1,3000);
[tt5,f5,M5,M]=animFFT(s2f,44100,128,0);

[i,j]=find(M5 == max(M5));
f5(j)/19.49 // vitesse max

/////////////////////////////////////////////////////////////////////////
//////// Calcul de la vitesse de balle suite à la animFFT ///////////////
/////////////////////////////////////////////////////////////////////////

fichier = 'test2_3_dehors_metal.wav';[z,Fs,bits]=wavread(fichier);s2=z(2,:);s2test = s2(100:9000);s2f =  filtrage(s2, 0, 28000);
[tt5,f5,M5]=animFFT(s2f,44100, 512 ,50 , 1);
fichier = 'test2_4_dehors_metal.wav';[z,Fs,bits]=wavread(fichier);s2=z(2,:);s2test = s2(1:14000);
[s2Centre, s2f, MaxFreq] = plotFFT(s2,Fs);
s2f =  filtrage(s2, MaxFreq-100, MaxFreq+1);// filtrage pour le spin
[M5,tt5,f5]=animDensite(s2f,44100, 512 ,10 , 1, 0.0,1);
[m,k]=max(M5,'c');
figure();plot(tt5,f5(k)/19.49);figure();plot(f5,sum(M5,'r'));
fichier = 'test2_3_dehors_metal.wav';[z,Fs,bits]=wavread(fichier);s2=z(2,:);s2test = s2(100:9000);s2f =  filtrage(s2, 100, 2000);
[tt5,f5,M5]=animFFT(s2f,44100, 512 ,50 , 1);
fichier = 'test2_3_dehors_metal.wav';[z,Fs,bits]=wavread(fichier);s2=z(2,:);s2test = s2(100:9000);s2f =  filtrage(s2, 1000, 6000);
[tt5,f5,M5]=animFFT(s2f,44100, 512 ,50 , 1);
tic;[tt5,f5,M5]=animFFT(s2,44100, 512 ,50 , 1);
[VClub, VBall] = Info(M5);toc



// l'idee est de déterminer l'angle d'envole entre le SmashFactor donnee par le constructeur
// et le smashfactor observe. Du coup on obtien le loft dynamic
// en appairant la vitesse du club mesuré au radar et celui calculé par le modèle.
// attention toute fois, il faut revoir la définition de la vitesse de club par le radar.
// la vitesse max du club n'est pas la vitesse du club à l'impact....
// impact club = début du vol de balle (T(vitesse enregistrée max))

[V0Clubkmh,dynamicLoft_deg] = BallGolf(VBall, 30, 0,VBall / VClub,0.04545,0.2)
[t,VOL,Res] = Golfball(18, VClub, '7', 0,0,dynamicLoft_deg-30 ,1);

/////////////////////////////////////////////////////////////////////////////
f = figure();subplot(1,6,1);g=gca();g.axes_reverse=["on","off","off"];plot(sum(M5.^2,'r'),f5);subplot(1,2/3,1);f.color_map = jjetcolormap(64);Sgrayplot(tt5,f5,M5);title("Fenetre 3000 pts Hz non filtré");xlabel("s");ylabel("Hz");
f = figure();f.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M5);title("Fenetre 3000 pts Hz non filtré");xlabel("s");ylabel("Hz");
f = figure();f.color_map = matcolormap(64);Sgrayplot(tt5,f5,M5);title("Fenetre 3000 pts RPM non filtré");xlabel("s");ylabel("Hz");
f = figure();f.color_map = hotcolormap(64);Sgrayplot(tt5,f5,M5);title("Fenetre 3000 pts Hz non filtré");xlabel("s");ylabel("Hz");
f = figure();f.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M);title("Fenetre 3000 pts Hz non filtré");xlabel("s");ylabel("Hz");

/////////////////////////////////////////////////////////////////////////////////////

s2Centre2 = sin(Fvb*2*%pi*t+%pi/4);
a=angle(fft(s2_filtreWavelet));
figure();subplot(2,1,1);plot(a)
subplot(2,1,2);plot(abs(real(fft(s2Centre))))

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
fichier = 'fer7Radar.wav';
[z,Fs,bits]=wavread(fichier); // lecture du fichier wav
s2 = z(2,:);// prise en compte uniquement du canal 2, car le 1 n'est pas utilise dans mon appli
[s2Centre, s2f] = plotFFT(s2(1:10000),Fs);
s2f =  filtrageVitesse(s2Centre);
[tt5,f5,M5,M]=animFFT(s2f,44100,300,0);

s2f =  filtrage(s2,14000,22000);
[s2Centre, s2f] = plotFFT(s2f,Fs);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
fichier = 'Fer7_5.wav';
[z,Fs,bits]=wavread(fichier); // lecture du fichier wav
s2 = z(2,:);// prise en compte uniquement du canal 2, car le 1 n'est pas utilise dans mon appli
[s2Centre, s2f] = plotFFT(s2(6800:11300),Fs);
s2f =  filtrageVitesseSpin(s2Centre);
[tt5,f5,M5,M]=animFFT(s2f,44100,600,0);




//////////////////////////////////////////////////////////////////////////////////////////////////////////:
//////////////////////////////////////////////////////////////////////////////////////////////////////////:
figure();plot(1);ax = gca();ax.auto_clear = 'on';
Fs = 44100;
N = 512;//Fs*0.01;
f = Fs*(0:(N)-1)/N;
[wft,wfm,fr] = wfir('bp',64,[1000/Fs 6000/Fs],'hm',[-1 -1]); // définition de la fenetre du filtre passa bande
//[s2f zf] = filter(wft,1,s2); // filtrage du signal par le filtre passe bande

while %T
    //y = pa_recordwav(N,Fs ,1);
    s2_fft = fft(filter(wft,1,pa_recordwav(N,Fs ,1)'));
    //s2_fft = abs(real(fft(filter(wft,1,pa_recordwav(N,Fs ,1)'))));
    //plot(f(2:$/2),s2_fft(2:$/2));
    s2_densite_real = abs(real(s2_fft.*conj(s2_fft)));
    plot(f(2:$/16),s2_densite_real(2:$/16));
    [m,k]=max(s2_densite_real);
    //m
    //f(k)/19.49
end

/////////////////////////////////////////::
// affichage continue de la capture sonnore
//figure();plot(1);ax = gca();ax.auto_clear = 'on';
Fs = 44100;
N1 = 512;//Fs*0.1;// nb echantillon
f1 = Fs*(0:(N1)-1)/N1;
N2 = 512;//Fs*1.0;// nb echantillon
N3 = N1 + N2;// nb echantillon
f2 = Fs*(0:(N2)-1)/N2;
f3 = Fs*(0:(N3)-1)/N3;
[wft,wfm,fr] = wfir('bp',64,[1000/Fs 6000/Fs],'hm',[-1 -1]); // définition de la fenetre du filtre passa bande
tic;
while %T
    //tic();
    s1 = pa_recordwav(N1,Fs ,1)';//toc();tic();
    //s1_fft = abs(real(fft(filter(wft,1,pa_recordwav(N1,Fs ,1)'))));
    //plot(s1);
    if max(abs(s1))>0.3 then
        s2 = pa_recordwav(N2,Fs ,1)'; 
        break;
    end;//toc();
end;
s2 = [s1 s2];
plot(s1);
figure();plot(s2);
s2_fft = abs(real((fft(filter(wft,1, s2)))));
figure();plot(f3(2:$/8),s2_fft(2:$/8));
conjuguee_s2 = conj(s2_fft);
s2_densite = s2_fft.*conjuguee_s2;
s2_densite_real = abs(real(s2_densite));
figure();plot(f3(2:$/8),s2_densite_real(2:$/8));
[m,k]=max(s2_densite_real);
[M5,tt5,f5]=animDensite(s2,44100, 512 ,16 , 1, 0.0,1);
/////////////////////////////////////////////////////////////////////
[i,j]=find(M5 == max(M5));
a=f5(j);
b=a(1:$-1);
c=a(2:$);
plot(c-b);

a=max(M5,'r');// amplitude en fonction de la fréq
b=a(1:$-1);
c=a(2:$);
figure();plot(f5(1:$-1),c-b);
figure();plot(f5,a);


aa=max(M5,'c');// amplitude en fonction de la tu temps
bb=aa(1:$-1);
cc=aa(2:$);
figure();plot(tt5(1:$-1),cc-bb);
figure();plot(tt5,aa);
/////////////////////////////////////////////////////////////////////

s2test=(s2Centre(2200:4800));


[s2Centre, s2f, MaxFreq] = plotFFT([s2test],Fs);
s2f =  filtrage(s2, MaxFreq-100, MaxFreq+1);
[s2Centre, s2f, MaxFreq] = plotFFT([s2test, zeros(1,2000)],Fs);
s2f =  filtrage(s2, MaxFreq-100, MaxFreq+1);
[M5,tt5,f5]=animDensite(s2f,44100, 512 ,10 , 1, 0.05,1);

///////////////////////////////////////////////////////////////////////

fichier = 'test2_2_dehors_metal.wav';[z,Fs,bits]=wavread(fichier);s2=z(2,:);s2test = s2(4001:12192);
//s2test  = s2(6000:12192);figure();plot(s2test);// vecteur uniquement de la ball
//[s2Centre, s2f, MaxFreq] = plotFFT(s2test,Fs);
Fs = 44100;
N=size(s2test,'c'); // definition du nombre d'échantillons
fq=Fs*(0:(N)-1)/N;
tau = 1 / Fs; // interval temporel de l'echantillonnage
t = (0:N - 1) * tau; // construction du vecteur temps
[wft,wfm,fr] = wfir('bp',64,[600/Fs 6000/Fs],'hm',[-1 -1]);
s2Centre = s2test - mean(s2test);

[s2f zf] = filter(wft,1,s2Centre);
[M5,tt5,f5]=animDensite(s2f,44100, 512 ,16 , 1, 0.0,1);
fig = figure();fig.color_map = jetcolormap(128);Sgrayplot(tt5,f5,M5);xlabel("s");ylabel("Hz");
fig = figure();fig.color_map = jetcolormap(2);Sgrayplot(1:size(T,1),1:size(T,2),T);
s2f_fft = fft(s2f);
s2f_fft_real = abs(real(s2f_fft));
[m,n] = max(s2f_fft_real(2:$/2));
a = fq(2:$/2);
MaxFreq = a(n);
s2f =  filtrage(s2Centre, (MaxFreq-1), (MaxFreq+1));// filtrage pour centrer sur le spin
[M5,tt5,f5]=animDensite(s2f,44100, 512 ,16 , 1, 0.0,1);

[vClub, vBall, SmashFactor] = Info(M5,f5)
// filtrage pour la tête de club ubiquement
s2f =  filtrage(s2Centre, (MaxFreq-1000)/SmashFactor, (MaxFreq-200)/SmashFactor);// filtrage pour la tête de club ubiquement
[M5,tt5,f5]=animDensite(s2f,44100, 512 ,16 , 1, 0.0,1);
[M5,tt5,f5]=animDensite(s2Centre,44100, 512 ,16 , 1, 0.0,1);
[thetaLoft, ShafLeanImp, launchAngle, spin] = LaunchAngle(vBall, vClub, '7')
[t,VOL,Res] = Golfball(18, vClub, '7', 0,0,ShafLeanImp ,1);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

s2test  = s2(7000:10096);figure();plot(s2test);// vecteur uniquement de la ball
Fs = 44100;
N=size(s2test,'c'); // definition du nombre d'échantillons
fq=Fs*(0:(N)-1)/N;

Nf = 512;
Nr = 16;
s2Centre = s2test - mean(s2test);

y = sgolayfilt(s2Centre,12,23);

plot(s2Centre);
plot(y,'r');
s2_fft = fft(s2Centre);
s2_densite = abs(real(s2_fft.*conj(s2_fft)));



// Visualisation de la vitesse de balle et spin
// entre 2000tr/min et 10000 tr/min soit entre (freq_Ball - 24Hz) à (freq_Ball - 121Hz)
figure();plot(fq(1:$/2),s2_densite(1:$/2));
plot(fq(1:$/2),dbv(s2_densite(1:$/2)),'r');
plot(fq(1:$/2),s2_densite(1:$/2).* dbv(s2_densite(1:$/2)),'y');
[M5,tt5,f5]=animDensite(s2Centre,44100, 512 ,16 , 1, 0.0,1);
fig = figure();fig.color_map = jetcolormap(128);Sgrayplot(tt5,f5,M5);xlabel("s");ylabel("Hz");

// vitesse de la balle = Freq_Max
[m,n] = max(s2_densite(1:$/2).* dbv(s2_densite(1:$/2)));
a = fq(2:$/2);
MaxFreq = a(n);

// recherche du rpm du Spin
MaxSpin = MaxFreq - 121;
MinSpin = MaxFreq - 30;
[iMin,jMin]=find(fq > MaxSpin);
[iMax,jMax]=find(fq < MinSpin);
fq_fen = fq(jMin(1):jMax($));
s2_densite_fen = s2_densite(jMin(1):jMax($));
figure();plot(fq(jMin(1):jMax($)),s2_densite(jMin(1):jMax($)))

[val, pos] = max(s2_densite_fen);

Spin = Freq2RpmSpin(MaxFreq - max(fq_fen(pos)));

/////////
s2_fft = fft(y);
s2_densite = abs(real(s2_fft.*conj(s2_fft)));
plot(fq,s2_densite,'r');

plot(fq,abs(real(fft(s2Centre-y').*conj(fft(s2Centre-y')))));
plot(fq,abs(real(fft(s2Centre-s2f).*conj(fft(s2Centre-s2f)))));
[T,X1] = splitSignal(s2Centre,Fs, Nf, Nr);



plot(fq,signal/max(signal),'c');
plot(fq,(h1.*signal)/max((h1.*signal)),'r');
plot(fq,(h2.*signal)/max((h2.*signal)),'b');
plot(fq,(h3.*signal)/max((h3.*signal)),'b');
plot(T(516/2-1,1:$),'y');
plot(T(516/2,1:$),'b');
plot(T(516/2+1,1:$),'r');

TT = [];
for ii = 1:size(M5,2)
    TT(ii,:) = splitSignal(M5(ii,:),Fs, Nf/2, Nr);
end;

plot(f5,M(1,1:$/2))


[m,k]=max(M5,'c');
figure();plot(tt5,f5(k)/19.49);figure();plot(f5,sum(M5,'r'));

dT = tt5(2:$) - tt5(1:$-1);
dT($+1) = dT($);
figure();plot(tt5,f5(k)'.*dT/19.49)

Distance = f5(k)'.*dT/19.49;
SumD = 0;
for ii=2:size(Distance,1)
    SumD(ii) = SumD(ii-1)+Distance(ii-1);
end

figure();plot(tt5,SumD)

















[m,k]=max(s2Centre);
t(k)









plot(tt5(2:$),SumD(2:$))./tt5(2:$))

plot3d1(tt5'*1e5,f5,M5*1e4)
////////////////////////////////////////////////////////////////////////////////


fichier = 'Fer7_seul_4.wav';[z,Fs,bits]=wavread(fichier);s2=z(2,:);s2test = s2(4001:12192);

//s2test  = s2(6000:12192);figure();plot(s2test);// vecteur uniquement de la ball
//[s2Centre, s2f, MaxFreq] = plotFFT(s2test,Fs);
Fs = 44100;
N=size(s2test,'c'); // definition du nombre d'échantillons
fq=Fs*(0:(N)-1)/N;
tau = 1 / Fs; // interval temporel de l'echantillonnage
t = (0:N - 1) * tau; // construction du vecteur temps
[wft,wfm,fr] = wfir('bp',64,[2000/Fs 2001/Fs],'hm',[-1 -1]);
s2Centre = s2test - mean(s2test);

[s2f zf] = filter(wft,1,s2Centre);
[M5,tt5,f5]=animDensite(s2f,44100, 512 ,16 , 1, 0.0,1);
fig = figure();fig.color_map = jetcolormap(128);Sgrayplot(tt5,f5,M5);xlabel("s");ylabel("Hz");
fig = figure();fig.color_map = jetcolormap(4);Sgrayplot(tt5,f5,M5);xlabel("s");ylabel("Hz");

///////////////////////////////////////////////////////////////////////////////////////////////











[wft,wfm,fr] = wfir('bp',64,[0.03 0.08],'hm',[-1 -1]);
s2 = filter(wft,1, test1(Fs*5.8:Fs*6.1));
tic;[M5,tt5,f5]=animDensite(s2,44100, 512 ,16 , 1, 0.0,1);toc
[m,k]=max(M5,'c');figure();plot(tt5,f5(k)/19.49);
figure();plot(f5,sum(M5,'r'));[m,k]=max(sum(M5,'r'));
f(k)/19.49

tic;[M5,tt5,f5]=animDensite(s2,44100, 512 ,16 , 1, 0.0,0);toc
fig = figure();fig.color_map = jetcolormap(128);
plot(1);ax = gca();ax.auto_clear = 'on';
for ii=1:size(tt5,1)
    //Sgrayplot(tt5,f5,M5);xlabel("s");ylabel("Hz");
    plot(f5,M5(ii,:));
    //sleep(50);
end;


/////////////////////////////////////////////////////////////////////////////////////////////////////

fichier = 'Serie22.wav';[s2,Fs,bits]=wavread(fichier);
test = s2;
Fs = 44100;
nbEchantillon = 1024*1;
f1 = Fs*(0:(nbEchantillon)-1)/nbEchantillon;
//fig = figure();fig.color_map = hotcolormap(128);drawlater();plot(1);ax = gca();ax.auto_clear = 'on';tic;
[wft,wfm,fr] = wfir('bp',64,[0.04 0.09],'hm',[-1 -1]);
// boucle de détection de début de swing, ou le joueur bouge, pas la vitesse max
for ii=2:size(test , 2)/nbEchantillon,
    //tic;
    //drawnow();
    //drawlater();
    //[M5,tt5,f5]=animDensite(test((ii)*nbEchantillon+1:nbEchantillon*ii),44100, nbEchantillon ,16 , 0, -100.0,1);
    //fig = figure();fig.color_map = jetcolormap(128);Sgrayplot(tt5,f5,M5);
    s2_fft = abs(real(fft(filter(wft,1, test((ii)*nbEchantillon+1:nbEchantillon*(ii+1))))));
    if (max(s2_fft,'c') > 3) then,
        break;
    end;
    //toc
    //Sgrayplot(tt5,f5,M5);
    //plot(f(1:$/2), s2_fft(1:$/2));
end;//toc
[m,k] = max(s2_fft,'c');//vitesse probable du club
//drawnow();

////////// Plot de la FFT Club puis Balle juste avec la FFT //////////////////////////////////
s2_fft = abs(real(fft(filter(wft,1, test((ii-1)*nbEchantillon+1:nbEchantillon*(ii+1))))));
f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);
figure();plot(f1(1:$/2)/19.49, s2_fft(1:$/2)/max(s2_fft(1:$/2)),'r');
s2_fft = abs(real(fft(filter(wft,1, test((ii+2)*nbEchantillon+1:nbEchantillon*(ii+4))))));
f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);
plot(f1(1:$/2)/19.49, s2_fft(1:$/2),'b');f1(k)/19.49


/////////////////////////////////////  SIMULATION //////////////////////////////////////////
/////// recherche de la fonction sinus appérée à la fréquence sin(wt) = sin(2*%pi*F*t))
// Form a signal containing a 50 Hz sinusoid of amplitude 0.7 and a 120 Hz sinusoid of amplitude 1.
// S = 0.7*sin(2*pi*50*t) + sin(2*pi*120*t);
[wft,wfm,fr] = wfir('bp',64,[0.04 0.09],'hn',[-1 -1]);
[wft,wfm,fr] = wfir('bp',64,[2000/Fs 2001/Fs],'hm',[-1 -1]);
s2_fft = abs(real(fft(filter(wft,1, test((ii)*nbEchantillon+1:nbEchantillon*(ii+4))))));
[m,k] = max(s2_fft,'c');
f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);
t = (0:size(f1,2)-1)/Fs;
Ref = GenereSignalSimu(44100, 4096, 0.03, 120, 160, 5000,[1 1 1], 0);
[wft,wfm,fr] = wfir('bp',64,[150*19.49/Fs 169*19.49/Fs],'hm',[-1 -1]);
ff1 = Fs*(0:(size(Ref,2))-1)/size(Ref,2);
Ref_fft = abs(real(fft(filter(wft,1, Ref))));
plot(ff1(1:$/2)/19.49, Ref_fft(1:$/2)/max(Ref_fft(1:$/2)),'c');
plot(f1(1:$/2), s2_fft(1:$/2)/max(s2_fft(1:$/2)),'b');f1(k)/19.49
plot(f1(1:$/2), s2_fft(1:$/2)/max(s2_fft(1:$/2))-Ref_fft(1:$/2)/max(Ref_fft(1:$/2)),'g');f1(k)/19.49

[M5,tt5,f5]=animDensite(filter(wft,1, Ref),44100, 512 ,16 , 0, -75,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5/19.49);

t = (0:size(test,2)-1)/Fs;

test = test - Ref;
////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////
[m,k] = max(s2_fft,'c');
f1(k)/19.49
//[M5,tt5,f5]=animDensite(filter(wft,1, test),44100, 512 ,16 , 0, 0.0,1);tic
[M5,tt5,f5]=animDensite(filter(wft,1, test((ii-1)*nbEchantillon+1:nbEchantillon*(ii+3))),44100, 512 ,16 , 0, -75,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5/19.49);
[vClub, vBall, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info(M5,f5,'7')

MM5 = (M5 ==1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,MM5/19.49);
figure();plot(f5/19.49,sum(MM5,'r'));

a = sum(MM5,'c');
a(find(a==0)) = %nan;


s2_fft = abs(real(fft(filter(wft,1, test((ii-1)*nbEchantillon+1:nbEchantillon*(ii+3))))));f1 = Fs*(0:(4*nbEchantillon)-1)/(4*nbEchantillon);figure();plot(f1(1:$/2)/19.49, s2_fft(1:$/2));
s2_fft = abs(real(fft(filter(wft,1, test((ii)*nbEchantillon+1:nbEchantillon*(ii+3))))));f1 = Fs*(0:(3*nbEchantillon)-1)/(3*nbEchantillon);figure();plot(f1(1:$/2)/19.49, s2_fft(1:$/2));
[t,VOL,Res] = Golfball(18, vClub, '7', 0,-0,ShafLeanImp ,0);

///////////////////////////////////////////////////////////////////////////////
/////////////// détection précise de la vitesse initiale de la balle
[wft,wfm,fr] = wfir('bp',64,[0.04 0.09],'hm',[-1 -1]);
[M5,tt5,f5]=animDensite(filter(wft,1, test((ii)*nbEchantillon+1:nbEchantillon*(ii+3))),44100, 512 ,16 , 0, -90,1);
//fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5/19.49);
[vClub, vBall, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info(M5,f5,'7')
freqC = vBall*19.49/Fs ;[wft,wfm,fr] = wfir('bp',64,[freqC freqC+0.00001 ],'hm',[-1 -1]);
[tt5,f5,MB5]=animDensite(filter(wft,1, test((ii-1)*nbEchantillon+1:nbEchantillon*(ii+10))),44100, 512 ,16 , 0, -90.0,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,MB5/19.49);

/////////// spin
freqC = (vBall+5)*19.49/Fs ;[wft,wfm,fr] = wfir('bp',64,[freqC - 0.000001 freqC ],'hm',[-1 -1]);
[tt5,f5,MS5]=animDensite(filter(wft,1, test((ii)*nbEchantillon+1:nbEchantillon*(ii+20))),44100, 4096 ,16 , 0, -900.0,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,MS5/19.49);

/////////////// puis détection précise de la vitesse initiale du club
freqC = vClub*19.49/Fs ;[wft,wfm,fr] = wfir('bp',64,[freqC freqC+0.000001 ],'hm',[-1 -1]);
[tt5,f5,MC5]=animDensite(filter(wft,1, test((ii-1)*nbEchantillon+1:nbEchantillon*(ii+2))),44100, 512 ,16 , 0, -90.0,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,MC5/19.49);


[vClub, vBall, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info(M5,f5,'7')


M55 = -M5(1:$-1,:)+M5(2:$,:);
M55 = [0*(1:size(M55,'c'));M55];
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,(M5+M5.*M55)/19.49);

[m,k]=max(M5,'c');
figure();plot(tt5,f5(k)/19.49);
figure();plot(f5/19.49,sum(M5,'r'));
f_fft = abs(real(fft(f5(k)-mean(f5(k)))));;
ffft = Fs*(0:(size(f_fft,2))-1)/size(f_fft,2);
figure();
subplot(2,1,1);plot(ffft(1:$/2),f_fft(1:$/2));
a=angle(fft(f_fft(1:$/2)));
subplot(2,1,2);plot(ffft(1:$/2),a);

////////////////////////////////////////////////////////////////////////////////
// Etude de la variation de l'angle de l'objet eclaire en focntion de la vitesse doppler mesurée
//
// F = 2*V*(Ft/c)*Cos(theta) = 19.49*V*Cos(Theta)
// dF = 19.49*V*sin(theta)
Theta = [20*%pi/180; VOL(2:$,3)./VOL(2:$,1)];

plot(VOL(:,1),cos(atan(Theta))*160);





















//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Algo de détection de vitesse club et balle //////////////
/////////////////////////// avec la fft capturée                       //////////////
// lecture du signal
// lecture de largeur de spectre se fait à -20db
//

 forder = 64;
[wft,wfm,fr] = wfir('bp',forder,[0.044 0.09],'hm',[-1 -1]);
    //Calculate window coefficients
    //[win_l_,cwp_]=window(wtype,forder,ffpar);
    //wtype = 'hm';
    //ffpar = [-1 -1];
    forder = 64;
    fl = 0.044; // fcbasse = 2000/44100
    fh = 0.09; // fchaute = 6000/44100    no2=(forder-1)/2;
    ino2 = int(no2);
    xt=(-no2:no2);
    un=ones(1,forder);
    // hamming
    win_l=.54*un+.46*cos(2*%pi*xt/(forder-1));
    //hanning
    //win_l=.5*un+.5*cos(2*%pi*xt/(n-1));
    //Get forder samples of the appropriate filter type
    //hfilt_=ffilt(ftype,forder,fl,fh);
    //Calculate n samples of the sinc function
    //Band pass filter
    wc=%pi*(fh+fl);
    fl=(fh-fl)/2;
    //x=filt_sinc(forder,fl);
    wl   = fl*2*%pi;
    xn   = sin(wl*(-no2:no2));
    xd   = %pi*(-no2:no2);
    if ino2==no2 then
        xn(no2+1) = 2*fl;
        xd(no2+1) = 1;
    end
    x=xn./xd;
    y=2*cos(wc*(-no2:no2));
    hfilt=x.*y;
    //Multiply window with sinc function
    wft_=win_l.*hfilt;



Fs = 44100;
nbEchantillon = 256*1;
fichier = 'serie211.wav';
[test,Fs,bits]=wavread(fichier); // serie211 : 122.68kmh et 154.23kmh
tic;
test = test - mean(test);
// boucle de détection de début de swing, ou le joueur bouge
// l'indice ii donne le début du signal
for ii=2:size(test , 2)/nbEchantillon,
    s2_fft = abs(real(fftpadding(filter(wft,1, test((ii)*nbEchantillon+1:nbEchantillon*(ii+1))))));
    if (max(s2_fft,'c') > 3) then,
        break;
    end;
end;//
// Identificatin de la vitesse de club
s2_fft = abs(real(fftpadding(filter(wft,1, test((ii-1)*nbEchantillon+1:nbEchantillon*(ii+3))))));//1024 echantillon.
//[s2_fft, f1] = FftFiltree(test((ii)*nbEchantillon+1:nbEchantillon*(ii+2)),Fs,1700,6000);
//f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);plot(f1(1:$/2)/19.49, s2_fft(1:$/2)/max(s2_fft(1:$/2)),'r');
//[M5,tt5,f5]=animDensite(filter(wft,1, (test((ii-5)*nbEchantillon+1:nbEchantillon*(ii+50)))),44100, 1024 ,16 , 1, -90,1);
[m,k] = max(s2_fft,'c');VClub = Fs*((k)-1)/size(s2_fft,2)/19.49;//vitesse probable du club, qui est la première détection de vitesse seuille

// Identification de la vitesse de balle probable
s2_fft = abs(real(fft(filter(wft,1, test((ii+7)*nbEchantillon+1:nbEchantillon*(ii+20))))));
//f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);plot(f1(1:$/2)/19.49, s2_fft(1:$/2)/max(s2_fft(1:$/2)));
//[s2_fft, f1] = FftFiltree(test((ii+4)*nbEchantillon+1:nbEchantillon*(ii+8)),Fs,1700,6000);
//[M5,tt5,f5]=animDensite(filter(wft,1, (test((ii+3)*nbEchantillon+1:nbEchantillon*(ii+8)))),44100, 256 ,16 , 1, -90,1);
//f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);plot(f1(1:$/2)/19.49, s2_fft(1:$/2)/max(s2_fft(1:$/2)),'b');
[m,k] = max(s2_fft,'c');VBall = Fs*((k)-1)/size(s2_fft,2)/19.49;//vitesse probable de la balle, qui est la première détection de vitesse seuil f1 = Fs*((k)-1)/size(s2_fft,2)

// identification précise de la vitesse de la balle avec un filtrage passe bande fin
freqC = VBall*19.49/Fs ;[wft,wfm,fr] = wfir('bp',forder,[freqC-0.000001 freqC+0.000001 ],'hm',[-1 -1]);
s2_fft = abs(real(fftpadding(filter(wft,1, test((ii+7)*nbEchantillon+1:nbEchantillon*(ii+20))))));
//
//f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);
//plot(f1(1:$/2)/19.49, s2_fft(1:$/2)/max(s2_fft(1:$/2)),'g');
[m,k] = max(s2_fft,'c');VBall = Fs*((k)-1)/size(s2_fft,2)/19.49;//vitesse probable de la balle, qui est la première détection de vitesse seuil
toc
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////
//
// Filtrage pour déterminer le spin balle
//
///////////////////////////////////////////
tic;
[wft1,wfm1,fr1] = wfir('bp',forder,[(VBall*19.49-RpmSpin2Freq(7000))/Fs (VBall*19.49-RpmSpin2Freq(3000))/Fs],'hm',[-1 -1]);
[wft2,wfm2,fr2] = wfir('sb',forder+1,[(VBall*19.49-100)/Fs (VBall*19.49+100)/Fs],'hm',[-1 -1]);
[wft3,wfm3,fr3] = wfir('bp',forder,[(VBall*19.49+RpmSpin2Freq(3000))/Fs (VBall*19.49+RpmSpin2Freq(7000))/Fs],'hm',[-1 -1]);
[wft4,wfm4,fr4] = wfir('bp',forder+1,[(VBall*19.49+RpmSpin2Freq(3000))/Fs (VBall*19.49+RpmSpin2Freq(15000))/Fs],'hm',[-1 -1]);

//[wft,wfm,fr] = wfir('bp',forder+1,[(VBall-0.5)*19.49/Fs (VBall+0.5)*19.49/Fs],'hm',[-1 -1]);
//s2_fft = abs(real(fftpadding(filter(wft,1, (x)))));
//[s2_fft, f1] = FftFiltree(test((ii+4)*nbEchantillon+1:nbEchantillon*(ii+8)),Fs,(VBall-0.5)*19.49 , (VBall+0.5)*19.49);
//f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);
//plot(f1(1:$/2), s2_fft(1:$/2)/max(s2_fft(1:$/2)),'g');

s2_fftb = abs(real(fft( filter(wft1,1, filter(wft2,1, x)))));
//[s2_fftb, f1] = FftFiltree(test((ii+4)*nbEchantillon+1:nbEchantillon*(ii+8)),Fs,(VBall*19.49-RpmSpin2Freq(10000)) , VBall*19.49-RpmSpin2Freq(3000));
//f1 = Fs*(0:(size(s2_fftb,2))-1)/size(s2_fftb,2);
//plot(f1(1:$/2), s2_fftb(1:$/2)/max(s2_fftb(1:$/2)),'b')
[m k]=max(s2_fftb); MIN = (f1(k));

s2_fftc = abs(real(fft( filter(wft4, 1,filter(wft2, 1,x)))));
//[s2_fftc, f1] = FftFiltree(test((ii+4)*nbEchantillon+1:nbEchantillon*(ii+8)),Fs,(VBall*19.49+RpmSpin2Freq(3000)) , (VBall*19.49+RpmSpin2Freq(10000)));
//f1 = Fs*(0:(size(s2_fftc,2))-1)/size(s2_fftc,2);
//plot(f1(1:$/2), s2_fftc(1:$/2)/max(s2_fftc(1:$/2)),'r')
[m k]=max(s2_fftc); MAX = (f1(k));
toc
[Freq2RpmSpin(VBall*19.49-MIN) Freq2RpmSpin(-VBall*19.49+MAX)]

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Ref_total = GenereSignalSimu(44100, 4096, 0.03, VClub, VBall, [Freq2RpmSpin(VBall*19.49-MIN) Freq2RpmSpin(-VBall*19.49+MAX)], [1, 1, 0], 0);
ff1 = Fs*(0:(size(Ref_total,2))-1)/size(Ref_total,2); 
[wft,wfm,fr] = wfir('bp',forder,[(VBall-5)*19.49/Fs (VBall+5)*19.49/Fs],'hm',[-0 -0]);
Ref_total_fft = abs(real(fftpadding(filter(wft,1,Ref_total))));

plot(ff1(1:$/2), Ref_total_fft(1:$/2)/max(Ref_total_fft(1:$/2)),'b-o');

[wft1,wfm1,fr1] = wfir('bp',forder,[(VBall*19.49-RpmSpin2Freq(9000))/Fs (VBall*19.49+RpmSpin2Freq(9000))/Fs],'hm',[-1 -1]);
s2_fftb = abs(real(fftpadding( filter(wft1,1, x))));
f1 = Fs*(0:(size(s2_fftb,2))-1)/size(s2_fftb,2);plot(f1(1:$/2), s2_fftb(1:$/2)/max(s2_fftb(1:$/2)),'r');

[wft,wfm,fr1] = wfir('bp',forder,[(MIN)/Fs (MAX)/Fs],'hm',[-1 -1]);
[M5, tt5, f5]= animDensite(filter(wft,1, Ref_total),44100, 4096 ,64 , 0, -0,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5(:,1:$-1)/19.49);

[M5, tt5, f5]= animDensite(filter(wft,1, x),44100, 4096 ,64 , 0, -0,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5(:,1:$-1)/19.49);

xd = wden(x,'heursure','s','one',1,'sym8');figure();plot(t,xd);
s2_fft = abs(real(fftpadding( ( xd))));
f1 = Fs*(0:(size(xd,2))-1)/size(xd,2); 
figure();plot(f1(1:$/2), s2_fft(1:$/2)/max(s2_fft(1:$/2)),'b');
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




// calcul du signal de référence simulé en fonction de la vitesse de balle pour identifier le spin
t = (0:size(f1,2)-1)/Fs;
spin = 7634;//a déterminer

Ref_total = GenereSignalSimu(44100, 4096, 0.03, VClub, VBall, spin, [1, 1, 0], 0);
ff1 = Fs*(0:(size(Ref_total,2))-1)/size(Ref_total,2); 

[wft,wfm,fr] = wfir('bp',forder,[(VBall-5)*19.49/Fs (VBall+5)*19.49/Fs],'hm',[-0 -0]);
Ref_total_fft = abs(real(fftpadding(Ref_total)));//
Ref_total_fft = abs(real(fftpadding(filter(wft,1,Ref_total))));

Ref_spin  = GenereSignalSimu(44100, 4096, 0.03, VClub, VBall, spin, [0, 1, 0], 0);
[wft,wfm,fr] = wfir('bp',forder,[(VBall*19.49+RpmSpin2Freq(3000)))/Fs (VBall*19.49+RpmSpin2Freq(3000)))/Fs],'hm',[-1 -1]);
Ref_spin_fft = abs(real(fftpadding( Ref_spin)));//

Ref_ball = GenereSignalSimu(44100, 4096, 0.03, VClub, VBall, spin, [1, 0, 0], 0);
[wft,wfm,fr] = wfir('bp',forder,[(VBall-5)*19.49/Fs (VBall+5)*19.49/Fs],'hm',[-1 -1]);
Ref_ball_fft = abs(real(fftpadding(Ref_ball))); // 

//////////////////////////////////////////
//
// Affichage des courbes simulées et captées
//
/////////////

figure();
plot2d(ff1(1:$/2)/19.49, [Ref_total_fft(1:$/2)/max(Ref_total_fft(1:$/2)); Ref_ball_fft(1:$/2)/max(Ref_ball_fft(1:$/2)); Ref_spin_fft(1:$/2)/max(Ref_spin_fft(1:$/2))]');
f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);
plot(f1(1:$/2)/19.49, s2_fft(1:$/2)/max(s2_fft(1:$/2)),'r');
legends(['Signal Total: VClub: ' + string(VClub)+' kmh';'Signal Ball: ' + string(VBall) + ' kmh';'Signal Spin: '+ string(spin)+'rpm';'Signal Capturé'],[1 2 3 5],opt=1);


[M5,tt5,f5]=animDensite(filter(wft,1, test((ii+4)*nbEchantillon+1:nbEchantillon*(ii+19))),44100, 1024 ,16 , 0, -90,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5/19.49);
[vClub, vBall, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info(M5,f5,'7')


///////////////////////////////////////////////////
//
// Vue d'une coupe bande fin sur la vitesse
// identification précise de la vitesse de la balle avec un filtrage passe bande fin
//
//////////////

forder = 64;
x = test((ii+4)*nbEchantillon+1:nbEchantillon*(ii+8));
freqC = VBall*19.49/Fs ;[wft1,wfm1,fr1] = wfir('bp',forder,(VBall-5)*19.49/Fs (VBall +5)*19.49/Fs],'hm',[-1 -1]);
//freqC = VBall*19.49/Fs ;[wft2,wfm2,fr2] = wfir('sb',forder+1,[(VBall-0.5)*19.49/Fs (VBall+0.5)*19.49/Fs],'hm',[-1 -1]);
//s2_fft = abs(real(fftpadding(filter(wft2,1, filter(wft1,1, x)))));


[m k]=max(s2_fft);MIN = (f1(k)-31); MAX = (f1(k)+31);
ll = find(((f1) >= MIN) & ((f1) <= MAX));
s2_fftb = s2_fft;
s2_fftb(ll) = 0;
f1 = Fs*(0:(size(s2_fftb,2))-1)/size(s2_fftb,2);plot(f1(1:$/2)/19.49, s2_fftb(1:$/2)/max(s2_fftb(1:$/2)),'b');


[m k]=max(s2_fft(1:ll(1))); MIN = (f1(k)); [m k]=max(s2_fft(ll($):$/2));MAX = (f1(ll($)+k-1));


[wft1,wfm1,fr1] = wfir('bp',forder,[(VBall*19.49+RpmSpin2Freq(3000))/Fs (VBall*19.49+RpmSpin2Freq(9000))/Fs],'hm',[-1 -1]);
s2_fftb = abs(real(fftpadding( filter(wft1,1, x))));
f1 = Fs*(0:(size(s2_fftb,2))-1)/size(s2_fftb,2);plot(f1(1:$/2), s2_fftb(1:$/2)/max(s2_fftb(1:$/2)),'r');

[wft1,wfm1,fr1] = wfir('bp',forder,[(VBall*19.49-RpmSpin2Freq(9000))/Fs (VBall*19.49-RpmSpin2Freq(3000))/Fs],'hm',[-1 -1]);
s2_fftb = abs(real(fftpadding( filter(wft1,1, x))));
f1 = Fs*(0:(size(s2_fftb,2))-1)/size(s2_fftb,2);plot(f1(1:$/2), s2_fftb(1:$/2)/max(s2_fftb(1:$/2)),'b');





/////////////////////////////////////////////////////////////////////////////////////////////////
// essais sur plusieurs fréquences ou echantillonnages avec fenêtre glissante
//
i=1;
freqC = (500*i)/Fs ;[wft,wfm,fr] = wfir('bp',64,[freqC freqC+0.000001 ],'hm',[-1 -1]);
[M5, tt5, f5]= animDensite(filter(wft,1, test((ii-2)*nbEchantillon+1:nbEchantillon*(ii+19))),44100, 4096/(2^i) ,16 , 0, -550,1);;
M5[];
for i=2:8;
    freqC = (3100)/Fs ;[wft,wfm,fr] = wfir('bp',64,[freqC freqC+0.000001 ],'hm',[-1 -1]);
    [M5, tt5, f5]=animDensite(filter(wft,1, test((ii-2)*nbEchantillon+1:nbEchantillon*(ii+19))),44100, 4096/(2^i) ,16 , 0, -190,1);
    fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5/19.49);
end
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5/19.49);












/////////////////////////////////////////
//
//          Wavelet
//          recherche du temps de la vitesse max
//
///////////////////////////////////////

fichier = 'serie211.wav';
[test,Fs,bits]=wavread(fichier); // serie211 : 122.68kmh et 154.23kmh
test = test - mean(test);
[wft,wfm,fr] = wfir('bp',64,[0.044 0.09],'hm',[-1 -1]);
Fs = 44100;
nbEchantillon = 1024*1;
// boucle de détection de début de swing, ou le joueur bouge
// l'indice ii donne le début du signal
for ii=2:size(test , 2)/nbEchantillon,
    s2_fft = abs(real(fft(filter(wft,1, test((ii)*nbEchantillon+1:nbEchantillon*(ii+1))))));
    if (max(s2_fft,'c') > 3) then,
        break;
    end;
end;//

freqC = (VBall*19.49)/Fs ;[wft,wfm,fr] = wfir('bp',64,[freqC freqC+0.000001 ],'hm',[-1 -1]);
x = test((ii-2)*nbEchantillon+1:nbEchantillon*(ii+15));
f1 = Fs*(0:(size(x,2))-1)/size(x,2);
t = (0:size(x,2)-1)/Fs;

level = 2;
//[cA,cD] = dwt(filter(wft,level, x),'haar');
[C,L]=wavedec(filter(wft,1, x),level,'haar');wavedecplot(C,L);
scf();clf();
subplot(511)
plot(x,'r')
subplot(512)
plot(C(1:L(1)))
subplot(513)
s2_filtreWavelet = C(L(1):sum(L(1:2)));
plot(s2_filtreWavelet,'g');
//f1 = Fs*(0:(size(s2_filtreWavelet,2))-1)/size(s2_fft,2);
//plot(f1,abs(real(fft(filter(wft,1, s2_filtreWavelet)))));
subplot(514)
plot(C(sum(L(1:2)):sum(L(1:3))),'b');//detcoef(C,L,2)
subplot(515)
plot(C(sum(L(1:3)):sum(L(1:4))),'c');//plot(detcoef(C,L,2))

level = 2;
[C,L]=wavedec(filter(wft,1, x),level,'haar');wavedecplot(C,L);
d1 = detcoef(C, L, level);
d1up = dyadup(d1,0);
subplot(211)
plot(t,x)
subplot(212)
plot(d1up)




freqC = (VBall*19.49)/Fs ;[wft,wfm,fr] = wfir('bp',64,[freqC freqC+0.000001 ],'hm',[-1 -1]);
[M5, tt5, f5] = animDensite(filter(wft,1, x),44100, 512 ,64 , 0, -70, 1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5/19.49);
figure();subplot(211);plot(f5/19.49,sum(M5,'r'));subplot(212);plot(tt5,sum(M5,'c'));
[m,k] = max(sum(M5,'c')); tt5(k);// temps du shoot
[m,k] = max(M5,'c');f5(142)/19.49://vitesse max

x = GenereSignalSimu(44100, 4096, 0.03, VClub, VBall, 6000,[1 1 1], 0);

N=256;
t = (0:size(x,2)-1)/Fs;
f1 = Fs*(0:N-1)/N;
scales = [1:1:N];
[coef] = cwt(filter(wft,1, x),scales,'mexh');
w=wcodemat(coef,N);
cmap=jetcolormap(N);
c=ind2rgb(w,cmap); // SIP function
imshow(c);
figure();plot(t,abs(coef(1,:))); 
f1(1) = .00001;
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(t,f1/19.49,abs(coef)');

f1 = Fs*(0:N-1)/60;figure();subplot(2,1,1);plot(f1/19.49,sum(coef,'c'));subplot(2,1,2);plot(t,sum(coef,'r'))
[m,k] = max(coef); t(k(2))
cwtplot(coef,scales,%t,64);p = graycolormap(256);
f = gcf();f.color_map=p;f.background=256;
// Calcul de la transformée en ondelettes continue
//w=TOC(fct,'OT_Mexicain(t,a,b)',a,b);
aa=gca();
// Légende du graphique sur X
legGradx=[];
// On veut 6 graduations
nbGradX=6
// Position des graduations
px=[];
for i=0:nbGradX-1
    ind=min(fix(i*size(coef,2)/(nbGradX-1))+1,size(coef,2));
    px=[px ind];
    legGradx=[legGradx; string(fix(100*t(ind))/100)];
end
aa.x_ticks = tlist(["ticks", "locations", "labels"], px', legGradx);
figure();plot(t,x);
plot2d(f1(k(2)-256:k(2)+1024),abs(real(fft(filter(wft,1, x(k(2)-256:k(2)+1024))))));

[thr,sorh,keepapp] = ddencmp('den','wv',x);
xd = wden(x,'heursure','s','one',1,'sym8');figure();plot(t,xd);
[M5,tt5,f5]=animDensite(filter(wft,1, xd),44100, 128 ,32 , 0, -550,1);

fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5/19.49);
[M5,tt5,f5]=animDensite(filter(wft,1, x(k(2)-256:k(2)+1024)),44100, 1024 ,16 , 0, -75,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5/19.49);


// Calcul de l'angle de décolage zMdfl3#j infini
// Vmesure = Vreelle * cos(launchAngle)
// on admet que sur la courte distance la vitesse est constante et correspond à
// la vitesse max (où la balle et pile poile dans le faisceau avec une incidence à 0)
// shoot à 0.06,
// le max est 156kmh à 0.1s soit (0.1-0.06)*156/3.6= 1.7333m
// et 149.7kmh à 0.25s soit (0.25-0.06)*156/3.6 = 8.233m, soit 0.15s plus tard ==> 0.15*156/3.6 = 6.5m
// donc en 6.5m la mesure à perdue 6.3kmh
// ainsi Vm2/Vm1 = cos(launchAngle) ==> acos(Vm2/Vm1)*180/%pi = 
// launchAngle = acos(149.7/156)*180/%pi = 16.34°

freqC = (VBall*19.49)/Fs ;
f1 = Fs*(0:(size(test,2))-1)/size(test,2);

[wft,wfm,fr] = wfir('bp',512,[freqC freqC+0.000001 ],'hm',[-1 -1]);
[wft]=ffilt('bp',64,200000,500000);

plot(f1,abs(real(fft(test)))/max(abs(real(fft(test)))));
plot(f1,abs(real(fft(filter(wft,1, test))))/max(abs(real(fft(filter(wft,1, test))))),'r');


[wft,wfm,fr] = wfir('bp',512,[freqC freqC+0.000001 ],'re',[-1 -1]);
figure();
plot(f1,abs(real(fft(test)))/max(abs(real(fft(test)))));
plot(f1,abs(real(fft(filter(wft,1, test))))/max(abs(real(fft(filter(wft,1, test))))),'g');

Ref_1 = GenereSignalSimu(44100, 4096, 0.02, VClub, VBall, Spin(2), [1 1 1], 0);
Ref_2 = GenereSignalSimu(44100, 4096, 0.02, VClub, VBall, Spin(2), [1 1 0], 0.2*%pi/2);
figure();plot([Ref_1 ; Ref_2]');




//////////////////////////////////////////////////////////////////////////
//
//              
//
////////////////////////////////////////////////////////
fichier = 'serie211.wav';
//fichier='serie11.wav';
[x,Fs,bits]=wavread(fichier); 
//x=x(2,:); 
Fs = 44100;
t = (0:size(xx,2)-1)/Fs;
figure();plot(t,x);// serie211 : 122.68kmh et 154.23kmh
[wft,wfm,fr] = wfir('bp',64,[2000/Fs 6000/Fs ],'re',[-1 -1]);
//wft=iir(3,'bp','ellip',[1000/Fs 6000/Fs ],[.08 .03]);
//[wfm,fr]=frmag(wft,256);
//xx = filter(wfm,1,x);
[VBall, VClub, Spin, SpinAxis,  xresult]=GetSwing(filter(wft,1,x), Fs, 2000, 6000, 3)
[thetaLoft, ShafLeanImp, launchAngle, SpinZ] = LaunchAngle(VBall, VClub, '7')
FaceAngle = 0;
[t,VOL,Res] = Golfball(18, VClub, '7', 0,FaceAngle, ShafLeanImp ,1);

xx = (x(size(x,2)/3:size(x,2)/3*2));
[wft,wfm,fr] = wfir('bp',64,[1500/Fs 6000/Fs ],'re',[-1 -1]);
[M5,tt5,f5]=animDensite(filter(wft,1,x),44100, 256 ,32 , 1, -50,1);

[tt,f,M1] = animFFT(filter(wft,1,xx),44100,512,16,1,0.5);


I=filter(wft,1,2*%pi*cos(t*Fs).*xx);
Q=filter(wft,1,2*%pi*sin(t*Fs).*xx);
enveloppe = (sqrt(I.^2+Q.^2));
t = (0:size(xx,2)-1)/Fs;
figure();
plot(t,enveloppe,'b');
plot(t,xx,'r');

xy = xx(1:99);
xy(1) = mean(xx(2:$));
for ii=2:size(xx,2)
    xy(ii) = mean(xx([1 :ii-1, ii+1: size(xx,2)]));
end
xy = xy';

//////////////////////////////////////////////////////////////

[tps,VOL,Res] = Golfball(18, 164, 'D', -0, -0, 2, 0);
VOL(find(VOL(:,1)==0),1) = 0.00001;
VTotale = sqrt(VOL(:,2).^2+VOL(:,4).^2+VOL(:,6).^2);
fichier = 'serie211.wav';
//fichier='serie11.wav';
[x,Fs,bits]=wavread(fichier); 
//x=x(2,:); 
Fs = 44100;
[wft,wfm,fr] = wfir('bp',64,[2150/Fs 4000/Fs ],'re',[-1 -1]);
[M5,tt5,f5]=animDensite(filter(wft,1,x),44100, 256 ,32 , 1, -18,1);
// angle de décollage
// composé de phi pout l'angle la hauteur par rapport à Y vers le haut
// théta pour la largeur par raport Z dérive sur la droite/gauche
// les radars sont disposés sur l'axe Y et Z?
// R1 la référence
// R2 30cm à droite sur l'axe Z
// R3 30cm en haut sur l'axe Y
d = 0.000000001; // pour éviter la division par zéro
dz = 0.3; // distance du 2èm ou 3èm radar par rapport au principal
position = 1.5; // distance du de la valise par rapport à la balle
L =(VOL(:,1)); // Longueur (x))
H = (VOL(:,3));// Hauteur (y))
l = (VOL(:,5));// largeur (z))

VL =(VOL(:,2)); // Longueur (x))
VH = (VOL(:,4));// Hauteur (y))
Vl = (VOL(:,6));// largeur (z))

// elevation
phi = asin((H) ./ (sqrt((L + position).^2 + (H).^2 + (l).^2) + d)); // ASIN(Hauteur / (sqrt( Largeur^2+Longueur^2 +Hauteur^2) + d))
phi(1) = 0*%pi/180;
phiz = asin((H) ./ (sqrt((L + position).^2 + (l-dz).^2 + (H).^2) + d));// // Phi avec le radar décallé latérallement
phiz(1) = 0*%pi/180;
phiy = asin((H-dz) ./ (sqrt((L + position).^2 + (l).^2 + (H-dz).^2) + d));// // Phi avec le radar décallé verticallement
phiy(1) = -90*%pi/180;

figure();title('Phi mesure d''Elevation si on décale de 30cm à droite sur Z le long du sol');
subplot(2,2,1);plot(L,[phi phiz phiy]*180/%pi);xlabel('philongueur phiHauteur m');ylabel('Dégre °');legend(['phi'; 'phil'; 'phiHy']);
subplot(2,2,2);plot(L,[(phiz-phi) (phiy-phi)]*180/%pi);xlabel('phi-phiz m');ylabel('Delta Dégre °');legend(['phiz-phi'; 'phiy-phi']);

// derive laterale
theta = acos((L + position)./(sqrt((L + position).^2+(l).^2 ) + d));//
theta(1) = 0;
thetaz = asin((l-dz)./(sqrt((L + position).^2+(l-dz).^2 ) + d));// 
thetaz(1) = 0;

title('Lateral mesure de la Derive si on monte de 30cm à droite sur Z le long du sol');
subplot(2,2,3);plot(L,[theta thetaz]*180/%pi);xlabel('thétaZ m');ylabel('Dégre °');legend(['theta'; 'thetaz']);
subplot(2,2,4);plot(L,(theta-thetaz)*180/%pi);ylabel('Delta Dégre °');xlabel('théta - thétaZ m');legend(['theta-thetaz']);

newV  = VTotale.*cos(phi).*cos(theta);
newVy = VTotale.*cos(phiy).*cos(theta);
newVy(1) = newV(1);
newVz = VTotale.*cos(phiz).*cos(thetaz);
figure();subplot(1,2,1);plot(L,[VTotale , newV, newVy, newVz]*3.6);legend(['V Reelle'; 'newV R0'; 'newVy Rup'; 'newVz Rright']);
subplot(1,2,2);plot(L,H);

155/cos(19*%pi/180)
[m,k]=max(M5,'c');
figure();plot(tt5,f5(k)/19.49);figure();plot(f5,sum(M5(:,1:$-1),'r'));
figure();plot(tt5,f5(k)'/19.49/3.6.*(tt5));

[n,l]=max(M5(a=find(M5>0.5)),'c');
figure();plot(tt5,f5(l)/19.49);figure();plot(f5,sum(M5(:,1:$-1),'r'));

//////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// DEPHASAGE entre 2 signaux : negatif = retard, positif = avance
// phi/lambda = tau/T ==> tau = phi / (lambda*F)
// tau = phi/(2*%pi*F)
// theta = 10*%pi/180
// D = lambda
// phi = (2*%pi/lambda)*D*sin(theta)
Fs = 44100;
t=0:0.01:5;
Vlumiere = 299792458 ; // m/s 
Freq = 10.525e9; // 1/s
lambda = Vlumiere / Freq;
F = 3000; // 3kHz
wt = 2*%pi*t;
dephasage = %pi/4;
s1=cos(wt);
s2=cos(wt+dephasage);

// plot des signaux 
plot(t,[s1; s2]);legend('s1','s2')
//calcul du spectre par TF
tfS1=fft(s1);
tfS2=fft(s2);
tfS12=fft(s1+s2);

//soit H la pseudo fct de transfert
H=tfS1./tfS2;
[fmax Amax]=max(abs(tfS1));

//phase_H=phase(H);
phase_H=angle(H);
f1 = Fs*(0:(size(tfS1,2))-1)/size(tfS1,2);
plot(f1(1:$/2),phase_H(1:$/2));
// On regarde la phase à la frequence ou TfS1 est max
phi = phase_H(Amax)

//On regarde la conséquence sur FFT 

plot(f1(1:$/2), abs(real(tfS1(1:$/2)/max(real((tfS1(1:$/2)))))),'r');
plot(f1(1:$/2), abs(real(tfS2(1:$/2)/max(abs(real(tfS2(1:$/2)))))),'b'); // ==> Aucun changement normale ;-)
plot(f1(1:$/2),real(tfS1(1:$/2).*conj(tfS1(1:$/2))/max(real((tfS1(1:$/2).*conj(tfS1(1:$/2)))))),'g');
// 
tau = phi / (lambda*F)
Theta = asin(tau * Vlumiere / 0.02) * 180/%pi
// tau = sin(10*%pi/180)*0.01/Vlumiere
/////////////////////////////////////////////////////////////////////////////

[c, lag] = xcorr(s1,s2);
[maxC,I]=max(c);
lag(I)

////////////////////////////////////////////////////////////////////////////
h1= hilbert(s1);
h2= hilbert (s2);
 
P1=unwrap(angle(h1));
P2=unwrap(angle(h2));
 
dephasageResult = mean(P2-P1)


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
fichier = 'serie211.wav';












//fichier='Serie34.wav';
//fichier = uigetfile("*.wav", "D:\Users\mm\Documents\Golf\Radar\");[x,Fs,bits]=wavread(fichier); xg = x(1,:);xd = x(2,:);
[x,Fs,bits]=wavread(fichier); xg = x(1,:);
Fs = 44100;
Vlumiere = 299792458 ; // m/s 
Freq = 10.525e9; // 1/s
lambda = Vlumiere / Freq;
v = 156/(1000*3600);
w = 5000/9.5493107;
t = 0; // Amplitude max pour T = 2*%pi, donc pour t=0;
r = 0.04267 /2;
Fdopp_B = (2/lambda)*(v + r*w*cos(w*t))
Rpm = (((Fdopp_B*lambda/2) - v) / r)*9.5493107
[wft,wfm,fr] = wfir('bp',64,[800/Fs 5000/Fs ],'rec',[-1 -1]);
[M5,tt5,f5]=animDensite(filter(wft,1,x),44100, 256 ,32 , 1, -18,1);
xfft = real(abs(fft( filter(wft,1,x))));
xdensite = abs(real(xfft.*conj(xfft)));
f1 = Fs*(0:(size(xfft,2))-1)/size(xfft,2);
f1 = Fs*(0:(size(xdensite,2))-1)/size(xdensite,2);
plot(f1(1:$/2),xfft(1:$/2)/max(xfft(1:$/2)));
plot(f1(1:$/2),xdensite(1:$/2),'r');


t=(0:44100)/44100;
xhat=1/50*cos ((2995*2*%pi*t) + 0.35* sin( 2*%pi*(800)*t)) ;
// x = xhat;
// A1*cos(porteuse + A2*sin(signalModulant)))
xfft = real(abs(fft( filter(wft,1,x));
f1 = Fs*(0:(size(xfft,2))-1)/size(xfft,2);
plot(f1(1:$/2),xfft(1:$/2)/max(xfft(1:$/2)),'r');

plot(xhat,'b'); 

// 3118.4kmh et 1559.2 kmh
// Carrier = Porteuse
// y = fmdemod(xModulated, Carrier amplitude signal, Message signal amplitude in volts, Carrier signal amplitude in volts, Carrier signal frequency in Hz, Sampling frequency in samples/sec)

f1 = Fs*(0:(size(x($/2.3:$/2.25),2))-1)/size(x($/2.3:$/2.25),2);
xfft = real(abs(fft( filter(wft,1,x($/2.3:$/2.25)))));
plot(f1(1:$/2), xfft(1:$/2)/max(xfft(1:$/2)),'r');
[A,B] = max(xfft);
maxXfft = (f1(B))

y = fmdemod(filter(wft,1,x($/2.3:$/2.25)), 15, maxXfft, Fs);plot(y);
f1 = Fs*(0:(size(y,2))-1)/size(y,2);
yfft = real(abs(fft( filter(wft,1,y))));
[A,B] = max(yfft);
figure();plot(f1(1:$/2), yfft(1:$/2)/max(yfft(1:$/2)),'b');
MAX = (f1(B))


fichier = uigetfile("*.wav", "C:\Users\mm\Documents\GolfBall-master\sci");[x,Fs,bits]=wavread(fichier); xg = x(1,$/1.4:$/1.25);xd = x(2,$/1.4:$/1.25);
//xg = x(1,1:$/1.15);xd = x(2,1:$/1.15);

[wft,wfm,fr] = wfir('bp',256,[1800/Fs 6000/Fs ],'hm',[-1 -1]);

[M52,tt5,f52]=animDensite(filter(wft,1,xg),44100, 256 ,32 , 1, -0, 1);
[M51,tt5,f51]=animDensite(filter(wft,1,xd),44100, 256 ,32 , 1, -0, 1);
figure();plot(tt5,sum(M51(:,1:$-1),'c'));plot(tt5,sum(M52(:,1:$-1),'c'),'r');
figure();plot(f5,sum(M51(:,1:$-1),'r')/max(sum(M51(:,1:$-1),'r')));plot(f5,sum(M52(:,1:$-1),'r')/max(sum(M52(:,1:$-1),'r')),'r');
figure();plot(f5,sum(M51($/2:$,1:$-1),'r')/max(sum(M51($/2:$,1:$-1),'r')),'b');plot(f5,sum(M52($/2:$,1:$-1),'r')/max(sum(M52($/2:$,1:$-1),'r')),'r');
[a,b] = max(sum(M51(:,1:$-1),'r'))
[c,d] = max(sum(M52(:,1:$-1),'r'))
//angle entre les 2 radars en degré.
//f(b) et f(d) sont les freq max !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
acos(f52(d)/f51(b))*180/%pi
[wft1,wfm,fr] = wfir('sb',255,[(f51(b)-50)/Fs (f51(b)+50)/Fs ],'hm',[-1 -1]);
[M52,tt5,f52]=animDensite(filter(wft1,1,(filter(wft,1,xg)),44100, 256 ,32 , 1, -0, 1);


fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M51(:,1:$-1));
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M52(:,1:$-1)/19.49);
[t,VOL,Res] = Golfball(8,110,'7',10,-15,-10,1);

[l,c]=find(M52(:,1:$-1)==1);
plot(tt5(l),f5(c),'b*');

//////////////////////////// HILBERT ///////////////////////////
/// Frequency demodulation: Demodulates the FM waveform   //////
/// by modulating the Hilbert transform of y by a complex //////
/// exponential of frequency -fc Hz and obtains the       //////
/// instantaneous frequency of the result.                //////
/// F(t) = d(phase)/dt / (2*%pi))  Frequence instantanéé  //////
///
fichier = uigetfile("*.wav", "D:\Users\mm\Documents\Golf\Radar\");[x,Fs,bits]=wavread(fichier); 
xg = x(1,1:$);xd = x(2,1:$);
xg = x(1,$/1.4:$/1.25);xd = x(2,$/1.4:$/1.25);
//xg = x(1,1:$/1.15);xd = x(2,1:$/1.15);
xg = x(1,$/4.:$/1.2);xd = x(2,$/4.:$/1.2);

[wft,wfm,fr] = wfir('bp',256,[1500/Fs 5000/Fs ],'re',[-1 -1]);
[wft1,wfm,fr] = wfir('sb',255,[2950/Fs 3100/Fs ],'re',[-1 -1]);
[wft2,wfm,fr] = wfir('lp',255,[4000/Fs 6000/Fs ],'re',[-1 -1]);
plot((diff(unwrap(angle(hilbert(filter(wft,1,filter(wft1,1,xg))))))/(2*%pi*1/Fs)));
plot((diff(unwrap(angle(hilbert(filter(wft,1,xg)))))/(2*%pi*1/Fs)),'r');

dt = 1/Fs;
fc = 2930; // carrier porteuse = vitesse de la balle
df = 500; // modulation excursion
fm = 1100; // modulation

tAx = dt:dt:0.1; // time axis in seconds
u = sin(2*%pi*fc*tAx + (df/fm)*cos(2*%pi*fm*tAx));
u = filter(wft,1,xg);
u = filter(wft,1,filter(wft1,1,xg)); // u = signal filtre
w = unwrap(angle(hilbert(u))); // xa = hilbert(u); PHASE = angle(hilbert(u));
v = (diff(w)/(2*%pi*dt)); // instantaneous frequency 
subplot(2,1,1);
t=(0:size(u,2)-1)/Fs;
plot(t,u/max(u))
subplot(2,1,2);
t=(0:size(v,2)-1)/Fs;
plot(t,filter(wft2,1,abs(v)),'r');
mean(v(50:$-50))
min(v(50:$-50))
max(v(50:$-50))
// definir l'enveloppe de l'espace de Hilbert et cela défini la plage de fréquence instantanée ==> Démodulation
[enveloppe,t] = Enveloppe(v, Fs,1500, 4000);
plot(t,enveloppe);

y = demod(filter(wft,1,xg),fc,Fs,'fm');plot(y,'r')
y = demod(u,fc,Fs,'fm');plot(y);//plot(u)
[M52,tt5,f5]=animDensite(u,44100, 256 ,32 , 0, 0,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M52(:,1:$-1));

xg=x(1,1:$);
xg = x(1,$/1.3:$/1.1);xd = x(2,$/1.3:$/1.1);
xg = x(1,$/4.:$/1.2);xd = x(2,$/4.:$/1.2);
xgf = filter(wft,1,xg); // passe band filtrage centre sur 1kHz 6kHz
xa = (hilbert(xgf));
xgi = (imag(hilbert(xgf)));
u = filter(wft1,1,xgf); // filtrage coupe bande sur la frequence de la balle
xgiu=(u.*xgi);
[M52,tt5,f5]=animDensite(xgf,44100, 256 ,32 , 0, -5,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M52(:,1:$-1));
f=figure();plot(f5,sum(M52($/2:$,1:$-1),'r'),'r'); // vitesse du shaft (1er pic), vitesse de la balle 2eme pic)

[M52,tt5,f5]=animDensite(filter(wft,1,xgiu),44100, 256 ,32 , 0, -155,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M52(:,1:$-1));
scf(f);
figure();plot(f5,sum(M52($/2:$,1:$-1),'r'),'b'); // vitesse du club (1er pic), vitesse de la balle 2eme pic)

figure();plot([0:size(xa,'c')-1]/Fs,u);
plot([0:size(xa,'c')-1]/Fs,abs(xa).^2,'r');


plot(f5,M52(120,1:$-1))

f=figure();f.color_map = jetcolormap(64);
w=wigner(xg,ones(1:64),12,128);
N = 69;
tau = 1 / Fs; // interval temporel de l'echantillonnage
t = (0:N - 1) * tau*1000; // construction du vecteur temps
plot3d1(t*10000,[1:64]*Fs/(64),abs(w(1:69,1:64))*1000);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(t,[1:64]*Fs/(64*1000),abs(w(1:69,1:64)));


tfrqview

//////////////////////////////////////////////////////
fichier = uigetfile("*.wav", "D:\Users\mm\Documents\Golf\Radar\");[x,Fs,bits]=wavread(fichier); 
xg = x(1,1:$);xd = x(2,1:$);
u = filter(wft,1,xg);
a=((diff(unwrap(angle(hilbert(filter(wft1,1,filter(wft,1,xg($/3.5:$/2)))))))/(2*%pi*1/Fs)));
a=((diff(unwrap(angle(hilbert(xg($/3.5:$/2)))))/(2*%pi*1/Fs)));
y = sgolayfilt(a,9,69)';
[M52,tt5,f5]=animDensite(a,44100, 256*4 ,32*4 , 0, -150,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M52(:,1:$-1));
figure();plot(f5,sum(M52($/2:$,1:$-1),'r'),'b'); // vitesse du club (1er pic), vitesse de la balle 2eme pic)
[m,k] = max(sum(M52($/2:$,1:$-1),'r'));f5(k)
Freq2RpmSpin(f5(k));


fichier = uigetfile("*.wav", "D:\Users\mm\Documents\Golf\Radar\");[x,Fs,bits]=wavread(fichier); xg = x(1,1:$);xd = x(2,1:$);
[vBall, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,a] = Info2(filter(wft,1,xg),'7'),fichier
N = size(xg,2);
dt = 1/Fs;
fc = 2930; // carrier porteuse = vitesse de la balle
df = 50; // modulation excursion
FreqRPM = 800; // modulation//Freq2RpmSpin(833) = 5273.0706 tr/min rpm = 552 rad/s =  1 rad/sec? The answer is 9.54929659643 RPM
dFreqRPM = 50;
m=dFreqRPM/FreqRPM;

vb=1500; // 150 kmh
r = 0.043/2;
c = 299792458 ;
lambda = c /10.525E9;
tAx = dt:dt:N/Fs; // time axis in seconds

at = 1;
modt = 1;
sa = at*exp(-%i*2*%pi*fc*tAx);

smod = modt*exp(-%i*2*(2*%pi*FreqRPM)*r*sin(2*%pi*FreqRPM*tAx)/lambda);
// signal du patch B:

//
S = modt*cos(2*%pi*fc*tAx+m*sin(2*%pi*FreqRPM*tAx));
clf;
F0 = vBall*19.49;
D=1.5;//:3;
[fm,am,iflaw] = doppler(N,Fs,F0,D,vBall/3.6,1);
subplot(211); plot(real(am.*fm));xtitle(("Signal "));
subplot(212); plot(iflaw);
[ifhat,t]=instfreq(sigmerge(am.*fm,smod,15),11:502,10);
plot(t,ifhat,'g');
xtitle(("Instantaneous frequency"))
legend([_("Requested"),_("Observed")]);
//[M52,tt5,f5]=animDensite(real(sigmerge((am.*fm)',noisecg(N)',15)),44100, 256 ,32 , 1, -150,1);
[M52,tt5,f5]=animDensite(real(sigmerge((am.*fm)',smod,1)),44100, 256 ,32 , 1, -150,1);
figure();plot(f5,sum(M52($/2:$,1:$-1),'r'),'b'); // vitesse du club (1er pic), vitesse de la balle 2eme pic)



[Y,IFLAW]=fmsin(N);
phi=if2phase(iflaw)



/////////////////////////////////
// Modele de signale Echo s(t) //
/////////////////////////////////
fichier = uigetfile("*.wav", "D:\Golf\Radar\");[x,Fs,bits]=wavread(fichier); 
xg = x(1,1:$);xd = x(2,1:$);

[wft,wfm,fr] = wfir('bp',256,[1300/Fs 4000/Fs ],'re',[-1 -1]);
[wft1,wfm,fr] = wfir('sb',255,[2900/Fs 3000/Fs ],'re',[-1 -1]);

dt = 1/Fs;
fc = 2930; // carrier porteuse = vitesse de la balle
df = 50; // modulation excursion
fm = 800; // modulation//Freq2RpmSpin(833) = 5273.0706 tr/min rpm = 552 rad/s =  1 rad/sec? The answer is 9.54929659643 RPM
vb=150; // 150 kmh
r = 0.043/2;
c = 299792458 ;
lambda = c /10.525E9;
tAx = dt:dt:0.01; // time axis in seconds
at = 1;
sa = at*exp(-%i*2*%pi*fc*tAx);
modt = 1;
smod = modt*exp(-%i*2*(2*%pi*fm)*r*sin(2*%pi*fm*tAx)/lambda);
u = real(sa.*smod);
plot(tAx,u);
uf  = filter(wft,1,u);
uf1 = filter(wft,1,filter(wft1,1,u));
w = unwrap(angle(hilbert(u)));
v = (diff(w)/(2*%pi*dt)); // instantaneous phase differential 
subplot(2,1,1);
t=(0:size(u,2)-1)/Fs;
plot(t,u)
subplot(2,1,2);
t=(0:size(v,2)-1)/Fs;
plot(t,abs(v))

[M52,tt5,f5]=animDensite(filter(wft,1,xg),44100, 256 ,32 , 0, -5,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M52(:,1:$-1));
[vBall, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,xgBall] = Info2(filter(wft,1,xg),'7');fichier
[wft1,wfm,fr] = wfir('sb',255,[(vBall*19.49-200)/Fs (vBall*19.49+200)/Fs ],'re',[-1 -1]);
[M52,tt5,f5]=animDensite(xgBall,44100, 1024 ,128 , 0, -5,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M52(:,1:$-1));
figure();plot(f5,max(M52($/2:$,1:$-1),'r'),'b'); // vitesse du club (1er pic), vitesse de la balle 2eme pic)
figure();plot(f5,sum(M52(:,1:$-1),'r'),1);

[vBall, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,xgBall] = Info2(filter(wft,1,xg),'7');fichier
[wft1,wfm,fr] = wfir('bp',256,[(vBall*19.49-200)/Fs (vBall*19.49+200)/Fs ],'re',[-1 -1]);
[M52,tt5,f5]=animDensite(filter(wft1,1,xg),44100, 256*8 ,32*8 , 1, -5,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M52(:,1:$-1));
[m,k]=(find((M52($/2:$,1:$-1))==1));
plot(tt5(m)',f5(k),'o');


[M52,tt5,f5]=animDensite(filter(wft1,1,xg),44100, 256*8 ,32*8 , 0, -5,1);
[m,k]=find(M52($/2:$,1:$-1)==1);
subplot(2,1,1);
plot(tt5(m)',f5(k),'ro');
Dg=(f5(k)/19.49/3.6).*tt5(m)';
subplot(2,1,2);
plot(tt5(m),Dg,'ro');
[M52,tt5,f5]=animDensite(filter(wft1,1,xd),44100, 256*8 ,32*8 , 0, -5,1);
[m,k]=find(M52($/2:$,1:$-1)==1);
subplot(2,1,1);
plot(tt5(m)',f5(k),'b*');
subplot(2,1,1);
Dd=(f5(k)/19.49/3.6).*tt5(m)';
subplot(2,1,2);
plot(tt5(m),Dd,'b*');

figure();
plot(tt5(m),Dd-Dg);



///////////////////////
[wft,wfm,fr] = wfir('bp',256,[1300/Fs 6000/Fs ],'re',[-1 -1]);
[M52,tt5,f5]=animDensite(filter(wft,1,xg(4410:$)),44100, 256*32 ,32*32 , 0, -5,1);
fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M52(:,1:$-1));
[vBall, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,xgBall] = Info2(filter(wft,1,xg),'7');fichier
[wft1,wfm,fr] = wfir('sb',255,[(vBall*19.49-200)/Fs (vBall*19.49+200)/Fs ],'re',[-1 -1]);
[M52,tt5,f5]=animDensite(filter(wft1,1,xg),44100, 256*32 ,32*32 , 0, -55,1);

s2_fft = abs(real(fft(filter(wft,1, test((ii)*nbEchantillon+1:nbEchantillon*(ii+1))))));

h = figure();
for i = 1:size(M52,'r')
    plot(f5,sum(M52(i,1:$-1),'r'),1);
end
f=figure();plot(f5,sum(M52($/2:$,1:$-1)/max(sum(M52($/2:$,1:$-1),'r')),'r')); // vitesse du shaft (1er pic), vitesse de la balle 2eme pic)


//////////////////// Video Read ////////////////////////

function my_eventhandler(win, x, y, ibut)
    if ibut==-1000 then return,end
    [x,y]=xchange(x,y,'i2f')
    xinfo(msprintf('Event code % d %d at mouse position is (%f,%f)',win ,ibut,x,y));
endfunction

plot2d()
fig = gcf() ;
fig.event_handler = 'my_eventhandler' ;
fig.event_handler_enable = "on" ;
fig.event_handler_enable = "off" ; //suppress the event handler

n = camopen(0);
sleep(10);
im = camread(n); //get a frame
imshow(im);
camcloseall();


///////////////////////////////  ANN ////////////////////////////////
// reseau de neuronnes
//
// 5 entrées
// 20 cachés
// 1 sorties
N = [5 , 20 , 1];
// training cycles
T = 400;

// input training
//    Res = [V0Club,V0Ballkmh, alphaClubPath*180/%pi,gamaFacePath*180/%pi,SpinZ,SpinY,Xchute, Zchute,XX($,1) , ZZ($,1),Ymax,LaunchAngledeg,VerticalLand,sum(t)];
x=[];
// sortie désirées
t=[];
for ii=1:1
    x($+1,:)=[80+ii/10,6,0,0,-8];
   [temps,VOL,Res] = Golfball(8,80+ii/10,'7',0,0,-8,0);
    t($+1,:)=Res;
end

// learning rate and threshold
lp = [0.1, 0];

W = ann_FF_init(N);
// Training 
W = ann_FF_Std_online(x,t,N,W,lp,T);

// full run
ann_FF_run(x,N,W);

function D = Plage(nb, minA, maxA)
    a = rand(1:nb); 
    b = (a-min(a));
    c = b/max(b);
    D = c*(max(maxA)-min(minA))+minA;
endfunction

nb = 2;
VC = Plage(nb , 80 , 110);
SM = Plage(nb , 1.1 , 1.5);
VB = SM.*VC;
launchAngleTab = [];

for ii=1:size(VC,2),
    [thetaLoft, ShafLeanImp, launchAngle, SpinZC, gamaFacePath] = LaunchAngle(VC(ii), VB(ii), '7', 0);
    launchAngleTab($+1) = ShafLeanImp;
end 

[wlaunchAngle,blaunchAngle] = ann_PERCEPTRON([VC' , VB'] , launchAngleTab');
ylaunchAngle = ann_PERCEPTRON_run(P , wlaunchAngle , blaunchAngle);


////////////////////////
//
// simulation de signal
//
////////////////////////////
fichier = uigetfile("*.wav", "D:\Users\mm\Documents\Golf\Radar\");[x,Fs,bits]=wavread(fichier); 
xg = x(1,1:$);xd = x(2,1:$);xg = xg - mean(xg);xd = xd - mean(xd);
xgp = padding(xg,2);
xgp = padding2(xg);
xgp = padding3(xg);
plot(xg);
xgpp = [xg,zeros(xg)];
[vBall, vBallCorrigee, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,PointShoot,tt5d,f5d] = Info2(xgp,'7',44100*2);
x = xgp(PointShoot+600:$) - mean(xgp(PointShoot+600:$));
plot(x,'b');
Ec = 0.5; 
Em = 0.5; 
Fs = 44100;
fs = Fs;
fc = 3000; 
fm = 800; 
tfin = 0.1;
[x,Vm,Vc]=pmmod(Ec,Em,fm,fc,fs,tfin);
[x,Vm,Vc]=fmmod(Ec,Em,fm,fc,fs,tfin);
[x,Vm,Vc]=ampmod(Ec,Em,fm,fc,fs,tfin);
[wft,wfm,fr] = wfir('bp',254,[1500/Fs 6000/Fs ],'hn',[-1 -1]);
[wft1,wfm,fr] = wfir('sb',255,[3000/Fs 3040/Fs ],'hm',[-1 -1]);
[wft2,wfm,fr] = wfir('bp',254,[2000/Fs 4000/Fs ],'re',[-1 -1]);
[wft3,wfm,fr] = wfir('lp',254,[6000/Fs 4000/Fs ],'re',[-1 -1]);
[M52,tt5,f5]=animDensite(filter(wft2,1,x),44100, 256*8 ,32*8 , 1, 0,1);
[M52,tt5,f5]=animDensite(x,44100, 256*8 ,32*8 , 1, 0,1);
[M52,tt5,f5]=animDensite(filter(wft1,1,filter(wft,1,xgp(PointShoot:$))),44100*2, 256*8 ,32*8 , 1, -10, 1);
f=figure();plot(f5,sum(M52($/2:$,1:$-1)/max(sum(M52($/2:$,1:$-1),'r')),'r')); // vitesse du shaft (1er pic), vitesse de la balle 2eme pic)

// Longueur onde 10.525Ghz = 2.84838439905
// 24Ghz = 1.249135241667
xgraw = xg.*window('hn',length(xg));

y=real(fft(xg,1));
xgp = padding(xg,2);
y=abs(real(fft(padding(xg,size(xg,2),6),1)));




// tests sur BARTLETT
xgp = padding2(xg);
[vBall, vBallCorrigee, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,PointShoot,tt5d,f5d] = Info2(xgp,'7',44100*2);

[M52,tt5,f5]=animDensite(filter(wft1,1,filter(wft,1,xgp(PointShoot:$))),44100*2, 256*2 ,32 , 1, -150,1);
plot(f5,sum(M52($/2:$,1:$-1),'r')/size(M52,2));
plot(tt5,M52(:,$/2));
plot(f5,M52($/2,2:$));

sm=pspect(32 , 256,'tr',(filter(wft,1,xgp(PointShoot:$))));smsize=max(size(sm));fr=(1:smsize)/smsize;plot(f5(1:$/2)*2,(sm(1:$/2)));


f = figure();f.color_map = hotcolormap(4);Sgrayplot(tt5,f5,M52(:,1:$-1));
f = figure();f.color_map = jetcolormap(2);Sgrayplot(tt5,f5,M52(:,1:$-1));


t=0:1/(2*44100):16882/(44100*2);
t=t(1:$-1);
u = cos(2*3020*%pi*t);
demo = xg.*u;
demodp = padding2(demo);

Ec = 0.5; 
Em = 0.2; 
Fs = 44100;
fs = Fs;
fc = 2728; 
fm = 0; 
tfin = 0.1;
tfin = (size(xg,2)-1)/Fs;
[xp,Vm,Vc,t]=pmmod(Ec,Em,fm,fc,fs,tfin);
[xf,Vm,Vc,t]=fmmod(Ec,Em,fm,fc,fs,tfin);
[xa,Vm,Vc,t]=ampmod(Ec,Em,fm,fc,fs,tfin);

[M52,tt5,f5]=animDensite(xp,44100, 256*2 ,32 , 1, -150,1);
figure();plot(f5,sum(M52($/2:$,1:$-1),'r')/size(M52,2));
[M52,tt5,f5]=animDensite(xf,44100, 256*2 ,32 , 1, -150,1);
figure();plot(f5,sum(M52($/2:$,1:$-1),'r')/size(M52,2));
[M52,tt5,f5]=animDensite(xg,44100, 256*2 ,32 , 1, -150,1);
figure();plot(f5,sum(M52($/2:$,1:$-1),'r')/size(M52,2));


[vBall, vBallCorrigee, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,PointShoot,tt5d,f5d] = Info2(xgp,'7',44100);
[wft1,wfm,fr] = wfir('sb',255,[(vBall*19.49-130)/Fs (vBall*19.49+130)/Fs],'hm',[-1 -1]);
[x]=ampdemod(xg(PointShoot:$), Vc, fc, fs, Em, Ec);
[M52,tt5,f5]=animDensite(filter(wft1 ,1,filter(wft,1,x)),44100, 256*4 ,32*4 , 1, -150,1);
figure();plot(f5,sum(M52($/2:$,1:$-1),'r')/size(M52,2));
[x,x2] = demod(xg(PointShoot:$), fc, fs, 'fm');
[M52,tt5,f5]=animDensite(filter(wft1 ,1,filter(wft,1,x)),44100, 256*4 ,32*4 , 1, -150,1);
figure();plot(f5,sum(M52($/2:$,1:$-1),'r')/size(M52,2));



N=size(xgp(PointShoot:$),2);
[fm,am,iflaw]=doppler(N, 44100, 5400, 2, 43.,1);
clf;
xg = real(am.*fm)';
subplot(211); plot(xg);xtitle(_("Signal "))
subplot(212); plot(iflaw);



clf;gcf().color_map= jetcolormap(128);
subplot(211);Sgrayplot(tt5,f5,M52(:,1:$-1));
[HT,RHO,THETA] = htl(M52,129,427,1,'plot');
subplot(212);grayplot(RHO,THETA,HT);
[m,ind]=max(HT);theta=THETA(ind(2));rho=RHO(ind(1));
xl=N/2-rho*sin(theta)+60*cos(theta)*[-1 1];
yl=N/2+rho*cos(theta)+60*sin(theta)*[-1 1];
xpoly(xl/max(tt5),yl/max(f5));

[MARGT,MARGF,E]=margtfr(M52(:,1:$-1)',tt5,f5);
subplot(211); plot(tt5,MARGT); 
subplot(212); plot(f5,MARGF);
subplot(211); plot((0:(size(xg(PointShoot/2:$/2),2)-1))/44100,xg(PointShoot/2:$/2));




[vBall, vBallCorrigee, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,PointShoot,tt5d,f5d,xg] = Info2('7',44100);
Fs = 44100*2;
[wft,wfm,fr] = wfir('bp',254,[1500/Fs 12000/Fs ],'hn',[-1 -1]);
[wft1,wfm,fr] = wfir('sb',255,[(vBall*19.49-30)/Fs (vBall*19.49+30)/Fs ],'hm',[-1 -1]);
[wft2,wfm,fr] = wfir('bp',254,[(vBall*19.49-1000)/Fs (vBall*19.49+1000)/Fs ],'re',[-1 -1]);
[wft3,wfm,fr] = wfir('bp',254,[30/Fs 6000/Fs ],'re',[-1 -1]);
[wftb,wfm,fr] = wfir('bp',256,[(FBall/1.5)/Fs (FBall/1)/Fs ],'re',[-1 -1]); // coupe bande autour de la club 
[x,y] = demod(filter(wft,1,xg(PointShoot:$)), vBall*19.49, Fs,'am');
x=filter(wft2,1,xg(PointShoot:$));

[x2,y2] = demod(x, 800, Fs,'pm');
[M51,tt5,f5]=animDensite(filter(wft1, 1,filter(wft2, 1,xg(PointShoot+600:$))),Fs, 256*16*2 ,32*16*2 , 1, -50,1);
[M51,tt5,f5]=animDensite(filter(wft3,1,x2),Fs, 256*16*2 ,32*16*2 , 1, -10,0);
[M52,tt5,f5]=animDensite(filter(wft2, 1,xg(PointShoot+600:$)),Fs, 256*16*2 ,32*16*2 , 1, -10,1);
plot(f5,sum(M51($/2:$,1:$-1),'r'),'r'); 
figure();plot(f5,sum(M51($/2:$,1:$-1)/max(sum(M51($/2:$,1:$-1),'r')),'r'),'r'); 
plot(f5,sum(M52($/2:$,1:$-1)/max(sum(M52($/2:$,1:$-1),'r')),'r')); 

xgpf = filter(wft,1,filter(wft,1,xgp(9500:$/2)));
[SP,F]=frpowerspec(xgpf+0*%i);plot(F*44100*2,SP);

[thr,sorh,keepapp] = ddencmp('den','wv',xgpf);
xd = wdencmp('gbl',xgpf,'db3',4,thr,sorh,keepapp);
plot(xd,'g');plot(xgpf,'r');
[SP,F]=frpowerspec(xd+0*%i);plot(F($/2:$)*44100*2,SP($/2:$));
[m,k]=max(SP($/2+1:$));F(size(SP($/2:$),'r')+k)*44100*2


[xd,SP, F] = Info3('7', 'D:\Users\mm\Documents\Golf\Radar\2019-02-24-F7-133m-160kmh-5432trm-311trm-cs117.wav');

fichier = uigetfile("*.wav", "D:\Golf\Radar\");

fichier = 'D:\Golf\Radar\2019-02-24-F7-133m-160kmh-5432trm-311trm-cs117.wav';
fichier = 'D:\Users\mm\Documents\Golf\Radar\2019-05-05-2.wav';
fichier = 'D:\Users\mm\Documents\Golf\Radar\2019-05-05-debruite.wav';
fichier = 'D:\Users\mm\Documents\Golf\Radar\1150_TrMin.wav';
fichier = 'D:\Users\mm\Documents\Golf\Radar\test1_7_dehors_metal.wav';
[x,Fs,bits]=wavread(fichier); 
xg = x(1,1:$);xg = xg - mean(xg); LeWav =xg;//pour AnaSpecEga
xd = x(2,1:$);xd = xd - mean(xd);LeWav=xg;xgp = xg;
[vBall, vBallCorrigee, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,PointShoot,tt5xd,f5xd] = Info2(xd, '7',44100)
[vBall, vBallCorrigee, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,PointShoot,tt5xg,f5xg] = Info2(xg, '7',44100)
[Bg,kg]=gsort(tt5xg,'g','i');
figure();plot(Bg,f5xg(kg),'b');
[Bd,kd]=gsort(tt5xd,'g','i');
plot(Bd,f5xd(kd),'r');
xgp = padding2(xg);
Fs = Fs*2;
[wft,wfm,fr] = wfir('bp',254,[1200/Fs 12000/Fs ],'hn',[-1 -1]);
xgpf = filter(wft,1,filter(wft,1,xgp));
[M51,tt5,f5]=animDensite(xgpf,Fs, 256*16*2 ,32*16*2 , 1, -50,1);
[M51,tt5,f5]=animDensite3(xgpf,Fs, 256*2 ,32*2 , 1, -50,1);

//Denoise
[thr,sorh,keepapp] = ddencmp('den','wv',xgpf);
xd = wdencmp('gbl',xgpf,'db3',4,thr,sorh,keepapp);
[M51,tt5,f5]=animDensite3(xd,Fs, 256*16*4 ,32*16*4 , 1, -50,1);

plot(f5,sum(M51(3*$/4:$,1:$)/max(sum(M51(3*$/4:$,1:$),'r')),'r'),'r');

[x1,y] = demod(xd, 3000, Fs,'am');
[wft1,wfm,fr] = wfir('lp',254,[600/Fs 12000/Fs ],'hn',[-1 -1]);
x1 = filter(wft1,1,x1);
[M51,tt5,f5]=animDensite3(x1,Fs, 256*16*2 ,32*16*2 , 1, -10,1);
[x2,y2] = demod(x1, 250, Fs,'pm');
[wft2,wfm,fr] = wfir('lp',254,[300/Fs 12000/Fs ],'hn',[-1 -1]);
x2 = filter(wft2,1,x2);
[M51,tt5,f5]=animDensite3(x2,Fs, 256*16*2 ,32*16*2 , 1, -10,1);

N=size(xd,2);
[fm,am,iflaw] = doppler(N,44100,5400/60,0.1,168,1);
clf;
xg_hat = real(am.*fm)'; plot(xg_hat);xtitle(_("Signal "))
[M51,tt5,f5]=animDensite3(xg_hat,Fs, 256*16*2 ,32*16*2 , 1, -10,1);
f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);


// Modelisation
t=0:1/44100:.3;
s_B = signal_B(t,44,5400);
s_modb = smod_B(t,44,5400);
s_A = signal_A(t,44,5400);

[x1,y] = demod(abs(real(s_B)), 3000, 44100,'am');
[wft1,wfm,fr] = wfir('bp',254,[60/44100 6000/44100 ],'hn',[-1 -1]);
x1 = filter(wft1,1,x1);
[M51,tt5,f5]=animDensite(x1,44100, 256*16*2 ,32*16*2 , 1, -10,1);
[x2,y2] = demod(x1, 250, 44100,'pm');
[wft2,wfm,fr] = wfir('lp',254,[300/44100 12000/44100 ],'hn',[-1 -1]);
x2 = filter(wft2,1,x2);
[M51,tt5,f5]=animDensite(x2,44100, 256*16*2 ,32*16*2 , 1, -10,1);
plot(f5,sum(M51(3*$/4:$,1:$-1)/max(sum(M51(3*$/4:$,1:$-1),'r')),'r'),'b');

[M51,tt5,f5]=animDensite((real(s_modb)), 44100, 256*16*2 ,32*16*2 , 1, -10,1);

h2=window('hn',63);
[TFR,T,F]=tfrsp(xd(9000:$),1:256*8*2,128*8*2,h2);
[TFR,T,F]=tfrwv(xd(9000:$));
clf;gcf().color_map= jetcolormap(128);
subplot(121);Sgrayplot(T,F(1:$/2)*44100*2,TFR(1:$/2,:)');
subplot(122);plot(fftshift(F'*44100*2),fftshift(TFR(:,100)));

tt5=1:64;f5=1:256;
clf;gcf().color_map= jetcolormap(128);
Sgrayplot(tt5,f5,abs(stft'));


for ii=1:25
 [j,k] = max(M5Sqr(ii,:));
 plot(ii,f5(k),'o')
end;

[wft,wfm,fr] = wfir('bp',254,[1200/Fs 6000/Fs ],'hn',[-1 -1]);
Vitesse = max(afficheVitesse(afficheDenoise(filter(wft,1,xg(getPointShoot(xg,Fs,158):$)),0.01),Fs))/19.49
afficheVitesse(padding2(filter(wft,1,afficheDenoise(xg,0.0))),2*Fs);


PoinShoot = getPointShoot(xg,Fs,158)

[wft1,wfm,fr] = wfir('bp',254,[1200/Fs 3000/Fs ],'hn',[-1 -1]);

xp2 = xg(1:PoinShoot-100);
xp2f = filter(wft1,1,padding2(afficheDenoise(xp2,0.0)));
[M5,tt5,f5]=animDensite3(xp2f,2*Fs, 256*4 ,32*4 , 0, -15,0);
fig=figure();subplot(221);
fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M5(:,1:$));
subplot(223);plot(f5,sum(M5(:,1:$)/max(sum(M5(:,1:$),'r')),'r'),'b');

[wft,wfm,fr] = wfir('bp',254,[1000/Fs 6000/Fs ],'hn',[-1 -1]);
xp = xg(PoinShoot+200:$);

xpf = filter(wft,1,padding2(afficheDenoise(xp,0.0)));
[M5,tt5,f5]=animDensite3(xpf,2*Fs, 256*16*2 ,32*16*2 , 0, -15,0);
subplot(222);
fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M5(:,1:$));
subplot(224);plot(f5,sum(M5(:,1:$),'r'),'b');

freqMax = afficheDemod(xpf, 2*Fs, 153)*60*2



N=size(xpf,2);
F0 = 5200;
D=1.5;//:3;
t=getT(xpf,2*Fs);
[fm,am,iflaw] = doppler(N,2*Fs,F0,D,155/3.6,1);
subplot(211); plot(t,real(am.*fm)/2);xtitle(("Signal "));
subplot(212); plot(t,iflaw);
[ifhat,t]=instfreq(sigmerge(am.*fm,xp,15),11:502,10);
plot(t,ifhat,'g');


sig = sum(M5(:,1:$),'r');
pics = find(diff(sig,2)<0);
plot(f5(pics),sig(pics));// ici
[u, i] = gsort(-sig(pics)) ;
hauteur = sig(pics(i)) ;
plot(f5(pics(i)), sig(pics(i)) ,'ro')


pics2 = find(diff(sig(pics),2)<0);
plot(f5(pics2),sig(pics2));// ici


s_B = signal_B(t,3000/19.49,5400);
[enveloppe,t] = Enveloppe(xg, Fs,2500, 3500);
xg_hat = real(enveloppe.*s_B);
[M51,tt5,f5]=animDensite3(xg_hat(5000:$), 44100, 256*16 ,32*16 , 0, 0,0);
fig=figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M51(:,1:$-1));
subplot(223);
plot(f5,sum(M51(:,1:$-1)/max(sum(M51(:,1:$-1),'r')),'r'),'b');
[m,k]=max(sum(M51(:,1:$-1),'r'));
f5(k)


///Extracting instantaneous amplitude,phase,frequency – application of Analytic signal/Hilbert transform
[wft,wfm,fr] = wfir('bp',254,[2500/Fs 3500/Fs ],'hn',[-1 -1]);
z = hilbert(filter(wft,1,xg)); //%form the analytical signal
inst_amplitude = abs(z); //%envelope extraction
inst_phase = unwrap(angle(z));//%inst phase
regenerated_carrier = cos(inst_phase);
inst_freq = diff(inst_phase)/(2*%pi)*Fs;//%inst frequency
plot(t, filter(wft,1,xg));
plot(t,inst_amplitude,'r');
plot(t,regenerated_carrier);
plot(t,inst_freq);

v = diff(inst_phase)/(2*%pi/Fs); // instantaneous phase differential normalized to Hz
plot(t,v); //

s=fft(regenerated_carrier);
nbEchantillon = size(s,2);
f=Fs*(0:(nbEchantillon)-1)/nbEchantillon;
plot(f,s);


fichier = 'C:\Users\mm\Documents\Golf\Radar\2019-02-24-F7-133m-160kmh-5432trm-311trm-cs117.wav';
fichier = 'C:\Users\mm\Documents\Golf\Radar\2019-05-05-2.wav';
fichier = 'C:\Users\mm\Documents\Golf\Radar\2019-05-05-debruite.wav';
fichier = 'C:\Users\mm\Documents\Golf\Radar\1150_TrMin.wav';
fichier = 'C:\Users\mm\Documents\Golf\Radar\test1_7_dehors_metal.wav';
[x,Fs,bits]=wavread(fichier); 
xg = x(1,1:$);mu= [mean(x);stdev(x)];xg = (xg - mu(1))/mu(2);
xg = xg(5000:9095);//Pour mettre la taille en puissance de 2
xd = x(2,1:$);xd = xd - mean(xd);
xg_hat = real(signal_B(getT(xg,Fs), 3015.0165/19.49/3.6, 5400)); // stimateur du signal Doppler
[wft,wfm,fr] = wfir('bp',254,[300/Fs 8000/Fs ],'re',[-1 -1]);
[wft1,wfm,fr] = wfir('bp',254,[10/Fs 300/Fs ],'re',[-1 -1]);
xgpf = filter(wft,1,padding3(xg));t=getT(xgpf,2*Fs);plot(t,xgpf);
xgpf_hat = filter(wft,1,padding3(xg_hat));t=getT(xgpf_hat,2*Fs);plot(t,xgpf_hat,'r')
s=abs(real(fft(xgpf)));s_hat=abs(real(fft(xgpf_hat)));nbEchantillon = size(s,2);f=2*Fs*(0:(nbEchantillon)-1)/nbEchantillon;plot(f,[s', s_hat']);
[x1,y] = demod(xgpf, 3015.0165, 2*44100,'am');
[x1_hat,y] = demod(xgpf_hat, 3015.0165, 2*44100,'am');
[py,fy] = pWelch(filter(wft1,1,x1),[],0,length(x1),Fs);
[py_hat,fy] = pWelch(filter(wft1,1,x1_hat),[],0,length(x1),Fs);
s=abs(real(fft(xgpf)));s_hat=abs(real(fft(xgpf_hat)));nbEchantillon = size(s,2);f=2*Fs*(0:(nbEchantillon)-1)/nbEchantillon;plot(f,[s', s_hat']);
plot(fy,[py, py_hat*20]);
[x2,y] = demod(filter(wft1,1,x1), 100, 2*44100,'fm');
[x2_hat,y] = demod(filter(wft1,1,x1_hat), 100, 2*44100,'fm');
s=abs(real(fft(x2)));s_hat=abs(real(fft(x2_hat)));nbEchantillon = size(s,2);f=2*Fs*(0:(nbEchantillon)-1)/nbEchantillon;plot(f,[s', s_hat']);
[py,fy] = pWelch(filter(wft1,1,x2),[],0,length(xgpf),Fs);
[py_hat,fy] = pWelch(filter(wft1,1,x2_hat),[],0,length(xgpf),Fs);
plot(fy,[py, py_hat*1]);
t=getT(x2, 2*Fs);
plot(t,[filter(wft1,1,x2)' filter(wft1,1,x2_hat)']);

plot(real(afficheDenoise(xgpf,0.0)));
[M51,tt5,f5]=animDensite3(xgpf_hat,2*44100, 256*4*4 ,32*4*4 , 0, -10,1);
figure();plot(f5,sum(M51(:,1:$)/max(sum(M51(:,1:$),'r')),'r'),'r');
[M51,tt5,f5]=animDensite3(afficheDenoise(xgpf,0.0),2*44100, 256*4*4 ,32*4 , 0, -1, 1);
plot(f5,sum(M51(:,1:$)/max(sum(M51(:,1:$),'r')),'r'),'r');

[M51,tt5,f5]=animDensite3(x1,2*44100, 256*4 ,32*4 , 1, 50,0);
[M51,tt5,f5]=animDensite3(filter(wft1,1,x1_hat),2*44100, 256*4 ,32*4 , 1, 50,0);



// Create a modulated signal in the same form as in equation (1)
A = 1; 
tp = 2^12; 
omega1 = 560; 
omega2 = 1; 
gamma = %pi/7; 
beta = 5; 
t = 0:2*%pi/tp:2*%pi*(1-1/tp); 
x = A*cos(omega1*t+gamma+beta*sin(omega2*t));
plot(t(1:round(length(x)/omega2)),x(1:round(length(x)/omega2)));

//Transform the signal into the frequency domain
ff = fft(x); 
ax = linspace(-tp/%pi/4,(tp-2)/%pi/4,tp);
dbf = 20*log10((abs(ff)/length(ff)*2+10E-12)/10E-12);
dbf = fftshift(dbf);
plot(ax,dbf);

//Set negative frequencies to zero, and double all positive frequencies (remember not to
//double the zero frequency) and inverse transform back to the time domain to create
//the analytic signal
gf = ff; //%create dummy variable
gf(2:$) = 2*ff(2:$); //%double positive freq’s
gf($/2+1:$) = 0; //%set negative freq’s to zero
g = ifft(gf); //%transform back to time domain
dbg = 20*log10((abs(gf)/length(gf)*2+10E-12)/10E-12);
dbg = fftshift(dbg);
plot(ax,dbg);

//Now calculate the instantaneous angle of the analytic signal and the unwrapped
//instantaneous phase of the original signal
pha = angle(g); //%instantaneous phase of analytic signal
phau = unwrap(pha); //% unwrap phase

//If the carrier frequency is known this can be done by multiplying the carrier frequency
//by the inverse of the sampling frequency and subtracting from the unwrapped
//instantaneous phase, or if the carrier frequency is unknown then the linear fit of the
//unwrapped phase will give the estimate of Ωt . This method is used here
p = polyfit(t,phau,1); //%linear fit to unwrapped
//%phase
p(2) = phau(1);
omega1t = polyval(p,t);
phaus = phau - omega1t; //%subtract linear offset

mf = fft(phaus); //%spectrum of phase demodulated signal en rad/s
dbmf = 20*log10((abs(mf)/length(mf)*2+10E-12)/10E-12);
dbmf = fftshift(dbmf);
plot(ax,dbmf);

//If the actual linear phase is used then the mass line can be
//seen to be removed:
phaus2 = phau - omega1*t; //%removal of linear phase increase if carrier freq is known
mf2 = fft(phaus2);
dbmf2 = 20*log10((abs(mf2)/length(mf2)*2+10E-12)/10E-12);
dbmf2 = fftshift(dbmf2);
plot(ax,dbmf2);

gammae1 = abs(mf(1))/length(mf);
betae1 = abs(mf(omega2+1))/length(mf)*2;
gammae2 = abs(mf2(1))/length(mf2);
phie1 = angle(mf(omega2+1))+%pi/2;
betae2 = abs(mf2(omega2+1))/length(mf2)*2;
phie2 = angle(mf2(omega2+1))+%pi/2;

[wft,wfm,fr] = wfir('bp',254,[300/Fs 8000/Fs ],'re',[-1 -1]);
[wft1,wfm,fr] = wfir('bp',254,[10/Fs 300/Fs ],'re',[-1 -1]);
t=(0:size(xg,2)-1)/Fs;
xgf = filter(wft,1,(xg));
subplot(2,1,1);
plot(t,xgf);
subplot(2,1,2);
plot(t(1:$-1),filter(wft1, 1,abs(diff(unwrap(angle(hilbert(filter(wft,1,xg)))))/(2*%pi*1/Fs))),'r');


endfunction
