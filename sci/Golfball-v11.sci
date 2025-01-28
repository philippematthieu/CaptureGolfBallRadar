// Copyright (C) 2016 - mm - Author
//
// About your license if you have any
//
// Date of creation: 16 juin 2016
//
//SCI2C: DEFAULT_PRECISION= DOUBLE
//
// ReadWave : FFT_Mat.sce
// 

function Sac = GetSac()
    Type            = ['D'      ;'W5'     ;'H3'    ;'5'      ;'6'     ;'7'     ;'8'      ;'9'    ;'PW'     ;'AW'    ;'SW'      ;'LW'   ;'PU'   ]; 
    Loft            = [11       ;18       ;18      ;23       ;26      ;30      ;34.5     ;39     ;44       ;52      ;54        ;60     ;3      ]; 
    Poids           = [0.300    ;0.250    ;0.250   ;0.200    ;0.200   ;0.200   ;0.200    ;0.200  ;0.200    ;0.200   ;0.200     ;0.200  ;0.300  ]; 
    ecoeff          = [.83      ;0.78     ;0.76    ;0.87     ; 0.84   ;0.68    ;0.7      ;0.6    ;0.78     ;0.68    ;0.68      ;.4     ;0.68   ];   // Smash Factor mais plus conplexe (voir formule qui prend en compte m M et miss ) Augmentation = Vitesse Ball plus mportante
    coeffSpin       = [180      ;280.6435 ;250.000 ;300.0000 ;240     ;400     ;300.7425 ;400.99 ;501.2375 ;441.089 ;452.51722 ;500    ;501.2375 ];   // Capacite du club a addherer et fournir du backspin. dependant des marques de clubs. diminution = moins haut et un choui moins loin
    coeffSpinLift   = [500      ;500      ;500     ;420      ;420     ;580     ;420      ;308    ;308      ;308     ;308       ;308    ;308]           // Capacite du club a addherer et fournir du liftspin. dependant des marques de clubs. diminu = moins latérale 
    Cl1             = [0.64     ;0.65     ;0.54    ;0.54     ;0.83    ;0.42    ;0.53     ;0.53   ;0.52     ;0.52    ;0.51      ;0.51   ;0];   // coefficent de proportionnalite ,diminu = moins haut et moins 
    Cm              = [4.3e-2   ;2.8e-2   ; 2.6e-2 ;1.8e-2   ; 1.6e-2 ;1.1e-2   ;1.1e-2  ;0.9e-2 ;0.62e-2  ;0.5e-2  ;0.45e-2   ;0.4e-2 ;0.0e-2 ];   // coefficent de proportionnalite ,diminu = moins haut et moins     
    Sac             = struct('Type', Type, 'Loft' ,Loft  ,'Poids',Poids, 'ecoeff',ecoeff,'coeffSpin',coeffSpin,'Cm',Cm,'Cl1',Cl1,'coeffSpinLift',coeffSpinLift); 
endfunction

