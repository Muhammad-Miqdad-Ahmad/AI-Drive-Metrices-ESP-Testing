#include <config.h>

// ─── HTML: CAPTIVE PORTAL ────────────────────────────────────────────────────
const char PORTAL_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>DriveSense Setup</title>
<style>
  *{box-sizing:border-box;margin:0;padding:0}
  :root{
    --bg:#080c10;
    --panel:#0e1620;
    --border:#1c2e42;
    --amber:#f59e0b;
    --amber-dim:#a16207;
    --amber-glow:rgba(245,158,11,.15);
    --text:#e2e8f0;
    --muted:#64748b;
    --red:#ef4444;
    --green:#22c55e;
  }
  body{
    min-height:100vh;
    background:var(--bg);
    display:flex;
    align-items:center;
    justify-content:center;
    font-family:'Courier New',Courier,monospace;
    color:var(--text);
    padding:20px;
    overflow:hidden;
  }

  /* Subtle grid overlay */
  body::before{
    content:'';
    position:fixed;inset:0;
    background-image:
      linear-gradient(rgba(245,158,11,.03) 1px,transparent 1px),
      linear-gradient(90deg,rgba(245,158,11,.03) 1px,transparent 1px);
    background-size:40px 40px;
    pointer-events:none;
  }

  .card{
    width:100%;max-width:420px;
    background:var(--panel);
    border:1px solid var(--border);
    border-radius:4px;
    padding:40px 36px;
    position:relative;
    box-shadow:0 0 60px rgba(245,158,11,.08), 0 20px 60px rgba(0,0,0,.5);
  }

  /* Amber corner accents */
  .card::before,.card::after{
    content:'';position:absolute;
    width:16px;height:16px;
    border-color:var(--amber);
    border-style:solid;
  }
  .card::before{top:-1px;left:-1px;border-width:2px 0 0 2px;}
  .card::after{bottom:-1px;right:-1px;border-width:0 2px 2px 0;}

  .logo{
    display:flex;align-items:center;gap:12px;
    margin-bottom:8px;
  }
  .logo-icon{
    width:36px;height:36px;
    border:2px solid var(--amber);
    border-radius:2px;
    display:flex;align-items:center;justify-content:center;
    color:var(--amber);
    font-size:18px;
    flex-shrink:0;
    box-shadow:0 0 12px var(--amber-glow);
  }
  .logo-text{
    font-size:20px;font-weight:700;
    letter-spacing:.12em;
    color:var(--amber);
    text-shadow:0 0 20px rgba(245,158,11,.4);
  }
  .subtitle{
    font-size:11px;letter-spacing:.2em;
    color:var(--muted);text-transform:uppercase;
    margin-bottom:36px;
    padding-left:48px;
  }

  .field{margin-bottom:20px}
  .field label{
    display:block;
    font-size:10px;letter-spacing:.2em;text-transform:uppercase;
    color:var(--muted);
    margin-bottom:8px;
  }
  .field input{
    width:100%;
    background:#060a0e;
    border:1px solid var(--border);
    border-radius:2px;
    padding:12px 14px;
    color:var(--text);
    font-family:inherit;
    font-size:14px;
    outline:none;
    transition:border-color .2s, box-shadow .2s;
  }
  .field input:focus{
    border-color:var(--amber);
    box-shadow:0 0 0 3px var(--amber-glow);
  }

  .scan-btn{
    width:100%;
    background:none;
    border:1px solid var(--border);
    color:var(--muted);
    font-family:inherit;
    font-size:10px;letter-spacing:.15em;text-transform:uppercase;
    padding:8px;
    cursor:pointer;
    border-radius:2px;
    margin-bottom:24px;
    transition:border-color .2s,color .2s;
  }
  .scan-btn:hover{border-color:var(--amber-dim);color:var(--amber);}

  #scan-results{
    background:#060a0e;
    border:1px solid var(--border);
    border-radius:2px;
    max-height:140px;overflow-y:auto;
    margin-top:-16px;margin-bottom:20px;
    display:none;
  }
  #scan-results.visible{display:block}
  .ssid-item{
    padding:9px 14px;
    font-size:13px;
    cursor:pointer;
    border-bottom:1px solid var(--border);
    display:flex;align-items:center;justify-content:space-between;
    transition:background .15s;
  }
  .ssid-item:last-child{border-bottom:none}
  .ssid-item:hover{background:var(--amber-glow)}
  .signal{font-size:10px;color:var(--muted)}

  .connect-btn{
    width:100%;
    background:var(--amber);
    border:none;
    border-radius:2px;
    padding:14px;
    color:#000;
    font-family:inherit;
    font-size:12px;letter-spacing:.2em;text-transform:uppercase;
    font-weight:700;
    cursor:pointer;
    transition:opacity .2s,box-shadow .2s;
    box-shadow:0 0 20px rgba(245,158,11,.2);
  }
  .connect-btn:hover{opacity:.9;box-shadow:0 0 30px rgba(245,158,11,.4);}
  .connect-btn:active{opacity:.75}
  .connect-btn:disabled{opacity:.4;cursor:not-allowed}

  .status{
    margin-top:20px;
    font-size:11px;letter-spacing:.1em;
    text-align:center;
    min-height:16px;
    display:flex;align-items:center;justify-content:center;gap:8px;
  }
  .dot{
    width:6px;height:6px;
    border-radius:50%;
    background:var(--muted);
    animation:none;
  }
  .dot.pulsing{
    background:var(--amber);
    animation:pulse 1s ease-in-out infinite;
    box-shadow:0 0 8px var(--amber);
  }
  .dot.ok{background:var(--green)}
  .dot.err{background:var(--red)}
  @keyframes pulse{0%,100%{opacity:1}50%{opacity:.3}}

  .divider{
    border:none;border-top:1px solid var(--border);
    margin:28px 0;
  }
  .footer{
    font-size:9px;letter-spacing:.15em;text-transform:uppercase;
    color:var(--muted);text-align:center;
    opacity:.5;
  }
