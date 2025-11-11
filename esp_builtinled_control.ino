#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
const char* ssid = "ESP8266_LED_AP";
const char* password = "12345678";
ESP8266WebServer server(80);
const int ledPin = LED_BUILTIN;

// Enhanced HTML page with beautiful modern design
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP8266 LED Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    * {margin:0;padding:0;box-sizing:border-box;}
    body {
      font-family:'Segoe UI',Tahoma,Geneva,Verdana,sans-serif;
      background:linear-gradient(135deg,#02343F 0%,#01262E 100%);
      min-height:100vh;
      display:flex;
      align-items:center;
      justify-content:center;
      padding:20px;
    }
    .container {
      background:#F0EDCC;
      border-radius:30px;
      padding:50px 40px;
      box-shadow:0 20px 60px rgba(0,0,0,0.5);
      max-width:450px;
      width:100%;
      animation:fadeIn 0.6s ease;
    }
    @keyframes fadeIn {
      from {opacity:0;transform:translateY(30px);}
      to {opacity:1;transform:translateY(0);}
    }
    h1 {
      color:#02343F;
      margin-bottom:10px;
      font-size:32px;
      text-align:center;
      font-weight:700;
    }
    .subtitle {
      color:#02343F;
      text-align:center;
      margin-bottom:40px;
      font-size:16px;
      opacity:0.8;
    }
    .status {
      text-align:center;
      margin-bottom:30px;
      padding:15px;
      border-radius:15px;
      background:#02343F;
      font-size:18px;
      font-weight:600;
      color:#F0EDCC;
      transition:all 0.3s ease;
    }
    .status.active {
      background:#02343F;
      box-shadow:0 5px 15px rgba(2,52,63,0.6);
      transform:scale(1.05);
    }
    .button-group {
      display:flex;
      gap:15px;
      margin-bottom:30px;
    }
    button {
      flex:1;
      font-size:18px;
      padding:18px 20px;
      border:none;
      border-radius:15px;
      cursor:pointer;
      font-weight:600;
      transition:all 0.3s ease;
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
      background:rgba(255,255,255,0.5);
      transform:translate(-50%,-50%);
      transition:width 0.6s,height 0.6s;
    }
    button:active::before {
      width:300px;
      height:300px;
    }
    .btn-on {
      background:linear-gradient(135deg,#11998e 0%,#38ef7d 100%);
      color:white;
      box-shadow:0 4px 15px rgba(56,239,125,0.4);
    }
    .btn-on:hover {
      transform:translateY(-3px);
      box-shadow:0 8px 25px rgba(56,239,125,0.6);
    }
    .btn-off {
      background:linear-gradient(135deg,#ee0979 0%,#ff6a00 100%);
      color:white;
      box-shadow:0 4px 15px rgba(238,9,121,0.4);
    }
    .btn-off:hover {
      transform:translateY(-3px);
      box-shadow:0 8px 25px rgba(238,9,121,0.6);
    }
    .led-visual {
      width:100px;
      height:100px;
      border-radius:50%;
      margin:0 auto 20px;
      background:radial-gradient(circle,#ddd 0%,#999 100%);
      position:relative;
      transition:all 0.4s ease;
      box-shadow:0 0 20px rgba(0,0,0,0.2);
    }
    .led-visual.on {
      background:radial-gradient(circle,#4FC3F7 0%,#0288D1 100%);
      box-shadow:0 0 40px rgba(79,195,247,0.8),0 0 80px rgba(79,195,247,0.5);
      animation:pulse 2s infinite;
    }
    @keyframes pulse {
      0%,100% {box-shadow:0 0 40px rgba(79,195,247,0.8),0 0 80px rgba(79,195,247,0.5);}
      50% {box-shadow:0 0 60px rgba(79,195,247,1),0 0 120px rgba(79,195,247,0.7);}
    }
    .footer {
      text-align:center;
      color:#02343F;
      font-size:14px;
      margin-top:20px;
      opacity:0.7;
    }
    @media (max-width:480px) {
      .container {padding:30px 25px;}
      h1 {font-size:26px;}
      .button-group {flex-direction:column;}
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>LED Controller</h1>
    <p class="subtitle">ESP8266 Smart Control</p>
    
    <div class="led-visual" id="ledVisual"></div>
    
    <div class="status" id="status">LED is OFF</div>
    
    <div class="button-group">
      <button class="btn-on" onclick="toggleLED('on')">
        <span>Turn ON</span>
      </button>
      <button class="btn-off" onclick="toggleLED('off')">
        <span>Turn OFF</span>
      </button>
    </div>
    
    <div class="footer">
      Powered by BlinkNBuild
    </div>
  </div>
  
  <script>
    const ledVisual = document.getElementById('ledVisual');
    const status = document.getElementById('status');
    
    function toggleLED(state) {
      fetch('/led?state=' + state)
        .then(response => {
          if(response.ok) {
            updateUI(state);
          }
        })
        .catch(err => console.error('Error:', err));
    }
    
    function updateUI(state) {
      if(state === 'on') {
        ledVisual.classList.add('on');
        status.classList.add('active');
        status.textContent = 'LED is ON';
      } else {
        ledVisual.classList.remove('on');
        status.classList.remove('active');
        status.textContent = 'LED is OFF';
      }
    }
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", htmlPage);
}

void handleLED() {
  String state = server.arg("state");
  if (state == "on") {
    digitalWrite(ledPin, LOW);  // active LOW LED
  } else if (state == "off") {
    digitalWrite(ledPin, HIGH);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("Access Point: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/led", handleLED);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}