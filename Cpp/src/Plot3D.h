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
#include <cmath>

static void write_traj_3d_html(const std::string& path,
                               const std::vector< std::vector<double> >& mf,
                               const std::string& titre,
                               const std::string& infosHtml = "",
                               const std::string& specJson = "") {
    if (mf.empty()) return;
    double xmax = 1, zmax = 1;
    int iapex = 0;
    for (size_t i = 0; i < mf.size(); i++) {
        xmax = std::max(xmax, mf[i][0]);
        if (mf[i][4] > mf[iapex][4]) iapex = (int)i;
        zmax = std::max(zmax, mf[i][4]);
    }
    FILE* f = fopen(path.c_str(), "w");
    if (!f) return;
    fprintf(f, "<!DOCTYPE html><html><head><meta charset='utf-8'><title>%s</title>\n", titre.c_str());
    fprintf(f, "<script src='lib/three.min.js'></script><script src='lib/OrbitControls.js'></script>\n");
    fprintf(f, "<style>body{margin:0;font-family:Helvetica}\n"
               "#panel{position:fixed;left:0;top:0;width:250px;height:100%%;background:#1e1e2e;color:#eee;padding:12px;box-sizing:border-box;overflow:auto;font-size:13px}\n"
               "#panel h1{font-size:15px;margin:4px 0 10px;color:#8ab4ff}\n"
               "#panel .k{color:#9aa0b4;margin-top:6px}\n"
               "#panel .v{color:#fff;font-size:15px;font-weight:bold}\n"
               "#panel .hint{position:fixed;bottom:8px;left:12px;color:#889;font-size:11px}\n"
               "canvas{margin-left:250px}\n"
               "</style></head><body>\n");
    fprintf(f, "<div id='panel'><h1>%s</h1>%s<div class='hint'>glisser=turn &middot; molette=zoom<br>vitesse anim: <input id='vit' type='range' min='1' max='30' value='8' style='width:110px'> <span id='vitv'>8</span>x</div></div>\n",
            titre.c_str(), infosHtml.c_str());
    fprintf(f, "<script>\nvar traj=[");
    for (size_t i = 0; i < mf.size(); i += 2)
        fprintf(f, "[%g,%g,%g],", mf[i][0], mf[i][4], -mf[i][2]);  // (x, hauteur z, -latéral y)
    fprintf(f, "];\nvar PW=250;var SH=0;var renderer=new THREE.WebGLRenderer({antialias:true});renderer.setSize(innerWidth-PW,innerHeight-SH);document.body.appendChild(renderer.domElement);\n");
    fprintf(f, "var scene=new THREE.Scene();scene.background=new THREE.Color(0xffffff);\n");
    fprintf(f, "var cam=new THREE.PerspectiveCamera(45,innerWidth/innerHeight,0.1,2000);cam.position.set(-30,%.0f,%.0f);\n", zmax*1.2, xmax*0.35);
    fprintf(f, "var ctl=new THREE.OrbitControls(cam,renderer.domElement);ctl.target.set(%.0f,0,0);\n", xmax/2);
    // sol vert
    fprintf(f, "var sol=new THREE.Mesh(new THREE.PlaneGeometry(%.0f,200),new THREE.MeshLambertMaterial({color:0x3a9d3a}));sol.rotation.x=-Math.PI/2;sol.position.set(%.0f,0,0);scene.add(sol);\n", xmax*1.1, xmax/2);
    fprintf(f, "var grid=new THREE.GridHelper(%.0f,20,0x888888,0xcccccc);grid.position.set(%.0f,0.01,0);scene.add(grid);\n", xmax*1.1, xmax/2);
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
    // balle animée
    fprintf(f, "var ball=new THREE.Mesh(new THREE.SphereGeometry(0.8),new THREE.MeshLambertMaterial({color:0xffffff}));scene.add(ball);\n");
    fprintf(f, "var k=0;var VIT=8;document.getElementById('vit').oninput=function(){VIT=+this.value;document.getElementById('vitv').textContent=VIT;};\n");
    fprintf(f, "function anim(){ball.position.copy(pts[Math.floor(k)%%pts.length]);k+=0.15*VIT;ctl.update();renderer.render(scene,cam);requestAnimationFrame(anim);}anim();\n");
    fprintf(f, "addEventListener('resize',function(){cam.aspect=(innerWidth-PW)/innerHeight;cam.updateProjectionMatrix();renderer.setSize(innerWidth-PW,innerHeight-SH);});\n");
    // --- frame fixe en bas : densité spectrale (spectrogramme) si fournie ---
    if (!specJson.empty()) {
        fprintf(f, "var S=%s;SH=190;ML=56;\n", specJson.c_str());
        fputs(R"JS(
var sc=document.createElement('canvas');sc.id='spec';document.body.appendChild(sc);
sc.style.cssText='position:fixed;bottom:0;left:'+(250+ML)+'px;border-top:2px solid #555;background:#000';
var off=document.createElement('canvas');off.width=S.nframe;off.height=S.nbin;var og=off.getContext('2d');
var im=og.createImageData(S.nframe,S.nbin);
var vir=[[68,1,84],[72,35,116],[59,82,139],[33,145,140],[94,201,98],[253,231,37]];
function cmap(v){v=Math.max(0,Math.min(1,v));var x=v*5,i=Math.floor(x),f=x-i;if(i>4)i=4;
return [vir[i][0]+(vir[i+1][0]-vir[i][0])*f, vir[i][1]+(vir[i+1][1]-vir[i][1])*f, vir[i][2]+(vir[i+1][2]-vir[i][2])*f];}
for(var fr=0;fr<S.nframe;fr++)for(var b=0;b<S.nbin;b++){var v=S.img[fr*S.nbin+b]/255.0;var cc=cmap(v);
var i=((S.nbin-1-b)*S.nframe+fr)*4;im.data[i]=cc[0];im.data[i+1]=cc[1];im.data[i+2]=cc[2];im.data[i+3]=255;}
og.putImageData(im,0,0);
function drawSpec(){var w=innerWidth-250-ML;if(w<100)w=100;sc.width=w;sc.height=SH;sc.style.width=w+'px';sc.style.height=SH+'px';
var g=sc.getContext('2d');g.imageSmoothingEnabled=true;g.drawImage(off,0,0,w,SH);
g.strokeStyle='#00ffff';g.lineWidth=2;g.beginPath();var started=false;
for(var fr=0;fr<S.nframe;fr++){var r=S.ridge[fr];
if(fr>0&&fr<S.nframe-1)r=(S.ridge[fr-1]+r+S.ridge[fr+1])/3;
var fx=fr/(S.nframe-1)*w;var fy=(1-(r-S.fmin)/(S.fmax-S.fmin))*SH;
if(!started){g.moveTo(fx,fy);started=true;}else g.lineTo(fx,fy);}g.stroke();
g.strokeStyle='#ffffff';g.setLineDash([4,3]);g.lineWidth=1.5;g.beginPath();var xi=S.i0/(S.nframe-1)*w;g.moveTo(xi,0);g.lineTo(xi,SH);g.stroke();g.setLineDash([]);
g.fillStyle='#fff';g.font='11px Helvetica';g.fillText('impact',xi+4,24);
for(var km=0;km<=250;km+=50){var fy=(1-((km*19.49)-S.fmin)/(S.fmax-S.fmin))*SH;if(fy<0||fy>SH)continue;
g.fillText(km+' km/h',-ML+2,fy+3);g.strokeStyle='#666';g.beginPath();g.moveTo(0,fy);g.lineTo(w,fy);g.stroke();}
var tmax=(S.nframe-1)*(S.hop||1024)/44100;for(var t=0;t<=tmax+0.001;t+=0.1){var tx=t/tmax*w;g.fillText(t.toFixed(1)+'s',tx+2,SH-4);}
g.fillText('spectrogramme R1 — 0..250 km/h, passe-haut 50 km/h, ridge cyan, impact pointille',4,14);}
drawSpec();
)JS", f);
    }
    fprintf(f, "</script></body></html>\n");
    fclose(f);
}

#endif // PLOT3D_H_
