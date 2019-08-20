/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <stasradio.h>

#ifndef STASSID
#define STASSID "SAN"
#define STAPSK  "37212628"
#endif

#define LED_BUILTIN 2

const char* host = "esp8266radio";
const char* ssid = STASSID;
const char* password = STAPSK;





int ledState = HIGH;
ESP8266WebServer server(80);
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
const char* volindex = "<font size=40><a href='/'>MAIN</a> <a href='/vol+'>(+)</a> <a href='/vol-'>(-)</a> VOL=";

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT); 
  Serial.begin(9600);
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    //MDNS.begin(host);
    server.on("/", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", serverIndex);
    });
    server.on("/vol+", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      vol+=1;
      if (vol>volmax){vol=volmax;}
      setvol(vol);
      server.send(200, "text/html", volindex+String(vol)+"</font>");
    });
    server.on("/vol-", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      vol-=1;
      if (vol<0){vol=0;}
      setvol(vol);
      server.send(200, "text/html", volindex+String(vol)+"</font>");
    });
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

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.printf("Ready! Open http://%s.local in your browser\n", host);
    digitalWrite(LED_BUILTIN, 0);
  } else {
    Serial.println("WiFi Failed");
    digitalWrite(LED_BUILTIN, 1);
  }
}

void loop(void) {
  server.handleClient();
  //MDNS.update();
  /*unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    //if (ledState == LOW) {
    //  ledState = HIGH;  // Note that this switches the LED *off*
    //} else {
    //  ledState = LOW;  // Note that this switches the LED *on*
    // }
    ledState =!ledState;
    digitalWrite(LED_BUILTIN, ledState);
  }
*/
  /*if (v==255) {dir=-1;}
  if (v==0) {dir=1;}
  v+=dir;
  analogWrite(LED_BUILTIN,v);
  delay(2);
 */
	


}