#include <advancedSerial.h>
#include "ESP8266WiFi.h"
#include <PubSubClient.h>

typedef enum sensor_code_e {
	temperature = 116,
	humidity = 104,
	brightness = 98,
	soil = 115,
  heat_index = 105,
	undefined = 0
} sensor_code;

struct sensors {
	int _temperature;
	int _humidity;
	int _brightness;
	int _soil;
  int _heat_index;

	void set(sensor_code, int);
};

class MqttManager
{
  public:
    MqttManager(HardwareSerial *serial);
    void init(PubSubClient* wifi, const char* server, const int port);
    void send(sensors *data);  
    void loop();
  private:
    HardwareSerial * _HardSerial;
    PubSubClient * _client;
    void reconnect();
    char message[50];
};

bool isDigit(char);
