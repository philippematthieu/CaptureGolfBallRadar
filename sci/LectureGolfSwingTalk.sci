function [t,x9,Res,Valeurs] = lanceGolfBall(GraphOn,allFiles,Files)
// Copyright (C) 2016 - mm - Author
//
// About your license if you have any
//
// Date of creation: 2 mai 2016
//
// fichier = uigetfile('*.json');

if allFiles == 0,
    if Files == '',
        [Files,Directory] = uigetfile('*.json');
        chdir(Directory);
    end;    
else
    Directory   = uigetdir();
    chdir(Directory);
    Files    = ls('*.json');
end;
X9 = [];
Valeurs = [];
Type            = ['D'; 'W3'; 'W4'; 'W5'; 'H2'; 'H3'; 'H4'; 'H5'; '3'; '4'; '5'; '6'; '7';'8'; '9'; 'PW'; 'AW'; 'SW1'; 'SW2';'LW' ;'PU' ];
//Valeurs (1,:) = ['Date','Heure','Club',' ClubSpeed','VBall','speedBall','BackSpin','backspin','Lift','DistFlight','distance','DistRoll','Deviation','DevRoll','Height','VLaunch','VLand','Time','ClubPath','FaceToPath','LoftAngleAdd','LoftAngleImp','LieAngleImp','LieAngleAdd','ShaftLeanAdd','ShaftLeanImp','FaceToAddress','AttackAngle','SwingPlane','PlaneAngleUp','PlaneAngleDown','loft','TOP_ADD','TOP_TB','TOP_TOP','TOP_DS','TOP_IMP','SIDE_ADD','SIDE_TB','SIDE_TOP','SIDE_DS','SIDE_IMP','FR_ADD','FR_TB ','FR_TOP','FR_DS ','FR_IMP']
for ii = 1:size(Files,1),
    // Ouverture du fichier
    fid=mopen(Files(ii),'rb');
    disp(Files(ii));
    if allFiles == 1,
        [Date , r1]  = strsplit(part(Files(ii),3:length(Files(ii))-5),'_');
    else
        Date = ['20160519','100000'];
    end;
    mseek(0,fid);
    Ligne = mgetl(fid);
    mclose(fid);
    // Traitement des donnees
    ClubType        = Ligne(grep(Ligne,"ClubType"));
    InfosOnes       = Ligne(grep(Ligne,[ "FaceToAddress" , "FaceToPath", "ClubPath", "AttackAngle",  "SwingPlane","PlaneAngleUp","PlaneAngleDown","""speed""", "loft", "backspin", "distance", "ClubSpeed","ClubType"]));
    InfosTwice      = Ligne(grep(Ligne,["LoftAngle" , "LieAngle" , "ShaftLean"]));
    InfosPOS        = Ligne(grep(Ligne,["TOP_ADD","TOP_TB","TOP_TOP","TOP_DS","TOP_IMP","SIDE_ADD","SIDE_TB","SIDE_TOP","SIDE_DS","SIDE_IMP","FR_ADD","FR_TB","FR_TOP","FR_DS","FR_IMP"]));
    aa              = tokens(InfosTwice(1),[':',',']);
    Value           = strtod(aa(2));
    // Adresse
    aa              = tokens(InfosTwice(1),[':',',']);
    LoftAngleAdd    = strtod(aa(2));
    aa              = tokens(InfosTwice(2),[':',',']);
    LieAngleAdd     = strtod(aa(2));
    aa              = tokens(InfosTwice(3),[':',',']);
    ShaftLeanAdd    = strtod(aa(2));
    
    // Impact
    aa              = tokens(InfosTwice(5),[':',',']);
    LoftAngleImp    = strtod(aa(2));
    aa              = tokens(InfosTwice(4),[':',',']);
    LieAngleImp     = strtod(aa(2));
    aa              = tokens(InfosTwice(6),[':',',']);
    ShaftLeanImp    = strtod(aa(2));
    aa              = tokens(InfosOnes(1),[':',',']);
    FaceToAddress   = strtod(aa(2));
    aa              = tokens(InfosOnes(2),[':',',']);
    FaceToPath      = strtod(aa(2));
    aa              = tokens(InfosOnes(3),[':',',']);
    ClubPath        = strtod(aa(2));
    aa              = tokens(InfosOnes(4),[':',',']);
    AttackAngle     = strtod(aa(2));
    aa              = tokens(InfosOnes(5),[':',',']);
    SwingPlane      = strtod(aa(2));
    aa              = tokens(InfosOnes(6),[':',',']);
    PlaneAngleUp    = strtod(aa(2));
    aa              = tokens(InfosOnes(7),[':',',']);
    PlaneAngleDown  = strtod(aa(2));
    aa              = tokens(InfosOnes(8),[':',',']);
    speed           = strtod(aa(2));
    aa              = tokens(InfosOnes(9),[':',',']);
    loft            = strtod(aa(2));
    aa              = tokens(InfosOnes(10),[':',',']);
    backspin        = strtod(aa(2));
    aa              = tokens(InfosOnes(11),[':',',']);
    distance        = strtod(aa(2));
    aa              = tokens(InfosOnes(12),[':',',']);
    ClubSpeed       = strtod(aa(2));
    aa              = tokens(InfosOnes(13),[':',',']);
    Club            = strtod(aa(2));
    
    // POS
    aa              = tokens(InfosPOS(1),[':',',']);
    TOP_ADD    = strtod(aa(2));
    aa              = tokens(InfosPOS(2),[':',',']);
    TOP_TB    = strtod(aa(2));
    aa              = tokens(InfosPOS(3),[':',',']);
    TOP_TOP    = strtod(aa(2));
    aa              = tokens(InfosPOS(4),[':',',']);
    TOP_DS    = strtod(aa(2));
    aa              = tokens(InfosPOS(5),[':',',']);
    TOP_IMP    = strtod(aa(2));
    aa              = tokens(InfosPOS(6),[':',',']);
    SIDE_ADD    = strtod(aa(2));
    aa              = tokens(InfosPOS(7),[':',',']);
    SIDE_TB    = strtod(aa(2));
    aa              = tokens(InfosPOS(8),[':',',']);
    SIDE_TOP    = strtod(aa(2));
    aa              = tokens(InfosPOS(9),[':',',']);
    SIDE_DS    = strtod(aa(2));
    aa              = tokens(InfosPOS(10),[':',',']);
    SIDE_IMP    = strtod(aa(2));
    aa              = tokens(InfosPOS(11),[':',',']);
    FR_ADD    = strtod(aa(2));
    aa              = tokens(InfosPOS(12),[':',',']);
    FR_TB    = strtod(aa(2));
    aa              = tokens(InfosPOS(13),[':',',']);
    FR_TOP    = strtod(aa(2));
    aa              = tokens(InfosPOS(14),[':',',']);
    FR_DS    = strtod(aa(2));
    aa              = tokens(InfosPOS(15),[':',',']);
    FR_IMP    = strtod(aa(2));


    // Simulation
    [t,x9,Res] = Golfball(18.3,ClubSpeed*3600/1000,Type(Club),ClubPath,FaceToPath,ShaftLeanImp,GraphOn);
    // Date , Heure, Club ,  ClubSpeed , VBall , speedBall , BackSpin , backspin , Lift , DistFlight , distance , DistRoll , Deviation , DevRoll , Height , VLaunch , VLand , Time , ClubPath , FaceToPath , LoftAngleAdd , LoftAngleImp , LieAngleImp , LieAngleAdd , ShaftLeanAdd , ShaftLeanImp , FaceToAddress , AttackAngle , SwingPlane , PlaneAngleUp , PlaneAngleDown , loft , TOP_ADD , TOP_TB , TOP_TOP , TOP_DS , TOP_IMP , SIDE_ADD , SIDE_TB , SIDE_TOP , SIDE_DS , SIDE_IMP , FR_ADD , FR_TB  , FR_TOP , FR_DS  , FR_IMP
    X9 = [X9;x9];
    Valeurs(ii+1,:) = [strtod(Date(1)), strtod(Date(2)), Club,ClubSpeed*3600/1000,Res(2),speed*3600/1000,Res(5), backspin ,Res(6),Res(7),distance,Res(9),Res(8),Res(10),Res(11),Res(12),Res(13),Res(14),ClubPath,FaceToPath,...
    LoftAngleAdd,LoftAngleImp,LieAngleImp,LieAngleAdd,ShaftLeanAdd,ShaftLeanImp,FaceToAddress,AttackAngle,SwingPlane,PlaneAngleUp,PlaneAngleDown,loft,...
    TOP_ADD,TOP_TB,TOP_TOP,TOP_DS,TOP_IMP,SIDE_ADD,SIDE_TB,SIDE_TOP,SIDE_DS,SIDE_IMP,FR_ADD,FR_TB ,FR_TOP,FR_DS ,FR_IMP];
