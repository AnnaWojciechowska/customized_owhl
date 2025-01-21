/*	settimeSerial.ino
	changed by Anna Wojciechowska <anna.wojciechowska@gmail.com
	Script based on Luke Miller 2015 https://github.com/millerlp/OWHL

	To use this program, upload it to your board, then open the
	serial monitor at 57600 baud.
	To set time - send a 'newline' when you hit return (see the
	menu in the lower right of the Arduino serial monitor window).

	If needed adjust utcTimeOffset = -1 to adjust from you local time to UTC
*/
#include <Wire.h>
#include <SPI.h> // not used here, but needed to prevent a RTClib compile error
#include "RTClib.h"

// AVR-based Arduinos (Uno, MEGA etc) use the Wire bus for I2C
// with the pins located near the Aref pin.
// SAM-based Arduinos (Due) refer to the bus attached to those
// same pins near Aref as "Wire1", so this little block of code
// automagically swaps Wire or Wire1 in as necessary in the
// main code below.
// #ifdef __AVR__
//	#define Wire Wire	// For AVR-based Arduinos
// #elif defined(ARDUINO_SAM_DUE)
//	#define Wire Wire1	// for Arduino DUE, Wire1 I2C bus
// #endif

// Setup an instance of DS1307 naming it rtc
// You can leave this unchanged when using a DS3231, since they are
// both updated using the same I2C commands and addresses
RTC_DS1307 rtc;

// Declare variables to hold user input
uint16_t myyear;
uint8_t mymonth;
uint8_t myday;
uint8_t myhour;
uint8_t myminute;
uint8_t mysec;
DateTime myTime;
DateTime currTime;
long millisVal;
char utcTime[9];

void setUTCtime()
{
	String timeStr = __TIME__; // Format: "HH:MM:SS"
	int hours = timeStr.substring(0, 2).toInt();
	int minutes = timeStr.substring(3, 5).toInt();
	int seconds = timeStr.substring(6, 8).toInt();
	int utcTimeOffset = -1;
	hours = (hours + utcTimeOffset + 24) % 24;
	snprintf(utcTime, sizeof(utcTime), "%02d:%02d:%02d", hours, minutes, seconds);
}

void setup()
{
	Serial.begin(57600); // adjust your serial monitor baud to 57600 to match
	while (!Serial)
	{
		; // wait for serial port to connect. Needed for Leonardo only
	}

	setUTCtime();
	Serial.println(F("Preset values:"));
	Serial.println(__DATE__);
	Serial.println(utcTime);
	// Serial.println(__TIME__);
	Serial.println(F("Displaying the RTC clock time"));

	if (!rtc.isrunning())
	{
		Serial.println("rtc is NOT running!");
	}

	// Wire.begin();
	rtc.begin(); // Calls Wire.begin() internally
	millisVal = millis();
}

void loop()
{
	if (millis() > millisVal + 1000)
	{
		millisVal = millis(); // update millisVal
		currTime = rtc.now(); // read current time from the rtc
		Serial.print(F("RTC time, timezone: UTC: "));

		char buf[] = "YYYY-MM-DD hh:mm:ss";			 // create a character array to hold the time as a string
		Serial.println(currTime.toString(buf, 100)); // print the time as a string
	}
	if (Serial.read() == '\n')
	{
		Serial.println(F("Setting time"));
		rtc.adjust(DateTime(__DATE__, utcTime));
	}
}
