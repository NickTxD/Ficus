#include "ficus.h"

#define led_pin D0 

const byte buflen = 10;
char buf[buflen];
sensor_code s;
sensors data;
int value, i;
unsigned long timer;

const char* ssid = "Virgin Wi-Fi true";         // Название  WiFi сети
const char* password = "181181181181";          // Пароль WiFi сети

const char *mqtt_server = "192.168.1.163"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT

const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
ESP8266WebServer server(80);

WiFiClient espClient;

PubSubClient client(espClient);
MqttManager mqtt(&Serial);

void blink_led(){ 
  static int prev_time = millis();
  static bool state = false;

  if (millis() - prev_time > 200)
  {
    if (state) { digitalWrite(led_pin, LOW);  }
    else       { digitalWrite(led_pin, HIGH); }
    state = !state;
    prev_time = millis();
  }
  
}

void setupWebUpdater(void){
  server.on("/", HTTP_GET, [](){
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  server.on("/update", HTTP_POST, [](){
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
    ESP.restart();
  },[](){
      HTTPUpload& upload = server.upload();
      if(upload.status == UPLOAD_FILE_START){
        Serial.setDebugOutput(true);
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
       }else if(upload.status == UPLOAD_FILE_WRITE){
          if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
            Update.printError(Serial);
          }
        }else if(upload.status == UPLOAD_FILE_END){
          if(Update.end(true)){ //true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          }else{
            Update.printError(Serial);
          }
          Serial.setDebugOutput(false);
        }
      yield();
    });
  server.begin();
  Serial.printf("Ready!\n");
}

void setup_wifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    blink_led(); 
    delay(100);
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/* 
 * Когда пришло сообщение с MQTT брокера
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(D1, D2);

  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW);

  pinMode(led_pin, OUTPUT); 
  
  setup_wifi();

  setupWebUpdater();

  mqtt.init(&client, mqtt_server, mqtt_port);
  //client.setCallback(callback);
  
  value = 0;
  i = 0;
  s = undefined;
  memset(buf, 0, buflen); // опустошаем буфер

  digitalWrite(led_pin, HIGH); 
}

void loop() {

  server.handleClient();

  mqtt.loop();
  
  int count = Wire.requestFrom(8, 32); /* request & read data of size 32 from slave */
  
  while(Wire.available()){
    timer = millis();
    char c = Wire.read();
    if (c == 0) break;
    Serial.print(c);
    if (!isDigit(c))
    {
      if (s != (int)c && s != undefined)
      {
        i = 0;
        value = atoi(buf);
        data.set(s, value);
        memset(buf, 0, buflen); // опустошаем буфер
      }
      switch (c)
      {
        case temperature:
        { s = temperature; break; }
        case humidity:
        { s = humidity;    break; }
        case brightness:
        { s = brightness;  break; }
        case soil:
        { s = soil;        break; }
        case heat_index:
        { s = heat_index;  break; }
        default:
        { s = undefined; }
      }
    }
    else if (s != undefined) {
      buf[i]=c;
      i++;
    }
  }

  if (millis()-timer > 5000)
  {
    data._arduino = 0;
    Serial.println("Rebooting Arduino");
    digitalWrite(D3, HIGH);
    delay(3000);
    digitalWrite(D3, LOW);
  } else {
    data._arduino = 65280;  
  }

  digitalWrite(led_pin, LOW); 
  mqtt.send(&data);
  digitalWrite(led_pin, HIGH); 
  
  delay(1000);

}