</style>
</head>
<body>
<div class="card">
  <div class="logo">
    <div class="logo-icon">&#9651;</div>
    <span class="logo-text">DRIVESENSE</span>
  </div>
  <div class="subtitle">Network Configuration</div>

  <button class="scan-btn" onclick="scanNetworks()">&#8635; Scan for Networks</button>
  <div id="scan-results"></div>

  <div class="field">
    <label>Network SSID</label>
    <input type="text" id="ssid" placeholder="Enter WiFi name" autocomplete="off" autocorrect="off" autocapitalize="off" spellcheck="false">
  </div>
  <div class="field">
    <label>Password</label>
    <input type="password" id="pass" placeholder="Enter password">
  </div>

  <button class="connect-btn" id="btn" onclick="connect()">CONNECT</button>

  <div class="status">
    <div class="dot" id="dot"></div>
    <span id="status-text">Waiting for input</span>
  </div>

  <hr class="divider">
  <div class="footer">ESP32 WROOM &nbsp;|&nbsp; DriveSense FYP</div>
</div>

<script>
function setStatus(msg, state) {
  var d = document.getElementById('dot');
  var s = document.getElementById('status-text');
  s.textContent = msg;
  d.className = 'dot ' + (state || '');
}

function scanNetworks() {
  setStatus('Scanning...', 'pulsing');
  fetch('/scan').then(r => r.json()).then(data => {
    var box = document.getElementById('scan-results');
    box.innerHTML = '';
    if (!data.length) {
      box.innerHTML = '<div class="ssid-item" style="color:var(--muted)">No networks found</div>';
    } else {
      data.forEach(function(n) {
        var el = document.createElement('div');
        el.className = 'ssid-item';
        var bars = n.rssi > -60 ? '▂▄▆█' : n.rssi > -75 ? '▂▄▆·' : n.rssi > -85 ? '▂▄··' : '▂···';
        el.innerHTML = '<span>' + n.ssid + '</span><span class="signal">' + bars + '</span>';
        el.onclick = function() {
          document.getElementById('ssid').value = n.ssid;
          box.classList.remove('visible');
          document.getElementById('pass').focus();
        };
        box.appendChild(el);
      });
    }
    box.classList.add('visible');
    setStatus('Select a network', '');
  }).catch(function() { setStatus('Scan failed', 'err'); });
}

function connect() {
  var ssid = document.getElementById('ssid').value.trim();
  var pass = document.getElementById('pass').value;
  if (!ssid) { setStatus('SSID cannot be empty', 'err'); return; }

  var btn = document.getElementById('btn');
  btn.disabled = true;
  setStatus('Connecting…', 'pulsing');

  fetch('/save', {
    method:'POST',
    headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:'ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass)
  }).then(r => r.json()).then(function(d) {
    if (d.ok) {
      setStatus('Credentials saved — rebooting', 'ok');
    } else {
      setStatus('Save failed, try again', 'err');
      btn.disabled = false;
    }
  }).catch(function() {
    setStatus('Request failed', 'err');
    btn.disabled = false;
  });
}
</script>
</body>
</html>
)rawhtml";