#include <Wire.h>
#include "DHT.h"
#define DHTPIN A0               // Pin to which DHT sensor is connected
#define SOILPIN A1               // Pin to which soil sensor is connected
#define DHTTYPE DHT11           // Used sensor type DHT 11
DHT dht(DHTPIN, DHTTYPE);       // Initialize the sensor

int errors;                     // Number of consecutive errors reading from the sensor

struct sensors_struct {
  int temperature;
  int humidity;
  int hic;
  //int brightness;
  int soil;
  char msg[20];
} sensors;


void setup() {
  Wire.begin(8);                 // join i2c bus with address 8
  Wire.onReceive(receiveEvent);  // register receive event
  Wire.onRequest(requestEvent);  // register request event
  Serial.begin(115200);          // start serial for debug

  dht.begin();                   // start dht sensor
  errors = 0;                    // reset the counter

  Serial.println("Start");


  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, interrupt, FALLING);
}

void(* reset) (void) = 0;
void interrupt() {
  Serial.println("Reboot!");
  reset();
}
void loop() {
  delay(2000);

  if (errors > 4)
    reset();

  getDataFromSensors();
}

void printDebug() {
  Serial.print("Humidity: ");
  Serial.print(sensors.humidity);
  Serial.print(" % ");
  Serial.print("Temperature: ");
  Serial.print(sensors.temperature);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(sensors.hic);
  Serial.print(" *C ");
  Serial.print("Soil: ");
  Serial.print(sensors.soil);
  Serial.println(" %");
  Serial.println(sensors.msg);
}
// function that reads data from sensors
void getDataFromSensors() {

  float humidity = dht.readHumidity();        //Read humidity
  float temperature = dht.readTemperature();  // Read temperature as Celsius
  float soil = analogRead(SOILPIN);           // Read soil humidity

  if (isnan(humidity) || isnan(temperature)) {       // Check if any reads failed and exit early (to try again).
    Serial.println("Failed to read from DHT sensor!");
    errors++; //increase the counter
    return;
  }
  errors = 0; //reset the counter

  float hic = dht.computeHeatIndex(temperature, humidity, false); // Compute heat index in Celsius (isFahreheit = false)

  //save data to struct
  sensors.temperature = temperature;
  sensors.humidity = humidity;
  sensors.hic = (int)hic;
  sensors.soil = (int)(-1 * (soil / 10.23 - 100));
  sprintf(sensors.msg, "t%dh%di%ds%d", sensors.temperature, sensors.humidity, sensors.hic, sensors.soil);

  printDebug();
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
  /*while (0 < Wire.available()) {
    char c = Wire.read();
    Serial.print(c);
    }
    Serial.println();    */
}
// function that executes whenever data is requested from master
void requestEvent() {
  Wire.write(sensors.msg);  /*send string on request */
}
