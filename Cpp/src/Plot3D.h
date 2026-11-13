//============================================================================
// Plot3D.h — exporte la trajectoire en HTML/three.js autonome (lib/three.min.js
// + lib/OrbitControls.js locaux) : vue 3D rotatable a la souris (OrbitControls),
// zoom molette, balle animee le long du vol, sol vert, apex en rouge.
// Ouvrir le .html dans Safari/Chrome/Firefox. Zéro dépendance de compilation.
// matriceFlight : lignes [x, vx, y, vy, z, vz] — repère C++ : z = hauteur.
// Dans three.js Y est la verticale : on mappe (x, z, y) -> (x, z, -y).
//============================================================================
#ifndef PLOT3D_H_
#define PLOT3D_H_

#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

// dossier de l'exécutable en cours (pour retrouver lib/ à côté du binaire)
static std::string exeDir() {
#if defined(__APPLE__)
    char buf[4096]; uint32_t sz = sizeof buf;
    if (_NSGetExecutablePath(buf, &sz) == 0) {
        std::string s = buf; size_t p = s.find_last_of('/');
        return (p == std::string::npos) ? "." : s.substr(0, p);
    }
#elif defined(__linux__)
    char buf[4096]; ssize_t n = readlink("/proc/self/exe", buf, sizeof buf - 1);
    if (n > 0) { buf[n] = 0; std::string s = buf; size_t p = s.find_last_of('/'); return (p == std::string::npos) ? "." : s.substr(0, p); }
#endif
    return ".";
}

// rend le HTML autonome : si lib/three.min.js + lib/OrbitControls.js manquent à côté
// du HTML généré (binaire lancé depuis un autre dossier), on les copie depuis le
// dossier de Plot3D.h (ou les candidats usuels) — sinon THREE est undefined et la
// page est blanche (panneau HTML ok, mais pas de scène 3D ni de spectrogramme).
static void ensureLibNextTo(const std::string& htmlPath) {
    auto exists = [](const std::string& p) { FILE* g = fopen(p.c_str(), "rb"); if (g) { fclose(g); return true; } return false; };
    std::string dir = ".";
    size_t p = htmlPath.find_last_of("/\\");
    if (p != std::string::npos) dir = htmlPath.substr(0, p);
    if (exists(dir + "/lib/three.min.js") && exists(dir + "/lib/OrbitControls.js")) return;
    std::string hdir = __FILE__;
    size_t q = hdir.find_last_of("/\\");
    hdir = (q == std::string::npos) ? "." : hdir.substr(0, q);
    std::string ed = exeDir();
    std::string cands[] = { ed, hdir, ".", "Cpp/src", "src", "../Cpp/src", "../src", "../../Cpp/src" };
    for (const std::string& d : cands) {
        std::string s3 = d + "/lib/three.min.js", sO = d + "/lib/OrbitControls.js";
        if (exists(s3) && exists(sO)) {
            system(("mkdir -p \"" + dir + "/lib\"").c_str());
            auto cp = [](const std::string& a, const std::string& b) {
                FILE* ia = fopen(a.c_str(), "rb"); FILE* ob = fopen(b.c_str(), "wb");
                if (!ia || !ob) { if (ia) fclose(ia); if (ob) fclose(ob); return; }
                char buf[65536]; size_t n;
                while ((n = fread(buf, 1, sizeof buf, ia)) > 0) fwrite(buf, 1, n, ob);
                fclose(ia); fclose(ob);
            };
            cp(s3, dir + "/lib/three.min.js");
            cp(sO, dir + "/lib/OrbitControls.js");
            return;
        }
    }
}

