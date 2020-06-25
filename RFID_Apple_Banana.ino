//Neopixel setup

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      47

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400);

/**************************************************************************/

//Ghost Shadow Light setup
#include <math.h>
double Thermistor(int RawADC) {
  double Temp;
  Temp = log(10000.0 * ((1024.0 / RawADC - 1)));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp = Temp - 273.15;
  Temp = (Temp * 9.0) / 5.0 + 32.0;
  return Temp;
}

/**************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:

// Use this line for a breakout with a software SPI connection (recommended):
//Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
//Adafruit_PN532 nfc(PN532_SS);

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
// also change #define in Adafruit_PN532.cpp library file
#define Serial SerialUSB
#endif

int pinOut = 10;

void setup(void) {
#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif
  Serial.begin(115200);
  Serial.println("Hello!");

  pinMode(pinOut, OUTPUT);

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


  //******************************Neopixel*******************************************

  //strip.begin(); // This initializes the NeoPixel library
  //strip.show();
  pixels.begin();
  strip.setBrightness(100);
  pixels.setBrightness(100);
  pixels.show();

  //******************************Neopixel*******************************************
  
  digitalWrite(pinOut, HIGH);   // turn the LED on (HIGH is the voltage level)


  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A Card ...");
}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  int val;
  double temp;
  val = analogRead(0);
  temp = Thermistor(val);
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println(" F");

//-------------------------------------------------------Apple------------------------------------------------------------
  if (success && uid[0] == 0x95) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ...
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      digitalWrite(pinOut, LOW);

      delay(10);                       // wait for a second

      //******************************Neopixel*******************************************

      redred();

      //******************************Neopixel*******************************************

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];

        success = nfc.mifareclassic_ReadDataBlock(4, data);

        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");

          // Wait a bit before reading the card again
          delay(10);
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
          delay(10);
          darkenred();

          /*originally turn on function, but here it is used for turning off. 
          I think negative and positive were reveresed on either side - NO/NC? NO/COM? NC/COM? VCC/GND? */
          if (temp >= 150) {
            digitalWrite(pinOut, LOW);
          }
          else {
            digitalWrite(pinOut, HIGH);   // turn the LED on (HIGH is the voltage level)
            delay(10);                       // wait for a second
          }

          strip.show();
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
        delay(10);
        darkenred();

        //originally turn on function, but here it is used for turning off. I don't know what happened.
        if (temp >= 150) {
          digitalWrite(pinOut, LOW);
        }
        else {
          digitalWrite(pinOut, HIGH);   // turn the LED on (HIGH is the voltage level)
          delay(10);                       // wait for a second
        }

        strip.show();
      }
    }
//----------------------------------------------------Banana------------------------------------------------------------
  }else if(success && uid[0] == 0xD5){
     // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ...
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      digitalWrite(pinOut, LOW);

      delay(10);                       // wait for a second

      //******************************Neopixel*******************************************

      yellowyellow();

      //******************************Neopixel*******************************************

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];

        success = nfc.mifareclassic_ReadDataBlock(4, data);

        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");

          // Wait a bit before reading the card again
          delay(10);
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
          delay(10);
          darkenyellow();

          /*originally turn on function, but here it is used for turning off. 
          I think negative and positive were reveresed on either side - NO/NC? NO/COM? NC/COM? VCC/GND? */
          if (temp >= 150) {
            digitalWrite(pinOut, LOW);
          }
          else {
            digitalWrite(pinOut, HIGH);   // turn the LED on (HIGH is the voltage level)
            delay(10);                       // wait for a second
          }

          strip.show();
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
        delay(10);
        darkenyellow();

        //originally turn on function, but here it is used for turning off. I don't know what happened.
        if (temp >= 150) {
          digitalWrite(pinOut, LOW);
        }
        else {
          digitalWrite(pinOut, HIGH);   // turn the LED on (HIGH is the voltage level)
          delay(10);                       // wait for a second
        }

        strip.show();
      }
    }
  }
}

// 255 to 0
void darkenred() {
  Serial.begin(115200);
  uint16_t i, j;

  for (j = 255; j > 0; j--) {
    for (i = 0; i < strip.numPixels(); i++) {
      pixels.setPixelColor(i, pixels.Color(j,0,0));
    }
    pixels.show();
    delay(0.1);
    Serial.println(j);
  }
}

void darkenyellow() {
  Serial.begin(115200);
  uint16_t i, j;

  for (j = 255; j > 0; j--) {
    for (i = 0; i < strip.numPixels(); i++) {
      pixels.setPixelColor(i, pixels.Color(j,j,0));
    }
    pixels.show();
    delay(0.1);
    Serial.println(j);
  }
}

void redred(){
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(255,0,0));
      pixels.show();
      delay(20);
    } 
}


void yellowyellow(){
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(255,255,0));
      pixels.show();
      delay(20);
    } 
}


