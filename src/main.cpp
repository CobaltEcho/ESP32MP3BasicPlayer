// Play MP3 files from SD Card

#include <Arduino.h>
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#define SPI_SPEED SD_SCK_MHZ(4)  // I dont think this is needed for ESP32
//  SD Card Pins
#define SCK 18  // GPIO 18
#define MISO 19 // GPIO 19
#define MOSI 23 // GPIO 23
#define CS 2    // GPIO 2

// For amp hookup for speaker:
// Audio Signal + : Analog Audio Out = Pin GPIO 25 / DAC1
// Audio Signal - : Ground

File dir;
AudioOutputI2S *output = NULL;
AudioFileSourceSD *source = NULL;
AudioGeneratorMP3 *decoder = NULL;
bool first = true;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(1000);

  audioLogger = &Serial;
  output = new AudioOutputI2S(0, 1); // Using Internal DAC1, Analog Audio on Pin 25
  source = new AudioFileSourceSD();
  decoder = new AudioGeneratorMP3();

#if defined(ESP8266)
  SD.begin(SS, SPI_SPEED);
  Serial.println("SD initialised on ESP8266.");
#else
  SD.begin(CS);
  Serial.println("SD initialised.");
#endif
  dir = SD.open("/");

  // if (!SD.begin(CS, SPI_SPEED)) I dont think SPI_SPEED is needed for ESP32.  Uncomment this line and comment the next for ESP8266.  I think.
  if (!SD.begin(CS))
  {
    Serial.println("Problem starting SD");
    return;
  }
  
}

void loop() {
  if ((decoder) && (decoder->isRunning())) {
    if (!decoder->loop()) decoder->stop();
  } else {
    File file = dir.openNextFile();
    if (file) {      
      if (String(file.name()).endsWith(".mp3")) {
        source->close();
        if (source->open(file.name())) { 
          Serial.printf_P(PSTR("Playing '%s' from SD card...\n"), file.name());
          decoder->begin(source, output);
        } else {
          Serial.printf_P(PSTR("Error opening '%s'\n"), file.name());
        }
      } 
    } else {
      Serial.println(F("Playback from SD card done\n"));
      delay(1000);
    }       
  }
}