// Copyright (C) 2016 - mm - Author
//
// About your license if you have any
//
// Date of creation: 26 oct. 2016
//
//// Copyright (C) 2016 - mm - Author
////
//// About your license if you have any
////
//// Date of creation: 10 oct. 2016
//// Anntenne SMA

//chdir('D:\Golf\portaudio_0.2\portaudio_0.2');
//exec('D:\Golf\portaudio_0.2\portaudio_0.2\loader.sce',-1)
//chdir('D:\Golf\stftb\');
//exec('D:\Golf\stftb\loader.sce',-1)

chdir('C:\Users\f009770\Documents\GolfBall-master\sci');
//stacksize('max');

/////////////////////////////////////////////////////////////////

function phase = angle(A)
    phase = atan(imag(A),real(A))
endfunction;


////////////////////////////////////////////////////////////////////
function [s2Centre, s2f,MaxFreq] = plotFFT(s2,Fs)
    //[wft,wfm,fr]=wfir('bp',48,[1000/22050 5000/22050],'hm',[-1 -1]);
    [wft,wfm,fr] = wfir('bp',64,[0.03 0.11],'hm',[-1 -1]);
    N=size(s2,'c'); // definition du nombre d'échantillons
    tau = 1 / Fs; // interval temporel de l'echantillonnage
    t = (0:N - 1) * tau; // construction du vecteur temps
    w = 6000*%pi/180;
    //s2 = sin(w*t).*sin(w/3*t);
    s2Centre = s2 - mean(s2); // recentrage du signal autour de l'axe abscice.
    //s2Centre(abs(s2Centre)<0.0012)=0; // filtrage bourin pour supprimer les valeurs faibles
    //s2=s2(1:9000)=0;
    f=Fs*(0:(N)-1)/N;
    [s2f zf] = filter(wft,1,s2Centre); // filtrage du signal par le filtre passe bande

    //densite spectrale 
    s2_fft = (fft(s2Centre));
    s2f_fft = fft(s2f);
    conjuguee_s2 = conj(s2_fft);
    conjuguee_s2f = conj(s2f_fft);

    s2_densite = s2_fft.*conjuguee_s2;
    s2f_densite = s2f_fft.*conjuguee_s2f;


    s2f_densite_real = abs(real(s2f_densite));
    n=50;
    s2f_densite_real_moy = filter(ones(1,n),n,s2f_densite_real);
    s2_densite_real = abs(real(s2_densite));
    n=50;
    s2_densite_real_moy = filter(ones(1,n),n,s2_densite_real);    //

    s2_fft_real = abs(real(s2_fft));
    s2f_fft_real = abs(real(s2f_fft));
    [m,n]=max(s2f_fft_real(2:$/2));
    a = f(2:$/2);
    MaxFreq = a(n);
    //s2_densite(1,$/2+1:$) = [];
    figure();title(' signal');plot2d(t,s2Centre);xlabel('(t)');ylabel('Amp wav');
    figure();title(' FFT');plot2d(f(2:$/2),s2_fft_real(2:$/2));plot2d(fr*Fs,wfm);xlabel('Freq (Hz)');ylabel('Amp wav');
    figure();title('Filtered FFT');plot2d(f(2:$/2),s2f_fft_real(2:$/2));plot2d(fr*Fs,wfm);xlabel('Freq (Hz)');ylabel('Amp wav');
    figure();title('Power Spectral Density');plot2d(f(2:$/2),s2_densite(2:$/2));plot2d(fr*Fs,wfm);xlabel('Freq (Hz)');ylabel('$\int density(t).dt$');
    figure();title('Filtered Power Spectral Density');plot2d(f(2:$/2),s2f_densite(2:$/2));plot2d(fr*Fs,wfm);xlabel('Freq (Hz)');ylabel('$\int density(t).dt$');
    figure();title('Mean Filtered Power Spectral Density');plot2d(f(2:$/2),s2f_densite_real_moy(2:$/2));plot2d(fr*Fs,wfm);xlabel('Freq (Hz)');ylabel('$\int density(t).dt$');
    figure();title('Mean Power Spectral Density');plot2d(f(2:$/2),s2_densite_real_moy(2:$/2));plot2d(fr*Fs,wfm);xlabel('Freq (Hz)');ylabel('$\int density(t).dt$');
    //title(a_tl,"Signal Filtré");plot(a_tl,t,s2f);
    //title(a_bl,"FFT Filtrée");plot(a_bl,f(2:$/2),s2f_fft(2:$/2));
    s2_densite = s2_densite(2:$/2);
endfunction
//////////////////////// Fin de plotFFT() //////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Tracking ball
function TrackingBall()
    n = aviopen('red-car-video.avi');
    im = avireadframe(n); //get a frame
    obj_win = camshift(im, [12, 6, 15, 13]); //initialize tracker
    while ~isempty(im),
        obj_win = camshift(im); //camshift tracking          
        obj_win
        im = rectangle(im, obj_win, [0,255,0]);
        imshow(im);
        im = avireadframe(n);
    end;
    aviclose(n);
endfunction;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%% fenetre de Hanning
function H=hanning(N)
    t=[1:N];
    H=0.5-0.5*cos(2*%pi*t/N);
endfunction

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function wrpm = Freq2RpmSpin(RadarFreq)
    //wrpm = RadarFreq * 60 * 3.6 /(%pi*0.043 * 19.49);//v = 2πr × RPM × (60/1000) km/hr
    wrpm = RadarFreq*60/(2*%pi*0.021335*19.49*3.6);
endfunction
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function RadarFreq = RpmSpin2Freq(wrpm)
    //RadarFreq = wrpm /( 60 * 3.6 /(%pi*0.043 * 19.49));
    RadarFreq = wrpm * 2*%pi*0.021335*19.49*3.6/60;
endfunction