static void write_traj_3d_html(const std::string& path,
                               const std::vector< std::vector<double> >& mf,
                               const std::string& titre,
                               const std::string& infosHtml = "",
                               const std::string& specJson = "",
                               const std::string& golferHtml = "",
                               double faceDeg = std::numeric_limits<double>::quiet_NaN()) {
    if (mf.empty()) return;
    ensureLibNextTo(path);
    double xmax = 1, zmax = 1;
    int iapex = 0;
    for (size_t i = 0; i < mf.size(); i++) {
        xmax = std::max(xmax, mf[i][0]);
        if (mf[i][4] > mf[iapex][4]) iapex = (int)i;
        zmax = std::max(zmax, mf[i][4]);
    }
    // fairway toujours carré : côté >= distance parcourue par la balle (marge 10%, min 50 m)
    double side = std::max(xmax * 1.1, 50.0);
    int gdiv = (int)std::max(2.0, std::round(side / 10.0)); // carreaux de ~10 m
    FILE* f = fopen(path.c_str(), "w");
    if (!f) return;
    fprintf(f, "<!DOCTYPE html><html><head><meta charset='utf-8'><title>%s</title>\n", titre.c_str());
    fprintf(f, "<script src='lib/three.min.js'></script><script src='lib/OrbitControls.js'></script>\n");
    fprintf(f, "<style>body{margin:0;font-family:Helvetica;overflow:hidden}\n"
               "#panel{position:fixed;left:0;top:0;width:250px;height:100%%;background:#1e1e2e;color:#eee;padding:12px;box-sizing:border-box;overflow:auto;font-size:13px;z-index:10}\n"
               "#panel h1{font-size:15px;margin:4px 0 10px;color:#8ab4ff}\n"
               "#panel .k{color:#9aa0b4;margin-top:6px}\n"
               "#panel .v{color:#fff;font-size:15px;font-weight:bold}\n"
               "#panel .hint{position:fixed;bottom:8px;left:12px;color:#889;font-size:11px}\n"
               "#panelR{position:fixed;right:0;top:0;width:230px;height:100%%;background:#16281e;color:#eee;padding:12px;box-sizing:border-box;overflow:auto;font-size:13px;z-index:10}\n"
               "#panelR h1{font-size:15px;margin:4px 0 10px;color:#7bd88a}\n"
               "#panelR .k{color:#9aa0b4;margin-top:6px}\n"
               "#panelR .v{color:#fff;font-size:15px;font-weight:bold}\n"
               "#panelR .big{color:#7bd88a;font-size:22px;font-weight:bold}\n"
               "#view{margin-left:250px;margin-right:230px}\n"
               "#gl{margin-left:250px;margin-right:230px;display:block}\n"
               "#specWrap{position:fixed;bottom:0;left:250px;right:230px;background:#000;border-top:2px solid #555;box-sizing:border-box;z-index:5}\n"
               "#specHandle{position:absolute;top:-6px;left:0;right:0;height:12px;cursor:ns-resize;z-index:6}\n"
               "#specHandle:hover{background:rgba(138,180,255,0.25)}\n"
               "#spec{display:block;width:100%%;height:100%%}\n"
               "</style></head><body>\n");
    fprintf(f, "<div id='panel'><h1>%s</h1>%s<div class='hint'>glisser=turn &middot; molette=zoom<br>vitesse anim: <input id='vit' type='range' min='1' max='30' value='8' style='width:110px'> <span id='vitv'>8</span>x%s</div></div>\n",
            titre.c_str(), infosHtml.c_str(),
            specJson.empty() ? "" : "<br>spectro: drag=zoom &middot; molette=zoom &middot; dblclick=clic-droit=reset");
    if (!golferHtml.empty())
        fprintf(f, "<div id='panelR'><h1>⛳ Pour le golfeur</h1>%s</div>\n", golferHtml.c_str());
    fprintf(f, "<script>\nvar traj=[");
    for (size_t i = 0; i < mf.size(); i += 2)
        fprintf(f, "[%g,%g,%g],", mf[i][0], mf[i][4], -mf[i][2]);  // (x, hauteur z, -latéral y)
    fprintf(f, "];\nvar PW=250,PR=230;var SH=0;var renderer=new THREE.WebGLRenderer({antialias:true});renderer.setSize(innerWidth-PW-PR,innerHeight-SH);renderer.domElement.id='gl';document.body.appendChild(renderer.domElement);\n");
    fprintf(f, "var scene=new THREE.Scene();scene.background=new THREE.Color(0xffffff);\n");
    fprintf(f, "var cam=new THREE.PerspectiveCamera(45,innerWidth/innerHeight,0.1,2000);cam.position.set(-30,%.0f,%.0f);\n", zmax*1.2, side*0.4);
    fprintf(f, "var ctl=new THREE.OrbitControls(cam,renderer.domElement);ctl.target.set(%.0f,0,0);\n", side/2);
    // sol vert carré (côté = side, centré sur la trajectoire en X, centré en Z)
    fprintf(f, "var sol=new THREE.Mesh(new THREE.PlaneGeometry(%.0f,%.0f),new THREE.MeshLambertMaterial({color:0x3a9d3a}));sol.rotation.x=-Math.PI/2;sol.position.set(%.0f,0,0);scene.add(sol);\n", side, side, side/2);
    fprintf(f, "var grid=new THREE.GridHelper(%.0f,%d,0x888888,0xcccccc);grid.position.set(%.0f,0.01,0);scene.add(grid);\n", side, gdiv, side/2);
    // lumiere
    fprintf(f, "scene.add(new THREE.AmbientLight(0xffffff,0.7));var d=new THREE.DirectionalLight(0xffffff,0.8);d.position.set(-50,80,30);scene.add(d);\n");
    // courbe trajectoire
    fprintf(f, "var pts=traj.map(function(p){return new THREE.Vector3(p[0],p[1],p[2]);});\n");
    fprintf(f, "scene.add(new THREE.Line(new THREE.BufferGeometry().setFromPoints(pts),new THREE.LineBasicMaterial({color:0x1a56db})));\n");
    // projection 2D de la trajectoire sur le sol (fairway) : (x, 0, z lateral), ligne pointillée grise
    fprintf(f, "var ptsSol=traj.map(function(p){return new THREE.Vector3(p[0],0.05,p[2]);});\n");
    fprintf(f, "var solLine=new THREE.Line(new THREE.BufferGeometry().setFromPoints(ptsSol),new THREE.LineDashedMaterial({color:0x333333,dashSize:1.5,gapSize:1}));solLine.computeLineDistances();scene.add(solLine);\n");
    // apex
    fprintf(f, "var ap=new THREE.Mesh(new THREE.SphereGeometry(1.2),new THREE.MeshBasicMaterial({color:0xff0000}));ap.position.set(traj[%d][0],traj[%d][1],traj[%d][2]);scene.add(ap);\n", iapex, iapex, iapex);
    // face de club à l'impact : ligne grasse orange HORIZONTALE = le PLAN de la face (perpendiculaire
    // à la direction de lancement), centrée sur l'impact. a = faceDeg = α₀ + GamaFacePath ; la normale
    // de la face pointe selon (cos a, 0, -sin a), donc le plan de la face est dirigé selon (sin a, 0, cos a).
    if (std::isfinite(faceDeg))
        fprintf(f, "var a=-%.6f*Math.PI/180,p0=pts[0],L=8;"  // signe - : dans le modèle, gfp>0 courbe la balle du cote oppose a la normale de face (Magnus), la fleche pointe du cote de la courbure
                   "var face=new THREE.Mesh(new THREE.CylinderGeometry(0.25,0.25,L,12),new THREE.MeshBasicMaterial({color:0xff8800}));"
                   "face.geometry.rotateX(Math.PI/2);face.rotation.y=a;"
                   "face.position.set(p0.x,0.2,p0.z);scene.add(face);\n", faceDeg);
    // équerre : en PLUS du plan de face (orange), tube ROUGE perpendiculaire = normale de la face
    // (direction de lancement de la face), avec flèche (cône) au bout. dirn = (cos a, 0, -sin a).
    if (std::isfinite(faceDeg))
        fprintf(f, "var Ln=6,dirn=new THREE.Vector3(Math.cos(a),0,-Math.sin(a));"
                   "var nrm=new THREE.Mesh(new THREE.CylinderGeometry(0.12,0.12,Ln,10),new THREE.MeshBasicMaterial({color:0xff2222}));"
                   "nrm.geometry.rotateX(Math.PI/2);nrm.rotation.y=a+Math.PI/2;"
                   "nrm.position.set(p0.x+dirn.x*Ln/2,0.2,p0.z+dirn.z*Ln/2);scene.add(nrm);"
                   "var fl=new THREE.Mesh(new THREE.ConeGeometry(0.5,1.4,12),new THREE.MeshBasicMaterial({color:0xff2222}));"
                   "fl.geometry.rotateX(Math.PI/2);fl.rotation.y=a+Math.PI/2;"
                   "fl.position.set(p0.x+dirn.x*(Ln+0.7),0.2,p0.z+dirn.z*(Ln+0.7));scene.add(fl);\n");
    // balle animée
    fprintf(f, "var ball=new THREE.Mesh(new THREE.SphereGeometry(0.8),new THREE.MeshLambertMaterial({color:0xffffff}));scene.add(ball);\n");
    fprintf(f, "var k=0;var VIT=8;document.getElementById('vit').oninput=function(){VIT=+this.value;document.getElementById('vitv').textContent=VIT;};\n");
    fprintf(f, "function anim(){ball.position.copy(pts[Math.floor(k)%%pts.length]);k+=0.15*VIT;ctl.update();renderer.render(scene,cam);requestAnimationFrame(anim);}anim();\n");
    fprintf(f, "addEventListener('resize',function(){resize3D();if(typeof drawSpec==='function')drawSpec();});\nfunction resize3D(){cam.aspect=(innerWidth-PW-PR)/(innerHeight-SH);cam.updateProjectionMatrix();renderer.setSize(innerWidth-PW-PR,innerHeight-SH);}\n");
    // --- frame fixe en bas : densité spectrale (spectrogramme) si fournie ---
    if (!specJson.empty()) {
        fprintf(f, "var S=%s;SH=190;ML=56;\n", specJson.c_str());
        fputs(R"JS(
// conteneur docké : attaché au panneau gauche (left:250px) et au bord droit (right:0)
var wrap=document.createElement('div');wrap.id='specWrap';wrap.style.height=SH+'px';document.body.appendChild(wrap);
var handle=document.createElement('div');handle.id='specHandle';handle.title='tirer pour redimensionner';wrap.appendChild(handle);
var sc=document.createElement('canvas');sc.id='spec';wrap.appendChild(sc);
// drag du handle pour changer la hauteur (redimensionnable)
var dragging=false,startY=0,startH=0;
handle.addEventListener('mousedown',function(e){dragging=true;startY=e.clientY;startH=SH;e.preventDefault();e.stopPropagation();});
addEventListener('mousemove',function(e){if(!dragging)return;var nh=startH+(startY-e.clientY);if(nh<80)nh=80;if(nh>innerHeight-100)nh=innerHeight-100;SH=nh;wrap.style.height=SH+'px';drawSpec();resize3D();});
addEventListener('mouseup',function(){dragging=false;});
handle.addEventListener('dblclick',function(){SH=190;wrap.style.height=SH+'px';drawSpec();resize3D();});
var off=document.createElement('canvas');off.width=S.nframe;off.height=S.nbin;var og=off.getContext('2d');
var im=og.createImageData(S.nframe,S.nbin);
var vir=[[68,1,84],[72,35,116],[59,82,139],[33,145,140],[94,201,98],[253,231,37]];
function cmap(v){v=Math.max(0,Math.min(1,v));var x=v*5,i=Math.floor(x),f=x-i;if(i>4)i=4;
return [vir[i][0]+(vir[i+1][0]-vir[i][0])*f, vir[i][1]+(vir[i+1][1]-vir[i][1])*f, vir[i][2]+(vir[i+1][2]-vir[i][2])*f];}
for(var fr=0;fr<S.nframe;fr++)for(var b=0;b<S.nbin;b++){var v=S.img[fr*S.nbin+b]/255.0;var cc=cmap(v);
var i=((S.nbin-1-b)*S.nframe+fr)*4;im.data[i]=cc[0];im.data[i+1]=cc[1];im.data[i+2]=cc[2];im.data[i+3]=255;}
og.putImageData(im,0,0);
// --- état du zoom : plage visible en frames [vx0,vx1] et en fréquence [vy0,vy1] ---
var vx0=0,vx1=S.nframe-1,vy0=S.fmin,vy1=S.fmax;
function resetZoom(){vx0=0;vx1=S.nframe-1;vy0=S.fmin;vy1=S.fmax;drawSpec();}
function drawSpec(){
var cw=wrap.clientWidth;if(cw<ML+100)cw=ML+100;var w=cw-ML;sc.width=cw;sc.height=SH;sc.style.width=cw+'px';sc.style.height=SH+'px';
var g=sc.getContext('2d');g.imageSmoothingEnabled=true;g.fillStyle='#000';g.fillRect(0,0,cw,SH);
var f0=Math.max(0,Math.round(vx0)),f1=Math.min(S.nframe-1,Math.round(vx1));if(f1<=f0)f1=f0+1;
var b0=Math.max(0,Math.round((vy0-S.fmin)/(S.fmax-S.fmin)*(S.nbin-1)));
var b1=Math.min(S.nbin-1,Math.round((vy1-S.fmin)/(S.fmax-S.fmin)*(S.nbin-1)));if(b1<=b0)b1=b0+1;
// offscreen: ligne 0 = bin nbin-1 (haut) -> source y = nbin-1-b1, hauteur = b1-b0+1
g.drawImage(off,f0,S.nbin-1-b1,f1-f0+1,b1-b0+1,ML,0,w,SH);
// ridge cyan (lissé), limité à la plage visible
g.strokeStyle='#00ffff';g.lineWidth=2;g.beginPath();var started=false;
for(var fr=f0;fr<=f1;fr++){var r=S.ridge[fr];
if(fr>0&&fr<S.nframe-1)r=(S.ridge[fr-1]+r+S.ridge[fr+1])/3;
var fx=ML+(fr-vx0)/(vx1-vx0)*w;var fy=(1-(r-vy0)/(vy1-vy0))*SH;
if(!started){g.moveTo(fx,fy);started=true;}else g.lineTo(fx,fy);}g.stroke();
// impact (pointillé blanc) si visible
if(S.i0>=vx0&&S.i0<=vx1){var xi=ML+(S.i0-vx0)/(vx1-vx0)*w;
g.strokeStyle='#ffffff';g.setLineDash([4,3]);g.lineWidth=1.5;g.beginPath();g.moveTo(xi,0);g.lineTo(xi,SH);g.stroke();g.setLineDash([]);
g.fillStyle='#fff';g.font='11px Helvetica';g.fillText('impact',xi+4,24);}
// grille + labels km/h (axe Y = fréquence -> km/h via f/19.49)
g.font='11px Helvetica';
for(var km=0;km<=350;km+=25){var f=km*19.49;if(f<vy0||f>vy1)continue;
var fy=(1-(f-vy0)/(vy1-vy0))*SH;
g.fillStyle='#aaa';g.fillText(km+'',2,fy+3);
g.strokeStyle='#444';g.lineWidth=1;g.beginPath();g.moveTo(ML,fy);g.lineTo(ML+w,fy);g.stroke();}
// labels temps (axe X)
var t0f=f0*(S.hop||1024)/44100,t1f=f1*(S.hop||1024)/44100;
var step=Math.max(1,Math.ceil((f1-f0)/8));
for(var fr=f0;fr<=f1;fr+=step){var tx=ML+(fr-vx0)/(vx1-vx0)*w;var t=fr*(S.hop||1024)/44100;
g.fillStyle='#aaa';g.fillText(t.toFixed(2)+'s',tx+2,SH-4);}
// titre + plage courante
g.fillStyle='#fff';
var zoomed=(vx0>0||vx1<S.nframe-1||vy0>S.fmin+0.001||vy1<S.fmax-0.001);
g.fillText('spectrogramme R1 — '+(vy0/19.49).toFixed(0)+'..'+(vy1/19.49).toFixed(0)+' km/h, t '+t0f.toFixed(2)+'..'+t1f.toFixed(2)+' s'+(zoomed?'  [ZOOM] drag=zoomer · molette=zoom · dblclick=clic-droit=reset':'  (drag=zoomer · molette=zoom · dblclick=reset)'),ML+4,14);
// rectangle de sélection en cours
if(sel){g.strokeStyle='#fff';g.setLineDash([3,3]);g.lineWidth=1;
g.strokeRect(Math.min(sel.x0,sel.x1),Math.min(sel.y0,sel.y1),Math.abs(sel.x1-sel.x0),Math.abs(sel.y1-sel.y0));g.setLineDash([]);}
}
// --- interactions zoom sur le canvas spectrogramme ---
var sel=null;
sc.addEventListener('mousedown',function(e){if(e.button!==0)return;var r=sc.getBoundingClientRect();sel={x0:e.clientX-r.left,y0:e.clientY-r.top,x1:e.clientX-r.left,y1:e.clientY-r.top};e.preventDefault();});
addEventListener('mousemove',function(e){if(!sel)return;var r=sc.getBoundingClientRect();sel.x1=e.clientX-r.left;sel.y1=e.clientY-r.top;drawSpec();});
addEventListener('mouseup',function(e){if(!sel)return;var r=sc.getBoundingClientRect();var x1=e.clientX-r.left,y1=e.clientY-r.top;
var w=sc.width-ML,x0=Math.min(sel.x0,x1)-ML,x2=Math.max(sel.x0,x1)-ML,y0=Math.min(sel.y0,y1),yy=Math.max(sel.y0,y1);sel=null;
if(w>0&&x2-x0>15&&yy-y0>10){
var nf0=vx0+(x0/w)*(vx1-vx0),nf1=vx0+(x2/w)*(vx1-vx0);
var ny0=vy0+(1-yy/SH)*(vy1-vy0),ny1=vy0+(1-y0/SH)*(vy1-vy0);
vx0=Math.round(nf0);vx1=Math.round(nf1);vy0=ny0;vy1=ny1;
if(vx1<=vx0)vx1=vx0+1;if(vy1<=vy0)vy1=vy0+(S.fmax-S.fmin)/100;}
drawSpec();});
sc.addEventListener('dblclick',function(e){e.preventDefault();resetZoom();});
sc.addEventListener('contextmenu',function(e){e.preventDefault();resetZoom();});
sc.addEventListener('wheel',function(e){e.preventDefault();var r=sc.getBoundingClientRect();var mx=e.clientX-r.left-ML,my=e.clientY-r.top;var w=sc.width-ML;if(w<=0||mx<0)return;
var fx=vx0+(mx/w)*(vx1-vx0),fy=vy0+(1-my/SH)*(vy1-vy0),z=e.deltaY>0?1.25:0.8;
var a=fx-(fx-vx0)*z,b=fx+(vx1-fx)*z,c=fy-(fy-vy0)*z,d=fy+(vy1-fy)*z;
if(b-a<2){a=fx-1;b=fx+1;}if(d-c<(S.fmax-S.fmin)/100){var m=(c+d)/2,dd=(S.fmax-S.fmin)/100;c=m-dd/2;d=m+dd/2;}
vx0=Math.round(a);vx1=Math.round(b);vy0=c;vy1=d;drawSpec();},{passive:false});
drawSpec();resize3D();
)JS", f);
    }
    fprintf(f, "</script></body></html>\n");
    fclose(f);
}

#endif // PLOT3D_H_
