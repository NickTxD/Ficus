#include "structures.h"

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
		{ temperature = value; break; }
		case humidity:
		{ this->humidity = value; break; }
		case brightness:
		{ this->brightness = value; break; }
		case soil:
		{ this->soil = value; break; }
	}
}