function [tt,f,M1] = animFFT(s2,Fs,nbEchantillon,pas,anim,seuil)
    N = size(s2,2);
    tau = 1 / Fs; // interval temporel de l'echantillonnage
    t = (0:N - 1) * tau; // construction du vecteur temps
    f=Fs*(0:(nbEchantillon)-1)/nbEchantillon;
    s2Centre = s2;// - mean(s2); // recentrage du signal autour de l'axe abscice.
    b=modulo(N,nbEchantillon);
    s2Centre=[s2Centre, zeros(nbEchantillon - b,1)'];


    if (anim > 0) then 
        figure();
        subplot(2,1,1);
        a = gca();
        a.auto_clear = "off";
        plot(t,s2);xlabel('(t)');ylabel('Amp Wav');
        subplot(2,1,2);
        a = gca();
        a.auto_clear = "on";
        s2max = max(s2);
        subplot(2,1,1);
        plot([0 0],[0 s2max],'r');
        e = gce();
    end;
    M1=[];tt=[];
    for ii=1:pas:(size(s2Centre,2) - nbEchantillon),
        if (anim > 1) then 
            drawlater();
            delete(e);
            subplot(2,1,1);
            plot([t(ii) t(ii)],[0 s2max],'r');
            e = gce();
        end;
        //densite spectrale 
        s2_fft = (fft(s2Centre(1,ii:ii + nbEchantillon)));
        if max(real(s2_fft)) < seuil then
            s2_densite = zeros(s2_fft);
        else
            s2_densite = abs(real(s2_fft));
            //s2_densite = s2_densite/max(s2_densite);
        end;
        tt = [tt;t(ii)];
        M1 = [M1;s2_densite];
        if (anim > 1) then 
            subplot(2,1,2);
            plot(f(1:$/2),s2_densite(1,1:$/2));xlabel('Freq (Hz)');ylabel('Power Spectral Density');
            drawnow();
        end;
    end;
    M1 = M1(:,1:$/2);
    f= f(1:$/2);
    if (anim > 0) then 
        subplot(2,1,2);
        //xset("colormap",jetcolormap(64));
        //Sgrayplot(tt,f,M1);
    end;
endfunction

function y = fftpadding(x)
    //y = fft([x zeros(size(x,2)-1)]);
    y = fft([x zeros(2-1)]);
endfunction

function w1 = padding(w,N)
    //w=window('re',N);
    P = length(w);
    w1 = [zeros(1,N*P)];
    //w1(1:N/2) = w(1:N/2);
    //w1(P*N:-1:P*N-N/2+1) = w(N:-1:N/2+1);
    w1(1:N:$) = w;
endfunction


function w1 = padding2(w)
    P = length(w);
    w1 = [zeros(1,2*P-1)];
    w1(1:2:$) = w;
    w1= [w1 zeros(1,(ceil(size(w1,2)/32)-size(w1,2)/32)*32)];
endfunction

function w1 = padding3(w)
    w2 = (w(1:$-1) + w(2:$))/2;
    P = length(w);
    w1 = [zeros(1,2*P-1)];
    w1(1:2:$) = w;
    w1(2:2:$) = w2;
    [w1 zeros(1,(ceil(size(w1,2)/32)-size(w1,2)/32)*32)];
endfunction

function [fftFiltree, f1] = FftFiltree(x, Fs, FreqMin, FreqMax, fType)
    if (fType == 0), 
        [wft,wfm,fr] = wfir('bp',64,[FreqMin/Fs FreqMax/Fs ],'re',[-1 -1]);
        fftFiltree = real(abs(fftpadding(filter(wft,1,x))));
        f1 = Fs*(0:(size(fftFiltree,2))-1)/size(fftFiltree,2);
    else
        ll = find(((f1) <= FreqMin) | ((f1) >= FreqMax));// Filtrage bourin par suppression de valeur
        fftFiltree(ll) = fftFiltree(ll)/20;    
        f1 = Fs*(0:(size(fftFiltree,2))-1)/size(fftFiltree,2);
    end;
endfunction

function xdensite = DensiteSpec(x)
    xfft = real(abs(fft(x)));
    xdensite = abs(real(xfft.*conj(xfft)));
    xdensite = xdensite(1:$/2);
endfunction

function [densiteFiltree, f1] = DensiteFiltree(x, Fs, FreqMin, FreqMax, fType)
    [fftFiltree, f1] = FftFiltree(x, Fs, FreqMin, FreqMax, fType)
    densiteFiltree = abs(real(fftFiltree.*conj(fftFiltree)));
endfunction

function [M1, tt, f] = animDensite(s2,Fs,nbEchantillon,pas,anim,seuil1,normalize)
    //animDensite(s2f,44100, 512 ,10 , 1, 0.0,1);
    N = size(s2,2);
    tau = 1 / Fs; // interval temporel de l'echantillonnage
    t = (0:N - 1) * tau; // construction du vecteur temps
    f=Fs*(0:(nbEchantillon)-1)/nbEchantillon;
    s2Centre = s2 - mean(s2); // recentrage du signal autour de l'axe abscice.
    //=modulo(N,nbEchantillon);
    //s2Centre=[s2Centre, zeros(nbEchantillon - b,1)'];//     s2Centre=[s2Centre, zeros(2*(nbEchantillon - b),1)'];


    if (anim ==1) then 
        figure();
        subplot(2,1,1);
        a = gca();
        a.auto_clear = "off";
        plot(t,s2);xlabel('(t)');ylabel('Amp Wav');
        //[enveloppe,time] = Enveloppe(s2, Fs,1300, 6000);
        //plot(t,enveloppe/max(enveloppe));
        subplot(2,1,2);
        a = gca();
        a.auto_clear = "on";
        s2max = max(s2);
        subplot(2,1,1);
        plot([0 0],[0 s2max],'r');
        e = gce();
    end;
    M1=[];tt=[];
    for ii=1:pas:(size(s2Centre,2) - nbEchantillon),
        if (anim ==1) then 
            drawlater();
            delete(e);
            subplot(2,1,1);
            plot([t(ii) t(ii)],[0 s2max],'r');xlabel('(t)');ylabel('Amp Wav');
            e = gce();
        end;
        //densite spectrale 
        s2CentreBW =s2Centre(1,ii:ii + nbEchantillon);// BARTLETT WELCH
        s2CentreBW = s2CentreBW.*window('hn',length(s2CentreBW));// BARTLETT WELCH
        s2_fft = (fftpadding(s2CentreBW));
        //        a = find(dbv(real(s2_fft)) < seuil);
        //        s2_fft(a) = 0;
        //        if max(dbv(real(s2_fft))) < seuil then
        //            s2_densite = zeros(s2_fft);
        //        else
        s2_densite = abs(real(s2_fft.*conj(s2_fft)));//abs(real(s2_fft.*conj(s2_fft)));
        //s2_densite = s2_densite .* s2_densite;
        a = find(dbv(real(s2_densite)) < seuil1);
        s2_densite(a) = 0;
        //a = find(dbv(real(s2_densite)) > seuil2);
        //s2_densite(a) = 0;
        if (normalize == 0) then
            s2_densite = s2_densite;
        else
            if (max(s2_densite) == 0) then  s2_densite = zeros(s2_densite); 
            else s2_densite = s2_densite/max(s2_densite);
            end;
        end;
        //        end;
        tt = [tt;t(ii)];
        M1 = [M1;s2_densite];
        if (anim == 1) then 
            subplot(2,1,2);
            plot(f(1:$/2),s2_densite(1,1:$/2-1));xlabel('(t)');ylabel('Speed (km/h)');
            drawnow();
        end;
    end;
    M1 = M1(:,1:$/2);
    f= f(1:$/2);
    if (anim < 3 ) then 
        subplot(2,1,2);
    else
        //xset("colormap",jetcolormap(64));
        colormap(jet);
        Sgrayplot(tt,f/19.49,M1(:,1:$-1)/19.49);xlabel('(t)');ylabel('Speed (km/h)');
    end;
    fig_ = gcf();
    fig_.background = -2
endfunction

function [M1, tt, f] = animDensite2(s2,Fs,nbEchantillon,pas,anim,seuil1,normalize)
    N = size(s2,2);
    tau = 1 / Fs; // interval temporel de l'echantillonnage
    t = (0:N - 1) * tau; // construction du vecteur temps
    f=Fs*(0:(nbEchantillon)-1)/nbEchantillon;
    s2Centre = s2 - mean(s2); // recentrage du signal autour de l'axe abscice.
    M1=[];
    tt=[];
    for ii=1:pas:(size(s2Centre,2) - nbEchantillon),
        s2CentreBW =s2Centre(1,ii:ii + nbEchantillon);// BARTLETT WELCH
        s2CentreBW = s2CentreBW.*window('hn',length(s2CentreBW));// BARTLETT WELCH
        [s2_densite,F]=frpowerspec(s2CentreBW++0.000000000001*%i);
        s2_densite = dbv(real(s2_densite));
        a = find(s2_densite) < seuil1;
        s2_densite(a) = -1000;
        if (normalize == 0) then
            s2_densite = s2_densite;
        else
            if (max(s2_densite) == 0) then
                s2_densite = zeros(s2_densite); 
            else 
                s2_densite = s2_densite/max(s2_densite);
            end;
        end;
        tt = [tt;t(ii)];
        M1 = [M1; s2_densite'];
    end;//Fin For
    M1 = M1(:,$/2:$-1);
    f= f(1:$/2);
    if (anim > 0) then 
        //subplot(2,1,2);
        figure();
        //xset("colormap",jetcolormap(64));
        colormap(jet);
        Sgrayplot(tt,f/19.49,M1(:,1:$)/19.49);
    end;
    fig_ = gcf();
    fig_.background = -2
endfunction


function [M1, tt, f] = animDensite3(s2,Fs,nbEchantillon,pas,anim,seuil1,normalize)
    N = size(s2,2);
    tau = 1 / Fs; // interval temporel de l'echantillonnage
    t = (0:N - 1) * tau; // construction du vecteur temps
    f=Fs*(0:(nbEchantillon)-1)/nbEchantillon;
    s2Centre = s2 - mean(s2); // recentrage du signal autour de l'axe abscice.
    M1=[];
    tt=[];
    for ii=1:pas:(size(s2Centre,2) - nbEchantillon),
        s2CentreBW =s2Centre(1,ii:ii + nbEchantillon);// BARTLETT WELCH
        s2CentreBW = s2CentreBW.*window('hn',length(s2CentreBW));// BARTLETT WELCH
        [s2_densite,F]=frpowerspec(s2CentreBW+0.000000000001*%i);
        s2_densite = (real(s2_densite));
        a = find(s2_densite) < seuil1;
        s2_densite(a) = -1000;
        if (normalize == 0) then
            s2_densite = s2_densite;
        else
            if (max(s2_densite) == 0) then
                s2_densite = zeros(s2_densite); 
            else 
                s2_densite = s2_densite/max(s2_densite);
            end;
        end;
        tt = [tt;t(ii)];
        M1 = [M1; s2_densite'];
    end;//Fin For
    M1 = real(M1(:,$/2:$-1));
    f= f(1:$/2);
    if (anim > 0) then 
        //subplot(2,1,2);
        figure();
        //xset("colormap",jetcolormap(64));
        colormap(jet);
        Sgrayplot(tt,f/19.49,M1(:,1:$)/19.49);
    end;
    fig_ = gcf();
    fig_.background = -2
endfunction


function [M1,tt,f] = animHilbert(s2,Fs,nbEchantillon,pas,anim,seuil)
    N = size(s2,2);
    tau = 1 / Fs; // interval temporel de l'echantillonnage
    t = (0:N - 1) * tau; // construction du vecteur temps
    f=Fs*(0:(nbEchantillon)-1)/nbEchantillon;
    s2Centre = s2;// - mean(s2); // recentrage du signal autour de l'axe abscice.
    b=modulo(N,nbEchantillon);
    s2Centre=[s2Centre, zeros(nbEchantillon - b,1)'];


    if (anim > 0) then 
        figure();
        subplot(2,1,1);
        a = gca();
        a.auto_clear = "off";
        plot(t,s2);
        subplot(2,1,2);
        a = gca();
        a.auto_clear = "on";
        s2max = max(s2);
        subplot(2,1,1);
        plot([0 0],[0 s2max],'r');
        e = gce();
    end;
    M1=[];tt=[];
    for ii=1:pas:(size(s2Centre,2) - nbEchantillon),
        if (anim > 1) then 
            drawlater();
            delete(e);
            subplot(2,1,1);
            plot([t(ii) t(ii)],[0 s2max],'r');
            e = gce();
        end;
        //         pause
        [s2_fft,F] = frpowerspec(hilbert(s2Centre(1,ii:ii + nbEchantillon)));//frpowerspec(s2CentreBW+0*%i
        if max(real(s2_fft)) < seuil then
            s2_densite = zeros(s2_fft);
        else
            s2_densite = abs(real(s2_fft));
            //s2_densite = s2_densite/max(s2_densite);
        end;
        tt = [tt;t(ii)];
        M1 = [M1;s2_densite'];
        if (anim > 1) then 
            subplot(2,1,2);
            plot(f(1:$/2),s2_densite(1,1:$/2));
            drawnow();
        end;
    end;
    M1 = M1(:,1:$/2);
    f= f(1:$/2);
    if (anim > 0) then 
        subplot(2,1,2);
        xset("colormap",jetcolormap(64));
        Sgrayplot(tt,f,M1);
    end;
endfunction

function out = dbv(in)
    in(find(in==0))= -%inf;
    out = 20 * log10(abs(in));
endfunction

function s2f = filtrageVitesse(s2)
    Fs = 44100;
    [wft,wfm,fr]=wfir('bp',64,[1000/Fs 8000/Fs],'tr',[-1 -1]); // définition de la fenetre du filtre passa bande
    [s2f zf] = filter(wft,1,s2); // filtrage du signal par le filtre passe bande
endfunction


function s2f = filtrageHigh(s2)
    Fs = 44100;
    [wft,wfm,fr]=wfir('bp',64,[14000/Fs 22000/Fs],'tr',[-1 -1]); // définition de la fenetre du filtre passa bande
    [s2f zf] = filter(wft,1,s2); // filtrage du signal par le filtre passe bande
endfunction

function s2f = filtrage(s2,FreqMin,FreqMax)
    Fs = 44100;
    [wft,wfm,fr]=wfir('bp',64,[FreqMin/Fs FreqMax/Fs],'tr',[-1 -1]); // définition de la fenetre du filtre passa bande
    [s2f zf] = filter(wft,1,s2); // filtrage du signal par le filtre passe bande
endfunction

function s2f = filtrageVitesseSpin(s2)
    Fs = 44100;
    [wft,wfm,fr]=wfir('bp',33,[0/Fs 6000/Fs],'tr',[-1 -1]); // définition de la fenetre du filtre passa bande
    [s2f zf] = filter(wft,1,s2); // filtrage du signal par le filtre passe bande
endfunction

function s2f = filtrageSpin2(s2)
    Fs = 44100;
    [wft,wfm,fr]=wfir('bp',64,[0/Fs 200/Fs],'tr',[-1 -1]); // définition de la fenetre du filtre passa bande
    [s2f zf] = filter(wft,1,s2); // filtrage du signal par le filtre passe bande
endfunction


function afficheMesCourbes()
    chdir('C:\Users\F009770\Documents\Golf\portaudio_0.2\');
    exec('C:\Users\F009770\Documents\Golf\portaudio_0.2\loader.sce',-1)
    chdir('C:\Users\F009770\Documents\Golf\Radar');
    stacksize('max');
    fichier = 'BalleFil5Tombe.wav';
    fichier = 'fer7Radar.wav';
    [z,Fs,bits]=wavread(fichier); // lecture du fichier wav
    s2 = z(2,:);// prise en compte uniquement du canal 2, car le 1 n'est pas utilise dans mon appli
    s2 = s2 -mean(s2); // recentrage du signal autour de l'axe abscice.
    //s2f=filtrageVitesse(s2(6500:11000)); // fer7_5
    s2f=filtrageVitesse(s2(1:17500)); // fer7Radar.wav
    [tt1,f1,M1]=animFFT(s2f,44100,30,0);
    [tt2,f2,M2]=animFFT(s2f,44100,300,0);
    [tt3,f3,M3]=animFFT(s2f,44100,600,0);
    [tt4,f4,M4]=animFFT(s2f,44100,1200,0);
    [tt5,f5,M5]=animFFT(s2f,44100,3000,0);
    figure();
    subplot(3,2,1);plot(s2f);
    subplot(3,2,2);xset("colormap",hotcolormap(64));Sgrayplot(tt1,f1/19.49,M1/19.49);title("Fenetre 30 pts km/h filtre 1kHz-6kHz");
    subplot(3,2,3);xset("colormap",hotcolormap(64));Sgrayplot(tt2,f2/19.49,M2/19.49);title("Fenetre 300 pts km/h filtre 1kHz-6kHz");
    subplot(3,2,4);xset("colormap",hotcolormap(64));Sgrayplot(tt3,f3/19.49,M3/19.49);title("Fenetre 600 pts km/h filtre 1kHz-6kHz");
    subplot(3,2,5);xset("colormap",hotcolormap(64));Sgrayplot(tt4,f4/19.49,M4/19.49);title("Fenetre 1200 pts km/h filtre 1kHz-6kHz");
    subplot(3,2,6);xset("colormap",hotcolormap(64));Sgrayplot(tt5,f5/19.49,M5/19.49);title("Fenetre 3000 pts km/h filtre 1kHz-6kHz");

    s2f=filtrageVitesse(s2(1:7000)); 
    [tt5,f5,M5]=animFFT(s2f,44100,300,0);
    M5Sqr = M5.^2;
    figure();plot(f5/19.49,sum(M5Sqr,'r'));
    [v,l]=max(sum(M5Sqr,'r'));
    vBall = f5(l)/19.49
    vClubMax = vBall /1.2;
    vClubMin = vBall / 1.5;
    ll = find((f5/19.49) < vClubMax);
    [v,l]=max(sum(M5Sqr(:,ll),'r'));
    vClub = f5(l)/19.49
    SmashFactor = vBall / vClub

    s2f=filtrageSpin2(s2(1:30000));
    [tt1,f1,M1]=animFFT(s2f,44100,30,0);
    [tt2,f2,M2]=animFFT(s2f,44100,300,0);
    [tt3,f3,M3]=animFFT(s2f,44100,600,0);
    [tt4,f4,M4]=animFFT(s2f,44100,1200,0);
    [tt5,f5,M5]=animFFT(s2f,44100,3000,0);
    figure();
    subplot(3,2,1);plot(s2f);
    subplot(3,2,2);xset("colormap",hotcolormap(64));Sgrayplot(tt1,f1*60,M1*60);title("Fenetre 30 pts RPM filtre 0kHz-200Hz");
    subplot(3,2,3);xset("colormap",hotcolormap(64));Sgrayplot(tt2,f2*60,M2*60);title("Fenetre 300 pts RPM filtre 0kHz-200Hz");
    subplot(3,2,4);xset("colormap",hotcolormap(64));Sgrayplot(tt3,f3*60,M3*60);title("Fenetre 600 pts RPM filtre 0kHz-200Hz");
    subplot(3,2,5);xset("colormap",hotcolormap(64));Sgrayplot(tt4,f4*60,M4*60);title("Fenetre 1200 pts RPM filtre 0kHz-200Hz");
    subplot(3,2,6);xset("colormap",hotcolormap(64));Sgrayplot(tt5,f5*60,M5*60);title("Fenetre 3000 pts RPM filtre 0kHz-200Hz");

    s2f=filtrageVitesse(s2(1:6000));
    [tt1,f1,M1]=animFFT(s2f,44100,30,0);
    [tt2,f2,M2]=animFFT(s2f,44100,300,0);
    [tt3,f3,M3]=animFFT(s2f,44100,600,0);
    [tt4,f4,M4]=animFFT(s2f,44100,1200,0);
    [tt5,f5,M5]=animFFT(s2f,44100,3000,0);
    figure();
    subplot(3,2,1);plot(s2f);
    subplot(3,2,2);xset("colormap",matcolormap(64));Sgrayplot(tt1,f1*60,M1*60);title("Fenetre 30 pts RPM filtre 1kHz-6kHz");
    subplot(3,2,3);xset("colormap",matcolormap(64));Sgrayplot(tt2,f2*60,M2*60);title("Fenetre 300 pts RPM filtre 1kHz-6kHz");
    subplot(3,2,4);xset("colormap",matcolormap(64));Sgrayplot(tt3,f3*60,M3*60);title("Fenetre 600 pts RPM filtre 1kHz-6kHz");
    subplot(3,2,5);xset("colormap",matcolormap(64));Sgrayplot(tt4,f4*60,M4*60);title("Fenetre 1200 pts RPM filtre 1kHz-6kHz");
    subplot(3,2,6);xset("colormap",matcolormap(64));Sgrayplot(tt5,f5*60,M5*60);title("Fenetre 3000 pts RPM filtre 1kHz-6kHz");
endfunction


function [vBall, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info(M5,f5,Club)
    M5Sqr = M5.^2;
    //figure();g=gca();g.axes_reverse=["on","off","off"];plot(sum(M5.^2,'r'),f5/19.49);
    [v,l]=max(sum(M5Sqr,'r')); // 2 version, a confirmer !!
    //[v,l]=max(M5Sqr,'c');
    vBall = max(f5(l)/19.49);
    vClubMax = vBall /1.2;
    vClubMin = vBall / 1.5;
    ll = find(((f5/19.49) >= vClubMin) & ((f5/19.49) <= vClubMax));
    [v,l]=max(sum(M5Sqr(:,ll),'r'));
    vClub = f5(ll(l))/19.49;
    SmashFactor = vBall / vClub;
    [thetaLoft, ShafLeanImp, launchAngle, SpinZ] = LaunchAngle(vBall, vClub, Club,0);
    //disp(SmashFactor,vClub, vBall);
endfunction

function [vBall, vBallCorrigee, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,PointShoot,tt5x,f5x] = Info2(xg, Club, Fs)
    //fichier = uigetfile("*.wav", "D:\Users\f009770\Documents\Golf\Radar\");[x,Fs,bits]=wavread(fichier); 
    //xg = x(1,1:$);xd = x(2,1:$);xd = xd - mean(xd);
    xg = xg - mean(xg);
    u = padding3(xg);
    Fs = Fs*2;
    //
    //  Recherche de la vitesse de la Balle
    //
    [wft,wfm,fr] = wfir('bp',256,[1000/Fs 6000/Fs ],'re',[-1 -1]); // passe bande
    xf = filter(wft,1,u); // passe bande filtrage centre sur 1kHz 6kHz
    [M5,tt5,f5] = animDensite3(xf,Fs, 256*16 ,32*16 , 0, -15,1);//xtitle("Freq complet");
    //fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5,M5(:,1:$-1));
    M5Sqr = M5.^2;
    [v,l]=max(sum(M5Sqr,'r')); // 2 version, a confirmer !
    FBall = max(f5(l));
    vBall = FBall / 19.49;
    [wftsb,wfm,fr] = wfir('sb',255,[(FBall*.95)/Fs (FBall*1.05)/Fs ],'re',[-1 -1]); // coupe bande autour de la balle 

    // JAUNE Tout le SIGNAL band pass sur 1000 à 6000
    [M5,tt5,f5]=animDensite3(xf,Fs, 256*2 ,32*2 , 0, -15,1);//xtitle("Freq complet");
    [m,k] = max(sum(M5.^2,'r')); 
    f=figure();plot(f5,sum(M5(1:$,1:$).^2/m,'r'),'y');     //
    //[m,k] = max(sum(M5,'r')); 
    //FBall = f5(k);vBall = FBall / 19.49;

    // Recherche de la vitesse du Club
    //
    Fenetre = 32;dt=0;
    [wft2,wfm,fr] = wfir('bp',256,[(FBall/1.5)/Fs (FBall/1)/Fs ],'re',[-1 -1]); // coupe bande autour de la club 
    [M5,tt5,f5]=animDensite3(filter(wft2,1,xf),Fs, 256*4 ,Fenetre*4 , 0, -150,0); // NON NORMALISE !!
    [m1,k1] = max(sum(M5.^2,'c')); PointShoot = tt5(k1)*Fs;// temps du shoot
    [m,k] = max(sum(M5.^2,'r')); 
    FClub = f5(k);
    vClub = FClub/19.49;

    // BLEUE BandPass du début au shoot Club. Vitesse du Club
    //[M5,tt5,f5]=animDensite(filter(wft2 ,1,xf(1:PointShoot+200)),Fs, 256*4 ,Fenetre*4 , 0, -50, 0); //xtitle("Freq around Smash Club");
    [m1,k1]=max(xf);
    [M5,tt5,f5]=animDensite3(filter(wft2 ,1,xf(k1-3*1024:k1+4*1024)),Fs, 256*4 ,Fenetre*4 , 0, -10, 0);
    [m,k] = max(sum(M5.^2,'r')); 
    scf(f);plot(f5,sum(M5(1:$,1:$).^2,'r')/m,'b'); // vitesse du shaft (1er pic), vitesse de la balle 2eme pic)    //
    FClub = f5(k);
    vClub = FClub/19.49;

    // ROUGE Hipass coupé de FreqBall
    [wft3 ,wfm,fr] = wfir('bp',255,[(FBall)/Fs (FBall+1500)/Fs ],'hm',[-1 -1]); // Passe haut arpès de la balle 
    [M5,tt5,f5]=animDensite3(filter(wft3 ,1,filter(wftsb ,1,xf(PointShoot+dt:$*3/4))),Fs, 256*16 ,32*16 , 0, -5,1);
    [m,k] = max(sum(M5,'r')); 
    scf(f);plot(f5,sum(M5(1:$,1:$)/m,'r'),'r'); // vitesse du shaft (1er pic), vitesse de la balle 2eme pic)
    FreqSpinMax = f5(k);

    // MAGENTA Pass Bas coupée de FreqBall
    [wft4,wfm,fr] = wfir('bp',256,[(FBall-1400)/Fs (FBall)/Fs ],'hm',[-1 -1]); 
    [M5,tt5,f5]=animDensite3(filter(wft4,1,filter(wftsb ,1,xf(PointShoot+dt:$*3/4))),Fs, 256*16 ,32*16 , 0, -5,1);
    [m,k] = max(sum(M5.^2,'r')); 
    FreqSpinMin = f5(k);
    scf(f);plot(f5,sum(M5(1:$,1:$).^2,'r')/m,'m');

    // VERT CoupeBand de FreqBall
    [wft,wfm,fr] = wfir('bp',256,[(FBall-1500)/Fs (FBall+1500)/Fs ],'re',[-1 -1]); // passe bande
    [M5,tt5,f5]=animDensite3(filter(wftsb ,1,filter(wft,1,xf(PointShoot+dt:$*3/4))),Fs, 256*16 ,32*16 , 0, -55,1);//coupe bande");
    [m,k] = max(sum(M5.^2,'r')); 
    scf(f);plot(f5,sum(M5(1:$,1:$).^2/m,'r'),'g'); // Passe bas avant de la balle 
    Mref = sum(M5(1:$,1:$).^2,'r');
    [e,f]=find((f5<(vBall*19.49-200)));
    [g,h]=max(Mref(f));FreqSpinMin = f5(h);
    [e,f]=find((f5>(vBall*19.49+100)));
    [g,h]=max(Mref(f));FreqSpinMax = f5(size(Mref,2)-size(Mref(f),2)+h);

    // affichage de la legende
    hl=legend(['vBall: '+string(vBall)+' smashfactor '+ string(vBall / vClub);'vClub: '+string(vClub);'FreqSpinMin: '+string((FreqSpinMin - 0)*1)+' Hz';'FreqSpinMax: '+string(FreqSpinMax);'BackSpinM: '+string(Freq2RpmSpin(FreqSpinMax-FreqSpinMin))+' rpm']);

    SpinLR = 0;//Freq2RpmSpin( [(FreqSpinMin - FBall) , (FreqSpinMax - FBall)]); // spin Left Rigth
    SpinZM = [15.*(FBall - FreqSpinMin), (FreqSpinMax - FBall)*15.;];
    SmashFactor = vBall / vClub;
    [thetaLoft, ShafLeanImp, launchAngle, SpinZC, gamaFacePath] = LaunchAngle(vBall, vClub, Club, SpinLR);
    vBallCorrigee = vBall/cos(launchAngle*%pi/180);    
    //disp(SmashFactor,vClub, vBall);
    [M51,tt5,f5]=animDensite3(filter(wft ,1,xf(PointShoot:$)),Fs, 256 ,32 , 0, -5,1);
    [wft5 ,wfm,fr] = wfir('bp',255,[(FBall+300)/Fs (FBall+1500)/Fs ],'hm',[-1 -1]); // Passe haut arpès de la balle 
    [M51,tt5,f5]=animDensite3(filter(wft ,1,xf(PointShoot:$)),Fs, 256 * 8*4,32 * 8*4 , 0, -5,1);
    //    [M51,tt5,f5]=animDensite(filter(wft3 ,1,xf(PointShoot:$)),Fs, 256 ,32 , 0, -5,1);
    [c,d]=find((M51==1));
    //[c,d]=find((M51>.8));
    figure();
    //plot(tt5(c(1:$-1)),f5(d(1:$-1))-f5(d(2:$)), 'rx');
    plot(tt5(c),f5(d),'bo');
    tt5x = tt5(c);f5x = f5(d);
endfunction

function [xd,SP, F] = Info3(Club,fichier,thrplus,range)
    [x,Fs,bits]=wavread(fichier); 
    xg = x(1,1:$);xd = x(2,1:$);xg = xg - mean(xg); xd = xd - mean(xd);xg = xg([range(1):range(2)]); xd = xd([range(1):range(2)]);
    [wft,wfm,fr] = wfir('bp',256,[1000/Fs 6000/Fs ],'re',[-1 -1]); // passe bande
    u = filter(wft,1,padding3(xg));
    Fs = Fs*2;
    [thr,sorh,keepapp] = ddencmp('den','wv',u);
    xd = wdencmp('gbl',u,'db3',4,thr+thrplus,sorh,keepapp);
    [SP,F]=frpowerspec(xd+0*%i);plot(F($/2:$)*44100*2,SP($/2:$));
    SP=SP($/2:$);F=F($/2:$);
endfunction


function [test, M5, tt5, f5, vClub, vBall, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = LaunchMonitoring(test, Coeff, spin)
    /////////////////////////// Affichage des courbes Simulé de références //////////////
    /////////////////////////// avec la fft capturée                       //////////////
    // lecture du signal
    // lecture de largeur de spectre se fait à -20db
    //
    //fichier = 'serie211.wav';
    forder = 64;
    //[test,Fs,bits] = wavread(fichier); // serie211 : 122.68kmh et 154.23kmh
    test = test - mean(test);
    [wft, wfm, fr] = wfir('bp',forder,[0.044 0.09],'hm',[-1 -1]);
    //Fs = 44100;
    nbEchantillon = 1024*1;
    // boucle de détection de début de swing, ou le joueur bouge
    // l'indice ii donne le début du signal
    for ii=2:size(test , 2)/nbEchantillon,
        s2_fft = abs(real(fft(filter(wft,1, test((ii)*nbEchantillon+1:nbEchantillon*(ii+1))))));
        if (max(s2_fft, 'c') > 3) then,
            break;
        end;
    end;//
    // Identificatin de la vitesse de club
    //[wft,wfm,fr] = wfir('bp',48,[0.044 0.09],'hm',[-1 -1]);
    s2_fft = abs(real(fft(filter(wft,1, test((ii)*nbEchantillon+1:nbEchantillon*(ii+1))))));
    f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);
    //figure();plot(f1(1:$/2)/19.49, s2_fft(1:$/2)/max(s2_fft(1:$/2)),'b');
    [m,k] = max(s2_fft,'c');
    VClub = Fs*((k)-1)/size(s2_fft,2)/19.49;//vitesse probable du club, qui est la première détection de vitesse seuille

    // Identification de la vitesse de balle probable
    s2_fft = abs(real(fft(filter(wft,1, test((ii+4)*nbEchantillon+1:nbEchantillon*(ii+5))))));
    //f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);
    //plot(f1(1:$/2)/19.49, s2_fft(1:$/2)/max(s2_fft(1:$/2)),'r');
    [m,k] = max(s2_fft,'c');
    VBall = Fs*((k)-1)/size(s2_fft,2)/19.49;//vitesse probable de la balle, qui est la première détection de vitesse seuil f1 = Fs*((k)-1)/size(s2_fft,2)

    [M5,tt5,f5]=animDensite(filter(wft,1, test((ii-1)*nbEchantillon+1:nbEchantillon*(ii+10))),44100, 1024 ,16 , 0, -70, 1);
    //fig = figure();fig.color_map = jetcolormap(64);Sgrayplot(tt5,f5/19.49,M5/19.49);
    [vClub, vBall, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZ] = Info(M5,f5,'7');

    // identification précise de la vitesse de la balle avec un filtrage passe bande fin
    freqC = VBall*19.49/Fs ;
    [wft,wfm,fr] = wfir('bp',forder,[freqC freqC+0.000001 ],'hm',[-1 -1]);
    s2_fft = abs(real(fft(filter(wft,1, test((ii+4)*nbEchantillon+1:nbEchantillon*(ii+8))))));
    //f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);
    //plot(f1(1:$/2)/19.49, s2_fft(1:$/2)/max(s2_fft(1:$/2)),'r');
    [m,k] = max(s2_fft,'c');
    VBall = Fs*((k)-1)/size(s2_fft,2)/19.49;//vitesse probable de la balle, qui est la première détection de vitesse seuil

    ///////////////////////////
    // calcul du signal de référence simulé en fonction de la vitesse de balle pour identifier le spin
    //
    //t = (0:size(f1, 2)-1)/Fs;
    //spin = 5500; //a déterminer par corrélation entre simulation et capture

    Ref_total = GenereSignalSimu(44100, 4096, 0.03, VClub, VBall, spin, Coeff, 0);
    ff1 = Fs*(0:(size(Ref_total,2))-1)/size(Ref_total,2); 

    [wft,wfm,fr] = wfir('bp',forder,[(VBall-5)*19.49/Fs (VBall+5)*19.49/Fs],'hm',[-0 -0]);
    //Ref_total_fft = abs(real(fft(Ref_total)));//
    Ref_total_fft = abs(real(fft(filter(wft,1,Ref_total))));

    Ref_spin  = GenereSignalSimu(44100, 4096, 0.03, VClub, VBall, spin, [0, 1, 0],0);
    [wft,wfm,fr] = wfir('bp',forder,[(VBall-5)*19.49/Fs (VBall+5)*19.49/Fs],'hm',[-1 -1]);
    Ref_spin_fft = abs(real(fft( Ref_spin)));//

    Ref_ball = GenereSignalSimu(44100, 4096, 0.03, VClub, VBall, spin, [1, 0, 0],0);
    [wft,wfm,fr] = wfir('bp',forder,[(VBall-5)*19.49/Fs (VBall+5)*19.49/Fs],'hm',[-1 -1]);
    Ref_ball_fft = abs(real(fft(filter(wft,1,Ref_ball)))); // 

    Ref_club = GenereSignalSimu(44100, 4096, 0.03, VClub, VBall, spin, [0, 0, 1],0);
    [wft,wfm,fr] = wfir('bp',forder,[(VClub-5)*19.49/Fs (VClub+5)*19.49/Fs],'hm',[-1 -1]);
    Ref_club_fft = abs(real(fft(Ref_club))); // 

    //////////////////////////////////////////
    // affichage des courbes simulées et captées
    figure();
    plot2d(ff1(1:$/2)/19.49,[ ...
    Ref_total_fft(1:$/2)/max(Ref_total_fft(1:$/2)); ...
    Coeff(1)*Ref_ball_fft(1:$/2)/max(Ref_ball_fft(1:$/2));...
    Coeff(2)*Ref_spin_fft(1:$/2)/max(Ref_spin_fft(1:$/2));...
    Coeff(3)*Ref_club_fft(1:$/2)/max(Ref_club_fft(1:$/2))]');
    f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);
    plot(f1(1:$/2)/19.49, s2_fft(1:$/2)/max(s2_fft(1:$/2)),'r');

    //    freqC = VBall*19.49/Fs ;
    //    [wft,wfm,fr] = wfir('sb',129,[freqC freqC+0.0000001 ],'re',[-1 -1]);
    //    s2_fft = abs(real(fft(filter(wft,1, test((ii+4)*nbEchantillon+1:nbEchantillon*(ii+8))))));
    //    plot(f1(1:$/2)/19.49, s2_fft(1:$/2)/max(s2_fft(1:$/2)),'r');
    //

    legends(['Signal Total';'Signal Ball: ' + string(VBall) + ' kmh';'Signal Spin: '+ string(spin(1))+' '+string(spin($))+'rpm';'Signal Capturé'; 'Signal Club VClub: ' + string(VClub) + ' kmh'],[1 2 3 5 4],opt=1);



    //    Pxy = fft(filter(wft,1, test((ii+4)*nbEchantillon+1:nbEchantillon*(ii+8))));
    //    figure();
    //    subplot(2,1,1);plot(ff1(1:$/2),Pxy(1:$/2));
    //    a=angle(Pxy(1:$/2));
    //    subplot(2,1,2);plot(ff1(1:$/2),a);
    //
    //    Pxy = fft(filter(wft,1, Ref_total/10));
    //    //figure();
    //    subplot(2,1,1);plot(ff1(1:$/2),Pxy(1:$/2),'r');
    //    a=angle(Pxy(1:$/2));
    //    subplot(2,1,2);plot(ff1(1:$/2),a,'r');
endfunction



//-----------------------------------------------------------------------------
// Written by Philippe.CASTAGLIOLA@univ-nantes.fr
// Université de Nantes & IRCCyN UMR CNRS 6597
//-----------------------------------------------------------------------------
function Y=savitzkygolayM(X,p,nL,nR)
    //-----------------------------------------------------------------------------
    [argout,argin]=argn()
    if (argin<3)|(argin>4)
        error("incorrect number of arguments")
    end
    if (p<=0)|(p~=floor(p))
        error("argument ''p'' must be an integer >= 1")
    end
    if (nL<0)|(nL~=floor(nL))
        error("argument ''nL'' must be an integer >= 0")
    end
    if argin==3
        nR=nL
    end
    if (nR<0)|(nR~=floor(nR))
        error("argument ''nR'' must be an integer >= 0")
    end
    n1=nL+nR+1
    if n1<p
        error("condition nL+nR+1 >= p not satisfied")
    end
    [row,col]=size(X)
    n=row*col
    X=[X(1)*ones(nL,1);matrix(X,n,1);X(n)*ones(nR,1)]
    n=n+nL+nR
    C=pinv(cumprod([ones(n1,1),(-nL:nR)'*ones(1,p)],"c"))
    c=C(1,:)
    Y=zeros(X)
    for i=1:n1
        Y(nL+1:n-nR)=Y(nL+1:n-nR)+X(i:n-n1+i)*c(i)
    end
    Y=matrix(Y(nL+1:n-nR),row,col)
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////
//This file is part of the Cardiovascular Wawes Analysis toolbox
//Copyright (C) 2014 - INRIA - Serge Steer
//This file must be used under the terms of the CeCILL.
//This source file is licensed as described in the file COPYING, which
//you should have received as part of this distribution.  The terms
//are also available at
//http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
function y=sgolayfilt(X,varargin)
    //filter signal using  Savitzky-Golay Filter.
    // y=sgolayfilt(X,B)
    // y=sgolayfilt(X,k,nf [,w]) 
    //
    //   X :
    //   B : a real n by n array: the set of filter coefficients: the early rows
    //       of B smooth based on future values and later rows smooth based on
    //       past values, with the middle row using half future and half past.
    //       In particular, you can use row i to estimate x(k) based on the i-1
    //       preceding values and the n-i following values of x values as 
    //       y(k) = B(i,:) * x(k-i+1:k+n-i).

    //   k : a positive scalar with integer value: the polynomial order, must
    //        be odd
    //   nf : a positive scalar with integer value: the FIR filter length,
    //        must be odd and greater the k+1 
    //   w : a real vector of length nf with positive entries: the weights.
    //         If omitted no weights are applied.   
    //   
    //   References:

    //   - Abraham Savitzky et Marcel J. E. Golay, « Smoothing and
    //     Differentiation of Data by Simplified Least Squares Procedures »,
    //     Analytical Chemistry, vol. 8, no 36,‎ 1964, p. 1627–1639 (DOI 10.1021/ac60214a047)

    //   - http://en.wikipedia.org/wiki/Savitzky%E2%80%93Golay_filter
    if type(X)<>1|~isreal(X) then
        error(msprintf(_("%s: Wrong type for argument %d: Real matrix expected.\n"),...
        "sgolayfilt",1))
    end
    if size(X,1) == 1 then X = X(:);end

    if or(size(varargin)==[2 3]) then
        B = sgolay(varargin(:))
    else
        B=varargin(1)
        if type(B)<>1|~isreal(B) then
            error(msprintf(_("%s: Wrong type for argument %d: Real matrix expected.\n"),...
            "sgolayfilt",2))
        end
    end
    n=size(B,1);
    if size(X,1) < n, 
        error(msprintf(_("%s: Wrong size for argument %d: At least %d expected.\n"),"sgolayfilt",1,size(B,1)))
    end

    // The first k rows of B are used to filter the first k points
    // of the data set based on the first n points of the data set.
    // The last k rows of B are used to filter the last k points
    // of the data set based on the last n points of the dataset.
    // The remaining data is filtered using the central row of B.
    k = floor(n/2);
    z = filter(B(k+1,:), 1, X);
    y = [B(1:k,:)*X(1:n,:) ; z(n:$,:) ; B(k+2:n,:)*X($-n+1:$,:) ];
endfunction

///////////////////////////////////////////////////////////////////////////////////////////////

//This file is part of the Cardiovascular Wawes Analysis toolbox
//Copyright (C) 2014 - INRIA - Serge Steer
//This file must be used under the terms of the CeCILL.
//This source file is licensed as described in the file COPYING, which
//you should have received as part of this distribution.  The terms
//are also available at
//http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
function [B,C] = sgolay(k,nf,w)
    // Savitzky-Golay Filter Design.
    //   k : a positive scalar with integer value: the polynomial order
    //   nf : a positive scalar with integer value: the FIR filter length,
    //        must be odd and greater the k+1 
    //   w : a real vector of length nf with positive entries: the
    //       weights. If omitted no weights are applied.

    //   B : a real n by n array: the set of filter coefficients: the early rows
    //       of B smooth based on future values and later rows smooth based on
    //       past values, with the middle row using half future and half past.
    //       In particular, you can use row i to estimate x(k) based on the i-1
    //       preceding values and the n-i following values of x values as 
    //       y(k) = B(i,:) * x(k-i+1:k+n-i).

    //   C : a real n by k+1 array: the matrix of differentiation filters.  Each
    //       column of C is a differentiation filter for derivatives of order
    //       P-1 where P is the column index.  Given a signal X with length nf,
    //       an estimate of the P-th order derivative of its middle value can be
    //       found from:
    //                      (P)
    //                     X   ((nf+1)/2) = P!*C(:,P+1)'*X
    //   References:

    //   - Abraham Savitzky et Marcel J. E. Golay, « Smoothing and
    //     Differentiation of Data by Simplified Least Squares Procedures »,
    //     Analytical Chemistry, vol. 8, no 36,‎ 1964, p. 1627–1639 (DOI 10.1021/ac60214a047)

    //   - http://en.wikipedia.org/wiki/Savitzky%E2%80%93Golay_filter

    if type(k)<>1|int(k)<>k|size(k,'*')<>1 then
        error(msprintf(_("%s: Wrong value for input argument #%d: An integer value expected.\n"),"sgolay",2))
    end

    if k > nf-1 then
        error('The degree must be less than the frame length.'),
    end
    if type(k)<>1|int(k)<>k|size(k,'*')<>1 then
        error(msprintf(_("%s: Wrong type for input argument #%d: a scalar with integer value expected.\n"),...
        "sgolay",1))
    end

    if type(nf)<>1|int(nf)<>nf|size(nf,'*')<>1 then
        error(msprintf(_("%s: Wrong type for input argument #%d: a scalar with integer value expected.\n"),...
        "sgolay",2))
    end

    if modulo(nf,2)<>1 then
        error(msprintf(_("%s: Wrong value for input argument #%d: Must be odd.\n"),"sgolay",2))
    end
    if nf<=k then
        error(msprintf(_("%s: Incompatible input arguments #%d and #%d: Argument #%d expected to be less than argument #%d.\n"),"sgolay",1,2,2,1))
    end

    //compute the Vandermonde matrix
    J=(ones(k+1,1)*(-(nf-1)./2:(nf-1)./2)).^((0:k)'*ones(1,nf));
    if argn(2)==3 then
        if type(w)<>1|~isreal(w) then
            error(msprintf(_("%s: Wrong type for argument %d: Real vector expected.\n"),...
            "sgolay",3))
        end
        if size(w,"*")<>nf then
            error(msprintf(_("%s: Incompatible input arguments #%d and #%d\n"),"sgolay",2,3))
        end
        // Check to see if all elements are positive
        if min(w) <= 0 then
            error(msprintf(_("%s: Wrong values for input argument #%d: Elements must be positive.\n"),...
            "sgolay",3))

        end
        // Diagonalize the vector to form the weighting matrix
        J = J*diag(sqrt(w))
    end
    //Compute the matrix of differentiators C=J'/(J*W*J')
    C = pinv(J); 
    // Compute the projection matrix B
    B = C*J;

endfunction

///////////////////////////////////////////////////////////////////////////////////////////////

function s1 = HighSpeed(FMin, FMax)
    figure();plot(1);ax = gca();ax.auto_clear = 'on';
    Fs = 44100;
    N = 256;//Fs*0.01;
    f = Fs*(0:(N)-1)/N;
    [wft,wfm,fr] = wfir('bp',128,[FMin/Fs FMax/Fs ],'hm',[-1 -1]);

    while %T
        //y = pa_recordwav(N,Fs ,1);
        //s2_fft = abs(real(fft(filter(wft,1,pa_recordwav(N,Fs ,1)'))));
        //plot(f(2:$/2),s2_fft(2:$/2));
        //s2_densite_real = abs(real(s2_fft.*conj(s2_fft)));
        //plot(f(2:$/16),s2_densite_real(2:$/16));
        //[m,k]=max(s2_densite_real);
        //m
        //f(k)/19.49
        //if (max(s2_fft,'c') > 3) then,
        if (max(abs(real(fft(filter(wft,1,pa_recordwav(N,44100 ,1)')))),'c') > .3) then,
            break;
        end;
    end
    s1 = pa_recordwav(N*3000,Fs ,1)';
    plot(s1);
    [M5,tt5,f5]=animDensite(filter(wft,1, s1),44100, 512 ,16 , 1, -75,1);
endfunction


function Ref = GenereSignalSimu(Fs, N, TImpact, VClub, VBall, RPMSpin,Coeff, Phase)
    f1 = Fs*(0:N-1)/N;
    t = (0:size(f1,2)-1)/Fs;

    //// AM Modulation
    //BasebandFrequency = 10e3;
    //CarrierFrequency = 100e3;
    //SamplingFrequency = 1e6;
    //BufferLength = 200;
    //n = 0:(BufferLength - 1);
    //BasebandSignal = sin(2*%pi*n / (SamplingFrequency/BasebandFrequency));
    //CarrierSignal = sin(2*%pi*n / (SamplingFrequency/CarrierFrequency));
    //plot(n, BasebandSignal)
    //plot(n, CarrierSignal)
    //ModulatedSignal_AM = CarrierSignal .* (1+BasebandSignal);
    //plot(n, BasebandSignal)
    //plot(n, ModulatedSignal_AM)
    //    
    //
    ////signal module FM
    //BasebandFrequency = 10e3;
    //CarrierFrequency = 100e3;
    //SamplingFrequency = 1e7;
    //BufferLength = 2000;
    //n = 0:(BufferLength - 1);
    //BasebandSignal = sin(2*%pi*n / (SamplingFrequency/BasebandFrequency));
    //CarrierSignal = sin(2*%pi*n / (SamplingFrequency/CarrierFrequency));
    //
    //BasebandSignal_integral = -cos(2*%pi*n / (SamplingFrequency/BasebandFrequency));
    //ModulatedSignal_FM = sin((2*%pi*n / (SamplingFrequency/CarrierFrequency)) + BasebandSignal_integral);
    //plot(n, ModulatedSignal_FM)

    tclub=t;
    kImpact = find(TImpact==t);
    kFin = find((TImpact+0.02)==t);
    Fvb = zeros(tclub);
    Fvb(kImpact:$) = VBall*19.49;
    tvb=t;
    tvb(1:kImpact)=0;
    tclub(kFin-1:$)=0;
    Fclub = zeros(tclub);
    Fclub(1:kImpact*3) = (VClub*19.49);//*(-(linspace(-1.,1,size(tclub(1:kImpact*3),2))).^2+0);
    Fclub(1:kImpact*3) = Fclub(1:kImpact*3) - min(Fclub);
    if (size(RPMSpin,2) == 1)
        Fspin1 = zeros(tvb);
        Fspin1(kImpact:$) = RpmSpin2Freq(RPMSpin);
        Fspin2 = zeros(tvb);
        Fspin2(kImpact:$) = RpmSpin2Freq(RPMSpin);
    else
        Fspin1 = zeros(tvb);
        Fspin1(kImpact:$) = RpmSpin2Freq(RPMSpin(1));
        Fspin2 = zeros(tvb);
        Fspin2(kImpact:$) = RpmSpin2Freq(RPMSpin(2));
    end;

    CoeffClub = [-sin(linspace(%pi,2*%pi,size(find(tclub~=0),2))) * Coeff(3) ];
    CoeffClub = [CoeffClub tclub(kFin-2:$)];
    Ref = Coeff(1)*sin(2*%pi*Fvb.*tvb + Phase) + Coeff(2)*(1*sin(2*%pi*(Fvb - Fspin1).*tvb + Phase) + 1*sin(2*%pi*(Fvb + Fspin2).*tvb) + Phase) + Coeff(3).*sin(2*%pi*CoeffClub.*Fclub.*tclub + 0) ;//;
    Ref = 0.15*Ref/max(Ref);
endfunction

function F=TOC(f,psi,A,B)
    // f expression littérale de la fonction
    // psi expression littérale de psi
    // A facteur d'échelle
    // B valeur de temps
    // Le résultat est une matrice comportant autant de lignes
    // que de facteurs d'échelle A et autant de colonnes
    // que de valeurs de temps
    F=zeros(length(A),length(B));
    [nbl nbc]=size(F);
    // Valeur du facteur d'échelle
    for i=1:nbl
        a=A(i);
        // Valeur de temps
        for j=1:nbc
            b=B(j);
            // Intégration du produit de l'ondelette par la
            // fonction au point a,b
            w=inttrap(t,eval(f).*eval(psi));
            F(i,j)=w;
        end
    end
endfunction
///////////////////////////////////////////////////////////////////////////////////////////////
function [VBall, VClub, Spin, SpinAxis, xresult]=GetSwing(x, Fs, FreqMin, FreqMax, seuil)
    x = x - mean(x);
    v = format();
    format(5);
    Fenetre = 4;
    nbEchantillon = 256*4/Fenetre;
    // boucle de détection de début de swing, ou le joueur bouge
    // l'indice ii donne le début du signal
    ii=1;
    s2_fft = 0;
    while ((max(s2_fft,'c') < seuil)),
        ii=ii+1;
        s2_fft = FftFiltree(x(nbEchantillon*(ii)*Fenetre+1:nbEchantillon*(ii+1)*Fenetre),Fs, FreqMin, FreqMax, 0);
    end;
    // Identification de la vitesse du Club
    [s2_fft, f1] = FftFiltree(x((ii*0.5)*nbEchantillon*Fenetre+1:nbEchantillon*Fenetre*(ii+1.5)),Fs,FreqMin,FreqMax, 0);
    [m,k] = max(s2_fft,'c');VClub = Fs*((k)-1)/size(s2_fft,2)/19.49;//vitesse probable du club, qui est la première détection de vitesse seuille

    // Identification de la vitesse de balle probable
    // itération 4 x, on prend le max d chaque itération, puis le max des max.
    VBall = 0;
    for jj = -2:0//8:24
        //pause
        [s2_ffta, f1] = FftFiltree(x((ii+(4+jj)*Fenetre)*nbEchantillon+1:nbEchantillon*(ii+(5+jj)*Fenetre)),Fs,FreqMin,FreqMax, 0);
        [m1,k1] = max(s2_ffta,'c');VBall = max(VBall,Fs*((k1)-1));//vitesse probable de la balle, qui est la première détection de vitesse seuil f1 = Fs*((k)-1)/size(s2_fft,2)
    end;
    VBall = VBall/size(s2_ffta,2)/19.49;

    // Calcul du SPIN
    MIN = 0;
    for jj=2:3
        [s2_fftb, f1] = FftFiltree(x((ii+(4+jj)*Fenetre)*nbEchantillon+1:nbEchantillon*(ii+(5+jj)*Fenetre)),Fs,(VBall*19.49-RpmSpin2Freq(10000)) , (VBall*19.49-RpmSpin2Freq(2000)), 0);
        [m1 k1]=max(s2_fftb); MIN = max(MIN, (f1(k1)));
    end;

    MAX = 0;
    for jj=2:3
        [s2_fftc, f1] = FftFiltree(x((ii+(4+jj)*Fenetre)*nbEchantillon+1:nbEchantillon*(ii+(5+jj)*Fenetre)),Fs,(VBall*19.49+RpmSpin2Freq(2000)) , (VBall*19.49+RpmSpin2Freq(10000)), 0);
        [m1 k1]=max(s2_fftc); MAX = max(MAX,(f1(k1)));
    end;

    Spin = [(Freq2RpmSpin(-MIN+VBall*19.49) ) Freq2RpmSpin((MAX - VBall*19.49)) Freq2RpmSpin((MAX-VBall*19.49)-(VBall*19.49-MIN))];
    SpinAxis = asin(Spin(3)/sqrt(((Spin(1)+Spin(2))/2)^2+Spin(3)^2))*180/%pi;
    //V0Ballms*sin(LaunchAngle);                   // Vy
    //gamaFacePath = asin(Spin(3) / (V0initms(4)* Sac.coeffSpinLift(Sac.Type == Club)));// 

    xresult = x((ii-1)*nbEchantillon+1:nbEchantillon*(ii+8));
    figure();
    f1 = Fs*(0:(size(s2_ffta,2))-1)/size(s2_ffta,2);
    plot(f1(1:$/2), s2_ffta(1:$/2)/max(s2_ffta(1:$/2)),'black'); // Centré sur le mouvement de la balle avec un passe bande large
    f1 = Fs*(0:(size(s2_fft,2))-1)/size(s2_fft,2);
    plot(f1(1:$/2), s2_fft(1:$/2)/max(s2_fft(1:$/2)),'r');       // Centré sur la vitesse du Club avec un passe bande étroit +/- 10Hz
    f1 = Fs*(0:(size(s2_fftb,2))-1)/size(s2_fftb,2);
    plot(f1(1:$/2), s2_fftb(1:$/2)/max(s2_fftb(1:$/2)),'cyan');  // centré sur le Spin MIN
    f1 = Fs*(0:(size(s2_fftc,2))-1)/size(s2_fftc,2);
    plot(f1(1:$/2), s2_fftc(1:$/2)/max(s2_fftc(1:$/2)),'b');     // centré sur le Spin Max
    title(['VClub: '+ string(VClub) + '  VBall: ' + string(VBall)+ '  Spin: ' +  string(Spin(1)) + ' ' + string(Spin(2)) + ' ' + string(Spin(3))]);
    legend(['VBall: ' + string(VBall) ; 'Spin bas: ' +  string(Spin(2)) ; 'VClub: '+ string(VClub) ; 'Spin haut: ' +  string(Spin(1))]);
    format(10);
endfunction 


function V=DebruitageGauche(U, scale, delay, threshold)
    // Débruitage par seuillage d'un signal u utilisant la transformée en ondelettes
    // par moyennisaton sur le demi-axe réel négatif (filtre presque causal avec un dÃ©lai)
    W=2^(scale-1)*6+delay; // largeur de la fenêtre
    // Initialisation
    x=Initialize(W);
    for n=2:delay
        // Mise Ã jour de l'état sans calcul de la sortie
        u=U(n-1);
        x=Update(x,u);
    end
    for n=(delay+1):length(U)
        // Mise Ã jour de l'état
        u=U(n-1);
        x=Update(x,u);
        // calcul de la sortie
        u=U(n);
        v=Outputs(x,u,scale, delay,threshold);
        V(n-delay)=v;
    end
endfunction


function[y] = pmdemod(x,Vc,Em,Ec,fc,fs)
    //x - FM modulated signal
    //Vc- carrier amplitude signal
    //Em - Message signal amplitude in volts
    //Ec - Carrier signal amplitude in volts
    //fc - Carrier signal frequency in Hz
    //fs - Sampling frequency in samples/sec
    Xdiff = diff(x); //Converting the FM signal into AM signal
    //Envelope changes are recovered by taking the difference
    //between successive samples
    Xdiff = [Xdiff,Xdiff($)];
    //AM demodulation using envelope detector
    Xdem = Xdiff.*Vc;
    //Digital IIR butterworth filter of 5th order, cutoff freuency = fc/fs
    H =  iir(5,'lp','butt',[fc/fs,0],[0,0]);
    num = coeff(H(2));
    den = coeff(H(3));
    num = num(length(num):-1:1);
    den = den(length(den):-1:1);
    [y,zi] =  filter(num,den,abs(Xdem));//AC to DC conversion and LPF filtering
    y = filter(num,den,y,zi);
    plot(y,'r');
    title('PM demodulated Signal')
endfunction


function y = fmdemod(x,fc,fs)
    //x - FM modulated signal
    //Vc- carrier amplitude signal
    //Em - Message signal amplitude in volts
    //Ec - Carrier signal amplitude in volts
    //fc - Carrier signal frequency in Hz
    //fs - Sampling frequency in samples/sec
    //Em = 2;//2 volts
    //Vc = 6;//6 volts
    //fc = 10;//in hertz
    //fs = 100;//in samples/sec
    //y = fmdemod(x,Vc,Em,Ec,fc,fs);
    Xdiff = diff(x); //Converting the FM signal into AM signal
    t=getT(xgf,Fs);
    Vc = cos(2*%pi*fc*t);
    //Envelope changes are recovered by taking the difference
    //between successive samples
    Xdiff = [Xdiff,Xdiff($)];
    //AM demodulation using envelope detector
    Xdem = Xdiff.*Vc;
    //Digital IIR butterworth filter of 5th order, cutoff freuency = fc/fs
    H =  iir(5,'lp','butt',[fc/fs,0],[0,0]);
    num = coeff(H(2));
    den = coeff(H(3));
    num = num(length(num):-1:1);
    den = den(length(den):-1:1);
    [y,zi] =  filter(num,den,abs(Xdem));//AC to DC conversion and LPF filtering
    y = filter(num,den,y,zi);
    //plot(y,'r');
    //title('FM demodulated Signal')
endfunction

function[y]=ampdemod(x,fc,fs,Em,Ec)
    //x - AM modulated signal
    //Vc - carrier signal
    //fc - carrier frequency
    //Em - message signal amplitude
    //Ec- carrier signal amplitude
    t=getT(xgf,Fs);
    Vc = cos(2*%pi*fc*t);
    xdem = x.*Vc;
    //IIR digital butterworth Low Pass filter of cutoff
    //frequency fc/fs and order 7
    H = iir(7,'lp','butt',[fc/fs,0],[.1,.1]);
    num = coeff(H(2));
    den = coeff(H(3));
    num = num(length(num):-1:1);
    den = den(length(den):-1:1);
    y = filter(num,den,xdem);
    //    y = y/Em;
    //    y = y-Ec;plot()
endfunction 

function [x,x2] = demod(y, Fc, Fs, method, P1)
    //%DEMOD Signal demodulation for communications simulations.
    //%   X = DEMOD(Y,Fc,Fs,METHOD,OPT) demodulates the carrier signal Y with a 
    //%   carrier frequency Fc and sampling frequency Fs, using the demodulation
    //%   scheme in METHOD.  OPT is an extra, sometimes optional, parameter whose
    //%   purpose depends on the demodulation scheme you choose.
    //%
    //%   Fs must satisfy Fs > 2*Fc + BW, where BW is the bandwidth of the
    //%   modulated signal.
    //%
    //%        METHOD            DEMODULATION SCHEME
    //%    'am',      Amplitude demodulation, double side-band, suppressed carrier
    //%    'amdsb-sc' OPT not used.
    //%    'amdsb-tc' Amplitude demodulation, double side-band,transmitted carrier
    //%               OPT is a scalar which is subtracted from the decoded message
    //%               signal.  It defaults to zero.
    //%    'amssb'    Amplitude demodulation, single side-band
    //%               OPT not used.
    //%    'fm'       Frequency demodulation
    //%               OPT is a scalar which specifies the constant of frequency 
    //%               modulation kf, which defaults to 1.
    //%    'pm'       Phase demodulation
    //%               OPT is a scalar which specifies the constant of phase 
    //%               modulation kp, which defaults to 1.
    //%    'pwm'      Pulse width demodulation
    //%               By setting OPT = 'centered' you tell DEMOD that the pulses 
    //%               are centered on the carrier period rather than being 
    //%               "left justified".
    //%    'ppm'      Pulse position demodulation
    //%               OPT is not used.
    //%    'qam'      Quadrature amplitude demodulation
    //%               For QAM signals, use [X1,X2] = DEMOD(Y,Fc,Fs,'qam')
    //%
    //%   If Y is a matrix, its columns are demodulated.
    //%
    //%   % Example:
    //%   %   Demodulate a received signal which is frequency modulated at 
    //%   %   carrier frequency 3KHz.
    //%   
    //%   Fs = 8000;                              % sampling frequency
    //%   t = (0:1000-1)/Fs;                      % time vector
    //%   s = 4*cos(2*pi*500*t);                  % modulating signal
    //%   x = modulate(s,3e3,Fs,'fm',0.1);        % modulated signal
    //%   rx = x + sqrt(1e-3)*randn(size(x));     % received signal
    //%   y = demod(rx,3e3,Fs,'fm');              % demodulated signal
    //%   [px,fx] = pwelch(rx,[],0,length(x),Fs); % PSD, received signal
    //%   [py,fy] = pwelch(y,[],0,length(y),Fs);  % PSD, demodulated signal
    //% 
    //%   subplot(211);
    //%   plot(fx/1e3,px); grid on
    //%   title('Received signal')
    //%   ylabel('Amplitude(Watts)')
    //% 
    //%   subplot(212);
    //%   plot(fy/1e3,py,'r'); grid on
    //%   title('Demodulated Signal')
    //%   xlabel('Frequency(KHz)');
    //%   ylabel('Amplitude(Watts)')
    //%
    //%   See also MODULATE in the Signal Processing Toolbox, and PAMDEMOD, 
    //%   QAMDEMOD, GENQAMDEMOD, FSKDEMOD, PSMDEMOD, MSKDEMOD in the
    //%   Communications System Toolbox.
    //
    //%   Copyright 1988-2013 The MathWorks, Inc.

    //narginchk(3,5)
    //% Cast to enforce precision rules
    //Fc = signal.internal.sigcasttofloat(Fc,'double','demod','Fc',    'allownumeric');
    //Fs = signal.internal.sigcasttofloat(Fs,'double','demod','Fs',    'allownumeric');

    //% Checks if Y is a valid numeric data input. Cast to double since filtfilt
    //% does not accept single inputs. 
    //isInputSingle = signal.internal.sigcheckfloattype(y,'single','demod','Y');
    y = double(y);
    x2=[];

    if Fc >= Fs/2,
        error(message('signal:demod:InvalidRange'));
    end

    if nargin<4,
        method = 'am';
    end

    [r,c]=size(y);
    if r*c == 0,
        x = []; return
    end
    if (r==1),   //% convert row vector to column
        y = y(:);  len = c;
    else
        len = r;
    end

    if strcmpi(method,'fm'),
        if nargin < 5, P1 = 1; end
        //P1 = signal.internal.sigcasttofloat(P1,'double','demod','OPT',        'allownumeric');
        t = (0:1/Fs:((len-1)/Fs))';
        t = t(:,ones(1,size(y,2)));
        yq = hilbert(y).*exp(-%i*2*%pi*Fc*t);
        x = (1/P1)*[zeros(1,size(yq,2)); diff(unwrap(angle(yq)))];
        x2=[];
    elseif strcmpi(method,'pm'),
        if nargin < 5, P1 = 1; end
        //P1 = signal.internal.sigcasttofloat(P1,'double','demod','OPT',        'allownumeric');
        t = (0:1/Fs:((len-1)/Fs))';
        t = t(:,ones(1,size(y,2)));
        yq = hilbert(y).*exp(-%i*2*%pi*Fc*t);
        x = (1/P1)*angle(yq);
    elseif strcmpi(method,'pwm'),
        //% precondition input by thresholding:
        y = y>.5;
        t = (0:1/Fs:((len-1)/Fs))';
        len = ceil( len * Fc / Fs);   //% length of message signal
        x = zeros(len,size(y,2));
        if nargin < 5
            P1 = 'left';
        end
        if strcmpi('centered',P1)
            for i = 1:len,
                t_temp = t-(i-1)/Fc;
                ind =  (t_temp >= -1/2/Fc) & (t_temp < 1/2/Fc) ;
                for j1 = 1:size(y,2)   //% for each column ...
                    x(i,j1) = sum(y(ind,j1))*Fc/Fs;
                end
            end
            x(1,:) = x(1,:)*2;
        elseif strcmpi('left',P1)
            for i = 1:len,
                t_temp = t-(i-1)/Fc;
                ind =  (t_temp >= 0) & (t_temp < 1/Fc) ;
                for j2 = 1:size(y,2)   //% for each column ...
                    x(i,j2) = sum(y(ind,j2))*Fc/Fs;
                end
            end
        else
            error(message('signal:demod:SignalErr'))
        end
        //%    w=diff([1; find(diff(y))]);   //% <-- a MUCH faster way, but not robust
        //%    x=w(1:2:length(w))/Fs;
    elseif strcmpi(method,'ptm') | strcmpi(method,'ppm'),
        //% precondition input by thresholding:
        y = y>.5;
        t = (0:1/Fs:((len-1)/Fs))'*Fc;
        len = ceil( len * Fc / Fs);   //% length of message signal
        x = zeros(len,size(y,2));
        for i = 1:len
            t_temp = t-(i-1);
            ind = find( (t_temp >= 0) & (t_temp<1) );
            for j3 = 1:size(y,2)    //% for each column ...
                ind2 = y(ind,j3)==1;
                x(i,j3) = t_temp(min(ind(ind2)));
            end
        end
    elseif strcmpi(method,'qam'),
        t = (0:1/Fs:((len-1)/Fs))';
        t = t(:,ones(1,size(y,2)));
        x = 2*y.*cos(2*%pi*Fc*t);
        x2 = 2*y.*sin(2*%pi*Fc*t);
        [b,a]=butter(5,Fc*2/Fs);
        for i = 1:size(y,2),
            x(:,i) = filtfilt(b,a,x(:,i));
            x2(:,i) = filtfilt(b,a,x2(:,i));
        end
        if (r==1),   //% convert x2 from a column to a row if necessary
            x2 = x2.';
        end

        if isInputSingle
            x2 = single(x2);
        end
    end
    if (r==1),   //% convert x from a column to a row
        x = x.';
    end

    //x = single(x);
endfunction

function [enveloppe,t] = Enveloppe(xx, Fs,fb, fh)
    [wft,wfm,fr] = wfir('bp',256,[fb/Fs fh/Fs ],'hm',[-1 -1]);
    t = (0:size(xx,2)-1)/Fs;
    I=filter(wft,1,2*%pi*cos(t*Fs).*xx);
    Q=filter(wft,1,2*%pi*sin(t*Fs).*xx);
    enveloppe = (sqrt(I.^2+Q.^2));
endfunction


function Xenv = specenv(Xamp, f)//peaks=peak_detect(signal,threshold)
    [Xpks, pksind] = findpeaks(Xamp+%eps);
    fpks = (pksind-1)*(f(2) - f(1));
    Xenv = interp1(fpks, Xpks, f, 'pchip');
    Eenv = smooth(Xenv);
endfunction


function [Rxx] = autocorrelation(x)
    //Autocorrelation of a given Input Sequence
    //Finding out the period of the signal using autocorrelation technique

    L = length(x);
    h = zeros(1,L);
    for i = 1:L
        h(L-i+1) = x(i);
    end
    N = 2*L-1;
    Rxx = zeros(1,N);
    for i = L+1:N
        h(i) = 0;
    end
    for i = L+1:N
        x(i) = 0;
    end
    for n = 1:N
        for k = 1:N
            if(n >= k)
                Rxx(n) = Rxx(n)+x(n-k+1)*h(k);
            end
        end
    end
    disp('Auto Correlation Result is')
    Rxx
    disp('Center Value is the Maximum of autocorrelation result')
    [m,n] = max(Rxx)
    disp('Period of the given signal using Auto Correlation Sequence')
    n
endfunction

function y=getDistance(u,club)
    [vBall, vBallCorrigee, vClub, SmashFactor,thetaLoft, ShafLeanImp, launchAngle, SpinZC, SpinZM, SpinLR, gamaFacePath,PointShoot] = Info2(u,club);
    [wft1,wfm,fr] = wfir('bp',256,[(vBall*19.49-1000)/Fs (vBall*19.49+1000)/Fs ],'hm',[-1 -1]);
    up=[u(PointShoot:$) zeros(1,2^ceil(log2(size(u(PointShoot:$),2)))-size(u(PointShoot:$),2))];
    y=filter(wft1,1,up);
    Fs = 44100;
    N = size(y,2);
    TailleWin = 256;
    n = N/TailleWin;
    f = Fs*(0:(TailleWin-1))/TailleWin;
    figure();
    for ii = 0:(n-1)
        s2_fft = abs(real(fft(y((1+ii*TailleWin):((ii+1)*TailleWin)))));
        [m(ii+1),k(ii+1)] = max(s2_fft);
    end;
    pause
endfunction


function xk=getArmonicFFT(u, niem)
    N=size(u,2);
    n = 1:N;
    for k = 1:N
        xk(1,k) =  sum(u.*exp((-2*%pi*%i/N).*n.*k));
    end
endfunction


// first input argument: the amplititude of message signal
// second input argument: the frequency of message signal
function s = cr_mod(valA,valB)
    clf;
    x= [0:0.001:2*%pi]';
    z= cos((50-valB)*x) - cos((50+valB)*x);
    z = valA *z;
    plot(x, z);
    xlabel("Time");
    ylabel("Amplititude");
    xtitle("Plot of amplitude modulated wave");
    s=0;
endfunction

function [x,Vm,Vc,t]=fmmod(Ec,Em,fm,fc,fs,tfin)
    //Ec -carrier amplitude in volts
    //Em - message signal amplitude in volts
    //fm - modulating signal frequency Hz
    //fc - carrier signal frequency in Hz
    //fs - sampling frequency in samples/sec
    //k - frequency sensitivity in Hz/volts
    t = 0:1/fs:tfin;
    Vm = Em*sin(2*%pi*fm*t);
    Vc = Ec*sin(2*%pi*fc*t);
    k = 1.5; 
    x =  Ec*sin((2*%pi*fc*t)+(k*Em/fm)*cos(2*%pi*fm*t));
    subplot(3,1,1)
    plot(t,Vm)
    title('Modulating Signal')
    subplot(3,1,2)
    plot(t,Vc)
    title('Carrier Signal')
    subplot(3,1,3)
    plot(t,x);
    legend('FM Signal')
    title('Frequency Modulated Signal')
endfunction
//Example
//Em = 2; //2 volts
//Ec = 6; //4 volts
//fm = 5; //2 Hz
//fc = 10; //10 Hz
//fs = 100; //samples/sec



function [x,Vm,Vc,t]=ampmod(Ec,Em,fm,fc,fs,tfin)
    //Ec -carrier amplitude in volts
    //Em - message signal amplitude in volts
    //fm - modulating signal frequency Hz
    //fc - carrier signal frequency in Hz
    //fs - sampling frequency in samples/sec
    t = 0:1/fs:tfin;
    Vm = Em*sin(2*%pi*fm*t);
    Vc = Ec*sin(2*%pi*fc*t);
    x = (Ec+Em*sin(2*%pi*fm*t)).*(sin(2*%pi*fc*t));
    subplot(3,1,1)
    plot(t,Vm)
    title('Modulating Signal')
    subplot(3,1,2)
    plot(t,Vc)
    title('Carrier Signal')
    subplot(3,1,3)
    plot(t,x);
    title('Amplitude Modulated Signal')
endfunction
//Example
//Em = 8; //8 volts
//Ec = 10; //20 volts
//fm = 2; //2 Hz
//fc = 10; //10 Hz
//fs = 100; samples/sec
//ampmod(Ec,Em,fm,fc,fs)


function[x,Vm,Vc,t]=pmmod(Ec,Em,fm,fc,fs,tfin)
    //Ec -carrier amplitude in volts
    //Em - message signal amplitude in volts
    //fm - modulating signal frequency Hz
    //fc - carrier signal frequency in Hz
    //fs - sampling frequency in samples/sec
    //k - Phase sensitivity in rad/volts
    t = 0:1/fs:tfin;
    Vm = Em*sin(2*%pi*fm*t);
    Vc = Ec*sin(2*%pi*fc*t);
    k = 1; 
    x =  Ec*sin((2*%pi*fc*t)+(k*Em)*sin(2*%pi*fm*t));
    subplot(3,1,1)
    plot(t,Vm)
    title('Modulating Signal')
    subplot(3,1,2)
    plot(t,Vc)
    title('Carrier Signal')
    subplot(3,1,3)
    plot(t,x);
    legend('PM Signal')
    title('Phase Modulated Signal')
endfunction
//Example
//Em = 2; //2 volts
//Ec = 6; //4 volts
//fm = 5; //2 Hz
//fc = 10; //10 Hz
//fs = 100; samples/sec


function freqMax = afficheDemod(x, Fs, v)
    [wft,wfm,fr] = wfir('bp',254,[1200/Fs 6000/Fs ],'hn',[-1 -1]);
    [x1,y] = demod(x, v * 19.49, Fs,'am');
    [wft1,wfm,fr] = wfir('bp',254,[30/Fs 12000/Fs ],'re',[-1 -1]);
    x1 = filter(wft1,1,x1);
    [M51,tt5,f5]=animDensite(filter(wft1,1,x1), Fs, 256*16 ,32*16 , 1, -0,1);
    figure();plot(f5,sum(M51(:,1:$-1)/max(sum(M51(:,1:$-1),'r')),'r'),'b');
    [x2,y2] = demod(x1, 43, Fs,'pm');
    [wft2,wfm,fr] = wfir('bp',254,[30/Fs 1200/Fs ],'re',[-1 -1]);
    x2 = filter(wft2,1,x2);
    [M51,tt5,f5]=animDensite(x2,44100, 256*16*2 ,32*16*2 , 0, -0,1);
    plot(f5,sum(M51(:,1:$-1)/max(sum(M51(:,1:$-1),'r')),'r'),'r');
    [m,k]=max(sum(M51(:,1:$-1),'r'));
    freqMax = f5(k);
endfunction


function freqMax = afficheDemod2(x, Fs, v)
    [wft,wfm,fr] = wfir('bp',254,[((v-50)*19.49)/Fs ((v+50)*19.49)/Fs ],'hn',[-1 -1]);
    [x2,y2] = demod(filter(wft,1,x), 100, Fs,'pm');
    [wft2,wfm,fr] = wfir('bp',254,[30/Fs 1200/Fs ],'re',[-1 -1]);
    x2 = filter(wft2,1,x2);
    [M51,tt5,f5]=animDensite3(x2,44100, 256*16*2 ,32*16*2 , 0, -0,1);
    plot(f5,sum(M51(:,1:$-1)/max(sum(M51(:,1:$-1),'r')),'r'),'r');

    [x1,y] = demod(x2, v * 19.49, Fs,'am');
    [wft1,wfm,fr] = wfir('bp',254,[30/Fs 200/Fs ],'re',[-1 -1]);
    x1 = filter(wft1,1,x1);
    [M51,tt5,f5]=animDensite3(filter(wft1,1,x1), Fs, 256*16 ,32*16 , 0, -0,1);
    plot(f5,sum(M51(:,1:$-1)/max(sum(M51(:,1:$-1),'r')),'r'),'b');
    legend('Phase Demo','Amp Demo');
    [m,k]=max(sum(M51(:,1:$-1),'r'));
    freqMax = f5(k);
endfunction


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

function AfficheTFCT(y, larFenetre, pasFenetre, beta, Fs)
//   Affichage de la TFCT du signal après fenêtrage
    nbEch = size(y,'r');
    fq=(0:nbEch-1)*Fs/nbEch;
    wTFD=window('hn',nbEch,beta)';
    wTFCT=window('hn',larFenetre,beta)';
    fMin = 1;
    fMax = Fs/2;
    indTFCT=find(fq>= fMin & fq<=fMax);
    
    stft=TFCT(y,wTFCT,pasFenetre,-1);
    [nbFreq nbSpec]=size(stft);// Nombre de fréquences Nombre de spectres
    indFreqHaute=indTFCT($);
    indFreqBasse=indTFCT(1);
    //Matplot(abs(stft(indFreqHaute:-1:indFreqBasse,:))/max(abs(stft))*256);
    f = figure();f.color_map = hotcolormap(4);Sgrayplot(tt5,fq,stft);
endfunction


//
// Transformée de Fourier à court terme
//
function X=TFCT(x,w,pas,signe)
    larFen=length(w);
    if signe==-1 then
        N=length(x);
        origFen=1:pas:N-1;
        X=zeros(larFen,length(origFen));
         for i=1:length(origFen)
            xf=[x(origFen(i):min(origFen(i)+larFen-1,N));zeros(larFen-(min(origFen(i)+larFen-1,N)-origFen(i)+1),1)];
            X(:,i)=fft(xf.*w);
        end
    else
        X=zeros(signe,1);
        sw2=zeros(signe,1);
        [nbFreq nbSpectre ]=size(x);
        for i=1:nbSpectre
            ind=(i-1)*pas+1:min((i-1)*pas+larFen,length(X));
            xx=ifft(x(:,i)).*w;
            X(ind)= X(ind)+xx(1:ind($)-ind(1)+1);
            sw2(ind)=sw2(ind)+w(1:ind($)-ind(1)+1).^2;
        end
        ind=find(sw2==0)
        if length(ind)>0
            disp('Attention fenêtre avec valeur nulle. Reconstruction incomplète');
        end
        
        ind=find(abs(sw2)>0);
        X(ind) = X(ind) ./ sw2(ind);
    end    
endfunction


function [fdb] = F_Dopp_B(t,v,rpm)
    r = 0.043/2;
    Vlumiere = 299792458 ; // m/s 
    Freq = 10.525e9; // 1/s
    lambda = Vlumiere / Freq;
    w = rpm*2*%pi/60;
    fdb = 2*(v+r*w*cos(w*t))/lambda;
endfunction

function s_B = signal_B(t,v,rpm)
    //t=1:size(xg,2);
    // v vitesse en m/s
 
    Vlumiere = 299792458 ; // m/s 
    Freq = 10.525e9; // 1/s
    lambda = Vlumiere / Freq;
    w = rpm*2*%pi/60;
    s_B = signal_A(t,v,rpm).*smod_B(t,v,rpm);
endfunction

function s_A = signal_A(t,v,rpm)
    // v vitesse en m/s
    r = 0.043/2;
    Vlumiere = 299792458 ; // m/s 
    Freq = 10.525e9; // 1/s
    lambda = Vlumiere / Freq;
    w = rpm*2*%pi/60;
    a_t = 2; // amplitude recu du signal ==> Enveloppe
    s_A = a_t * exp(-%i*2*%pi*(2*v/lambda)*t);
endfunction

function s_modb = smod_B(t,v,rpm)
    // v vitesse en m/s
    r = 0.043/2;
    Vlumiere = 299792458 ; // m/s 
    Freq = 10.525e9; // 1/s
    lambda = Vlumiere / Freq;
    w = rpm*2*%pi/60;
    d_t = 0.1; // intensite de reflexion du point B
    s_modb = exp(-%i*2*r*sin(w*t)/lambda)*d_t;
    s_modb = s_modb-mean(s_modb);
endfunction

function rpm = deltaF(df)
    r = 0.043/2;
    rpm = df*60/(2*%pi*r*19.49);
endfunction


function N = getPointShoot(x, Fs, FBall)
    Fenetre = 32;dt=0;
    [wft2,wfm,fr] = wfir('bp',256,[(FBall/1.5)/Fs (FBall/1)/Fs ],'re',[-1 -1]); // coupe bande autour de la club 
    [M5,tt5,f5]=animDensite3(filter(wft2,1,x),Fs, 256*2*4 ,Fenetre*4 , 0, -50,0); // NON NORMALISE !!
    [m1,k1] = max(sum(M5.^2,'c')); N = tt5(k1)*Fs;// temps du shoot
endfunction


function [Line, tt5] = afficheVitesse(x,Fs)
    [wft,wfm,fr] = wfir('bp',254,[1000/Fs 6000/Fs ],'hn',[-1 -1]);
    //[s2_densite,F]=frpowerspec(filter(wft,1,x)+0*%i);
    [M5,tt5,f5]=animDensite3(filter(wft,1,padding2(afficheDenoise(x,0.0))),2*Fs, 256*16*2 ,32*16*2 , 0, -15,0);
    M5Sqr = M5.^2;
    //drawlater();
    for ii=1:size(M5Sqr,1)
        [j,k] = max(M5Sqr(ii,:)); 
        Line(ii) = f5(k);
    end;
    //plot(tt5,Line,'o');
    //drawnow();
endfunction

function Sig = afficheDenoise(x,thrplus)
    [thr,sorh,keepapp] = ddencmp('den','wv',x);
    Sig = wdencmp('gbl',x,'db3',4,thr+thrplus,sorh,keepapp);
endfunction


function tt = getT(x,fs)
    tt=(0:size(x,2)-1)/fs;
endfunction

function y = Acquisition(Fe, D, nbEch)
    global z;
    z=[];
    nbCanaux=2;
    indFluxEntree = 0;
    DefNbCanaux(nbCanaux);
    DefFreqEch(Fe);
    PrepAcq(D,indFluxEntree);
    OuvrirFlux();
    DebutAcq();
    sleep(fix((nbEch*1000)/Fe)+1);
    [x err index]=LectureDonnee(nbEch);
    err=0;
    N=nbEch;
    t=(0:N-1)'/Fe;
    sleep(fix((nbEch*1000)/Fe)+1);
    while (size(z,1) < D*Fe )
        [x err index]=LectureDonnee(nbEch);
        tic()
        y=x(1:1:N)';
        z=[z ;y];
        e=toc();
        tps=fix((nbEch/Fe-e)*1000);
// Temps nécessaire pour avoir de nouveaux échantillons
        if tps>=0
            sleep(tps+1);
        end
    end
    FinAcq();
endfunction

function cf = polyfit1(x,y,n)
    A = ones(length(x),n+1)
    for i=1:n
        A(:,i+1) = x(:).^i
    end
    cf = lsq(A,y(:))
endfunction


function p = polyfitMat(x,y,n)
    x = x(:);
    y = y(:);
    mu= [mean(x);stdev(x)];
    x = (x - mu(1))/mu(2);
    //% Construct the Vandermonde matrix V = [x.^n ... x.^2 x ones(size(x))]
    V(:,n+1) = ones(length(x),1);//,class(x));
    for j = n:-1:1
        V(:,j) = x.*V(:,j+1);
    end
    [Q,R] = qr(V,'e');
    p = R\(Q'*y);
endfunction


function [PSD,F]=pWelch(x,varargin);
  //PSD,F]=pWelch(x,windowl,noverlap,nf,fs,opt)
  if argn(2)<1 then 
    error(msprintf(_("%s: Wrong number of input argument: %d to %d expected.\n"),"pWelch",1,6))
  end
  if type(x)<>1|and(size(x)>1)|~isreal(x) then
    error(msprintf(_("%s: Wrong type for input argument #%d: A  real vector expected.\n"),"pWelch",1))
  end
  x=x(:);
  
  [w,sec_step,F,pad,opt,scal]=pWelch_options(varargin)
  windowl=size(w,'*')
  
  wpower=w'*w;//the window energy
  
  nsecs=int((size(x,"*")-windowl+sec_step)/sec_step);
  if nsecs<=0 then
    PSD=[]
    f=[]
    return
  end
  ind=1:windowl;
  PSD=0;
  for k=1:nsecs
    xd=x(ind+(k-1)*sec_step);
    xe=w.*(xd-mean(xd));
    fx=fft([xe;pad]);
    PSD=PSD+real(fx.*conj(fx));
  end
  
  PSD=PSD/(nsecs*wpower*scal);

  if opt=="onesided" then
    n=ceil((size(F,'*')+1)/2)
    F=F(1:n)
    PSD=2*PSD(1:n)

  end
endfunction



function [w,sec_step,F,pad,opt,scal]=pWelch_options(optlist)
  //windowl,noverlap,nf,fs,opt,scale
  withF=%f
  
  nopt=size(optlist)
  if nopt<1|optlist(1)==[] then
    windowl=int(size(x,'*')/8)
    w=window("hm",windowl)';
  else
    windowl=optlist(1)
    if type(windowl)<>1|~isreal(windowl) then
      error(msprintf(_("%s: Wrong type for input argument #%d: A  real array expected.\n"),"pWelch",2))
    end
    if size(windowl,'*')>1 then
      w=windowl(:)
      windowl=size(w,'*')
    else
      w=window("hm",windowl)';
    end
  end
  if nopt<2|optlist(2)==[] then
    noverlap=round(windowl/2)
  else
    noverlap=optlist(2);
    if type(noverlap)<>1|~isreal(noverlap)|size(noverlap,'*')>1|int(noverlap)<>noverlap then
      error(msprintf(_("%s: Wrong type for input argument #%d: A positive integer expected.\n"),"pWelch",3))
    end
  end
  if nopt<4|optlist(4)==[] then
    fs=1
    scal=2*%pi
  else
    
    fs=optlist(4)
    if type(fs)<>1|size(fs,'*')<>1|fs<=0 then
      error(msprintf(_("%s: Wrong type for input argument #%d: A positive real expected.\n"),"pWelch",5))
    end
    scal=fs
  end
  
  if nopt<3|optlist(3)==[] then
    nf=max(256,2^nextpow2(windowl))
    F=(0:(nf-1))*(fs/nf);
  else
    nf=optlist(3)
    if type(nf)<>1|or(nf<0) then
      error(msprintf(_("%s: Wrong type for input argument #%d: A positive real array expected.\n"),"pWelch",4))
    end
    if size(nf,'*')==1 then
      F=(0:(nf-1))*(fs/nf)
    else
      F=nf
      nf=size(F,'*')
      withF=%t
    end
  end
  
  if nopt<5 then 
    if withF then opt="twosided"; else opt="onesided";end
  else
    opt=optlist(5)
    if type(opt)<>10|size(opt,'*')<>1 then
      error(msprintf(_("%s: Wrong type for input argument #%d: A character string expected.\n"),"pWelch",5))
    end
    if withF&opt<>"twosided" then
      error(msprintf(_("%s: Wrong value for input argument #%d: Must be in the set {%s}.\n"),"pWelch",5,"""twosided"""))
    elseif and(opt<>["onesided","twosided"]) then
       error(msprintf(_("%s: Wrong value for input argument #%d: Must be in the set {%s}.\n"),"pWelch",5,strcat(["""onesided""","""twosided"""],",")))
    end
  end
  F=(0:(nf-1))'*(fs/nf);
  sec_step=windowl-noverlap
  if windowl<nf then
    pad=zeros(nf-windowl,1)
  else
    pad=[]
  end
endfunction


function peaks=peak_detect(signal,threshold)

    // This function detect the peaks of a signal : 
    // --------------------------------------------
    // For an input row vector "signal" , the function return 
    // the position of the peaks of the signal.
    //
    // The ouput "peaks" is a row vector (size = number of peaks),
    // "peaks" =[] if no peak is found.
    //
    // Optional argument "threshold" eliminates the peaks under
    // the threshold value (noise floor). 
    //
    // Clipped peaks (more than 2 samples of the signal at the same value)
    // are not detected.
    // -------------------------------------------------------------------
    //     Jean-Luc GOUDIER      11-2011
    // -------------------------------------------------------------------

    [nargout,nargin] = argn(0);
    if nargin==2 then ts=threshold;
    end;
    if nargin==1 then ts=min(signal);
    end;

    [r c]=size(signal);
    Ct=getlanguage();
    if Ct=="fr_FR" then
        Msg="Erreur : le signal n''est pas un vecteur colonne";
    else
        Msg="Error : signal is not a row vector";
    end
    if r>1 then
        error(Msg);
    end;

    Lg=c-1; 
    d_s=diff(signal); 
    dd_s=[d_s(1),d_s(1,:)];               // diff first shift
    d_s=[d_s(1,:),d_s(Lg)];               // diff size correction
    ddd_s=[dd_s(1),dd_s(1,1:Lg)];         // diff second shift
    Z=d_s.*dd_s;                          // diff zeros

    peaks=find(((Z<0 & d_s<0)|(Z==0 & d_s<0 & ddd_s>0)) & signal>ts);

endfunction

function y = envFIR(x,n)
    //% construct ideal hilbert filter truncated to desired length
    fc = 1;
    t = fc/2 * ((1-n)/2:(n-1)/2)';

    hfilt = sinc(t) .* exp(%i*%pi*t);

    //% multiply ideal filter with tapered window
    beta = 8;
    firFilter = hfilt .* window('kr',n,beta)';
    firFilter = firFilter / sum(real(firFilter));

    //% apply filter and take the magnitude
    y = zeros(x);
    for chan=1:size(x,2)
        y(:,chan) = abs(conv(x(:,chan),firFilter,'same'));
    end
endfunction

// Copyright (C) 2013 - 2016 - Michael Baudin
// Copyright (C) 2010 - DIGITEO - Michael Baudin
// Copyright (C) 1993 - 1995 - Anders Holtsberg
//
// This file must be used under the terms of the CeCILL.
// This source file is licensed as described in the file COPYING, which
// you should have received as part of this distribution.  The terms
// are also available at
// http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt

function [y,delta]=polyval(varargin)
    // Polynomial evaluation
    //
    // Calling Sequence
    //   y = polyval(p,x)
    //   y = polyval(p,x,S)
    //   y = polyval(p,x,S,mu)
    //   y = polyval(p,x,[],mu)
    //   [y,delta] = polyval(p,x,S)
    //   [y,delta] = polyval(p,x,S,mu)
    //
    // Parameters
    //   p : a (n+1)-by-1 matrix of doubles, the coefficients of the polynomial, with powers in decreasing order
    //   x : a nx-by-my matrix of doubles, the points where to evaluate the polynomial
    //   S : the optional data structure from polyfit
    //   y : a nx-by-my matrix of doubles, the value of the polynomial at x
    //   delta : a nx-by-my matrix of doubles, the error estimate on y
    //
    // Description
    // If p is a vector of length n+1 whose elements are the coefficients of
    // a polynomial, then <literal>y=polyval(p,x)</literal> is the value of 
    // the polynomial, defined by its coefficients p, evaluated at x.
    //
    // These coefficients are ordered with powers in decreasing order:
    //
    // <latex>
    // p(x) = p_1 x^n + p_2 x^{n-1} + ... + p_n x + p_{n+1}.
    // </latex>
    //
    // If x is a matrix, the polynomial is evaluated at all
    // points in x.
    //
    // <literal>[y,delta]=polyval(p,x,S)</literal> uses the optional output 
    // generated by <literal>polyfit</literal> to generate error 
    // estimates, y+/-delta.
    // If the observations used by polyfit were associated 
    // with independent random errors from the normal distribution, 
    // approximately 68% of the predictions are in the interval 
    // [p-delta,p+delta].
    //
    // In the example below, we show how to use this 
    // in order to compute the bounds for the population predicted by 
    // a quadratic polynomial. 
    //
    // Examples
    // p=[3 2 1];
    // y=polyval(p,[5 7 9])
    // expected=[86 162 262]
    //
    // // Evaluate for x matrix
    // x = linspace(0,%pi,10);
    // y = sin(x);
    // p = polyfit(x,y,3);
    // // Evaluate at x matrix
    // x = linspace(0,%pi,12);
    // x=matrix(x,3,4)
    // f=polyval(p,x)
    // y=sin(x)
    //
    // // Evaluate 95% confidence bounds, 
    // // i.e. +/- 2*delta
    // // Source: [1,2]
    // cdate=(1790:10:1990)';
    // pop=[
    //     3.929214   
    //     5.308483   
    //     7.239881   
    //     9.638453   
    //     12.860702  
    //     17.063353  
    //     23.191876  
    //     31.443321  
    //     38.558371  
    //     50.189209  
    //     62.979766  
    //     76.212168  
    //     92.228496  
    //     106.02154  
    //     123.20262  
    //     132.16457  
    //     151.3258   
    //     179.32317  
    //     203.30203  
    //     226.5422   
    //     248.70987  
    //  ];
    // scf();
    // plot(cdate,pop,"+")
    // // Calculate fit parameters
    // [p,S] = polyfit(cdate,pop,2);
    // // Evaluate the fit and the prediction error estimate (delta)
    // [pop_fit,delta] = polyval(p,cdate,S);
    // // Plot the data, the fit, and the confidence bounds
    // plot(cdate,pop_fit,"g-")
    // plot(cdate,pop_fit+2*delta,"r:")
    // plot(cdate,pop_fit-2*delta,"r:")
    // // Annotate the plot
    // xlabel("Census Year");
    // ylabel("Population (millions)");
    // title("Quadratic Polynomial Fit with Confidence Bounds")
    // 
    // Authors
    // Copyright (C) 2013 - 2016 - Michael Baudin
    // Copyright (C) 2010 - DIGITEO - Michael Baudin
    // Copyright (C) 1993 - 1995 - Anders Holtsberg
    //
    // Bibliography
    // [1] http://en.wikipedia.org/wiki/Polynomial_interpolation
    // [2] http://www.mathworks.fr/fr/help/matlab/data_analysis/programmatic-fitting.html
    // [3] Numerical Computing with MATLAB, Cleve Moler, 2004
    // 

    [lhs,rhs]=argn();
    apifun_checkrhs ( "polyval" , rhs , 2:4 );
    apifun_checklhs ( "polyval" , lhs , 1:2 );
    //
    p = varargin ( 1 );
    x = varargin ( 2 );
    S = apifun_argindefault(varargin,3,[]);
    mu = apifun_argindefault(varargin,4,[]);
    //
    // Check Type
    apifun_checktype ( "polyval" , p , "p" , 1 , "constant" );
    apifun_checktype ( "polyval" , x , "x" , 2 , "constant" );
    if (S<>[]) then
        apifun_checktype ( "polyval" , S , "S" , 3 , "st" );
    end
    apifun_checktype ( "polyval" , mu , "mu" , 4 , "constant" );
    //
    // Check Size
    apifun_checkvector ( "polyval" , p , "p" , 1 );
    // Do not check x : vector or matrix
    if (mu<>[]) then
        apifun_checkvector ( "polyval" , mu , "mu" , 4, 2 );
    end
    //
    // Check content
    expfields=[
    "R"
    "df"
    "normr"
    ];
    if (S<>[]) then
        if (fieldnames(S)<>expfields) then
            error(msprintf(gettext("%s: Wrong input argument #%d: Expected output argument of polyfit"),"polyval",3));
        end
    end
    if (mu<>[]) then
        alpha = number_properties("tiniest");
        apifun_checkgreq ( "polyval" , mu(2) , "mu(2)" , 4, alpha );
    end
    // Standardize, if required
    if (mu<>[]) then
        x=(x-mu(1))/mu(2);
    end
    //
    y=[];
    delta=[];
    [nargout,nargin] = argn(0);

    [m,n] = size(x);
    nc = max(size(p));

    // Do general case where X is an array
    y = zeros(m,n);
    for i = 1:nc
        y = x .* y+p(i);
    end

    if (rhs>2)&(nargout>1) then
        // Compute delta
        x = x(:);
        R=S.R;
        degreef=S.df;
        normr=S.normr;
        // Construct Vandermonde matrix.
        V(:,nc) = ones(length(x),1);
        for j = nc-1:-1:1
            V(:,j) = x .* V(:,j+1);
        end
        // Solve system with backslash
        E = V/R;
        if nc==1 then
            e = sqrt(1+E .* E);
        else
            e = sqrt(1+sum(E.^2,"c")');
        end
        if degreef==0 then
            warning(msprintf(gettext("%s: zero degrees of freedom implies infinite error bounds."),"polyval"));
            delta = %inf*e;
        else
            delta = e*normr/sqrt(degreef);
        end
        delta = matrix(delta,m,n);
    end
endfunction
// Copyright (C) 2013 - 2016 - Michael Baudin
// Copyright (C) 2010 - DIGITEO - Michael Baudin
// Copyright (C) 1993 - 1995 - Anders Holtsberg
//
// This file must be used under the terms of the CeCILL.
// This source file is licensed as described in the file COPYING, which
// you should have received as part of this distribution.  The terms
// are also available at
// http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt

function [p,S,mu] = polyfit(x,y,n)
    // Polynomial curve fitting
    //
    // Calling Sequence
    //   p = polyfit(x,y,n)
    //   [p,S] = polyfit(x,y,n)
    //   [p,S,mu] = polyfit(x,y,n)
    //
    // Parameters
    //   x : a nx-by-mx matrix of doubles
    //   y : a nx-by-mx matrix of doubles
    //   n : a 1-by-1 matrix of doubles, integer value, n>=0
    //   p : a 1-by-(n+1) matrix of doubles, the coefficients of the polynomial
    //   S : a structure to estimate the errors of evaluations (see below )
    //   mu : a 1-by-2 matrix of doubles, the standardizing parameter : mu(1) is mean(x) and mu(2)=stdev(x). Use mu to standardize the data for a more accurate evaluation of p, using the mean and the standard deviation. 
    //
    // Description
    // <literal>polyfit(x,y,n)</literal> finds the coefficients of a polynomial p(x) of
    // degree n that fits the data, p(x(i)) to y(i), in a least-squares sense.
    //
    // These coefficients are ordered with powers in decreasing order:
    //
    // <latex>
    // p(x) = p_1 x^n + p_2 x^{n-1} + ... + p_n x + p_{n+1}.
    // </latex>
    //
    // The implementation is based on the QR decomposition. 
    // 
    // <literal>[p,S]=polyfit(x,y,n)</literal> returns the polynomial coefficients 
    // p and a data structure S for use with <literal>polyval</literal> to produce 
    // error estimates on predictions.
    //
    // The following are the fields of S.
    // <itemizedlist>
    //   <listitem><para>
    // S.R : Cholesky factors from the QR decomposition
    //   </para></listitem>
    //   <listitem><para>
    // S.df : the number of degrees of freedom : nx-(n+1)
    //   </para></listitem>
    //   <listitem><para>
    // S.norm : the 2-norm of the residuals
    //   </para></listitem>
    // </itemizedlist>
    //
    // The computation of the coefficients p can be difficult in the case where 
    // the inputs x are of very different orders of magnitude. 
    // In order to get more accurate results, we can use the mu output argument. 
    // This has the effect of computing the standard variable 
    // z from :
    //
    // <latex>
    // z_i = \frac{x_i-\overline{x}}{\sigma_x}
    // </latex>
    //
    // where <latex>\overline{x}</latex> is the mean and 
    // <latex>\sigma_x</latex> is the standard deviation. 
    // This reduces the condition number of the associated Vandermonde 
    // matrix. 
    // In this case, the mu argument must be passed to the polyval function 
    // in order to evaluate the polynomial. 
    // This allows polyval to evaluate the polynomial depending on z. 
    //
    // Examples
    // x = linspace(0,%pi,10)';
    // y = sin(x)
    // p = polyfit(x,y,3)
    // // Evaluate :
    // f = polyval(p,x)
    // // Compare with a table
    // disp([x y f abs(f-y)])
    // // Compare with a polynomial
    // ff = p(1)*x.^3+p(2)*x.^2+p(3)*x+p(4)
    // // Compare with a graphics
    // // Notice that the fit is good up to x=%pi, 
    // // then the fit is not good anymore.
    // x = linspace(0,4,100)';
    // y = sin(x);
    // f = polyval(p,x);
    // scf();
    // plot(x,y,"r-")
    // plot(x,f,"b-")
    // plot([%pi %pi],[-1 1],"g-")
    // legend(["Sin","Polynomial","Limit"],"in_lower_left");
    // xtitle("Degree 3 least squares polynomial","X","F(x)")
    //
    // // A difficult example : scaling is necessary.
    // x=[-1e15 1.e6 1e15]; 
    // y=[-12. 0 12.]; 
    // p=polyfit(x,y,2) // A warning is generated
    // [p,S,mu]=polyfit(x,y,2) // No problem
    // // Evaluate :
    // y2=polyval(p,x,S,mu)
    //
    // // Source [2]
    // // Caution : the urban definition changes in 1950
    // cdate=(1790:10:1990)';
    // pop=[
    //     3.929214   
    //     5.308483   
    //    7.239881   
    //    9.638453   
    //    12.860702  
    //    17.063353  
    //    23.191876  
    //    31.443321  
    //    38.558371  
    //    50.189209  
    //    62.979766  
    //    76.212168  
    //    92.228496  
    //    106.02154  
    //    123.20262  
    //    132.16457  
    //    151.3258   
    //    179.32317  
    //    203.30203  
    //    226.5422   
    //    248.70987  
    // ];
    // scf();
    // // Plot the data
    // plot(cdate,pop,"+")
    // // Calculate fit parameters
    // [p,S] = polyfit(cdate,pop,2);
    // // Evaluate the fit
    // pop_fit = polyval(p,cdate,S);
    // // Plot the fit
    // plot(cdate,pop_fit,"g-")
    // // Annotate the plot
    // legend("Polynomial Model","Data","Location","in_upper_left");
    // xtitle("","Census Year","Population (millions)");
    // 
    // Authors
    // Copyright (C) 2013 - 2016 - Michael Baudin
    // Copyright (C) 2010 - DIGITEO - Michael Baudin
    // Copyright (C) 1993 - 1995 - Anders Holtsberg
    //
    // Bibliography
    // [1] http://en.wikipedia.org/wiki/Polynomial_interpolation
    // [2] USA population : Population: 1790 to 1990, http://www.census.gov/population/censusdata/table-4.pdf
    // [3] http://www.mathworks.fr/fr/help/matlab/data_analysis/programmatic-fitting.html
    // [4] Numerical Computing with MATLAB, Cleve Moler, 2004
    // 

    [lhs,rhs]=argn();
    apifun_checkrhs ( "polyfit" , rhs , 3:3 );
    apifun_checklhs ( "polyfit" , lhs , 1:3 );
    //
    // Check Type
    apifun_checktype ( "polyfit" , x , "x" , 1 , "constant" );
    apifun_checktype ( "polyfit" , y , "y" , 2 , "constant" );
    apifun_checktype ( "polyfit" , n , "n" , 3 , "constant" );
    //
    // Check Size
    nx=size(x,"*");
    apifun_checkvector ( "polyfit" , x , "x" , 1 , nx );
    apifun_checkvector ( "polyfit" , y , "y" , 2 , nx );
    apifun_checkscalar ( "polyfit" , n , "n" , 3 );
    //
    // Check content
    apifun_checkgreq ( "polyfit" , n , "n" , 3 , 0 );
    apifun_checkflint ( "polyfit" , n , "n" , 3 );
    //
    x=x(:);
    y=y(:);
    //;
    if lhs==3 then
        // Compute mean and standard deviation
        mu(1)=mean(x);
        mu(2)=stdev(x);
        // Standardize the input x
        x=(x-mu(1))/mu(2);
    end
    //
    //% Construct the Vandermonde matrix V = [x.^n ... x.^2 x ones(size(x))]
    //vander=makematrix_vandermonde(x,n+1);
    vander(:,n+1) = ones(length(x),1);
    for j = n:-1:1
        vander(:,j) = x.*vander(:,j+1);
    end
    // Reverse the columns
    vander=vander(:,$:-1:1);
    [Q,R] = qr(vander,"e");
    Q = Q(:,1:size(R,2));
    R = R(1:size(R,2),:);
    //
    condR=cond(R);
    if (condR>1/sqrt(%eps)) then
        warning(msprintf("%s: The matrix R is ill-conditionned : %e.\n Please use the standardizing mu output argument.\n","polyfit",condR));
    end
    //
    p = R\(Q'*y); 
    r = y - vander*p; //residuals
    p=p';
    freed = nx - (n+1); //degree of freedom

    //on return : Choleski factor and the norm of residuals
    S.R=R;
    S.df=freed;
    S.normr=norm(r);
endfunction
