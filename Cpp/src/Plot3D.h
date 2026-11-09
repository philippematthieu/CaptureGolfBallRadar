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
                               const std::string& titre) {
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
    fprintf(f, "<style>body{margin:0}#info{position:fixed;top:8px;left:10px;font:13px Helvetica;color:#333;background:#fff8;padding:4px 8px;border-radius:6px}</style></head><body>\n");
    fprintf(f, "<div id='info'>%s — glisser = tourner, molette = zoom<br>vitesse anim: <input id='vit' type='range' min='1' max='30' value='8' style='width:120px'> <span id='vitv'>8</span>x</div>\n", titre.c_str());
    fprintf(f, "<script>\nvar traj=[");
    for (size_t i = 0; i < mf.size(); i += 2)
        fprintf(f, "[%g,%g,%g],", mf[i][0], mf[i][4], -mf[i][2]);  // (x, hauteur z, -latéral y)
    fprintf(f, "];\nvar renderer=new THREE.WebGLRenderer({antialias:true});renderer.setSize(innerWidth,innerHeight);document.body.appendChild(renderer.domElement);\n");
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
    fprintf(f, "addEventListener('resize',function(){cam.aspect=innerWidth/innerHeight;cam.updateProjectionMatrix();renderer.setSize(innerWidth,innerHeight);});\n");
    fprintf(f, "</script></body></html>\n");
    fclose(f);
}

#endif // PLOT3D_H_