function [t,VOL,Res] = Golfball(Tfin, V0Club, Club, alphaClubPath,gamaFacePath,ShafLeanImp,GraphOn)
    // [t,x]=ode45(@flightg,[t0,tfin],[0,V0x,0,V0y,0,V0z], [W_I, W_J, W_K]);
    // Doc de depart "Golf Ball Flight Dynamics Brett Burglund Ryan Street"
    // et Vol de la balle ""The physics of golf: The optimum loft of a driver A. Raymond Penner""
    // Tfin=18.3, V0Club=151, Club='D', alphaClubPath=-4,gamaFacePath=2
    // W_i  ; Effet Magnus sur Axe X, impossible avec la frappe en direction X
    // W_j Slice > 0 / Hook < 0; Effet MAgnus sur Axe Y
    // W_k BackSpin > 0 / TopSpin < 0 ==> effet Magnus sur Axe Z
    // [50, 19, 0]  ==> Fade
    // je rentre V0 en kilometre / h, alphaClubPath et gamaFacePath en degré
    // ball : 0.04593 grams et diametre 0.04267 m, rayon 0.021335
    // r = 1.225 kg/m^3 air density
    // Rho Air = 1.292*273.15/(273.15+ Temperature)
    // formule de la résistance de l''air en fonction de la vitesse²
    // R =1/2 . p . Cd . S . V^2
    // R = résistance de l''air
    // p = masse volumique de l''air
    // S = Air de la section droite perpendiculaire au mouvement (m²)
    // Cd = le coefficient de résistance aérodynamique
    // Fd(x) = 0.5 * Cd * p * A * sqrt(V(x)^2+V(y)^2+V(z)^2) * Vhat(x))
    // vhat est le vecteur unitaire de la vitesse dans les 3 dimensions.
    // V0Ballbfnms     = V0Clubms * cos(thetaLoft)*(1+e)/(1+m/M) ;     // Conversion Vitesse Club en vitesse balle
    // V0Ballbfpms     = -V0Clubms * sin(thetaLoft)/(1+m/M + 5/2);     // Conversion Vitesse Club en vitesse balle
    // V0Ballms        = sqrt(V0Ballbfnms^2 + V0Ballbfpms^2)* (1 - 0.3556*miss);
    // LaunchAngle     = thetaLoft + atan(V0Ballbfpms/V0Ballbfnms);
    // LaunchAngledeg  = LaunchAngle*180/%pi;                          // en radian
    // here miss is the amount (in cm) by which the sweet spot is missed
    // e = The Coefficient of Restitution (COR),
    // e=0.78 pour les club
    // e=0.83 pour les Drives
    // m = Mass of the ball (typically 46 grams or 1.62 ounces).
    // M = Mass of the clubhead (typically 200 grams or 7 ounces for driver)
    // D = 0.043 m
    // A 10% increase in clubhead speed with no change in clubhead weight increases ball velocity 10%.
    // A 10% increase in clubhead weight with no change in clubhead speed increases ball velocity only 1.7%.
    // 
    // Coefficient Magnus en kg/rad
    // 1 rad/sec? The answer is 9.54929659643 RPM
    // spin rate in rpm 
    // ou approximation Spin = Coef * V0 * sin(thetaLoft)
    // on considerera que la décélération de W est negligeable sur le temps de vol.
    // On recalculera le spin au contact du sol.
    //
    // nombre de Reynolds pour un golfeur professionnel 220000, (src
    // http://press.princeton.edu/chapters/s6-6_10592.pdf)
    // CM = 1/(2 + (v/R*sin)) 
    // CD = C + (1/(A + (B *v/R*? *sin(pi/2))^5/2))^2/5
    // Re = rho*UD/µ = VitesseBalle*Diametre/1.5e-5 
    // µ = 1.5x10^-6 m^2/s
    // G. Palmer, "Physics for game programmers," Apress, 2005. donne ls CL = r.w.sin(alpha))/V (w en rad/s, V en m/s, alpha angle en tre w et V = pi/2)
    // V= C*(1-Femise/Fpercue)/cos(theta)
    // theta = angle de propagation
    //
    // Nombre d'alveoles moyen est de 432
    //
    ////////////////////////////////////////////////
    // Trace cumulée de coups
    // somme des coups xx = [x9+ones(size(x9,1),1)*xd($,:)]; xx = [xd; xx];
    // [t,xd] = Golfball(18.3,141,'D',-0,-2,0,0); [t,x7] = Golfball(18.3,121,'7',-4,1,0,0);[t,xlw] = Golfball(18.3,50.9,'LW',3,-1,0,0); [t,xp] = Golfball(18.3,10.9,'PU',0,0,0,0); 
    // figure; xx = [xp+ones(size(xp,1),1)*xlw($,:)]; xx = [xlw; xx]; xx = [xx+ones(size(xx,1),1)*x7($,:)]; xx = [x7; xx]; xx = [xx+ones(size(xx,1),1)*xd($,:)]; xx = [xd; xx]; 
    // param3d(VOL(:,1),VOL(:,5),VOL(:,3),'foreground',23); e=gce(); e.foreground = color('red'); e.mark_style = 0;  e.thickness = 2; a = gca(); a.data_bounds =[ 0 , 500 ,-70, 70 , 0, 70];
    //
    //
    ///////////////////////////////////////////////////////////////////////////
    // Rebond de la balle de golf sur le green / Fairway
    // The run of a golf ball A. Raymond Penner
    // ImpactAngle = tan−1(|vix/viy|); // prendre vix = sqrt(vx^2+vz^2))
    // ThetaRebond = 15.4*(|vi|/18.6)*(ImpactAngle/44.4); // en m/s et degre
    // |vixprime| = vix * cos(ThetaRebond) − |viy| * sin(ThetaRebond)
    // |viyprime|= vix * sin(ThetaRebond) + |viy| * cos(ThetaRebond)    
    // eball = 0.510 − 0.0375 * |viyprime| + 0.000903 * |viyprime|^2; // si |viyprime| < 20 m/s
    // eball = 0.120; // si |viyprime| >= 20 m/s
    // muFriction = 0.40; // mu limite, mu < muc alors la balle va glisser, si mu > muc alors la balle va rouler sur la collision
    // pour un green plus ferme, on peut prendre un mu inférieur, par exemple 0.25 sur les tournoie pro?
    // il faut alors aussi ajuster muFrictionCritic
    // muFrictionCritic = 2*(|vixprime| + r*|wi|) / (7*(1+e)*|viyprime|)
    //
    //
    // Vitesse apres rebond glisse  mu < muc
    // vrxprime = vixprime − muFriction * |viyprime| *(1 + eball)
    // vryprime = eball * |viyprime|
    // ωr = ωi − (5*muFriction * 2r) * |viyprime| * (1 + eball)
    //
    // Vitesse apres rebond roule mu > muc 
    // vrxprime = (5/7)*vixprime - (2/7)*r*wi
    // vryprime = eball * |viyprime|
    // wr = -vrxprime/r
    //
    // vrx = vrxprime * cos(ThetaRebond) − vryprime * sin(ThetaRebond)
    // vry = vrxprime * sin(ThetaRebond) + vryprime * cos(ThetaRebond)
    //
    //
    ///////////////////////////////////////////////////////////////////////////
    // Le roulement est admis si le rebond est inferieur a 10mm
    // alors la deceleration est donnee par
    // rhog = 0.131;
    // a_green = -(5/7)*rhog*g; // ou rhog = 0.131 coeff de frottement pour le green et plus grand pour le fairway [0.065 0.196]
    // F = m * vxz - m*a_green
    //
    ///////////////////////////////////////////////////////////////////////////
    // Bibliographie :
    // Flight : GOLF BALL AERODYNAMICS P W BEARMAN & J K HARVEY (1976) Imperial College Of Science and Technolgy, Journal of The AERONAUTICAL QUARTERLY V27-Issue 112-P 122
    // Fligth : The physics of golf: The optimum loft of a driverA. Raymond Penner
    // impact of the clubhead of a driver : The physics of golf: The convex face of a driver A. Raymond Penner
    // Putting : The physics of putting. Canadian Journal of Physics, 80 :83–96, 2002. A.R. Penner.
    // Rolling : The run of a golf ball. A. Raymond Penner
    // Rebond : Golf Ball Landing, Bounce and Roll on Turf. Woo-Jin Roha, Chong-Won Leeb*
    // Notion de force de traînée – exercices corrigés (Daniel Huilier) LICENCE L3S5 2011-2011 Mécanique des Fluides TD –trainee-Corrigé Dany Huilier – début novembre 2010
    // Physics for Game Programmers. GRANT PALMER
    // Concours Centrale / SupElec 2012 : Physique du golf
    // Golf Ball Flight Dynamics Brett Burglund Ryan Street 5-13-2011 1
    //% %%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Caractéristique des clubs
    // ClubTypeNumber = [ 1;     2;    3;    4;    5;    6;    7;    8;    9 ;  10   ;  11   ; 12;  13;14 ;  15;  16 ;   17;    18;    19;   20;   21];
    // Type            =   ['D'; 'W3'; 'W4'; 'W5'; 'H2'; 'H3'; 'H4'; 'H5';'3'   ; '4'   ; '5'   ;'6'; '7';'8'; '9'; 'PW'; 'AW'; 'SW1'; 'SW2';'LW' ;'PU' ];
    // Fer Callaway XR, Drive et bois JPX 2013, balle insis soft
    //
    // dPhi = Phi(Fd1) - Phi(Fd2) = 2*%pi*sin(alpha)*D*Ftx/C
    //
    //    Ke = (1 + Sac.ecoeff(Sac.Type == '7'))/(1+m/Sac.Poids(Sac.Type == '7'));
    //    Kp = 1/(1+m/Sac.Poids(Sac.Type == '7') + 5/2);
    
    Sac = GetSac();
    TabBallSpeed    = [223.7    ; 205     ; 197.9 ; 180.3    ; 175.4  ; 167.4   ; 160.9  ; 149.7 ; 138.4   ; 111    ; 108.7    ; 104   ; 10]*1000/3600; // en m/s
    TabBallSpin     = [2628     ; 4501    ; 4693  ; 5081     ; 5943   ; 6699    ; 7494   ; 7589  ; 8403    ; 9300   ; 9600     ; 9300  ; 1]/9.54929659643;  // transforme  en rad / s
     
    //% %%%%%%%%%%%%%%%%%%%%%%%%%
    // 
    thetaLoft       = (Sac.Loft(Sac.Type == Club) + ShafLeanImp)*%pi/180;     // et transformation en radian
    alphaClubPath   = alphaClubPath*%pi/180;                // en radian
    gamaFacePath    = gamaFacePath*%pi/180;                 // en radian
    m               = 0.04545;                              // Mass de la balle en kg
    diam            = 0.043;                                // Diametre Balle
    M               = Sac.Poids(Sac.Type == Club);              // masse de la tete de club en kg
    e               = Sac.ecoeff(Sac.Type == Club);             // Coefficient de restitution Club / Balle
    V0Clubms        = V0Club * 1000 / 3600; ;                  // conversion en m/s
    // e = 0.86 - 0.0029 * V0Clubms * cos(thetaLoft);
    miss            = 0;                                    // Point d''impact sur le club en cm
    T               = 20 + 273.15; 
    rho_air         = 1.292*273.15/T;                       // kg/m^3 air density
    A               = %pi*(diam/2)^2;                       // Section en m^2 %pi*d^2
    //% %%%%%%%%%%%%%%%%%%%%%%%%
    // 
    Inertia = (2/5)*m*(diam/2)^2;

    V0Ballbfnms     = V0Clubms * cos(thetaLoft)*(1+e)/(1+m/M) ;     // Vitesse longitudinale dans le referentiel de decollage après impact (ref: The physics of golf: The optimum loft of a driverA. Raymond Penner)
    V0Ballbfpms     = -V0Clubms * sin(thetaLoft)/(1+m/M + 5/2);     // Vitesse perpendiculaire dans le referentiel de decollage après impact
    V0Ballms        = sqrt(V0Ballbfnms^2 + V0Ballbfpms^2)* (1 - 0.3556*miss); // Vitesse de decollage de la balle
    //    Ke = (1 + e)/(1+m/M);
    //    Kp = 1/(1+m/M + 5/2);    
    Psi             = thetaLoft + atan((V0Ballbfpms/V0Ballbfnms));    // angle de decollage de la balle
    LaunchAngle     = Psi;
    LaunchAngledeg  = LaunchAngle*180/%pi;                          // en radian
    V0Ballkmh       = V0Ballms * 3600/1000; 
    V0initms        = zeros(1,6);                                   // en m/s
    V0Facems        = zeros(1,6);                                   // en m/s

    //% %%%%%%%%%%%%%%%%%%%%%%%%%
    // Vecteur Vitesse initiale de la balle sur les 3 Axes
    V0initms(2) = V0Ballms*cos(LaunchAngle)*cos(0);            // Vx : vitesse sur la longueur
    V0initms(4) = V0Ballms*sin(LaunchAngle);                   // Vy : vitesse de la hauteur
    V0initms(6) = -V0Ballms*cos(LaunchAngle)*sin(0);           // Vz : vitesse latérale
    V0initms(3) = 0;// hauteur initiale
    //% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // reecriture de Vinitms pour prendre en compte l'angle club path pour dessiner le vecteur correspondant.
    // Le club path ne doit pas influencer sur le modèle. C'est une rotation du plan de tir.
    // je modélise et calcule sans club path pour ne pas intégrer cet angle,
    // puis applique l'angle pour réaliser une rotation du referentiel
    V0initmsPlot(2) = V0Ballms*cos(LaunchAngle)*cos(alphaClubPath);// Vx : 
    V0initmsPlot(4) = V0Ballms*sin(LaunchAngle);                   // Vy :  
    V0initmsPlot(6) = -V0Ballms*cos(LaunchAngle)*sin(alphaClubPath);// Vz :  alpha Chemin de club non vers la cible !!
     
    //% %%%%%%%%%%%%%%%%%%%%%%%%%
    // Vecteur représentant la face de Club à dessiner
    V0Facems(2) = V0Clubms*cos(-alphaClubPath - gamaFacePath);// Vx en radian de thetaLoft 
    V0Facems(4) = 0;                                          // Vy de la face de club, a zero sur la hauteur, on vera pour l''intégration du attaque angle 
    V0Facems(6) = V0Clubms*sin(-alphaClubPath - gamaFacePath);// Vz alpha Chemin de club non vers la cible !! 
     
    //% %%%%%%%%%%%%%%%%%%%%%%%%
    // Calcul des SPIN
    SpinX = 0;//V0initms(6) * Sac.coeffSpinLift(Type == Club) * sin(gamaFacePath);//  Le SPIN sur l'axe X n'est pas une composante dans l'axe du chemin de club. Le spin est appliqué sur le plan du sol
    SpinY = V0initms(4) * sin(gamaFacePath)* Sac.coeffSpinLift(Sac.Type == Club) ;//  SideSpin
    SpinZ = V0initms(2) * sin(thetaLoft)   * Sac.coeffSpin(Sac.Type == Club);// BackSpin
    
    //% %%%%%%%%%%%%%%%%%%%%%%%%%
    // vecteur Spin
    W(1) = SpinX;                                      // W_I
    W(2) = SpinY;                                      // W_J dans l''axe de la face de de club
    W(3) = SpinZ;                                      // W_K dans l''axe du chemin de club
    W= W/9.54929659643;                                // rpm en rad/s


    //% %%%%%%%%%%%%%%%%%%%%%%%%%
    // Lancement de ODE et calcul du vol
    //Init0 = [V0initms, W',Sac.Cm(Type == Club)]'; 
    Init0 = [V0initms, W',Sac.Cl1(Sac.Type == Club), m, diam]'; 
    T0 = 0; 
    T=[]; 
    t=[];
    X = []; 
    YE = []; 
    RD = [];
    for ii = 1:3 // je ne considère que 3 rebonds
        [Pos, rd] = ode('roots',Init0, T0, 0:0.1:Tfin, flightg, 1, events); 
        RD = [RD;rd];
        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        // resolution de l'angle  des rebonds. ///
         
         vi = sqrt(Pos(2,$)^2+Pos(6,$)^2); // calcul de la norme de la dernière vitesseplane avant Impact sol du rebond
         viy = Pos(4,$);
         ImpactAngle = atan(abs(vi/viy)); // prendre vi = sqrt(vx^2+vz^2))
         ThetaRebond = 0.2687807 * sqrt(vi^2+viy^2)/18.6*(ImpactAngle/0.7749262 ); // en m/s et degre
         vixprime = vi * cos(ThetaRebond) - abs(viy) * sin(ThetaRebond);
         viyprime = vi * sin(ThetaRebond) + abs(viy) * cos(ThetaRebond); 
         if abs(viyprime) < 20 ,
              eball = 0.510 - 0.0375 * abs(viyprime) + 0.000903 * abs(viyprime)^2;
         else
               eball = 0.120;
         end;
         muFriction = 0.40;
         muFrictionCritic = 2 * (vixprime + (diam/2) * Pos(9,$)) / (7*(1+eball) * abs(viyprime));
         if (muFriction < muFrictionCritic),
              vrxprime = vixprime - muFriction * abs(viyprime) *(1 + eball);
              vryprime = eball * abs(viyprime);
              Wr = Pos(9,$) - (5 * muFriction * 2 * (diam/2)) * abs(viyprime) * (1 + eball); //  (backspin)
         else
              vrxprime = (5/7) * vixprime - (2/7) * (diam/2) * Pos(9,$);
              vryprime = eball * abs(viyprime);
              Wr = -vrxprime / (diam/2);
         end
         vr = vrxprime * cos(ThetaRebond) - vryprime * sin(ThetaRebond);
         vry = vrxprime * sin(ThetaRebond) + vryprime * cos(ThetaRebond);
         vrx = vr / vi * Pos(2,$);          
         vrz = vr / vi * Pos(6,$);
        ///////////////////////// fin resolution rebond //////////////////////
        //
        // positionnement des nouvelles conditions initiales        
        Pos(3,$) = AltiSol(Pos(1,$),Pos(5,$)); // y hauteur mini est à zero.
        Pos(2,$) = vrx;                     // vitesse de X doit diminuer en fonction du sol
        Pos(4,$) = vry;                     // A chaque rebond sur le sol,la vitesse Y change de sens (calcule dans la resolution du rebond), 
        Pos(6,$) = vrz;                     // vitesse de Z doit diminuer en fonction du sol
        Pos(7,$) = 0;                       // nouveau spin à 0 pour le moment, mais à recalculer selon le modèle de terrain
        Pos(8,$) = 0;                       // je considère que le spin Wrx est nul... plus de lift donc.
        Pos(9,$) = Wr;                      // diminution du spin en vol....negligeable        
        Pos(10,$) = Sac.Cm(Sac.Type == Club);   // diminution du Cm en vol....negligeable, Mais il faudrait avoir un meilleur model car le Cm doit varier, puisque la vitesse  varie !!!
        Init0 =  Pos(:,$); 
        YE = [YE ,Pos];                     // cumule des points
        if ii == 1, 
            Xchute = Pos(1,$)*cos(alphaClubPath) + Pos(5,$)*sin(alphaClubPath); 
            Zchute = -Pos(1,$)*sin(alphaClubPath) + cos(alphaClubPath)*Pos(5,$);
            VerticalLand = ImpactAngle*180/%pi;
        end; // on recupere la distance du premier point d'impact
        t = [t,rd(1)];                        // On cumule le temps de vole de chaque rebond
    end; 

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //modélisation du roulage
    Pos(3,$) = AltiSol(Pos(1,$),Pos(5,$));   // y hauteur mini est à zero.
    Pos(4,$) = 0;                            // vitesse Y = 0
    Init0 = Pos(:,$); 
    T0 = 0; 
    T=[]; 
    [Pos, rd] = ode('roots',Init0, T0, 0:0.1:Tfin, rolling, 1, events2); // Nouveau calcul de modelisation du roulage sur green
    RD = [RD;rd];
    YE = [YE ,Pos];
    t = [t,rd(1)];
    X = YE';
    // changement de référenciel straight vers la prise en compte du chemin de club tourné par rapport à la cible !
    // En effet le chemin de club donne la direction initiale de la balle
    // mais n'influence pas sur le vol de celle-ci, comme le spin gere par le loft ou l'angle d'attaque du club.
    // On calcul donc le vol de la balle dans un repère ou seuls les SPIN impacts la trajectoire,
    // puis on reaslie un rotation d'angle alphaClubPath pour obtenir la trajectoire definitive par rapport au chemin de club
    XX = X(:,1:2)*cos(alphaClubPath) + X(:,5:6)*sin(alphaClubPath); 
    ZZ = -X(:,1:2)*sin(alphaClubPath) + X(:,5:6)*cos(alphaClubPath);
    YY = X(:,3:4);
    WW = X(:,7:9);
    VOL = [XX,YY,ZZ,WW];


    ZmaxMin =  max(abs(min(X(:,5))) ,abs(max(X(:,5)))); ;// calclu les min max du terrain vert
    Ymax =   max(YY(:,1));

    //% %%%%%%%%%%%%%%%%%%%%%%%%%
    // Affichage des arcs de distance
    if (GraphOn == 1) then
        hf = figure('position', [-8 -8 1366 618]);drawlater();
        hf.background = 13;
        surf([-50 300],[-40-ZmaxMin 40+ZmaxMin],zeros([0 300]'*[-40-ZmaxMin 40+ZmaxMin]),'color_flag',0); // affichage du sol
        h=gce();h.color_flag = 0; h.color_mode = 13;
        t0=[0.45*%pi:0.03:.55*%pi];// Cercle de lignes de distance
        for ii=0:20:250;
            param3d(ii*sin(t0),ii*cos(t0),zeros(cos(t0)), 177,63,'Long@Large@Haut@Large',[0,0],[ 0 , 250 ,-40-ZmaxMin, 40+ZmaxMin , 0, 50]);
            e = gce(); e.foreground = color('red');
            xstring(ii*sin(t0($)),ii*cos(t0($)),string(ii));
            xstring(ii*sin(t0(1)),ii*cos(t0(1)),string(ii));
            param3d([ii*sin(t0(1)) ii*sin(t0(1))],[-40-ZmaxMin -40-ZmaxMin],[-10 25], 177,63,'Long@Large@Haut@Large',[0,0],[ 0 , 250 ,-40-ZmaxMin, 40+ZmaxMin , 0, 50]); // 
            e = gce(); e.foreground = color('brown');e.thickness = 3;e.mark_style = 0;e.mark_size = 30;e.mark_foreground = color('green');e.foreground = color('brown');
            param3d([ii*sin(t0(1)) ii*sin(t0(1))],[40+ZmaxMin 40+ZmaxMin],[-10 25], 177,63,'Long@Large@Haut@Large',[0,0],[ 0 , 250 ,-40-ZmaxMin, 40+ZmaxMin , 0, 20]); // Arbres
            e = gce(); e.foreground = color('brown');e.thickness = 3;e.mark_style = 0;e.mark_size = 30;e.mark_foreground = color('green');e.foreground = color('brown');
        end;
        
    //% %%%%%%%%%%%%%%%%%%%%%%%%%
    // Affichage des arbres et air de jeux        
        param3d([0 250],[0 0],[0 0], 177,63,'Long@Large@Haut@Large',[0,0],[ 0 , 250 ,-40-ZmaxMin, 40+ZmaxMin , 0, 50]); // Ligne sur sol
        e = gce(); e.foreground = color('red');
        a = gca(); a.background = color('green');a.box="back_half";
        a.data_bounds = [ 0 , 250 ,-40-ZmaxMin, 40+ZmaxMin , 0, 40];

        param3d([200 200],[0 0],[0 2], 177,63,'Long@Large@Haut@Large',[0,0],[ 0 , 200 ,-40-ZmaxMin, 40+ZmaxMin , 0, 50]); // Drapeau
        e = gce(); e.foreground = color('yellow');e.thickness = 3;e.mark_style = 12;

        data_bounds = [ 0 , 250 ,-40-ZmaxMin, 40+ZmaxMin , 0, 40];
        a.data_bounds = data_bounds;
        a.box = 'off';
//        x=-0:1:250 ;y=100-x ;
//        deff('z=f(x,y)','z= ((x^4-y^4)/1e7)');
//        deff('z=f(x,y)','z= 0.3*atan(y) + 0.05*(x + y)');
//        fplot3d(x,y,AltiSol);
//        
        
    //% %%%%%%%%%%%%%%%%%%%%%%%%%
//        subplot(10,1,10);  
//        v = format();format(5);
//        xstring(0,0,['alphaClub: '  + string(alphaClubPath*180/%pi) + '°' + ...
//        ' | Face: ' + string(gamaFacePath*180/%pi) + '°' + ...
//        ' | Club: ' + Club + ...
//        ' | VClub: ' + string(V0Club) + ' km/h' + ...
//        ' | VBall: ' + string(V0Ballkmh) + ' km/h' + ...
//        ' | ShafLeanImp: ' + string(ShafLeanImp) + '°' + ...
//        ' | BackSpin: ' + string(SpinZ) + ' rpm,' +...
//        ' | Lift: '  + string(SpinY) + ' rpm,'  ; ...
//        ' | Pt Chute (m): ' + string([Xchute]) + ' m - ' +  string([Zchute]) + ' m' + ...
//        ' | Dist (m): ' +  string([XX($,1)]) + ' m - ' + string([ZZ($,1)]) + ' m' +...
//        ' | Max Height: ' + string(Ymax) + ' m' +...
//        ' | Vertical Launch: ' +  string(LaunchAngledeg) + '°' +...
//        ' | Vertical Land: ' + string(VerticalLand) + '°' +...
//        ' | Temps: ' +  string(sum(t)) + ' s' ]);//,' Temps:', string(sum(t))]
//        drawnow();format(v(2));
        subplot(1,1,1);  
        comet3d2(XX(:,1),ZZ(:,1),YY(:,1),'colors',color('blue')); // Trace du vol; Remplace la boucle for precedante.
       //% %%%%%%%%%%%%%%%%%%%%%%%%%
       a.data_bounds = [ 0 , 250 ,-40-ZmaxMin, 40+ZmaxMin , 0, 40];
       drawlater();
        param3d(XX(:,1),ZZ(:,1),zeros(X(:,3)), 177,63,'Long@Large@Haut@Large',[0,0],[ 0 , 250 ,-40-ZmaxMin, 40+ZmaxMin , 0, 50]); // ombre Ligne sur sol du vol
        
        v = format();format(6); // affichage des distances au point de chute
        xstring(XX($,1),ZZ($,1),[string(Xchute), string(Zchute);string(XX($,1)), string(ZZ($,1));' ', ' ']);
        format(v(2));
        subplot(3,2,1);  a = gca(); a.background = 1;
        plot(V0initms(1:2)',V0initms(3:4)','->r');   set(gca(),"grid",[5 7]);  
        plot(V0Facems(1:2)',V0Facems(3:4)','->.g'); set(gca(),"grid",[5 7]);  
        plot(XX(:,1),X(:,3)); set(gca(),"grid",[5 7]);  
        //grid on; 
        v = format();
        xlabel(['Long (m)']) ;
        ylabel('Haut (m)') ;
       
        subplot(3,2,2);  a = gca(); a.background = 1;
        plot(V0initmsPlot(1:2)',V0initmsPlot(5:6)','->r'); set(gca(),"grid",[5 7]);  
        plot(XX(:,1),ZZ(:,1)); set(gca(),"grid",[5 7]);  
        plot(V0Facems(1:2)',V0Facems(5:6)','->g'); set(gca(),"grid",[5 7]);  
        xlabel('Long (m)');
        ylabel('large (m)');
                
        v = format();format(6);
        subplot(10,10,31);
        xstring(0,0,['alphaClub'  ; string(alphaClubPath*180/%pi) + '°']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2;
        subplot(10,10,41);
        xstring(0,0,[ 'Face:' ; string(gamaFacePath*180/%pi) + '°']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2; 
        subplot(10,10,51);
        xstring(0,0,[ 'VClub' ; string(V0Club) + ' km/h' ]);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2; 
        subplot(10,10,61);
        xstring(0,0,['VBall' ; string(V0Ballkmh) + ' km/h']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2; 
        subplot(10,10,71);
        format(v(2));
        xstring(0,0,['ShafLeanImp' ; string(ShafLeanImp) + '°']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2;
        subplot(10,10,81);
        xstring(0,0,['BackSpin' ; string(SpinZ) + ' rpm']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2; 
        subplot(10,10,91);
        xstring(0,0,['Lift'  ; string(SpinY) + ' rpm']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2; 
        format(v(2)); 
        
        subplot(10,10,40);
        xstring(0,0,['Pt Chute' ; string([Xchute]) + ' m - ' +  string([Zchute]) + ' m']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2; 
        format(v(2)); 
        subplot(10,10,50);
        xstring(0,0,['Dist' ;  string([XX($,1)]) + ' m - ' + string([ZZ($,1)]) + ' m']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2; 
        format(v(2)); 
        subplot(10,10,60);
        xstring(0,0,['Max Height' ; string(Ymax) + ' m']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2; 
        format(v(2)); 
        subplot(10,10,70);
        xstring(0,0,['Vertical Launch' ;  string(LaunchAngledeg) + '°']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2; 
        format(v(2)); 
        subplot(10,10,80);
        xstring(0,0,['Vertical Land' ; string(VerticalLand) + '°']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2; 
        format(v(2)); 
        subplot(10,10,90);
        xstring(0,0,['Temps' ; string(t(1)) + ' ' + string(sum(t(2:$))) + 's']);//,' Temps:', string(sum(t))]
        a=gce();
        a.alignment = "center"; a.font_size = 2; 
        format(v(2));
        drawnow();
    else
        v = format();format(6);
        disp(['Club:',Club,' , VClub:', string(V0Club), 'km/h',' VBall:', string(V0Ballkmh),'km/h', 'ShafLeanImp: ',string(ShafLeanImp),',°, alphaClub: ', string(alphaClubPath*180/%pi),'° ,','Face:',string(gamaFacePath*180/%pi),'° ,',' backSpin:',string(SpinZ),'rpm,',' Lift:',string(SpinY),'rpm ,',' Pt Chute (m):', string([Xchute, Zchute]),', Dist (m):', string([XX($,1) , ZZ($,1)]),' Max Height:', string(Ymax),'m',' Vertical Launch:', string(LaunchAngledeg),' Vertical Land:', string(VerticalLand),' Temps:', string(t(1)) + 's ' + string(sum(t(2:$))) + 's ' + string(sum(t)) + 's']);
    end; // GraphOn
    format(v(2));
    Res = [V0Club,V0Ballkmh, alphaClubPath*180/%pi,gamaFacePath*180/%pi,SpinZ,SpinY,Xchute, Zchute,XX($,1) , ZZ($,1),Ymax,LaunchAngledeg,VerticalLand,sum(t)];
    format(10);
endfunction

//% %%%%%%%%%%%%%%%%%%%%%%%%%
//// //// Fonction de calcul de l'altitude du sol au point Longueur, largeur ( x(1), x(5) )////////////

function [Altitude] = AltiSol(Longueur,largeur)
    if (Longueur < 30) then
        Altitude = -0;
    else
        Altitude =  0;//-0.3*atan(y) + 0.05*(x + y);//(Longueur^4-(largeur)^4)/1e7; 
    end;
endfunction

function [vAltitude] = dAltiSol(vLongueur,vlargeur)
    if (vLongueur < 30) then
        vAltitude = -0;
    else
        vAltitude =  0;//4*(vLongueur^3-(vlargeur)^3)/1e7; 
    end;
endfunction

//// ////////////////////////////
function [value] = events(t,x)
    // il faut ajouter la detection de vitesse dx/dt, dy/dt ou dz/dt à zero.
    // soit x(2), x(4) ou x(6) à zero ou inférieur à un seuil de frottement.
    
    if (x(3) < AltiSol(x(1),x(5))) then 
        value = 0 ;// pour Y il vaut mieux des valeurs < 0     
    else
        value = x(3) ; // pour Y il vaut mieux des valeurs < 0           
    end;
endfunction


// ///////////////////////////////////
function Cm = getCm(Vm,Wm,v10)
    diam = 0.043;                     // Diametre Balle
    // Smits and Smith (1994) determined
    // Vm
    // Wm 
    // v10 = CM
    //Cm = Vm/(5*Wm);
    Cm = v10; // calcul avec sac.Cm
    Cm = v(10) * ((diam/2) * abs(Wm  / Vm)^0.4); // calcul avec sac.Cl1
endfunction

////////////////////////////////
function vprime=flightg(t,v) 
    // Fonction xprime
    // Constantes
    m = v(11);//0.04545; ;// en kg
    T = 20 + 273.15; 
    rho_air = 1.292*273.15/T;         // kg/m^3 air density
    diam = v(12);//0.043;                     // Diametre Balle
    A = %pi*(diam/2)^2;               // Section en m^2 %pi*r^2
    Vm = sqrt(v(2)^2+v(4)^2+v(6)^2);  // U
    Wm = sqrt(v(7)^2+v(8)^2+v(9)^2);  // nu en rad/s
    // %pi * Nrpm * diam / 60; // 
    Vp = %pi * 9.549296596425383 * Wm * diam / 60; // Peripheral velocity
    // Vp/Vm;
    //% %%%%%%%%%%%%%%
    // Drag de l''AIR en vitesse vectorielle
    // Calcul du CD et REynolds
    Re = Vm*diam / 1.5e-5; 
    // C_d = cd0+cdspin*spin/1000;// ou cd0 = 0.3008; cdspin = 0.0292
    C_d = 0.36 + 24/Re + 6/(1+sqrt(Re)); // relation de White pour une balle de golf
     
    //%  %%%%%%%%%%%%%%
    D =.5 * C_d * rho_air * A * Vm ^2; //résistance de l''air en fonction de la vitesse
    // force attractive G
    g = 9.81; ;// m/s^2
     
    //% %%%%%%%%%%%%%%
    // Magnus, en general, CL tourne autour de 0.14-0.19
    // Pour une sphere on a Cm = (diam/2)*Vp/Vm selon G. Palmer, "Physics for game programmers," Apress, 2005
    // SpinRateParam = (diam/2)*Vp/Vm; // Vp vitesse autour periférique en m/s, Vm la vitesse de la balle dans le fluide en m/s.
    // Cl = abs(Cl1*SpinRateParam^0.4);// Smits and Smiths ""A new aerodynamic model of a golf ball in flight 1994"
    // ou Cl1 = 0.54 pour un drive et 0.51 +-0.02 pour un fer 7
    // SRD = (dWm/dt*r^2/Vm^2) ; // SpinRateDecay
    // diminussion du CM implique moins de drag vers le haut ==> moins loin
    // Cl = -0.05 + sqrt(0.0025 + 0.36* abs((diam/2) *(Vp) / Vm));// CL selon G. Palmer, "Physics for game programmers," Apress, 2005, issu de Lieberman
    // M est le Lift Force
    //M = 0.5 * rho_air * v(10) * A/m; 
    //M = 0.5 * rho_air * Cl * A / m; 
    //M = 0.5 * rho_air *  A * (diam/2)/ m; // issus "L'effet Magnus" par Gilbert Gastebois qui simplifie M = 0.5*%pi*R^3*rho* wxv
    //M = 0.5 * rho_air *  A * getCm(Vm,Wm,v(10)); // expérimentalement, j'ai BallSpin / BallSpeed correcte, mais le calcul du Cm ne correspond pas a la realiste.
    M = 0.5 * rho_air *  A * (diam/2)* getCm(Vm,Wm,v(10)) * Vm; // expérimentalement, j'ai BallSpin / BallSpeed correcte, mais le calcul du Cm ne correspond pas a la realiste!!!.
    W_I = v(7);             // z spin rad/s
    W_J = v(8);             // x spin rad/s
    W_K = v(9);             // y spin (BackSpin) rad/s
    
    Fdx = -(D)*v(2) / Vm;      // Drag Force sur Longueur
    Fdy = -(D)*v(4) / Vm;    // Drag Force sur Hauteur
    Fdz = -(D)*v(6) / Vm;      // Drag Force sur largeur
    
    // M = 0.5 * rho_air *  A * ((diam/2)* (Cl1 * (diam/2 * abs(Wm  / Vm))^0.4) * Vm
    Fmx = (M)*(W_J*v(6)-W_K*v(4)); // Magnus Force sur Longueur 
    Fmy = (M)*(W_K*v(2)-W_I*v(6)); // Magnus Force sur Hauteur
    Fmz = (M)*(W_I*v(4)-W_J*v(2)); // Magnus Force sur largeur
    
    //vprime=zeros(10,1); 
    vprime(1) = v(2);       // 
    vprime(2) = (Fdx + Fmx)/m;// acceleration sur la longueur
    vprime(3) = v(4);       // y
    vprime(4) = -g+(Fdy + Fmy)/m;// Vy = integral(Accely), acceleration sur la hauteur
    vprime(5) = v(6);       // z = integral(Vz)
    vprime(6) = (Fdz + Fmz)/m;// acceleratin du deplacement lateral
    vprime(7) = 0;          // calcul de la diminution du Spin deceleration, considere comme nul
    vprime(8) = 0;          // calcul de la diminution du Spin deceleration, considere comme nul
    vprime(9) = -0;          // calcul de la diminution du Spin deceleration, considere comme nul
    vprime(10) = 0;         // 
    vprime(11) = 0;         // 
    vprime(12) = 0;         // 
    
endfunction


//// ////////////////////////////
function [value] = events2(t,x)
    value = x(2) - 0;       // arrêt d'intégration si la vitesse dans le sens du déplacement est null v(X) < seuil
endfunction

//// ////////////////////////////
function vprime=rolling(t,v) 
    // Fonction xprime
    // du roulement de la balle de golf
    // The physics of putting A.R. Penner
     
    //% %%%%%%%%%%%%%%
    // Constantes
    m = 0.04545;                // en kg
    T = 20 + 273.15; 
    rhog = 0.131;               // kg/m^3 
    g = 9.81;                   // m/s^2
    A = %pi*(diam/2)^2;         // Section en m^2 %pi*r^2
    
    //vprime=zeros(10,1); 
    vprime(1)= v(2);             // 
    vprime(2)= -(5/7)*rhog*g*v(2)/sqrt(v(6)^2+v(2)^2+v(4)^2); // 
    vprime(5)= v(6);                                   // 
    vprime(6)= -(5/7)*rhog*g*v(6)/sqrt(v(6)^2+v(2)^2+v(4)^2); //     
    vprime(3)= v(4);            // y' = x(4), vitesse actuelle
    vprime(4)= -(5/7)*rhog*g*dAltiSol(v(2),v(6))/sqrt(v(6)^2+v(2)^2+v(4)^2);//dAltiSol(vprime(2),vprime(6))*.1;// Vy = integral(Accely)
    vprime(7)=0;                // calcul de la diminution du Spin deceleration, considere comme nul
    vprime(8)=0;                // calcul de la diminution du Spin deceleration, considere comme nul
    vprime(9)=0;                // calcul de la diminution du Spin deceleration, considere comme nul
    vprime(10) = 0;         // 
    vprime(11) = 0;         // 
    vprime(12) = 0;         // 
endfunction


function [V0Clubkmh,dynamicLoft_deg, SpinZ] = BallGolf(V0Ballkmh,Psi,miss,e,m,M)
    // Calcul de la vitesse du club en fonction de la vitesse de 
    // la balle et de l'angle de décollage par rapport au sol.
    // V0Ballms vitesse de la balle en km/h
    // Psi : angle de decollage de la balle entree en degre
    // miss : decentrage de la ballse sur le club, en metre
    // e : smash factor
    // m : poid de la balle en kg
    // M : poid du club en kg
    ////////////////////////////////////////
    // ThetaLoft , inverse de LaunchAngle //
    ////////////////////////////////////////
    // thetaLoft = Psi - atan((-V0Clubms * sin(thetaLoft)/(1+m/M + 5/2))/(V0Clubms * cos(thetaLoft)*(1+e)/(1+m/M))); // angle de decollage de la balle
    // thetaLoft = Psi - atan((-sin(thetaLoft)/(1+m/M + 5/2))/(cos(thetaLoft)*(1+e)/(1+m/M))); // angle de decollage de la balle
    // thetaLoft = Psi - atan((-tan(thetaLoft)/(1+m/M + 5/2))/((1+e)/(1+m/M))); // angle de decollage de la balle
    // V0Ballbfpms     = -V0Clubms * sin(thetaLoft)/(1+m/M + 5/2);     // Vitesse perpendiculaire dans le referentiel de decollage après impact
    // V0Ballbfnms     = V0Clubms * cos(thetaLoft)*(1+e)/(1+m/M);      // Vitesse longitudinale dans le referentiel de decollage après impact (ref: The physics of golf: The optimum loft of a driverA. Raymond Penner)
    // V0Ballms        = sqrt(V0Ballbfnms^2 + V0Ballbfpms^2)* (1 - 0.3556*miss); // Vitesse de decollage de la balle
    // V0Ballms        = sqrt((V0Clubms * cos(thetaLoft)*(1+e)/(1+m/M))^2 + (-V0Clubms * sin(thetaLoft)/(1+m/M + 5/2))^2)* (1 - 0.3556*miss); // Vitesse de decollage de la balle
    // V0Ballms        = sqrt(V0Clubms(1 * cos(thetaLoft)*(1+e)/(1+m/M))^2 + (-1 * sin(thetaLoft)/(1+m/M + 5/2))^2)* (1 - 0.3556*miss); // Vitesse de decollage de la balle
    // Psi             = thetaLoft + atan(V0Ballbfpms/V0Ballbfnms);    // angle de decollage de la balle
    // Psi             = thetaLoft + atan(( -sin(thetaLoft)*(1+m/M)/((1+m/M + 5/2)*(1+e))) / (cos(thetaLoft)) );    // angle de decollage de la balle
    // Soit :
    D = (1+m/M)/((1+m/M + 5/2)*(1+e));
    // Psi             = thetaLoft + atan(( -sin(thetaLoft)* D ) / (cos(thetaLoft)) );    // angle de decollage de la balle
    // Psi             = thetaLoft + atan(-D * tan(thetaLoft) );    // angle de decollage de la balle
    // A = tan(thetaLoft) => thetaLoft = atan(A)
    // Psi             = atan(A) + atan(-D * A);    // angle de decollage de la balle
    // Alors : 
    // thetaLoft = Psi - atan(-A*D)
    // atan(A)   = Psi - atan(-A*D)
    // atan(A)   - Psi + atan(-A*D) = 0
    // atan(A)   + atan(-A*D) - Psi = 0
    // Or :
    // atan(x) + atan(y) = atan((x + y)/(1 - x*y))
    // Ainsi :
    // atan(A)   + atan(-A*D) - Psi = 0 => atan((A - A*D)/(1 + D*A^2)) = Psi
    // (A - A*D)/(1 + D*A^2) = tan(Psi)
    // (A - D*A) = tan(Psi)*(1 + D*A^2)
    // A*(1 - D)   = tan(Psi) + tan(Psi)*D*A^2
    // tan(Psi)*D*A^2 + (D-1)*A + tan(Psi) = 0
    Psi_rd = Psi *%pi/180;//Passage en radian, pour les formules
    V0Ballms = V0Ballkmh *1000/3600;
    a = tan(Psi_rd)*D;
    b = (D-1);
    c = tan(Psi_rd);
    // a*A^2 + b*A + c = 0 ==>
    // A1 = (-b + sqrt(b^2-4*a*c))/(2*a)
    A2 = (-b - sqrt(b^2-4*a*c))/(2*a);
    // thetaLoft = atan(A1) 
    // ou     
    dynamicLoft_rd = atan(A2); //==> On prendra A2 Par defaut
    ////////////////////////////////////////////////////////
    // Vitesse du club en fonction de la vitesse de balle //
    ////////////////////////////////////////////////////////
    // V0Ballms^2/ (1 - 0.3556*miss) = (V0Clubms * cos(thetaLoft)*(1+e)/(1+m/M))^2 + (-V0Clubms * sin(thetaLoft)/(1+m/M + 5/2))^2;
    // V0Ballms^2/ (1 - 0.3556*miss) = V0Clubms^2 * (cos(thetaLoft)*(1+e)/(1+m/M))^2 + V0Clubms^2*(-sin(thetaLoft)/(1+m/M + 5/2))^2;
    // V0Ballms^2/ (1 - 0.3556*miss) = V0Clubms^2 * ((cos(thetaLoft)*(1+e)/(1+m/M))^2 + (-sin(thetaLoft)/(1+m/M + 5/2))^2);
    // V0Ballms^2/ ((1 - 0.3556*miss)*((cos(thetaLoft)*(1+e)/(1+m/M))^2 + (-sin(thetaLoft)/(1+m/M + 5/2))^2)) = V0Clubms^2;
    V0Clubms = sqrt(abs(V0Ballms^2 / ((1 - 0.3556*miss)*((cos(dynamicLoft_rd)*(1+e)/(1+m/M))^2 + (-sin(dynamicLoft_rd)/(1+m/M + 5/2))^2))));
    dynamicLoft_deg = dynamicLoft_rd*180/%pi; // Passage en degre, plus facile a lire ;-)
    V0Clubkmh= V0Clubms * 3600/1000
endfunction


function [thetaLoft, ShafLeanImp, launchAngle, SpinZ, gamaFacePath] = LaunchAngle(v0Ballkmh, v0Clubkmh, TypeClub, SpinLR)
    Sac = GetSac();
    m               = 0.04545; // Mass de la balle en kg

    v0Ballms = v0Ballkmh/3.6;
    V0Clubms = v0Clubkmh/3.6;
    
    SmashFactor = v0Ballms / V0Clubms;

    gamaFacePath = 0;
    miss = 0; 
    //    Ke = (1 + e)/(1+m/M);
    //    Kp = 1/(1+m/M + 5/2);    
    Km = (1 - 0.3556*miss);//(1 - 0.3556*miss)
    
    Ke = (1 + Sac.ecoeff(Sac.Type == TypeClub))/(1+m/Sac.Poids(Sac.Type == TypeClub));// e et M
    Kp = 1/(1+m/Sac.Poids(Sac.Type == TypeClub) + 5/2); // M

//    V0Ballms                =  sqrt(V0Ballbfnms^2 + V0Ballbfpms^2)* (1 - 0.3556*miss); // On admet qu'il n'y a pas de coup raté ==> miss = 0
//    V0Ballms^2*Km^-2        =  V0Clubms^2 * cos(thetaLoft)^2      * Ke^2          + (-V0Clubms)^2 * sin(thetaLoft)^2 * Kp^2;
//    V0Ballms^2*Km^-2        =  V0Clubms^2 * (1-sin(thetaLoft)^2)  * Ke^2          + (-V0Clubms)^2 * sin(thetaLoft)^2 * Kp^2;
//    V0Ballms^2*Km^-2        =  V0Clubms^2*Ke^2 - V0Clubms^2*sin(thetaLoft)^2*Ke^2 + (-V0Clubms)^2*sin(thetaLoft)^2* Kp^2;
//    V0Ballms^2*Km^-2        =  V0Clubms^2*(Ke^2 - sin(thetaLoft)^2*Ke^2 + sin(thetaLoft)^2* (-Kp)^2);
//    (V0Ballms/(V0Clubms*Km))^2 - Ke^2 = sin(thetaLoft)^2*(-Ke^2 + (-Kp)^2);
//    ((v0Ballms/(V0Clubms*Km))^2 - Ke^2)/(-Ke^2 + (-Kp)^2) = sin(thetaLoft)^2;
    thetaLoft = asin(sqrt(abs( (SmashFactor^2-Ke^2) / (Kp^2-Ke^2)) ));
    
    V0Ballbfnms     =  V0Clubms * cos(thetaLoft) * Ke ;     // Vitesse longitudinale dans le referentiel de decollage après impact (ref: The physics of golf: The optimum loft of a driverA. Raymond Penner)
    V0Ballbfpms     = -V0Clubms * sin(thetaLoft) * Kp;     // Vitesse perpendiculaire dans le referentiel de decollage après impact
    launchAngle     = (thetaLoft + atan((V0Ballbfpms/V0Ballbfnms)));  
    //SpinX = 0;//-V0Ballms*cos(LaunchAngle)*sin(0) * Sac.coeffSpinLift(Type == Club) * sin(gamaFacePath);// Le SPIN sur l'axe X n'est pas une composante dans l'axe du chemin de club. Le spin est appliqué sur le plan du sol
    //SpinY = V0Ballms*sin(launchAngle) * sin(gamaFacePath)* Sac.coeffSpinLift(Type == Club) ;// 
    SpinZ = v0Ballms * cos(launchAngle)*cos(0) * sin(thetaLoft) * Sac.coeffSpin(Sac.Type == TypeClub);// 
    thetaLoft   = thetaLoft *180/%pi;
    launchAngle = launchAngle*180/%pi;
    ShafLeanImp = thetaLoft - Sac.Loft(Sac.Type == TypeClub); // et transformation en radian
    gamaFacePath = asin(SpinLR / (v0Ballms * sin(launchAngle) * Sac.coeffSpinLift(Sac.Type == TyupeClub)))*180/%pi;
endfunction
