#pragma once
// Automatisch erzeugt aus index.html (python3 gen_webui.py)
const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1">
<meta name="theme-color" content="#14161c">
<title>DMX-Steuerung</title>
<style>
:root{--bg:#14161c;--card:#1d2029;--line:#2c303c;--txt:#e8eaf0;--mut:#8b90a0;--acc:#4f8cff;--ok:#2ecc71;--bad:#ff4f5e;--warn:#f5a623}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
body{margin:0;background:var(--bg);color:var(--txt);font:15px/1.4 system-ui,-apple-system,Segoe UI,Roboto,sans-serif;padding-bottom:70px}
header{position:sticky;top:0;z-index:5;background:var(--bg);border-bottom:1px solid var(--line);padding:10px 14px;display:flex;align-items:center;gap:10px}
header h1{font-size:17px;margin:0;flex:1}
.dot{width:10px;height:10px;border-radius:50%;background:var(--mut);display:inline-block}
.dot.ok{background:var(--ok)}.dot.bad{background:var(--bad)}
main{max-width:760px;margin:0 auto;padding:12px}
.card{background:var(--card);border:1px solid var(--line);border-radius:14px;padding:14px;margin-bottom:12px}
.card h2{font-size:13px;text-transform:uppercase;letter-spacing:.06em;color:var(--mut);margin:0 0 10px}
nav{position:fixed;bottom:0;left:0;right:0;display:flex;background:var(--card);border-top:1px solid var(--line);z-index:6}
nav button{flex:1;background:none;border:0;color:var(--mut);padding:10px 2px 12px;font-size:12px}
nav button b{display:block;font-size:20px;line-height:1.1}
nav button.on{color:var(--acc)}
.page{display:none}.page.on{display:block}
button.b{background:#2a2f3c;color:var(--txt);border:1px solid var(--line);border-radius:10px;padding:10px 14px;font-size:15px;cursor:pointer}
button.b:active{transform:scale(.97)}
button.pri{background:var(--acc);border-color:var(--acc);color:#fff}
button.on-b{background:var(--ok);border-color:var(--ok);color:#06210f;font-weight:700}
button.off-b{background:var(--bad);border-color:var(--bad);color:#fff;font-weight:700}
button.sel{outline:2px solid var(--acc);background:#22324f}
.row{display:flex;gap:8px;flex-wrap:wrap;align-items:center}
.grow{flex:1}
.big{flex:1;padding:16px;font-size:17px}
.chips button{padding:8px 12px;font-size:14px}
.sl{display:flex;align-items:center;gap:10px;margin:8px 0}
.sl label{width:78px;color:var(--mut);font-size:13px}
.sl output{width:44px;text-align:right;font-variant-numeric:tabular-nums}
input[type=range]{flex:1;accent-color:var(--acc);height:28px}
input[type=text],input[type=number],input[type=password],select{background:#11131a;color:var(--txt);border:1px solid var(--line);border-radius:8px;padding:8px;font-size:15px;width:100%}
.wheelbox{display:flex;gap:14px;align-items:center;flex-wrap:wrap;justify-content:center}
#wheel{width:240px;height:240px;border-radius:50%;touch-action:none;cursor:crosshair}
.sw{display:grid;grid-template-columns:repeat(6,1fr);gap:8px;margin-top:10px}
.sw button{height:38px;border-radius:10px;border:1px solid var(--line)}
#prev{width:70px;height:70px;border-radius:14px;border:1px solid var(--line)}
.fx{display:grid;grid-template-columns:repeat(4,1fr);gap:8px}
.fx button{padding:10px 4px;font-size:13px}
.grid2{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.f label{display:block;font-size:12px;color:var(--mut);margin:8px 0 3px}
.fix{border:1px solid var(--line);border-radius:12px;padding:10px;margin-bottom:10px;background:#181b23}
.fix .hd{display:flex;gap:8px;align-items:center}
.chl{display:grid;grid-template-columns:44px 1fr 70px;gap:6px;align-items:center;margin-top:6px;font-size:13px}
.warn{color:var(--warn);font-size:13px}
.mut{color:var(--mut);font-size:13px}
#mon{display:grid;grid-template-columns:repeat(16,1fr);gap:2px;font-size:9px;font-variant-numeric:tabular-nums}
#mon div{background:#11131a;border-radius:3px;text-align:center;padding:2px 0;cursor:pointer;position:relative}
#mon div.ov{outline:1px solid var(--warn)}
#mon div i{position:absolute;left:0;bottom:0;height:3px;background:var(--acc)}
.scn{display:grid;grid-template-columns:repeat(3,1fr);gap:8px}
.scn button{padding:14px 6px;min-height:58px}
.scn button.empty{color:var(--mut);border-style:dashed}
#toast{position:fixed;left:50%;bottom:84px;transform:translateX(-50%);background:#000c;color:#fff;padding:8px 14px;border-radius:10px;display:none;z-index:9}
.badge{font-size:11px;padding:2px 7px;border-radius:8px;background:#2a2f3c;color:var(--mut)}
.badge.hot{background:var(--warn);color:#000}
#setup{position:fixed;inset:0;background:var(--bg);z-index:20;overflow:auto;display:none;padding:16px}
#setup .card{max-width:520px;margin:0 auto 12px}
.opt{display:block;border:1px solid var(--line);border-radius:12px;padding:12px;margin:8px 0;cursor:pointer}
.opt input{margin-right:8px}
.opt.on{border-color:var(--acc);background:#1b2742}
</style>
</head>
<body>
<div id="setup">
 <div class="card">
  <h1 style="font-size:20px;margin:0 0 6px">Willkommen 👋</h1>
  <p class="mut" style="margin:0">Wie soll sich die DMX-Steuerung mit deinem Handy oder PC verbinden? Das kannst du später unter <b>System</b> jederzeit ändern.</p>
 </div>
 <div class="card f">
  <label class="opt on" id="o0"><input type="radio" name="wm" value="0" checked><b>Direktverbindung</b><br><span class="mut">Der ESP32 öffnet sein eigenes, passwortgeschütztes WLAN. Handy oder PC verbinden sich direkt damit, ein Router wird nicht gebraucht. Adresse: <b>http://192.168.1.4</b></span></label>
  <label class="opt" id="o1"><input type="radio" name="wm" value="1"><b>Mit vorhandenem WLAN verbinden</b><br><span class="mut">Der ESP32 meldet sich in deinem WLAN an (nur 2,4 GHz). Erreichbar unter <b>http://dmx.local</b> oder seiner IP im Router. Klappt das nicht, öffnet er automatisch wieder die Direktverbindung.</span></label>
 </div>
 <div class="card f" id="sw1" style="display:none">
  <h2>Dein WLAN</h2>
  <button class="b" id="s_scan" style="width:100%">WLAN-Netze suchen</button>
  <div id="s_scanl" class="chips row" style="margin-top:6px"></div>
  <label>WLAN-Name (SSID)</label><input type="text" id="s_ssid" autocomplete="off">
  <label>WLAN-Passwort</label><input type="password" id="s_pass" autocomplete="off">
  <label><input type="checkbox" onclick="s_pass.type=this.checked?'text':'password'"> Passwort anzeigen</label>
 </div>
 <div class="card f">
  <h2 id="s_aph">Direktverbindung</h2>
  <label>Name des ESP32-WLANs</label><input type="text" id="s_apSsid" maxlength="32">
  <label>Passwort (mind. 8 Zeichen)</label><input type="text" id="s_apPass" maxlength="63">
  <p class="mut">Merke dir dieses Passwort gut. Du brauchst es für jede Direktverbindung.</p>
 </div>
 <div class="card"><button class="b pri" style="width:100%;padding:14px" id="s_save">Speichern & starten</button><div id="s_msg" class="mut" style="margin-top:8px"></div></div>
</div>
<header><span id="wdot" class="dot"></span><h1>DMX-Steuerung</h1><span id="artb" class="badge" style="display:none">Art-Net</span><span id="ovb" class="badge hot" style="display:none"></span><button class="b" id="bo" style="padding:6px 12px">Blackout</button></header>
<main>

<!-- ======================= STEUERUNG ======================= -->
<section class="page on" id="p-ctl">
 <div class="card">
  <h2>Scheinwerfer</h2>
  <div class="row chips" id="fixsel"></div>
  <div class="row" style="margin-top:10px">
   <button class="b big on-b" id="allon">AN</button>
   <button class="b big off-b" id="alloff">AUS</button>
  </div>
 </div>

 <div class="card">
  <h2>Farbe</h2>
  <div class="wheelbox">
   <canvas id="wheel" width="480" height="480"></canvas>
   <div style="display:flex;flex-direction:column;gap:8px;align-items:center"><div id="prev"></div><span class="mut" id="hexv">#000000</span></div>
  </div>
  <div class="sw" id="sw"></div>
  <div id="colsl"></div>
 </div>

 <div class="card">
  <h2>Helligkeit</h2>
  <div class="sl"><label>Dimmer</label><input type="range" id="dim" min="0" max="255"><output id="dimo"></output></div>
  <div class="sl"><label>Master</label><input type="range" id="master" min="0" max="255"><output id="mastero"></output></div>
  <div class="sl"><label>Überblend.</label><input type="range" id="fade" min="0" max="5000" step="50"><output id="fadeo"></output></div>
 </div>

 <div class="card">
  <h2>Effekte</h2>
  <div class="fx" id="fx"></div>
  <div class="sl"><label>Tempo</label><input type="range" id="speed" min="1" max="100"><output id="speedo"></output></div>
  <div class="sl"><label>Stroboskop</label><input type="range" id="strobe" min="0" max="100"><output id="strobeo"></output></div>
  <div class="mut">Effekte wirken auf die beim Start ausgewählten Scheinwerfer.</div>
 </div>
</section>

<!-- ======================= SZENEN ======================= -->
<section class="page" id="p-scn">
 <div class="card">
  <h2>Szenen abrufen</h2>
  <div class="scn" id="scnl"></div>
  <p class="mut">Tippen = abrufen. Leere Felder speichern den aktuellen Zustand. Lang drücken = speichern/löschen.</p>
 </div>
</section>

<!-- ======================= GERÄTE ======================= -->
<section class="page" id="p-fix">
 <div class="card">
  <h2>Scheinwerfer / DMX-Patch</h2>
  <div id="fixl"></div>
  <div id="conf" class="warn"></div>
  <div class="row" style="margin-top:8px">
   <select id="newprof" class="grow"></select>
   <button class="b" id="addfix">+ Hinzufügen</button>
  </div>
  <div class="row" style="margin-top:10px">
   <button class="b" id="autoaddr">Adressen automatisch</button>
   <button class="b pri grow" id="savefix">Speichern</button>
  </div>
 </div>
 <div class="card mut">
  Jeder Scheinwerfer belegt ab seiner <b>Startadresse</b> so viele Kanäle wie sein Modus hat. Stelle am Gerät dieselbe Adresse
  (z.B. „A001“, „d001“) und denselben Kanal-Modus (z.B. „7CH“) ein. Kanalfunktion „Festwert“ sendet immer den eingetragenen Wert
  (z.B. für Modus-/Makro-Kanäle). Beim Strobe-Kanal ist der Wert der Ruhewert (Shutter offen, meist 0, bei manchen Geräten 255).
 </div>
</section>

<!-- ======================= KANÄLE ======================= -->
<section class="page" id="p-ch">
 <div class="card">
  <h2>Kanal-Test</h2>
  <div class="grid2 f">
   <div><label>Kanal (1–512)</label><input type="number" id="tch" min="1" max="512" value="1"></div>
   <div><label>Anzahl</label><input type="number" id="tn" min="1" max="512" value="1"></div>
  </div>
  <div class="sl"><label>Wert</label><input type="range" id="tv" min="0" max="255" value="0"><output id="tvo">0</output></div>
  <div class="row"><button class="b" id="tfull">100 %</button><button class="b" id="tzero">0 %</button><button class="b" id="trel">Freigeben</button><button class="b off-b grow" id="tclr">Alle freigeben</button></div>
  <p class="mut">Übersteuert einzelne Kanäle direkt – praktisch, um Adressen und Kanal-Belegung zu prüfen.</p>
 </div>
 <div class="card">
  <h2>DMX-Monitor (Universum-Ausgang)</h2>
  <div id="mon"></div>
 </div>
</section>

<!-- ======================= EINSTELLUNGEN ======================= -->
<section class="page" id="p-set">
 <div class="card f">
  <h2>WLAN</h2>
  <div class="mut" id="winfo"></div>
  <label>Verbindungsart</label>
  <select id="w_mode"><option value="0">Direktverbindung (eigenes WLAN des ESP32, 192.168.1.4)</option><option value="1">Vorhandenes WLAN (Direktverbindung nur als Notfall)</option></select>
  <div class="row" style="margin-top:8px"><button class="b" id="scan">WLAN-Netze suchen</button></div>
  <div id="scanl" class="chips row" style="margin-top:6px"></div>
  <label>SSID</label><input type="text" id="w_ssid">
  <label>Passwort</label><input type="password" id="w_pass">
  <label>Gerätename (mDNS: name.local)</label><input type="text" id="w_host">
  <label><input type="checkbox" id="w_static"> Feste IP-Adresse</label>
  <div class="grid2"><div><label>IP</label><input type="text" id="w_ip"></div><div><label>Gateway</label><input type="text" id="w_gw"></div>
  <div><label>Subnetz</label><input type="text" id="w_mask"></div><div><label>DNS</label><input type="text" id="w_dns"></div></div>
  <label>Name des ESP32-WLANs (Direktverbindung)</label><input type="text" id="w_apSsid">
  <label>Passwort der Direktverbindung (min. 8 Zeichen)</label><input type="password" id="w_apPass">
  <button class="b pri" style="margin-top:12px;width:100%" id="savewifi">WLAN speichern & neu starten</button>
  <button class="b" style="margin-top:8px;width:100%" id="wizard">Einrichtungsassistent öffnen</button>
 </div>
 <div class="card f">
  <h2>DMX & Art-Net</h2>
  <div class="grid2">
   <div><label>DMX-TX Pin (GPIO)</label><input type="number" id="d_tx"></div>
   <div><label>DE/RE Pin (GPIO, 255=aus)</label><input type="number" id="d_de"></div>
   <div><label>Status-LED (GPIO, 255=aus)</label><input type="number" id="d_led"></div>
   <div><label>Bildrate (Hz, 10–44)</label><input type="number" id="d_rate"></div>
  </div>
  <label><input type="checkbox" id="d_art"> Art-Net-Empfang (Lichtpult/App per WLAN, UDP 6454)</label>
  <label>Art-Net Universum (0–32767)</label><input type="number" id="d_uni">
  <label>Beim Einschalten</label>
  <select id="d_boot"><option value="0">alles aus</option><option value="1">letzten Zustand wiederherstellen</option><option value="2">Szene laden</option></select>
  <label>Start-Szene</label><select id="d_bootScene"></select>
  <button class="b pri" style="margin-top:12px;width:100%" id="savedmx">Speichern</button>
 </div>
 <div class="card f">
  <h2>System</h2>
  <div class="mut" id="sysinfo"></div>
  <div class="row" style="margin-top:10px">
   <button class="b" id="export">Backup laden</button>
   <label class="b" style="display:inline-block">Backup einspielen<input type="file" id="import" accept=".json" hidden></label>
  </div>
  <label>Firmware-Update (.bin der App, nicht die „merged“-Datei)</label>
  <input type="file" id="fw" accept=".bin">
  <div class="row" style="margin-top:10px">
   <button class="b" id="doupd">Update starten</button>
   <button class="b" id="reboot">Neustart</button>
   <button class="b off-b" id="factory">Werkseinstellungen</button>
  </div>
 </div>
</section>
</main>

<nav>
 <button data-p="ctl" class="on"><b>&#9673;</b>Steuerung</button>
 <button data-p="scn"><b>&#9638;</b>Szenen</button>
 <button data-p="fix"><b>&#9881;</b>Geräte</button>
 <button data-p="ch"><b>&#9776;</b>Kanäle</button>
 <button data-p="set"><b>&#8942;</b>System</button>
</nav>
<div id="toast"></div>

<script>
const $=id=>document.getElementById(id);
const FN=['—','Dimmer','Rot','Grün','Blau','Weiß','Amber','UV','Strobe','Festwert','Dimmer fein'];
const PROF={
 'RGB (3 Kanäle)':[2,3,4],
 'RGBW (4 Kanäle)':[2,3,4,5],
 'Dimmer+RGB (4 Kanäle)':[1,2,3,4],
 'Dimmer+RGB+Strobe (5 Kanäle)':[1,2,3,4,8],
 'RGBW+Dimmer+Strobe (6 Kanäle)':[1,2,3,4,5,8],
 'PAR 7 Kanäle (Dim,R,G,B,W,Strobe,Modus)':[1,2,3,4,5,8,9],
 'PAR 7 Kanäle (R,G,B,W,Dim,Strobe,Modus)':[2,3,4,5,1,8,9],
 'RGBWA+UV (6 Kanäle)':[2,3,4,5,6,7],
 'Dim+RGBWA+UV+Strobe (8 Kanäle)':[1,2,3,4,5,6,7,8],
 'Dimmer (1 Kanal)':[1],
 'Dimmer 16 bit (2 Kanäle)':[1,10],
 'Benutzerdefiniert (8 Kanäle)':[0,0,0,0,0,0,0,0]
};
const FX=['Statisch','Regenbogen','Atmen','Lauflicht','Farbwechsel','Feuer','Disco','Blaulicht'];
const SW=['#ff0000','#ff6000','#ffb000','#ffff00','#80ff00','#00ff00','#00ffa0','#00ffff','#0080ff','#0000ff','#8000ff','#ff00ff','#ff0080','#ff8080','#ffffff','#ffd8a0','#ffa050','#000000'];
const CN=['Rot','Grün','Blau','Weiß','Amber','UV'];

let st=null,cfg=null,sel=new Set(),busy=0,lastTouch=0,scenes=[];

function toast(t){const e=$('toast');e.textContent=t;e.style.display='block';clearTimeout(e._t);e._t=setTimeout(()=>e.style.display='none',1800)}
async function api(u,b){const r=await fetch(u,b===undefined?{}:{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(b)});return r.json()}

// ---------- gedrosseltes Senden
let pend=null,inflight=false;
function send(o){lastTouch=Date.now();pend=Object.assign(pend||{},o);if(!inflight)flush()}
async function flush(){if(!pend)return;const b=pend;pend=null;inflight=true;try{await api('/api/control',b)}catch(e){toast('Keine Verbindung')}inflight=false;if(pend)setTimeout(flush,40)}
function ids(){return [...sel]}

// ---------- Navigation
document.querySelectorAll('nav button').forEach(b=>b.onclick=()=>{
 document.querySelectorAll('nav button').forEach(x=>x.classList.toggle('on',x===b));
 document.querySelectorAll('.page').forEach(p=>p.classList.toggle('on',p.id=='p-'+b.dataset.p));
 const p=b.dataset.p; if(p=='scn')loadScenes(); if(p=='fix'||p=='set')loadCfg();
});

// ---------- Farbrad
const wc=$('wheel'),wx=wc.getContext('2d');
(function drawWheel(){const W=wc.width,R=W/2,img=wx.createImageData(W,W);
 for(let y=0;y<W;y++)for(let x=0;x<W;x++){const dx=x-R,dy=y-R,d=Math.sqrt(dx*dx+dy*dy);const i=(y*W+x)*4;
  if(d>R){img.data[i+3]=0;continue}
  const h=(Math.atan2(dy,dx)/(2*Math.PI)+1)%1,s=d/R,[r,g,b]=hsv(h,s,1);
  img.data[i]=r;img.data[i+1]=g;img.data[i+2]=b;img.data[i+3]=255}
 wx.putImageData(img,0,0)})();
function hsv(h,s,v){let i=Math.floor(h*6),f=h*6-i,p=v*(1-s),q=v*(1-f*s),t=v*(1-(1-f)*s),r,g,b;
 switch(i%6){case 0:r=v;g=t;b=p;break;case 1:r=q;g=v;b=p;break;case 2:r=p;g=v;b=t;break;case 3:r=p;g=q;b=v;break;case 4:r=t;g=p;b=v;break;default:r=v;g=p;b=q}
 return [Math.round(r*255),Math.round(g*255),Math.round(b*255)]}
function wheelPick(e){const r=wc.getBoundingClientRect(),x=e.clientX-r.left-r.width/2,y=e.clientY-r.top-r.height/2;
 let d=Math.sqrt(x*x+y*y)/(r.width/2);if(d>1)d=1;const h=(Math.atan2(y,x)/(2*Math.PI)+1)%1;const [R,G,B]=hsv(h,d,1);setCol([R,G,B,0,-1,-1],true)}
let drag=false;
wc.addEventListener('pointerdown',e=>{drag=true;wc.setPointerCapture(e.pointerId);wheelPick(e)});
wc.addEventListener('pointermove',e=>{if(drag)wheelPick(e)});
wc.addEventListener('pointerup',()=>drag=false);

// ---------- Farbschieber
$('colsl').innerHTML=CN.map((n,k)=>`<div class="sl" id="cr${k}"><label>${n}</label><input type="range" min="0" max="255" id="c${k}"><output id="c${k}o"></output></div>`).join('');
for(let k=0;k<6;k++)$('c'+k).oninput=e=>{const c=[-1,-1,-1,-1,-1,-1];c[k]=+e.target.value;$('c'+k+'o').textContent=c[k];send({ids:ids(),col:c});localCol(c);preview()};
function setCol(c,stopFx){for(let k=0;k<6;k++)if(c[k]>=0){$('c'+k).value=c[k];$('c'+k+'o').textContent=c[k]}
 const o={ids:ids(),col:c,on:true};if(stopFx&&st&&st.effect){o.effect=0;st.effect=0;renderFx()}send(o);localCol(c);preview()}
function localCol(c){if(!st)return;for(const i of sel){const f=st.fixtures[i];if(!f)continue;for(let k=0;k<6;k++)if(c[k]>=0)f.col[k]=c[k]}}
function preview(){const r=+$('c0').value,g=+$('c1').value,b=+$('c2').value,w=+$('c3').value;
 const m=v=>Math.min(255,v+w*.8)|0;const hx='#'+[m(r),m(g),m(b)].map(v=>v.toString(16).padStart(2,'0')).join('');
 $('prev').style.background=hx;$('hexv').textContent='#'+[r,g,b].map(v=>v.toString(16).padStart(2,'0')).join('')+(w?' W'+w:'')}
$('sw').innerHTML=SW.map(c=>`<button style="background:${c}" data-c="${c}" title="${c}"></button>`).join('');
$('sw').onclick=e=>{const c=e.target.dataset.c;if(!c)return;const v=[1,3,5].map(i=>parseInt(c.substr(i,2),16));
 const hasW=[...sel].some(i=>st&&st.fixtures[i]&&(st.fixtures[i].caps&32));
 if(c=='#ffffff'&&hasW)setCol([0,0,0,255,0,0],true);else setCol([v[0],v[1],v[2],0,0,0],true)};

// ---------- Helligkeit, Effekte
function bindSl(id,key,fmt){$(id).oninput=e=>{const v=+e.target.value;$(id+'o').textContent=fmt?fmt(v):v;const o={};o[key]=v;if(key=='dim'){o.ids=ids();for(const i of sel)if(st.fixtures[i])st.fixtures[i].dim=v}send(o)}}
const pct=v=>Math.round(v/2.55)+'%';
bindSl('dim','dim',pct);bindSl('master','master',pct);bindSl('fade','fade',v=>(v/1000).toFixed(1)+'s');bindSl('speed','speed');bindSl('strobe','strobe',v=>v?v:'aus');
$('fx').innerHTML=FX.map((n,i)=>`<button class="b" data-i="${i}">${n}</button>`).join('');
$('fx').onclick=e=>{const i=e.target.dataset.i;if(i===undefined)return;st.effect=+i;renderFx();send({ids:ids(),effect:+i,on:true})};
function renderFx(){document.querySelectorAll('#fx button').forEach(b=>b.classList.toggle('sel',st&&+b.dataset.i==st.effect))}
$('allon').onclick=()=>{send({ids:ids(),on:true,blackout:false});sel.forEach(i=>st.fixtures[i].on=true);renderSel()};
$('alloff').onclick=()=>{send({ids:ids(),on:false});sel.forEach(i=>st.fixtures[i].on=false);renderSel()};
$('bo').onclick=()=>{st.blackout=!st.blackout;send({blackout:st.blackout});renderTop()};

// ---------- Auswahl
function renderSel(){if(!st)return;const h=st.fixtures.map((f,i)=>`<button class="b ${sel.has(i)?'sel':''}" data-i="${i}">${f.on?'● ':'○ '}${esc(f.n)}</button>`).join('');
 $('fixsel').innerHTML=`<button class="b" data-i="all">Alle</button>`+h}
$('fixsel').onclick=e=>{const b=e.target.closest('button');if(!b)return;const i=b.dataset.i;
 if(i=='all'){sel=sel.size==st.fixtures.length?new Set():new Set(st.fixtures.map((_,k)=>k))}else{sel.has(+i)?sel.delete(+i):sel.add(+i)}
 renderSel();syncCtl()};
let pressT;
$('fixsel').addEventListener('pointerdown',e=>{const b=e.target.closest('button');if(!b||b.dataset.i=='all')return;pressT=setTimeout(()=>{send({identify:+b.dataset.i});toast('Identifiziere '+st.fixtures[+b.dataset.i].n)},700)});
['pointerup','pointerleave'].forEach(ev=>$('fixsel').addEventListener(ev,()=>clearTimeout(pressT)));

function syncCtl(){if(!st)return;const f=st.fixtures[[...sel][0]];
 if(f){for(let k=0;k<6;k++){$('c'+k).value=f.col[k];$('c'+k+'o').textContent=f.col[k]}$('dim').value=f.dim;$('dimo').textContent=pct(f.dim)}
 let caps=0;for(const i of sel)caps|=st.fixtures[i]?st.fixtures[i].caps:0;if(!sel.size)caps=0xFFFF;
 [4,8,16,32,64,128].forEach((m,k)=>$('cr'+k).style.display=(k<3||caps&m)?'':'none');
 $('master').value=st.master;$('mastero').textContent=pct(st.master);
 $('fade').value=st.fade;$('fadeo').textContent=(st.fade/1000).toFixed(1)+'s';
 $('speed').value=st.speed;$('speedo').textContent=st.speed;
 $('strobe').value=st.strobe;$('strobeo').textContent=st.strobe||'aus';
 renderFx();preview()}
function renderTop(){if(!st)return;$('bo').className='b '+(st.blackout?'off-b':'');
 $('wdot').className='dot '+(st.wifi.sta?'ok':'bad');
 $('artb').style.display=st.art.en?'':'none';$('artb').className='badge '+(st.art.active?'hot':'');
 $('ovb').style.display=st.ovr?'':'none';$('ovb').textContent=st.ovr+' Kanäle manuell'}
function esc(s){return String(s).replace(/[&<>"]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;'}[c]))}

async function poll(){try{const s=await api('/api/state');const first=!st;
 if(Date.now()-lastTouch<1500&&st){st.wifi=s.wifi;st.art=s.art;st.ovr=s.ovr;renderTop();return}
 if(first&&s.setup)openWizard();st=s;if(first)sel=new Set(st.fixtures.map((_,i)=>i));sel=new Set([...sel].filter(i=>i<st.fixtures.length));
 renderSel();syncCtl();renderTop();
 $('sysinfo').innerHTML=`Firmware ${s.fw} · Laufzeit ${Math.floor(s.up/3600)}h ${Math.floor(s.up/60)%60}min · freier Speicher ${(s.heap/1024).toFixed(0)} kB`;
 $('winfo').innerHTML=s.wifi.mode==0?`Direktverbindung aktiv: WLAN <b>${esc(s.wifi.apSsid)}</b>, Adresse <b>http://192.168.1.4</b>`:((s.wifi.sta?`Verbunden mit <b>${esc(s.wifi.ssid)}</b>, IP <b>${s.wifi.ip}</b> (${s.wifi.rssi} dBm), auch erreichbar als <b>http://${esc(s.wifi.host)}.local</b>`:'Nicht mit dem WLAN <b>'+esc(s.wifi.ssid)+'</b> verbunden.'+(s.wifi.reason?' Grund: <b>'+esc(s.wifi.reason)+'</b>':''))+(s.wifi.ap?` · Direktverbindung als Notfall aktiv (192.168.1.4)`:''));
}catch(e){$('wdot').className='dot bad'}}
setInterval(poll,2000);poll();

// ---------- Szenen
async function loadScenes(){scenes=await api('/api/scenes');
 $('scnl').innerHTML=scenes.map(s=>`<button class="b ${s.used?'':'empty'}" data-s="${s.slot}">${s.used?esc(s.name):'+ Szene '+(s.slot+1)}</button>`).join('');
 const bs=$('d_bootScene');if(bs)bs.innerHTML=scenes.map(s=>`<option value="${s.slot}">${s.slot+1}: ${s.used?esc(s.name):'(leer)'}</option>`).join('');if(cfg)bs.value=cfg.dmx.bootScene}
async function saveScene(i){const n=prompt('Name der Szene:',scenes[i].used?scenes[i].name:'Szene '+(i+1));if(n===null)return;
 await api('/api/scenes',{act:'save',slot:i,name:n.slice(0,23)});toast('Gespeichert');loadScenes()}
let scT,scLong=false;
$('scnl').addEventListener('pointerdown',e=>{const b=e.target.closest('button');if(!b)return;scLong=false;scT=setTimeout(async()=>{scLong=true;const i=+b.dataset.s;
 if(scenes[i].used&&confirm('Szene „'+scenes[i].name+'“ löschen? (Abbrechen = überschreiben)')){await api('/api/scenes',{act:'del',slot:i});loadScenes()}else saveScene(i)},700)});
['pointerup','pointerleave'].forEach(ev=>$('scnl').addEventListener(ev,()=>clearTimeout(scT)));
$('scnl').onclick=async e=>{const b=e.target.closest('button');if(!b||scLong)return;const i=+b.dataset.s;
 if(!scenes[i].used)return saveScene(i);await api('/api/scenes',{act:'load',slot:i});toast('„'+scenes[i].name+'“ geladen');lastTouch=0;poll()};

// ---------- Geräte
$('newprof').innerHTML=Object.keys(PROF).map(k=>`<option>${k}</option>`).join('');
async function loadCfg(){cfg=await api('/api/config');renderFix();fillSet();loadScenes()}
function renderFix(){const fx=cfg.fixtures;
 $('fixl').innerHTML=fx.map((f,i)=>`<div class="fix" data-i="${i}">
  <div class="hd"><input type="text" value="${esc(f.n)}" data-k="n" maxlength="23" class="grow">
  <button class="b" data-a="id" title="Identifizieren">&#128161;</button><button class="b" data-a="up">&#9650;</button><button class="b" data-a="del">&#10005;</button></div>
  <div class="grid2 f"><div><label>Startadresse</label><input type="number" min="1" max="512" value="${f.a}" data-k="a"></div>
  <div><label>Kanäle (Modus)</label><input type="number" min="1" max="${cfg.maxCh}" value="${f.ch.length}" data-k="nch"></div></div>
  <div class="mut">Belegt DMX ${f.a}–${f.a+f.ch.length-1}</div>
  <label class="mut"><input type="checkbox" data-k="aw" ${f.aw?'checked':''}> Auto-Weiß (Weißanteil aus RGB auf W-Kanal)</label>
  ${f.ch.map((c,k)=>`<div class="chl"><span class="mut">CH${k+1}</span><select data-c="${k}">${FN.map((n,j)=>`<option value="${j}" ${j==c[0]?'selected':''}>${n}</option>`).join('')}</select>
   <input type="number" min="0" max="255" value="${c[1]}" data-v="${k}" title="Fest-/Ruhewert"></div>`).join('')}
 </div>`).join('')||'<p class="mut">Noch keine Scheinwerfer angelegt.</p>';
 checkConf()}
function checkConf(){const u=new Array(513).fill(-1),msg=[];cfg.fixtures.forEach((f,i)=>{for(let k=0;k<f.ch.length;k++){const a=f.a+k;
 if(a>512){msg.push(f.n+' reicht über Kanal 512 hinaus');break}if(u[a]>=0){msg.push(`Kanal ${a}: „${f.n}“ überschneidet sich mit „${cfg.fixtures[u[a]].n}“`);break}u[a]=i}});
 $('conf').innerHTML=msg.map(esc).join('<br>')}
$('fixl').oninput=e=>{const d=e.target.closest('.fix');if(!d)return;const f=cfg.fixtures[+d.dataset.i],t=e.target;
 if(t.dataset.k=='n')f.n=t.value;else if(t.dataset.k=='a'){f.a=Math.max(1,Math.min(512,+t.value||1));d.querySelector('.mut').textContent=`Belegt DMX ${f.a}–${f.a+f.ch.length-1}`;checkConf()}
 else if(t.dataset.k=='aw')f.aw=t.checked;
 else if(t.dataset.c!==undefined)f.ch[+t.dataset.c][0]=+t.value;else if(t.dataset.v!==undefined)f.ch[+t.dataset.v][1]=Math.max(0,Math.min(255,+t.value||0))};
$('fixl').onchange=e=>{const d=e.target.closest('.fix');if(!d||e.target.dataset.k!='nch')return;const f=cfg.fixtures[+d.dataset.i];
 const n=Math.max(1,Math.min(cfg.maxCh,+e.target.value||1));while(f.ch.length<n)f.ch.push([0,0]);f.ch.length=n;renderFix()};
$('fixl').onclick=e=>{const b=e.target.closest('button');if(!b)return;const i=+b.closest('.fix').dataset.i,a=b.dataset.a,fx=cfg.fixtures;
 if(a=='del'&&confirm('„'+fx[i].n+'“ entfernen?'))fx.splice(i,1);else if(a=='up'&&i>0)[fx[i-1],fx[i]]=[fx[i],fx[i-1]];else if(a=='id'){send({identify:i});return}renderFix()};
$('addfix').onclick=()=>{if(cfg.fixtures.length>=cfg.maxFix)return toast('Maximal '+cfg.maxFix+' Geräte');const p=PROF[$('newprof').value];
 const end=cfg.fixtures.reduce((m,f)=>Math.max(m,f.a+f.ch.length),1);
 cfg.fixtures.push({n:'Scheinwerfer '+(cfg.fixtures.length+1),a:Math.min(512,end),aw:false,ch:p.map(f=>[f,0])});renderFix()};
$('autoaddr').onclick=()=>{let a=1;cfg.fixtures.forEach(f=>{f.a=a;a+=f.ch.length});renderFix();toast('Adressen neu vergeben – Geräte entsprechend einstellen!')};
$('savefix').onclick=async()=>{await api('/api/config',{fixtures:cfg.fixtures});toast('Gespeichert');lastTouch=0;poll()};

// ---------- Kanal-Test & Monitor
const tsend=v=>api('/api/chan',{ch:+$('tch').value,n:+$('tn').value,v:v});
$('tv').oninput=e=>{$('tvo').textContent=e.target.value;tsend(+e.target.value)};
$('tfull').onclick=()=>{$('tv').value=255;$('tvo').textContent=255;tsend(255)};
$('tzero').onclick=()=>{$('tv').value=0;$('tvo').textContent=0;tsend(0)};
$('trel').onclick=()=>tsend(-1);
$('tclr').onclick=()=>api('/api/chan',{clear:true}).then(()=>toast('Alle Kanäle freigegeben'));
$('mon').innerHTML=Array.from({length:512},(_,i)=>`<div data-c="${i+1}"><i></i><span>${i+1}</span></div>`).join('');
$('mon').onclick=e=>{const d=e.target.closest('div[data-c]');if(d){$('tch').value=d.dataset.c;$('tn').value=1}};
const cells=[...document.querySelectorAll('#mon div')];
setInterval(async()=>{if(!$('p-ch').classList.contains('on'))return;try{const r=await api('/api/dmx');
 cells.forEach((c,i)=>{const v=parseInt(r.d.substr(i*2,2),16);c.firstChild.style.width=(v/2.55)+'%';c.lastChild.textContent=v||(i+1);c.style.color=v?'#fff':'#555';c.classList.toggle('ov',r.o[i]=='1')})}catch(e){}},500);

// ---------- Einstellungen
function fillSet(){const w=cfg.wifi,d=cfg.dmx;['ssid','pass','host','ip','gw','mask','dns','apSsid','apPass'].forEach(k=>$('w_'+k).value=w[k]);$('w_static').checked=!!w.static;$('w_mode').value=w.mode;
 $('d_tx').value=d.tx;$('d_de').value=d.de;$('d_led').value=d.led;$('d_rate').value=d.rate;$('d_art').checked=!!d.art;$('d_uni').value=d.uni;$('d_boot').value=d.boot}
$('savewifi').onclick=async()=>{const w={};['ssid','pass','host','ip','gw','mask','dns','apSsid','apPass'].forEach(k=>w[k]=$('w_'+k).value);w.static=$('w_static').checked?1:0;w.mode=+$('w_mode').value;
 if(w.apPass&&w.apPass.length<8)return toast('AP-Passwort: mind. 8 Zeichen');
 if(w.mode==1&&!w.ssid)return toast('Bitte WLAN-Name eintragen');if(!confirm('Speichern und neu starten? Klappt das WLAN nicht, öffnet der ESP32 nach 20 s wieder die Direktverbindung (192.168.1.4).'))return;
 await api('/api/config',{wifi:w});await api('/api/reboot',{});toast('Neustart …')};
$('savedmx').onclick=async()=>{const r=await api('/api/config',{dmx:{tx:+$('d_tx').value,de:+$('d_de').value,led:+$('d_led').value,rate:+$('d_rate').value,art:$('d_art').checked?1:0,uni:+$('d_uni').value,boot:+$('d_boot').value,bootScene:+$('d_bootScene').value}});
 if(r.reboot&&confirm('Pins geändert – jetzt neu starten?'))await api('/api/reboot',{});else toast('Gespeichert')};
$('reboot').onclick=()=>confirm('Neu starten?')&&api('/api/reboot',{}).then(()=>toast('Neustart …'));
$('factory').onclick=()=>confirm('Alle Einstellungen und Szenen löschen?')&&api('/api/factory',{}).then(()=>toast('Zurückgesetzt, Neustart …'));
$('export').onclick=async()=>{const c=await api('/api/config');const a=document.createElement('a');a.href=URL.createObjectURL(new Blob([JSON.stringify(c,null,1)],{type:'application/json'}));a.download='dmx-backup.json';a.click()};
$('import').onchange=async e=>{const f=e.target.files[0];if(!f)return;try{const c=JSON.parse(await f.text());await api('/api/config',{fixtures:c.fixtures,dmx:c.dmx});toast('Backup eingespielt');loadCfg()}catch(x){toast('Datei ungültig')}};
$('scan').onclick=async()=>{$('scanl').innerHTML='<span class="mut">Suche … (Verbindung kann kurz stocken)</span>';await api('/api/scan?start=1');
 for(let i=0;i<20;i++){await new Promise(r=>setTimeout(r,1000));let r;try{r=await api('/api/scan')}catch(e){continue}if(r.running)continue;
  const n=r.nets.sort((a,b)=>b.rssi-a.rssi);$('scanl').innerHTML=n.length?n.map(x=>`<button class="b" data-s="${esc(x.ssid)}">${esc(x.ssid||'(versteckt)')} · ${x.rssi} dBm · Kanal ${x.ch}</button>`).join(''):'<span class="mut">Keine Netze gefunden</span>';return}};
$('scanl').onclick=e=>{const b=e.target.closest('button');if(b){$('w_ssid').value=b.dataset.s;toast('SSID übernommen – Passwort prüfen und speichern')}};
async function openWizard(){$('setup').style.display='block';try{const c=await api('/api/config');$('s_apSsid').value=c.wifi.apSsid;$('s_apPass').value=c.wifi.apPass;$('s_ssid').value=c.wifi.ssid;$('s_pass').value=c.wifi.pass;wm(c.wifi.mode)}catch(e){}}
function wm(m){document.querySelector(`input[name=wm][value="${m}"]`).checked=true;$('o0').classList.toggle('on',m==0);$('o1').classList.toggle('on',m==1);$('sw1').style.display=m==1?'':'none';$('s_aph').textContent=m==1?'Notfall-Direktverbindung':'Direktverbindung'}
document.querySelectorAll('input[name=wm]').forEach(r=>r.onchange=()=>wm(+r.value));
$('wizard').onclick=openWizard;
$('s_scan').onclick=async()=>{$('s_scanl').innerHTML='<span class="mut">Suche …</span>';await api('/api/scan?start=1');
 for(let i=0;i<20;i++){await new Promise(r=>setTimeout(r,1000));let r;try{r=await api('/api/scan')}catch(e){continue}if(r.running)continue;
  const n=r.nets.filter(x=>x.ssid).sort((a,b)=>b.rssi-a.rssi);$('s_scanl').innerHTML=n.length?n.map(x=>`<button class="b" data-s="${esc(x.ssid)}">${esc(x.ssid)} · ${x.rssi>-60?'stark':x.rssi>-75?'mittel':'schwach'}</button>`).join(''):'<span class="mut">Keine Netze gefunden</span>';return}};
$('s_scanl').onclick=e=>{const b=e.target.closest('button');if(b){$('s_ssid').value=b.dataset.s;$('s_pass').focus()}};
$('s_save').onclick=async()=>{const m=+document.querySelector('input[name=wm]:checked').value,w={mode:m,apSsid:$('s_apSsid').value.trim(),apPass:$('s_apPass').value};
 if(!w.apSsid)return toast('Bitte Namen für das ESP32-WLAN eingeben');if(w.apPass.length<8)return toast('Passwort: mindestens 8 Zeichen');
 if(m==1){w.ssid=$('s_ssid').value.trim();w.pass=$('s_pass').value;if(!w.ssid)return toast('Bitte WLAN-Name eingeben')}
 await api('/api/config',{wifi:w});await api('/api/reboot',{});
 $('s_save').disabled=true;$('s_msg').innerHTML=m==0?`Gespeichert, der ESP32 startet neu. Verbinde dich in ca. 10 Sekunden mit dem WLAN <b>${esc(w.apSsid)}</b> (Passwort wie eben eingegeben) und öffne <b>http://192.168.1.4</b>.`
  :`Gespeichert, der ESP32 startet neu und verbindet sich mit <b>${esc(w.ssid)}</b>. Wechsle mit dem Handy in dieses WLAN und öffne <b>http://dmx.local</b> oder die IP aus dem Router. Klappt die Anmeldung nicht, ist er nach ca. 20 Sekunden wieder als <b>${esc(w.apSsid)}</b> unter http://192.168.1.4 erreichbar.`};
$('doupd').onclick=async()=>{const f=$('fw').files[0];if(!f)return toast('Bitte .bin wählen');const fd=new FormData();fd.append('fw',f);toast('Lade hoch …');
 const r=await fetch('/update',{method:'POST',body:fd});toast(await r.text())};
</script>
</body>
</html>
)rawliteral";
