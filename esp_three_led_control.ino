#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ESP8266_LED_AP";
const char* password = "12345678";

ESP8266WebServer server(80);

// Updated default pins
int led1Pin = 16;   // D0
int led2Pin = 4;    // D2
int led3Pin = 12;   // D6

// HTML PAGE
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP8266 Multi LED Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    * {
      margin:0;
      padding:0;
      box-sizing:border-box;
    }
    
    body {
      font-family:'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background:linear-gradient(135deg, #02343F 0%, #01262E 100%);
      min-height:100vh;
      display:flex;
      align-items:center;
      justify-content:center;
      padding:20px;
    }
    
    .container {
      background:#DDDDDD;
      border-radius:35px;
      padding:45px 40px;
      max-width:520px;
      width:100%;
      box-shadow:0 25px 70px rgba(0,0,0,0.6);
      animation:fadeIn 0.7s ease;
    }
    
    @keyframes fadeIn {
      from {
        opacity:0;
        transform:translateY(40px);
      }
      to {
        opacity:1;
        transform:translateY(0);
      }
    }

    h1 {
      text-align:center;
      color:#02343F;
      margin-bottom:8px;
      font-size:32px;
      font-weight:700;
      letter-spacing:-0.5px;
    }
    
    .subtitle {
      text-align:center;
      color:#02343F;
      margin-bottom:35px;
      font-size:15px;
      opacity:0.75;
      font-weight:500;
    }

    .led-card {
      background:#ffffff;
      border-radius:22px;
      padding:24px;
      margin-bottom:22px;
      box-shadow:0 8px 25px rgba(0,0,0,0.15);
      border:1px solid rgba(2,52,63,0.08);
      transition:transform 0.3s ease, box-shadow 0.3s ease;
    }
    
    .led-card:hover {
      transform:translateY(-2px);
      box-shadow:0 12px 35px rgba(0,0,0,0.2);
    }

    .led-title {
      font-size:21px;
      font-weight:700;
      color:#02343F;
      margin-bottom:16px;
      text-align:center;
      letter-spacing:-0.3px;
    }

    select {
      width:100%;
      padding:14px 16px;
      border-radius:12px;
      border:2px solid #02343F;
      margin-bottom:18px;
      font-size:15px;
      background:#ffffff;
      color:#02343F;
      font-weight:500;
      cursor:pointer;
      transition:all 0.3s ease;
      appearance:none;
      background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='12' height='12' viewBox='0 0 12 12'%3E%3Cpath fill='%2302343F' d='M6 9L1 4h10z'/%3E%3C/svg%3E");
      background-repeat:no-repeat;
      background-position:right 12px center;
      padding-right:40px;
    }
    
    select:hover {
      border-color:#0288D1;
      background-color:#f8f9fa;
    }
    
    select:focus {
      outline:none;
      border-color:#0288D1;
      box-shadow:0 0 0 3px rgba(2,136,209,0.15);
    }

    .led-visual {
      width:90px;
      height:90px;
      border-radius:50%;
      margin:0 auto 20px;
      background:radial-gradient(circle, #e0e0e0 0%, #9e9e9e 100%);
      transition:all 0.4s ease;
      box-shadow:inset 0 4px 8px rgba(0,0,0,0.2), 0 4px 12px rgba(0,0,0,0.25);
      position:relative;
    }
    
    .led-visual::after {
      content:'';
      position:absolute;
      top:20%;
      left:20%;
      width:30%;
      height:30%;
      background:rgba(255,255,255,0.3);
      border-radius:50%;
    }

    .led-visual.on {
      background:radial-gradient(circle, #ffffff 0%, #f0f0f0 100%);
      box-shadow:inset 0 4px 8px rgba(0,0,0,0.1),
                  0 0 25px rgba(255,255,255,0.9),
                  0 0 45px rgba(255,255,255,0.6),
                  0 4px 15px rgba(0,0,0,0.2);
    }

    .button-group {
      display:flex;
      gap:14px;
      margin-top:12px;
    }

    button {
      flex:1;
      padding:16px 20px;
      border:none;
      border-radius:14px;
      font-size:16px;
      font-weight:600;
      color:#ffffff;
      cursor:pointer;
      transition:all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
      box-shadow:0 4px 12px rgba(0,0,0,0.2);
      position:relative;
      overflow:hidden;
    }
    
    button::before {
      content:'';
      position:absolute;
      top:50%;
      left:50%;
      width:0;
      height:0;
      border-radius:50%;
      background:rgba(255,255,255,0.3);
      transform:translate(-50%, -50%);
      transition:width 0.5s, height 0.5s;
    }
    
    button:active::before {
      width:300px;
      height:300px;
    }

    .on-btn {
      background:linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
    }
    
    .on-btn:hover {
      transform:translateY(-3px);
      box-shadow:0 6px 20px rgba(56,239,125,0.4);
    }
    
    .on-btn:active {
      transform:translateY(-1px);
    }
    
    .off-btn {
      background:linear-gradient(135deg, #ee0979 0%, #ff6a00 100%);
    }
    
    .off-btn:hover {
      transform:translateY(-3px);
      box-shadow:0 6px 20px rgba(238,9,121,0.4);
    }
    
    .off-btn:active {
      transform:translateY(-1px);
    }

    .footer {
      text-align:center;
      font-size:14px;
      color:#02343F;
      opacity:0.7;
      margin-top:25px;
      font-weight:500;
    }
    
    @media (max-width:480px) {
      .container {
        padding:35px 30px;
      }
      h1 {
        font-size:28px;
      }
      .led-visual {
        width:75px;
        height:75px;
      }
    }
  </style>
</head>
<body>
<div class="container">
  <h1>Multi LED Controller</h1>
  <p class="subtitle">ESP8266 Smart Control</p>

  <!-- LED 1 -->
  <div class="led-card">
    <p class="led-title">LED 1</p>
    <div class="led-visual" id="v1"></div>

    <select id="p1" onchange="setPin(1)">
      <option value="16" selected>D0 (GPIO16)</option>
      <option value="5">D1 (GPIO5)</option>
      <option value="4">D2 (GPIO4)</option>
      <option value="0">D3 (GPIO0)</option>
      <option value="2">D4 (GPIO2)</option>
      <option value="14">D5 (GPIO14)</option>
      <option value="12">D6 (GPIO12)</option>
      <option value="13">D7 (GPIO13)</option>
      <option value="15">D8 (GPIO15)</option>
    </select>

    <div class="button-group">
      <button class="on-btn" onclick="toggleLED(1,'on')">Turn ON</button>
      <button class="off-btn" onclick="toggleLED(1,'off')">Turn OFF</button>
    </div>
  </div>

  <!-- LED 2 -->
  <div class="led-card">
    <p class="led-title">LED 2</p>
    <div class="led-visual" id="v2"></div>

    <select id="p2" onchange="setPin(2)">
      <option value="16">D0 (GPIO16)</option>
      <option value="5">D1 (GPIO5)</option>
      <option value="4" selected>D2 (GPIO4)</option>
      <option value="0">D3 (GPIO0)</option>
      <option value="2">D4 (GPIO2)</option>
      <option value="14">D5 (GPIO14)</option>
      <option value="12">D6 (GPIO12)</option>
      <option value="13">D7 (GPIO13)</option>
      <option value="15">D8 (GPIO15)</option>
    </select>

    <div class="button-group">
      <button class="on-btn" onclick="toggleLED(2,'on')">Turn ON</button>
      <button class="off-btn" onclick="toggleLED(2,'off')">Turn OFF</button>
    </div>
  </div>

  <!-- LED 3 -->
  <div class="led-card">
    <p class="led-title">LED 3</p>
    <div class="led-visual" id="v3"></div>

    <select id="p3" onchange="setPin(3)">
      <option value="16">D0 (GPIO16)</option>
      <option value="5">D1 (GPIO5)</option>
      <option value="4">D2 (GPIO4)</option>
      <option value="0">D3 (GPIO0)</option>
      <option value="2">D4 (GPIO2)</option>
      <option value="14">D5 (GPIO14)</option>
      <option value="12" selected>D6 (GPIO12)</option>
      <option value="13">D7 (GPIO13)</option>
      <option value="15">D8 (GPIO15)</option>
    </select>

    <div class="button-group">
      <button class="on-btn" onclick="toggleLED(3,'on')">Turn ON</button>
      <button class="off-btn" onclick="toggleLED(3,'off')">Turn OFF</button>
    </div>
  </div>

  <div class="footer">Powered by BlinkNBuild</div>

</div>

<script>
function toggleLED(id, state) {
  fetch(`/led?id=${id}&state=${state}`)
    .then(() => {
      let v = document.getElementById('v' + id);
      if (state === 'on') v.classList.add('on');
      else v.classList.remove('on');
    });
}

function setPin(id) {
  let pin = document.getElementById('p' + id).value;
  fetch(`/setpin?id=${id}&pin=${pin}`);
}
</script>

</body>
</html>
)rawliteral";

// ROUTES
void handleRoot() {
  server.send_P(200, "text/html", htmlPage);
}

void handleSetPin() {
  int id = server.arg("id").toInt();
  int pin = server.arg("pin").toInt();

  if (id == 1) led1Pin = pin;
  if (id == 2) led2Pin = pin;
  if (id == 3) led3Pin = pin;

  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  server.send(200, "text/plain", "PIN UPDATED");
}

void handleLED() {
  int id = server.arg("id").toInt();
  String state = server.arg("state");

  int pin = (id == 1) ? led1Pin : (id == 2 ? led2Pin : led3Pin);

  if (state == "on") digitalWrite(pin, HIGH);
  else digitalWrite(pin, LOW);

  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  pinMode(led1Pin, OUTPUT); digitalWrite(led1Pin, LOW);
  pinMode(led2Pin, OUTPUT); digitalWrite(led2Pin, LOW);
  pinMode(led3Pin, OUTPUT); digitalWrite(led3Pin, LOW);

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/led", handleLED);
  server.on("/setpin", handleSetPin);

  server.begin();

  Serial.println("WiFi AP Started!");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  server.handleClient();
}