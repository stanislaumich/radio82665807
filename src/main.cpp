/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "stasradio.h"
#include <EEPROM.h>
#include "ssid.h"
#include "radio.h"
#include "RDA5807M.h"
#include "Debounce.h"
//Работа....

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
unsigned long interval=200;
bool enabled=false;





Debounce bstop(pbstop);
Debounce bprev(pbprev);
Debounce bnext(pbnext);

RDA5807M radio;    // Create an instance of Class for RDA5807M Chip

int ledState = HIGH;
ESP8266WebServer server(80);
///////////////////////////////////////////////////////////////////////////////////////////////////////////  

void setvol(int vol)
{
 radio.setVolume(vol);
 //EEPROM.write(voladdr, vol);
}

void setfrq(int frq)
{
 radio.setBandFrequency(FIX_BAND, frq);
 //EEPROM.write(voladdr, vol);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////  
void setup(void) {
  Serial.begin(74880);
  Serial.println();
  Serial.println("Loading EEPROM...");
  EEPROM.begin(512);

  vol = EEPROM.read(voladdr);
  int t1=EEPROM.read(frqaddr1);
  int t2=EEPROM.read(frqaddr2);
  frq=t1*256+t2;
  Serial.println();
  Serial.print("VOL=");
  Serial.println(vol);
  Serial.print("t1=");
  Serial.println(vol);
  Serial.print("t2=");
  Serial.println(vol);
  Serial.print("FRQ=");
  Serial.println(vol);
  //EEPROM.write(addr, val);
  pinMode(LED_BUILTIN, OUTPUT); 
  

 // Initialize the Radio 
  radio.init();

  // Enable information to the Serial port
  radio.debugEnable();
///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Set all radio setting to the fixed values.
  radio.setBandFrequency(FIX_BAND, frq);
  radio.setVolume(vol);
  radio.setMono(false);
  radio.setMute(false);
  radio.setBassBoost(true);
  frqmin=radio.getMinFrequency();
  frqmax=radio.getMaxFrequency();
  frqstep=radio.getFrequencyStep();
  // radio.seekUp(bool toNextSender = true);   ///< Start a seek upwards from the current frequency.
  // radio.seekDown(bool toNextSender = true); 
///////////////////////////////////////////////////////////////////////////////////////////////////////////  
pinMode(pbstop, INPUT_PULLUP);
pinMode(pbprev, INPUT_PULLUP);
pinMode(pbnext, INPUT_PULLUP);
///////////////////////////////////////////////////////////////////////////////////////////////////////////  
  WiFi.mode(WIFI_AP_STA);

  WiFi.begin(ssid, password);
  
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    server.on("/", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", serverIndex+String(vol)+String("-")+String(frq));
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
      Serial.print("t1");Serial.println(t1);
      Serial.print("t2");Serial.println(t2);
      Serial.print("frq");Serial.println(frq);
      Serial.println("Wroted");
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
      Serial.print("t1");Serial.println(t1);
      Serial.print("t2");Serial.println(t2);
      Serial.print("frq");Serial.println(frq);
      Serial.println("Wroted");
      EEPROM.commit(); 
      server.send(200, "text/html", frqindex+String(frq)+" VOL="+String(vol)+"</font>");
    });///////////////////////////////////////////////////////////////////////////////////
    // String dir = server.arg("dir");
    server.on("/s", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      String frqs = server.arg("frq");
      frq=frqs.toInt();
      setfrq(frq);
      int t2=frq % 256;
      int t1=(frq-t2)/256;
      EEPROM.write(frqaddr1, t1);
      EEPROM.write(frqaddr2, t2);
      Serial.print("t1");Serial.println(t1);
      Serial.print("t2");Serial.println(t2);
      Serial.print("frq");Serial.println(frq);
      Serial.println("Wroted");
      EEPROM.commit(); 
      server.send(200, "text/html", frqindex+String(frq)+" VOL="+String(vol)+"</font>");
    });///////////////////////////////////////////////////////////////////////////////////
    server.on("/seekup", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      radio.seekUp(true);   ///< Start a seek upwards from the current frequency.
      delay(300);
      frq=radio.getFrequency();
      int t2=frq % 256;
      int t1=(frq-t2)/256;
      EEPROM.write(frqaddr1, t1);
      EEPROM.write(frqaddr2, t2);
      Serial.print("t1");Serial.println(t1);
      Serial.print("t2");Serial.println(t2);
      Serial.print("frq");Serial.println(frq);
      Serial.println("Wroted");
      EEPROM.commit(); 
      server.send(200, "text/html", serverIndex+String(frq)+" VOL="+String(vol)+"</font>");
    });
    server.on("/seekdown", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      radio.seekDown(true);
      delay(300);
      frq=radio.getFrequency();
      int t2=frq % 256;
      int t1=(frq-t2)/256;
      EEPROM.write(frqaddr1, t1);
      EEPROM.write(frqaddr2, t2);
      Serial.print("t1");Serial.println(t1);
      Serial.print("t2");Serial.println(t2);
      Serial.print("frq");Serial.println(frq);
      Serial.println("Wroted");
      EEPROM.commit(); 
      server.send(200, "text/html", serverIndex+String(frq)+" VOL="+String(vol)+"</font>");
    });///////////////////////////////////////////////////////////////////////////////////
    server.on("/fm", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      radio.setBandFrequency(RADIO_BAND_FM, radio.getMinFrequency());
      radio.setVolume(vol);
      radio.setMono(false);
      radio.setMute(false);
      radio.setBassBoost(true);
      frqmin=radio.getMinFrequency();
      frqmax=radio.getMaxFrequency();
      frqstep=radio.getFrequencyStep();
      radio.setBandFrequency(RADIO_BAND_FM, frqmin);
      frq=frqmin;
      server.send(200, "text/html", serverIndex+String(frq)+" VOL="+String(vol)+"</font>");
    });
    server.on("/am", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      radio.setBandFrequency(RADIO_BAND_AM, radio.getMinFrequency());
      radio.setVolume(vol);
      radio.setMono(false);
      radio.setMute(false);
      radio.setBassBoost(true);
      frqmin=radio.getMinFrequency();
      frqmax=radio.getMaxFrequency();
      frqstep=radio.getFrequencyStep();
      radio.setBandFrequency(RADIO_BAND_AM, frqmin);
      frq=frqmin;
      server.send(200, "text/html", serverIndex+String(frq)+" VOL="+String(vol)+"</font>");
    });///////////////////////////////////////////////////////////////////////////////////
    server.on("/m", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      radio.setMute(!radio.getMute());
      server.send(200, "text/html", helpindex);
    });///////////////////////////////////////////////////////////////////////////////////

    server.on("/help", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      
      server.send(200, "text/html", helpindex);
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

    Serial.print("IP address: http://");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_BUILTIN, 0);
    interval=3000;
    enabled=true;
  } else {
    Serial.println("WiFi Failed :( ");

    digitalWrite(LED_BUILTIN, 1);
    enabled=false;
    interval=5000;
  }

}

void loop(void) {
  server.handleClient();

  //bstop.read()?radio.setMute(!radio.getMute()):nop();
  //bnext.read()?radio.seekUp(true):nop();
  //bprev.read()?radio.seekDown(true):nop();

  unsigned long currentMillis = millis();
  if(enabled){
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState =!ledState;
    digitalWrite(LED_BUILTIN, ledState);

    char s[12];
    radio.formatFrequency(s, sizeof(s));
    Serial.print("Station:"); 
    Serial.println(s);
  
    Serial.print("Radio:"); 
    radio.debugRadioInfo();
  
    Serial.print("Audio:"); 
    radio.debugAudioInfo();
    Serial.println(WiFi.localIP());
   }
  }
}