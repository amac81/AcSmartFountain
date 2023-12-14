
// Import required libraries

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>  //https://github.com/me-no-dev/AsyncTCP using the latest dev version from @me-no-dev
#include <DNSServer.h>
#include <LittleFS.h>
#include <FastLED.h>

#define DEVICE_NAME      "ACSMARTFOUNTAIN" //Initial name of the Thing, to create an own Access Point.
// ESP32 supports up to 10 
#define MAX_CLIENTS 4

// Define the DNS interval in milliseconds between processing DNS requests
#define DNS_INTERVAL 30

/* -------------------------------------------------------------------- */
// How many NeoPixels we will be using, charge accordingly
#define NUM_LEDS 9

//The variation in yellow color to create the fire effect, define the interval where the color can change.
#define MIN_VARIATION 0
#define MAX_VARIATION 30

//Value must be between 0 & 1.
//If you never want a LED to be completly off, put 0.1 to min
#define MIN_INTENSITY 0.1
#define MAX_INTENSITY 1.0

//Speed for variations, higher is slower
#define NOISE_SPEED_COLOR 5
#define NOISE_SPEED_INTENSITY 3

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

double n;
double ni;
const byte RED = 255;

CRGB leds[NUM_LEDS];

/* -------------------------------------------------------------------- */

// Replace with your network credentials
const char* ssid = "AcSmartFountain";
const char* password = "";

const IPAddress localIP(192, 168, 4, 1);       // the IP address the web server, Samsung requires the IP to be in public space
const IPAddress gatewayIP(192, 168, 4, 1);       // IP address of the network should be the same as the local IP for captive portals
const IPAddress subnetMask(255, 255, 255, 0);  // no need to change: https://avinetworks.com/glossary/subnet-mask/

const String localIPURL = "http://192.168.4.1";  // a string version of the local IP with http, used for redirecting clients to your webpage

//motor slider controller
String pumpPowerValue = "0";

//RGB Led STRIP Hex color from webpage
String ledStripRGBColor = "0";

//RGB Led STRIP Brightness
String ledStripBrightness = "0";

//mist mode value control
String mistModeValue = "0";

const char* PARAM_INPUT = "value";

int rgbColor[3]; 

// Set RGB LED STRIP GPIO
#define DATA_PIN 2

// Set WATER PUMP CONTROL MODULE (HW-063) GPIOS
const int A1A = 27;
const int A1B = 26;
int pumpPower = 0;

// Set ULTRASONIC MIST GPIOS
// TODO


// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

DNSServer dnsServer;

void setUpDNSServer(DNSServer &dnsServer, const IPAddress &localIP) {
  // Set the TTL for DNS response and start the DNS server
  dnsServer.setTTL(3600);
  dnsServer.start(53, "*", localIP);
}

// Replaces placeholder with button section in your web page
String processor(const String& var){
  
  if (var == "PUMPPOWERSLIDERVALUE"){
    return pumpPowerValue;
  } 

  if (var == "MISTCONTROLSLIDERVALUE"){
    return mistModeValue;
  } 
  
  if (var == "LEDSTRIPRGBCOLOR"){
    return ledStripRGBColor;
  }

  if (var == "BRIGHTNESSSLIDERVALUE"){
    return ledStripBrightness;
  }

  return String();
}

void splitString (String str){
  String strs[3];
  int StringCount = 0;
  
  // Split the string into substrings
  while (str.length() > 0)
  {
    int index = str.indexOf('_');
    if (index == -1) // No space found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      strs[StringCount++] = str.substring(0, index);
      str = str.substring(index+1);
    }
  }

  rgbColor[0] = strs[0].toInt();
  rgbColor[1] = strs[1].toInt();
  rgbColor[2] = strs[2].toInt();
}

void updateLedStrip(){
    
    FastLED.setBrightness(ledStripBrightness.toInt());
    
    splitString(ledStripRGBColor);

    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(rgbColor[0], rgbColor[1], rgbColor[2]);
    }
    
    FastLED.show();
}

