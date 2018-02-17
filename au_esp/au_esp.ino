#include <Wire.h>
#include "structures.h"

const byte buflen = 10;
char buf[buflen];
sensor_code s;
sensors data;
int value, i;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(D1, D2);
  value = 0;
  i = 0;
  s = undefined;
  memset(buf, 0, buflen); // опустошаем буфер
}

void loop() {

  int count = Wire.requestFrom(8, 12); /* request & read data of size 12 from slave */
  
  while(Wire.available()){
    char c = Wire.read();
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
        default:
        { s = undefined; }
      }
    }
    else if (s != undefined) {
      buf[i]=c;
      i++;
    }
  }

  
// Serial.println();
delay(1000);

}

