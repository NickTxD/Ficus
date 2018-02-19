#include "ESP8266WiFi.h"
#include <PubSubClient.h>

#include "ficus.h"
#include <Wire.h>

#define led_pin D0

const byte buflen = 10;
char buf[buflen];
sensor_code s;
sensors data;
int value, i;

const char* ssid = "Virgin Wi-Fi true";         // Название  WiFi сети
const char* password = "181181181181";          // Пароль WiFi сети

const char *mqtt_server = "192.168.1.8"; // Имя сервера MQTT
const int mqtt_port = 1883; // Порт для подключения к серверу MQTT

WiFiClient espClient;
PubSubClient client(espClient);
MqttManager mqtt(&Serial);

void blink_led(){
  digitalWrite(led_pin, LOW);
  delay(200);
  digitalWrite(led_pin, HIGH);
  }

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    blink_led();
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
  
  pinMode(led_pin, OUTPUT);

  setup_wifi();

  mqtt.init(&client, mqtt_server, mqtt_port);
  //client.setCallback(callback);
  
  value = 0;
  i = 0;
  s = undefined;
  memset(buf, 0, buflen); // опустошаем буфер

   digitalWrite(led_pin, LOW);
}

void loop() {

  mqtt.loop();
  
  int count = Wire.requestFrom(8, 32); /* request & read data of size 32 from slave */
  
  while(Wire.available()){
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
  Serial.println("");
  mqtt.send(&data);
  
  delay(1000);

}

