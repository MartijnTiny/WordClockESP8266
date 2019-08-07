#include <Arduino.h>

#include "definitions.h"

#define BUILTIN_LED 16
// ***************************************************************************
// Load libraries for: WebServer / WiFiManager / WebSockets https://github.com/toblum/McLighting
// ***************************************************************************
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

// needed for library WiFiManager
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <ESP8266HTTPUpdateServer.h>

#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#include <WebSockets.h>           //https://github.com/Links2004/arduinoWebSockets
#include <WebSocketsServer.h>
#include <Hash.h>

#include <WiFiUdp.h>

#include <Time.h>
#include <TimeLib.h>
#include <Timezone.h> 
#include "NTPTime.h"


// ***************************************************************************
// Instanciate HTTP(80) / WebSockets(81) Server
// ***************************************************************************
ESP8266WebServer server ( 80 );
WebSocketsServer webSocket = WebSocketsServer(81);

ESP8266HTTPUpdateServer httpUpdater;

time_t prevDisplay = 0; // when the digital clock was displayed
unsigned long prevRainbow = 0; // when the digital clock was displayed
 
// ***************************************************************************
// Load libraries / Instanciate Neopixel
// ***************************************************************************
//#include <Adafruit_NeoPixel.h>
//#include <NeoPixelBus.h>
#include <NeoPixelBrightnessBus.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);

///********
const uint16_t PixelCount = 127; // make sure to set this to the number of pixels in your strip
NeoGamma<NeoGammaTableMethod> colorGamma;

//NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount);
NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount);


uint8_t strip_ptr[PixelCount];
uint8_t stackptr = 0;

int testHours = 0;
int testMinutes = 0;


//**********

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// ***************************************************************************
// Load library "ticker" for blinking status led
// ***************************************************************************
#include <Ticker.h>
Ticker ticker;

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}


// ***************************************************************************
// Callback for WiFiManager library when config mode is entered
// ***************************************************************************
//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  DBG_OUTPUT_PORT.println("Entered config mode");
  DBG_OUTPUT_PORT.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  DBG_OUTPUT_PORT.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}



// ***************************************************************************
// Include: Webserver
// ***************************************************************************
#include "spiffs_webserver.h"

// ***************************************************************************
// Include: Request handlers
// ***************************************************************************
#include "request_handlers.h"

// ***************************************************************************
// Include: Color modes
// ***************************************************************************

RgbColor defaultColor =  RgbColor(200,200,0);
const long oneSecondDelay = 1000;
long waitUntilFastTest = 0;


#include "colormodes.h"

