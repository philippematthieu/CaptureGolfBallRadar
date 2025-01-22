// Copyright (C) 2016 - Corporation - Author
//
// About your license if you have any
//
// Date of creation: 19 mai 2016
//
// dans l'environnement Cgwin dans /home/matthieu il y a un fichier expect (fichier expect test.exp).
// Celui-ci lance sur le Android une commande de copie du "result.json" dans le répertoire courant cygwin du de lancement
// sur le PC (fichier expect test.exp)
// Scilab analyse alors la date de modification. Si elle differe, alors il lance l'affichage de golf.
// 

unix('C:\Users\matthieu\AppData\Local\Android\sdk\platform-tools\adb  forward tcp:22 tcp:2222');
chdir('C:\cygwin64\home\matthieu\');
a = 'C:\cygwin64\home\matthieu\result.json ';
[t,x9,Res,Valeurs] = lanceGolfBall(1,0,a);
[x, ierr]  = fileinfo(a);
for ii = 1:5
    sleep(2000);
    unix('C:\cygwin64\bin\expect test.exp');
    [x1, ierr]  = fileinfo(a);
    if and(getdate(x(6)) == getdate(x1(6))); 
        //break;
    else
        [t,x9,Res,Valeurs] = lanceGolfBall(1,0,a);
        x = x1;
    end;
end;
