/**************************************************************************/
/*!
    @file     readMifare.pde
    @author   Gene Blanchard
	@credits  Adafruit Industries
	@license  BSD (see license.txt)

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

*/
/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <MotorShield.h>


#define PN532_SCK  (13)
#define PN532_MISO (12)
#define PN532_MOSI (11)
#define PN532_SS   (10)
MS_DCMotor motor(MOTOR_A);

int green = 5;
int red = 6;
int buzzer = 7;

// Use a software SPI connection
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

void setup(void) {
	// Status LEDs
	pinMode(red, OUTPUT);
	pinMode(green, OUTPUT);
	pinMode(buzzer, OUTPUT);
	
	// Open Serial
	Serial.begin(115200);
	Serial.println("Hello!");
	
	// Motor
	// engage the motor's brake 
	motor.run(BRAKE);
	motor.setSpeed(255);

	nfc.begin();

	uint32_t versiondata = nfc.getFirmwareVersion();
	if (! versiondata) {
		Serial.print("Didn't find PN53x board");
	while (1); // halt
	}
	// Got ok data, print it out!
	Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
	Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
	Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

	// configure board to read RFID tags
	nfc.SAMConfig();

	Serial.println("Waiting for an Doge (ISO14443A Card) ...");
}


void loop(void) {
	// Status LED
	digitalWrite(red, HIGH);
	digitalWrite(green, LOW);
	uint8_t success;
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
	uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

	// Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
	// 'uid' will be populated with the UID, and uidLength will indicate
	// if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
	success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

	if (success) {
		// Status LED
		digitalWrite(red, LOW);
		digitalWrite(green, HIGH);
		
		// Display some basic information about the card
		Serial.println("Found an doge!");
		Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
		Serial.print("  UID Value: ");
		nfc.PrintHex(uid, uidLength);
		Serial.println("");
		
		// Buzz
		Serial.println("Buzzing");
		tone(buzzer, 1000); // Send 1KHz sound signal...
		delay(1000);        // ...for 1 sec
  		noTone(buzzer);     // Stop sound...
  		
  		// Blast for n secs
  		Serial.println("Blasting");
		motor.run(FORWARD|RELEASE);
		delay(1500);
		motor.run(BRAKE);
		
		// Reset LEDs
		digitalWrite(red, LOW);
		digitalWrite(green, LOW);
		
  		// Wait around to blast again
		Serial.println("Waiting to blast again in ...");
		for (int i=10; i >= 0; i--){
			Serial.println(i);
			digitalWrite(red, LOW);
			digitalWrite(green, HIGH);
			delay(500);
			digitalWrite(red, HIGH);
			digitalWrite(green, LOW);
			delay(500);
		}
	}
	else
	{
		// Status LED
		digitalWrite(red, HIGH);
		digitalWrite(green, LOW);
	}
}
