#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <vector>
#include "LooMotion.h"
#include "PushButton.h"
#include "SPIFFS.h"
#include "GoogleHome.h"



/** The code herein written by the Author is released under the terms of the unlicense. https://unlicense.org/
 * @author https://github.com/lexfp 
 */
//time server
const char* ntpServer = "pool.ntp.org";

/** push button for testing/development only - not required*/
//const int pushButtonPin = 15; 
const int pushButtonPin = 0; //use 0 for boot button (no extra wiring)
PushButton pushButton;
GoogleHome googleHome;

String thisURL = "http://";


// Replace with your network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_NETWORK_PASSWORD";

char googleHomeName[] = "Living Room speaker";

String alertMP3[4] = { "/bathroomalert.mp3", 
                  "/bathroombandit.mp3",
                  "/bathroomintruder.mp3", 
                  "/mommycleanmypoop.mp3" };

LooMotion looMotion;
const int motionSensorPin = 27; //pin 27 for motion sensor
bool needToNotify = 0;

unsigned long lastTrigger = 0;
boolean startTimer = false;
#define MOTION_TIMEOUT_SECONDS 60

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Aisha2loo</title>
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.5rem;
    color: #143642;
  }
  body {
    margin: 0;
  }
  </style>
<title>Aisha 2 loo</title>
</head>
<body>
  <div class="topnav">
    <h1>Aisha 2 loo</h1>
  </div>
  <div class="content">
  <h1>
  <p><pre>
  ^..^      /
  /_/\_____/
     /\   /\
    /  \ /  \
  </pre> </p></h1>
      <h3><p class="motion">Motion detected: <span id="motion"></span></p></h3>
      <p class="timesp">Last Detection: <ul id="times"></ul> </p>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; 
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
    var message = JSON.parse(event.data);
    if (message.motion == 1){
      state = "Yes";
      beep();
    }
    else{
      state = "No";
    }
    document.getElementById('motion').innerHTML = state;
    
    var $times = document.getElementById('times');
    times.innerHTML = '';

    for(i in message.times) {
      var d = new Date(0); 
      d.setUTCSeconds(message.times[i]);  
      var listItem = document.createElement('li');
      listItem.innerHTML = d.toLocaleString()+' ('+timeSince(d)+' ago)';
      times.appendChild(listItem);
    }
  }
  function onLoad(event) {
    initWebSocket();
  }
  function beep() {
    var snd = new Audio("data:audio/wav;base64,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qJrx4KOQjahZxWbcZgztj2c49nKmkId44S71j0c8eV9yDK6uPRzx5X18eDvjvQ6yKo9ZSS6l//8elePK/Lf//IInrOF/FvDoADYAGBMGb7FtErm5MXMlmPAJQVgWta7Zx2go+8xJ0UiCb8LHHdftWyLJE0QIAIsI+UbXu67dZMjmgDGCGl1H+vpF4NSDckSIkk7Vd+sxEhBQMRU8j/12UIRhzSaUdQ+rQU5kGeFxm+hb1oh6pWWmv3uvmReDl0UnvtapVaIzo1jZbf/pD6ElLqSX+rUmOQNpJFa/r+sa4e/pBlAABoAAAAA3CUgShLdGIxsY7AUABPRrgCABdDuQ5GC7DqPQCgbbJUAoRSUj+NIEig0YfyWUho1VBBBA//uQZB4ABZx5zfMakeAAAAmwAAAAF5F3P0w9GtAAACfAAAAAwLhMDmAYWMgVEG1U0FIGCBgXBXAtfMH10000EEEEEECUBYln03TTTdNBDZopopYvrTTdNa325mImNg3TTPV9q3pmY0xoO6bv3r00y+IDGid/9aaaZTGMuj9mpu9Mpio1dXrr5HERTZSmqU36A3CumzN/9Robv/Xx4v9ijkSRSNLQhAWumap82WRSBUqXStV/YcS+XVLnSS+WLDroqArFkMEsAS+eWmrUzrO0oEmE40RlMZ5+ODIkAyKAGUwZ3mVKmcamcJnMW26MRPgUw6j+LkhyHGVGYjSUUKNpuJUQoOIAyDvEyG8S5yfK6dhZc0Tx1KI/gviKL6qvvFs1+bWtaz58uUNnryq6kt5RzOCkPWlVqVX2a/EEBUdU1KrXLf40GoiiFXK///qpoiDXrOgqDR38JB0bw7SoL+ZB9o1RCkQjQ2CBYZKd/+VJxZRRZlqSkKiws0WFxUyCwsKiMy7hUVFhIaCrNQsKkTIsLivwKKigsj8XYlwt/WKi2N4d//uQRCSAAjURNIHpMZBGYiaQPSYyAAABLAAAAAAAACWAAAAApUF/Mg+0aohSIRobBAsMlO//Kk4soosy1JSFRYWaLC4qZBYWFRGZdwqKiwkNBVmoWFSJkWFxX4FFRQWR+LsS4W/rFRb/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////VEFHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAU291bmRib3kuZGUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMjAwNGh0dHA6Ly93d3cuc291bmRib3kuZGUAAAAAAAAAACU=");  
    snd.play(); 
  }
  function timeSince(date) {
    var seconds = Math.floor((new Date() - date) / 1000);
    var interval = seconds / 31536000;
    if (interval > 1) {
      return Math.floor(interval) + " years";
    }
    interval = seconds / 2592000;
    if (interval > 1) {
      return Math.floor(interval) + " months";
    }
    interval = seconds / 86400;
    if (interval > 1) {
      return Math.floor(interval) + " days";
    }
    interval = seconds / 3600;
    if (interval > 1) {
      return Math.floor(interval) + " hours";
    }
    interval = seconds / 60;
    if (interval > 1) {
      return Math.floor(interval) + " minutes";
    }
    return Math.floor(seconds) + " seconds";
  }
