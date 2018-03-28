//#include "ESP8266WiFi.h"
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

const char* ssid = "Virgin Wi-Fi true";
const char* password = "181181181181";

const char *mqtt_server = "192.168.1.163";
const int mqtt_port = 1883;

byte codeOn[4] = {0xA0, 0x01, 0x01, 0xA2};
byte codeOff[4] = {0xA0, 0x01, 0x00, 0xA1};

const char* serverIndex = "LAMP<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
//ESP8266WebServer server(80);


WiFiClient espClient;

void callback(char* topic, byte* payload, unsigned int length);
PubSubClient client(mqtt_server, mqtt_port, callback, espClient);


/*void setupWebUpdater(void) {
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
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
}
*/
void setup_wifi() {

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  randomSeed(micros());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if((char)payload[0] == '0')
    Serial.write(codeOff, 4);
  else
    Serial.write(codeOn, 4);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  setup_wifi();

 // setupWebUpdater();

  client.setCallback(callback);

}
void mqttLoop(){
  while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      client.subscribe("room/lamp");
    } else {
      delay(5000);
    }
  }
  client.loop();
}

void loop() {
  
//  server.handleClient();
  
  mqttLoop();
  
  delay(100);
}






