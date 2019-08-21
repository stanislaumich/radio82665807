/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <stasradio.h>
#include <EEPROM.h>
#include <ssid.h>
//работа....

#define LED_BUILTIN 2
#ifndef STASSID
#define STASSID "ZTE54"
#define STAPSK  "121211119"
#endif
const char* host = "esp8266radio";
const char* ssid = STASSID;
const char* password = STAPSK;

unsigned long currentMillis;
unsigned long previousMillis;
unsigned long interval=100;
bool enabled=false;

//ESP8266WiFiMulti WiFiMulti;

int ledState = HIGH;
ESP8266WebServer server(80);

void setup(void) {
  Serial.begin(74880);
  EEPROM.begin(512);
  vol = EEPROM.read(voladdr);
  int t1=EEPROM.read(frqaddr1);
  int t2=EEPROM.read(frqaddr2);
  frq=t1*256+t2;
  Serial.println(vol);
  Serial.println(t1);
  Serial.println(t2);
  Serial.println(frq);
  //EEPROM.write(addr, val);
  pinMode(LED_BUILTIN, OUTPUT); 
  
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);

  //WiFiMulti.addAP("ZTE54", "121211119");
  //WiFiMulti.addAP("Stas", "121211119");
  //WiFiMulti.addAP("SAN", "37212628");

  WiFi.begin(ssid, password);
  //while (tries>0)
  //{
  
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
  //if ((WiFiMulti.run() == WL_CONNECTED)) {
    //MDNS.begin(host);
    server.on("/", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", serverIndex+"VOL="+String(vol)+"   FRQ="+String(frq));
    });///////////////////////////////////////////////////////////////////////////////////
    server.on("/vol+", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      vol+=1;
      if (vol>volmax){vol=volmax;}
      setvol(vol);
      EEPROM.write(voladdr, vol);
      EEPROM.commit(); 
      server.send(200, "text/html", volindex+String(vol)+" FRQ="+String(frq)+"</font>");
    });
    server.on("/vol-", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      vol-=1;
      if (vol<0){vol=0;}
      setvol(vol);
      EEPROM.write(voladdr, vol);
      EEPROM.commit(); 
      server.send(200, "text/html", volindex+String(vol)+" FRQ="+String(frq)+"</font>");
    });///////////////////////////////////////////////////////////////////////////////////
    server.on("/frq+", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      frq+=frqstep;
      if (vol>frqmax){frq=frqmax;}
      setfrq(frq);
      int t2=frq % 256;
      int t1=(frq-t2)/256;
      EEPROM.write(frqaddr1, t1);
      EEPROM.write(frqaddr2, t2);
      Serial.println(t1);
      Serial.println(t2);
      Serial.println(frq);
      EEPROM.commit(); 
      server.send(200, "text/html", frqindex+String(frq)+" VOL="+String(vol)+"</font>");
    });
    server.on("/frq-", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      frq-=frqstep;
      if (frq<frqmin){frq=frqmin;}
      setfrq(frq);
      int t2=frq % 256;
      int t1=(frq-t2)/256;
      EEPROM.write(frqaddr1, t1);
      EEPROM.write(frqaddr2, t2);
      Serial.println(t1);
      Serial.println(t2);
      Serial.println(frq);
      EEPROM.commit(); 
      server.send(200, "text/html", frqindex+String(frq)+" VOL="+String(vol)+"</font>");
    });///////////////////////////////////////////////////////////////////////////////////
    server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        //WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace)) { //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });
    server.begin();
    //MDNS.addService("http", "tcp", 80);

    Serial.print("IP address: http://");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_BUILTIN, 0);
    interval=2000;
    enabled=true;
  } else {
    Serial.println("WiFi Failed - ");
    //Serial.println(tries);
    digitalWrite(LED_BUILTIN, 1);
    enabled=false;
    interval=5000;
  }
 //tries-=1;
 //S}
 

}

void loop(void) {
  server.handleClient();
  //MDNS.update();
  unsigned long currentMillis = millis();
  if(enabled){
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    ledState =!ledState;
    digitalWrite(LED_BUILTIN, ledState);
  }
  }
	


}