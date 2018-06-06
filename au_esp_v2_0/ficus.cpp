#include "ficus.h"

bool isDigit(char c) {
  if(c>='0'&&c<='9') {
    return true;
  } else {
  return false;
  }
}

void sensors::set(sensor_code s, int value) {
	switch(s)
	{
		case temperature:
		{ _temperature = value; break; }
		case humidity:
		{ _humidity = value; break; }
		case brightness:
		{ _brightness = value; break; }
		case soil:
		{ _soil = value; break; }
    case heat_index:
    { _heat_index = value; break; }
	}
}

MqttManager::MqttManager(HardwareSerial *serial)
{
  _HardSerial = serial;
}

void MqttManager::init(PubSubClient* client, const char* server, const int port)
{
  _HardSerial->println("MQTT: INIT...");
  _client = client;
  _client->setServer(server, port);  
}

void MqttManager::send(sensors *data)
{
  _HardSerial->println("MQTT: SENDING");
  sprintf(message, "%d", data->_temperature);
  _client->publish("room/temperature", message);  
  sprintf(message, "%d", data->_humidity);
  _client->publish("room/humidity", message);  
  sprintf(message, "%d", data->_soil);
  _client->publish("room/soil", message); 
  sprintf(message, "%d", data->_brightness); 
  _client->publish("room/brightness", message);  
  sprintf(message, "%d", data->_heat_index); 
  _client->publish("room/heat_index", message); 
  sprintf(message, "%d", data->_arduino); 
  _client->publish("room/arduino", message);

}

void MqttManager::reconnect()
{
  // Loop until we're reconnected
  while (!_client->connected()) {
    _HardSerial->print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (_client->connect(clientId.c_str())) {
      _HardSerial->println("connected");
    } else {
      _HardSerial->print("failed, rc=");
      _HardSerial->print(_client->state());
      _HardSerial->println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void MqttManager::loop()
{
  if (!_client->connected()) {
    reconnect();
  }
  _client->loop();  
}