void readConfigFromSPIFFS (){
  
  //config.txt structure:
  //Rvalue;Gvalue;Bvalue;brightnessValue;pumpPowerValue;mistModeValue
  int configs[6] = {};
  int i = 0;

  File file = LittleFS.open("/config.txt", "r"); 
  if(!file){
      Serial.println("Failed to open file for reading");
      return;
  }
     
  while (file.available() && i<6) {
    configs[i] = file.parseInt();

    Serial.print("param ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(configs[i]);    
    Serial.println();

    i++;
  }  
  file.close();
  
  //apply configs

  rgbColor[0] = configs[0];
  rgbColor[1] = configs[1];
  rgbColor[2] = configs[2];
  
  FastLED.setBrightness(configs[3]);
  for (int i = 0; i < NUM_LEDS; i++) {
     leds[i] = CRGB(rgbColor[0], rgbColor[1], rgbColor[2]);
  }
  FastLED.show();

  pumpPower = configs[4];
  analogWrite(A1B,0);
  analogWrite(A1A, pumpPower); 

  char strAux[4] = {};
 
  pumpPowerValue = itoa(pumpPower, strAux, 10);
  mistModeValue = itoa(configs[5], strAux, 10);
  
  ledStripRGBColor = itoa(rgbColor[0], strAux, 10);
  ledStripRGBColor += "_";
  ledStripRGBColor += itoa(rgbColor[1], strAux, 10);
  ledStripRGBColor += "_";
  ledStripRGBColor += itoa(rgbColor[2], strAux, 10);
  ledStripBrightness = itoa(configs[3], strAux, 10);
}

void saveConfigs() {
  //config.txt structure:
  //Rvalue;Gvalue;Bvalue;brightnessValue;pumpPowerValue;mistModeValue
  
  File file = LittleFS.open("/config.txt", "w"); 
  if(!file){
      Serial.println("Failed to open file for write");
      return;
  }
     
  String fileLine = "";
  fileLine += rgbColor[0];
  fileLine += ";";
  fileLine += rgbColor[1];
  fileLine += ";";
  fileLine += rgbColor[2];
  fileLine += ";";
  fileLine += ledStripBrightness;
  fileLine += ";";
  fileLine += pumpPowerValue;
  fileLine += ";";
  fileLine += mistModeValue;

  if(file){
    file.println(fileLine);
  } 
  
  file.close();
  
}
 
void setup(){

   // To format all space in LITTLEFS - 1st use
   // LittleFS.format();
 
  
  // Begin LittleFS
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Serial port for debugging purposes
  Serial.begin(115200);
 
  
  delay(500); // power-up safety delay
  
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.clear();  // clear all pixel data
  FastLED.show();
  
  //motor controller
  pinMode(A1A, OUTPUT);
  pinMode(A1B, OUTPUT);
  
  // Set the WiFi mode to access point and station
  WiFi.mode(WIFI_MODE_AP);

  // Configure the soft access point with a specific IP and subnet mask
  WiFi.softAPConfig(localIP, gatewayIP, subnetMask);
  
  // Start the soft access point with the given ssid, password, channel, max number of clients
  WiFi.softAP(ssid);
    
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  setUpDNSServer(dnsServer, localIP);
  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

 // Route to load favicon.ico file
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/favicon.ico", "image/x-icon");
  });
  
  // Route to load style.css file
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/styles.css", "text/css");
  });

  // Route to load scripts.js file
  server.on("/scripts.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/scripts.js", "text/javascript");
  });

  // Send a GET request to <ESP_IP>/motorslider?value=<inputMessage>
  server.on("/pumppower", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/motorslider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      pumpPowerValue = inputMessage;

      pumpPower = pumpPowerValue.toInt();

      analogWrite(A1B,0);
      analogWrite(A1A, pumpPower);  

      saveConfigs();
      
    }
    else {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/ledstripcolor?value=<inputMessage>
  server.on("/ledstripcolor", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/ledstripcolor?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      ledStripRGBColor = inputMessage;
      
      updateLedStrip();
      saveConfigs();
    }
    else {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/brigthness?value=<inputMessage>
  server.on("/brigthness", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/brigthness?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      ledStripBrightness = inputMessage;
      updateLedStrip();
    }
    else {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/mistmode?value=<inputMessage>
  server.on("/mistmode", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/mistmode?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      mistModeValue = inputMessage;

      //

       // TODOOOOOOOOOOOOOOOOOO 
       // logic off mist control!!!
      
     //

     saveConfigs();

    }
    else {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK");
  });


  // Start server
  server.begin();

  readConfigFromSPIFFS();
}
 
void loop(){
  dnsServer.processNextRequest();  // I call this atleast every 10ms in my other projects (can be higher but I haven't tested it for stability)
  delay(DNS_INTERVAL);       // seems to help with stability, if you are doing other things in the loop this may not be needed
}