</script>
</body>
</html>
)rawliteral";

void notifyClients() {
  
  //Serial.print("Notifying all clients: ");
  //Serial.println(looMotion.toJSON());
  
  String r = String(random(sizeof(alertMP3)/sizeof(alertMP3[0])));
  String mp3URL = thisURL+r+".mp3";
  
  if (looMotion.getMotionState() == 1) {
    //motion was detected
    //googleHome.notifyTTS("Bathroom Intruder");
    //googleHome.notifyMP3("http://192.168.1.35/3.mp3");
    googleHome.notifyMP3(mp3URL.c_str());
  } else {
    //googleHome.notifyTTS("Bathroom now safe");
  }
  ws.textAll(looMotion.toJSON());
  needToNotify = 0;
}


/**
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "message") == 0) {
      //notifyClients();
    }
  }
}
*/

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      //handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


bool motionTimeout() {
  //return (startTimer && (millis() - lastTrigger > (MOTION_TIMEOUT_SECONDS*1000)));
  return (millis() - lastTrigger > (MOTION_TIMEOUT_SECONDS*1000));
}

// Checks if motion was detected and starts a timer
void IRAM_ATTR movementDetected() {
  bool timeout = motionTimeout();
  //Serial.print("Timeout: ");
  //Serial.println(timeout);
  //timeout will prevent any further motion detections to cause alerts until MOTION_TIMEOUT_SECONDS passes
  if (timeout)
  {
    Serial.println("MOTION DETECTED!!!");
    startTimer = true;
    lastTrigger = millis();
    looMotion.setMotionState(1);
    needToNotify = 1;
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  Serial.println("Aisha2loo starting...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.print("ESP32 url - ");
  thisURL += WiFi.localIP().toString().c_str();
  thisURL += "/";
  Serial.println(thisURL);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.on("/0.mp3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, alertMP3[0], "audio/mpeg");
  });
  server.on("/1.mp3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, alertMP3[1], "audio/mpeg");
  });
  server.on("/2.mp3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, alertMP3[2], "audio/mpeg");
  });
  server.on("/3.mp3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, alertMP3[3], "audio/mpeg");
  });

  // Start server
  server.begin();
  
  // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensorPin, INPUT_PULLUP);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensorPin), movementDetected, RISING);

  /** push button*/
  pushButton.init(pushButtonPin);

  /** google home*/
  googleHome.init(googleHomeName);


  //init time server
  configTime(0, 0, ntpServer);

  Serial.println("Aisha2loo started...");
}

void loop() {
  ws.cleanupClients();

  //for testing only
  if (pushButton.checkForClick() == 1) {
    movementDetected();
  }
  if (startTimer && (motionTimeout())) {
    looMotion.setMotionState(0);
    needToNotify = 1;
    startTimer = false;
    Serial.println("Motion stopped...");
  }
  if (needToNotify) 
  {
    notifyClients();
  }
}
