#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))

#define SERIAL_BAUDRATE 38400
#define STX 255
#define SERIAL_RX_BUFFER_SIZE 17
#define INPUT_BUFFERSIZE 16

#include "Arduino.h"

void parseBuffer(char* in_buff, uint32_t* lon, uint32_t* lat, float* alt, uint8_t* rssi, uint8_t* remrssi, uint8_t* noise, uint8_t* remnoise);
float udbm(uint8_t val);

char input_buffer[INPUT_BUFFERSIZE];

uint32_t 	lon 		= 0;
uint32_t 	lat 		= 0;
float 		alt 		= 0;
uint8_t 	rssi 		= 0;
uint8_t 	remrssi 	= 0;
uint8_t 	noise 		= 0;
uint8_t 	remnoise 	= 0;

void setup()
{
	Serial.begin(SERIAL_BAUDRATE);
	pinMode(LED_BUILTIN, OUTPUT);

	delay(100);
	SET(PORTB, 13);
	delay(2000);
	CLR(PORTB, 13);
}


void loop()
{
	while(Serial.available() > 0)	// check your UART buffer
	{
		uint8_t c = Serial.read();
		if(c == STX)				// when we have start byte of the message
		{
			Serial.readBytes(input_buffer, INPUT_BUFFERSIZE); 	// read message into the buffer
			parseBuffer(input_buffer, &lon, &lat, &alt, &rssi, &remrssi, &noise, &remnoise); // parse buffer and put values into the global variables
			rssi = udbm(rssi);			// convert uint8_t values to dBm
			remrssi = udbm(remrssi);
			noise = udbm(noise);
			remnoise = udbm(remnoise);
		}
	}

	/*
	 * Here you can put your code to do something
	 */

	if (alt > 0)	// simply check, if you got a right value from the trackPro
	{
		digitalWrite(LED_BUILTIN, HIGH);	// and vizualize it
	}
	else
	{
		digitalWrite(LED_BUILTIN, LOW);
	}

	/*
	 * End of user code
	 */
}

/**
 * UNIX trackPro message parser
 */
void parseBuffer(char* in_buff, uint32_t* lon, uint32_t* lat, float* alt, uint8_t* rssi, uint8_t* remrssi, uint8_t* noise, uint8_t* remnoise)
{
	memcpy(lon, &in_buff[0], 4);
	memcpy(lat, &in_buff[4], 4);
	memcpy(alt, &in_buff[8], 4);
	memcpy(rssi, &in_buff[12], 1);
	memcpy(remrssi, &in_buff[13], 1);
	memcpy(noise, &in_buff[14], 1);
	memcpy(remnoise, &in_buff[15], 1);
}

/**
 * Converts 8-bit RADIO_STATUS value to dBm
 */
float udbm(uint8_t val)
{
	return ((val/(float)1.9) - 127);
}