end;
//fid = mopen("ToutSwingTalk.txt",'wt');
//mputl(string(Valeurs),fid);
//mclose(fid);

csvWrite(Valeurs, 'ToutSwingTalk2.csv',';');


 Valeurs(Valeurs(:,10) < 10,:)=[]; // suppression des distance
// figure; club = 13;subplot(2,1,1);plot(Valeurs(Valeurs(:,3) == club,10),Valeurs(Valeurs(:,3) == club,13),'o'); title(['club: ' + string(Type(club))]);xlabel('DistFlight');ylabel('Deviation');
// subplot(2,1,2);plot(Valeurs(Valeurs(:,3) == club,10),Valeurs(Valeurs(:,3) == club,4),'o'); xlabel('DistFlight');ylabel('ClubSpeed');
// figure;plot(Valeurs(:,10), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]);title('Valeurs distance en fonction des Clubs');
// figure;plot(Valeurs(:,19), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]); title('Valeurs du ClubPath en fonction de tous les  clubs');xlabel('club');ylabel('ClubPath');
// figure;plot(Valeurs(:,20), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]);title('valeurs du FaceToPath en fonction de tous les clubs');xlabel('club');ylabel('FaceToPath');
// figure;plot(Valeurs(:,19), Valeurs(:,20),'o'); set(gca(),"grid",[5 7]);ylabel('FaceToPath (°)');xlabel('ClubFace');// valeurs du FaceToPath du ClubFace
// figure;plot(Valeurs(:,4), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]); title('Vitesse du club en fonction du Club');xlabel('Vitesse');ylabel('Club');// Vitesse du club en fonction du Club
// figure;plot(Valeurs(:,4), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]); title('Vitesse du club en fonction du Balle');xlabel('Vitesse');ylabel('Club');// Vitesse du club en fonction du balle
// figure;plot(Valeurs(:,3),Valeurs(:,13),'o'); set(gca(),"grid",[5 7]); title('Deviation en foction du Club');ylabel('Deviation (m)');xlabel('Club');
// size(Valeurs(abs(Valeurs(:,13))<20),1) / size(Valeurs,1);// Pourcentage de deviation acceptable
// figure;club = 1;plot(Valeurs(Valeurs(:,3) == club,3),Valeurs(Valeurs(:,3) == club,19),'o');title('ClubPath (-Gauche; +Droite)');xlabel('Club');ylabel('ClubPath');
// figure;club = 1;plot(Valeurs(Valeurs(:,3) == club,3),Valeurs(Valeurs(:,3) == club,20),'o');title('FaceToPath (-Fermé; +Ouvert)');xlabel('Club');ylabel('FaceToPath');
// figure; plot(Valeurs(Valeurs(:,3),28),'o'); // attackAngle doivent etre négatif
// figure;plot(Valeurs(:,28), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]);title('attackAngle doivent etre négatifs en fonction des Clubs');
// figure;plot(Valeurs(:,23), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]);title('LieAngleAdd en fonction des Clubs');
// figure;plot(Valeurs(:,24), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]);title('LieAngleImp en fonction des Clubs');
// figure;plot(Valeurs(:,25), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]);title('ShaftLeanAdd en fonction des Clubs');
//// ShootNum=10;[t,x9,Res] = Golfball(18.3,Valeurs(ShootNum,4),Type(Valeurs(ShootNum,3)),Valeurs(ShootNum,20),Valeurs(ShootNum,20),0,1);
// figure;plot(Valeurs(:,26), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]);title('ShaftLeanImp en fonction des Clubs');
// figure;plot(Valeurs(:,28), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]);title('AttackAnlge en fonction des Clubs');
// figure;plot(Valeurs(:,47), Valeurs(:,3),'o'); set(gca(),"grid",[5 7]);title('Front Impact en fonction des Clubs');
//
//// [t,VOL,Res]= Golfball(Tfin, V0Club,        Club, alphaClubPath, gamaFacePath,  ShafLeanImp,   GraphOn)
// [t,X9,Res] = Golfball(18.3, Valeurs(57,4),'LW',  Valeurs(57,19),Valeurs(70,20),Valeurs(57,26),0);
 
 
endfunction;
//
//figure;
//Type            = ['D'; 'W3'; 'W4'; 'W5'; 'H2'; 'H3'; 'H4'; 'H5'; '3'; '4'; '5'; '6'; '7';'8'; '9'; 'PW'; 'AW'; 'SW1'; 'SW2';'LW' ;'PU' ];
//for ii = 61:77//size(Valeurs,1),
//        [t,X9,Res] = Golfball(18.3, Valeurs(ii,4),Type(Valeurs(ii,3)),  Valeurs(ii,19),Valeurs(ii,20),Valeurs(ii,26),1);
//        if min(X9(:,1))<0, X9(:,1),Valeurs(ii,2),;end
//        if min(X9(:,3))<0, X9(:,3),Valeurs(ii,2),;end
//        //param3d(X9(:,1),X9(:,5),X9(:,3),180,89,'X@Z@Y'); set(gca(),"grid",[5 7]);
//        e = gce(); e.foreground = color('red');//e.mark_style = 9;
//        
//        //a = gca(); a.data_bounds =[ min(X9(:,1)) , max(X9(:,1)) ,-10+min(X9(:,3)), 10+max(X9(:,3)) , min(X9(:,3)), max(X9(:,3))];
//end;
////
//