// ***************************************************************************
// MAIN
// ***************************************************************************
void setup() {
  DBG_OUTPUT_PORT.begin(115200);

  // set builtin led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);
  pinMode(TRIGGER_PIN, INPUT);
  //pinMode(16, INPUT);


   if ( digitalRead(TRIGGER_PIN) == HIGH ) {
    DBG_OUTPUT_PORT.println("starting ap");
    WiFiManager NewWiFiManager;
        if (!NewWiFiManager.startConfigPortal("LauriesClock")) {
      DBG_OUTPUT_PORT.println("failed to connect and hit timeout");
      
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
   }
  // ***************************************************************************
  // Setup: WiFiManager
  // ***************************************************************************
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("LauriesClock")) {


    DBG_OUTPUT_PORT.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  DBG_OUTPUT_PORT.println("connected...yeey :)");
  ticker.detach();
  //keep LED on
  digitalWrite(BUILTIN_LED, LOW);
  
  httpUpdater.setup(&server); //start ota updater

  // ***************************************************************************
  // Setup: Neopixel
  // ***************************************************************************
  strip.Begin();
  strip.SetBrightness(brightness);
  strip.Show(); // Initialize all pixels to 'off'


  // ***************************************************************************
  // Setup: MDNS responder
  // ***************************************************************************
  MDNS.begin(HOSTNAME);
  DBG_OUTPUT_PORT.print("Open http://");
  DBG_OUTPUT_PORT.print(HOSTNAME);
  DBG_OUTPUT_PORT.println(".local/edit to see the file browser");


  // ***************************************************************************
  // Setup: WebSocket server
  // ***************************************************************************
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);


  // ***************************************************************************
  // Setup: SPIFFS
  // ***************************************************************************
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n\r", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    DBG_OUTPUT_PORT.printf("\n\r");
  }

  // ***************************************************************************
  // Setup: NTP Time check
  // ***************************************************************************
  //CheckTime.attach(NTPPOLLTIME,NTPCall);
  setSyncProvider(getNtpTime);
  setSyncInterval(NTPPOLLTIME);
  // ***************************************************************************
  // Setup: SPIFFS Webserver handler
  // ***************************************************************************
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);
  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/esp_status", HTTP_GET, []() {
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ", \"analog\":" + String(analogRead(A0));
    json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });


  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      handleNotFound();
  });

  server.on("/upload", handleMinimalUpload);

  server.on("/restart", []() {
    DBG_OUTPUT_PORT.printf("/restart:\n");
    server.send(200, "text/plain", "restarting..." );
    ESP.restart();
  });

  server.on("/reset_wlan", []() {
    DBG_OUTPUT_PORT.printf("/reset_wlan:\n");
    server.send(200, "text/plain", "Resetting WLAN and restarting..." );
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    ESP.restart();
  });


  // ***************************************************************************
  // Setup: SPIFFS Webserver handler
  // ***************************************************************************
  server.on("/set_brightness", []() {
    if (server.arg("c").toInt() > 0) {
      brightness = (int) server.arg("c").toInt() * 2.55;
    } else {
      brightness = server.arg("p").toInt();
    }
    if (brightness > 255) {
      brightness = 255;
    }
    if (brightness < 0) {
      brightness = 0;
    }
    strip.SetBrightness(brightness);

    if (mode == HOLD) {
      mode = ALL;
    }

    getStatusJSON();
  });

  server.on("/get_brightness", []() {
    String str_brightness = String((int) (brightness / 2.55));
    server.send(200, "text/plain", str_brightness );
    DBG_OUTPUT_PORT.print("/get_brightness: ");
    DBG_OUTPUT_PORT.println(str_brightness);
  });

  server.on("/get_switch", []() {
    server.send(200, "text/plain", (mode == OFF) ? "0" : "1" );
    DBG_OUTPUT_PORT.printf("/get_switch: %s\n", (mode == OFF) ? "0" : "1");
  });

  server.on("/get_color", []() {
    String rgbcolor = String(main_color.red, HEX) + String(main_color.green, HEX) + String(main_color.blue, HEX);
    server.send(200, "text/plain", rgbcolor );
    DBG_OUTPUT_PORT.print("/get_color: ");
    DBG_OUTPUT_PORT.println(rgbcolor);
  });

  server.on("/status", []() {
    getStatusJSON();
  });

  server.on("/off", []() {
    exit_func = true;
    mode = OFF;
    getArgs();
    getStatusJSON();
  });
  server.on("/test", []() {
    exit_func = true;
    getArgs();
    handleRootN();

      // digital clock display of the time
  DBG_OUTPUT_PORT.print(hour());
  int digits = minute();
  DBG_OUTPUT_PORT.print(":");
  if (digits < 10)
    DBG_OUTPUT_PORT.print('0');
  DBG_OUTPUT_PORT.print(digits);
  digits = second();
  DBG_OUTPUT_PORT.print(":");
  if (digits < 10)
    DBG_OUTPUT_PORT.print('0');
  DBG_OUTPUT_PORT.print(digits);
  DBG_OUTPUT_PORT.print(" ");
  DBG_OUTPUT_PORT.print(day());
  DBG_OUTPUT_PORT.print(".");
  DBG_OUTPUT_PORT.print(month());
  DBG_OUTPUT_PORT.print(".");
  DBG_OUTPUT_PORT.print(year());
  DBG_OUTPUT_PORT.println();
    //getStatusJSON();
  });
  server.on("/all", []() {
    exit_func = true;
    mode = ALL;
    getArgs();
    getStatusJSON();
  });

  server.on("/wipe", []() {
    exit_func = true;
    mode = WIPE;
    getArgs();
    getStatusJSON();
  });

  server.on("/rainbow", []() {
    exit_func = true;
    mode = RAINBOW;
    getArgs();
    getStatusJSON();
  });

  server.on("/rainbowCycle", []() {
    exit_func = true;
    mode = RAINBOWCYCLE;
    getArgs();
    getStatusJSON();
  });

  server.on("/theaterchase", []() {
    exit_func = true;
    mode = THEATERCHASE;
    getArgs();
    getStatusJSON();
  });

  server.on("/theaterchaseRainbow", []() {
    exit_func = true;
    mode = THEATERCHASERAINBOW;
    getArgs();
    getStatusJSON();
  });

  /*server.on("/tv", []() {
    exit_func = true;
    mode = TV;
    getArgs();
    getStatusJSON();
  });*/

  server.begin();
}

void loop() {
  server.handleClient();
  webSocket.loop();

  // Simple statemachine that handles the different modes
  if (mode == OFF) {
    //colorWipe(strip.Color(0, 0, 0), 50);
    uint16_t i;
    for (i = 0; i < PixelCount; i++) {
      RgbColor color = RgbColor(0,0,0);
      strip.SetPixelColor(i, color);
    }
    strip.Show();
    //mode = HOLD;
  }
  if (mode == ALL) {
    uint16_t i;
    clearEdge();
    for (i = 2; i < PixelCount-4; i++) {
      RgbColor color = RgbColor(main_color.red, main_color.green, main_color.blue);
      strip.SetPixelColor(i, color);
    }
    strip.Show();
    //mode = HOLD;
  }
  if (mode == WIPE) {
    RgbColor color = RgbColor(main_color.red, main_color.green, main_color.blue);
    colorWipe(color, delay_ms);
  }
  if (mode == RAINBOW) {
    rainbow(delay_ms);
  }
  if (mode == RAINBOWCYCLE) {
    rainbowCycle(delay_ms);
  }
  if (mode == THEATERCHASE) {
    RgbColor color = RgbColor(main_color.red, main_color.green, main_color.blue);
    theaterChase(color, delay_ms);
  }
  if (mode == THEATERCHASERAINBOW) {
    theaterChaseRainbow(delay_ms);
  }
  if (mode == HOLD) {
    if (exit_func) {
      exit_func = false;
    }
  }
  if (mode == CLOCK) {
    Clock(delay_ms);
  }
  if (mode == TEST) {
    fastTest();
  }
}
