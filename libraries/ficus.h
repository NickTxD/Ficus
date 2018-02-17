typedef enum sensor_code_e {
	temperature = 116,
	humidity = 104,
	brightness = 98,
	soil = 115,
	undefined = 0
} sensor_code;

struct sensors {
	int temperature;
	int humidity;
	int brightness;
	int soil;

	void set(sensor_code, int);
};

bool isDigit(char);
