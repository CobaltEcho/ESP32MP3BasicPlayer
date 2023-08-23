
// For amp hookup for speaker:
// Audio Signal + : Analog Audio Out = Pin GPIO 25 / DAC1
// Audio Signal - : Ground


#include <Arduino.h>
#include "AudioFileSourceID3.h"
#include "AudioOutputI2SNoDAC.h"
#include "AudioGeneratorWAV.h"
#include "AudioFileSourceSD.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "Audio.h"

//  SPI / SD Card Pins
#define SCK 18  // GPIO 18
#define MISO 19 // GPIO 19
#define MOSI 23 // GPIO 23
#define CS 2    // GPIO 2

AudioFileSourceSD *source = NULL;
AudioGeneratorWAV *wav = NULL;
AudioOutputI2SNoDAC *out = NULL;
AudioFileSourceID3 *id3 = NULL;

void playwav(String path)
{
  source = new AudioFileSourceSD();
  source->open(path.c_str());
  id3 = new AudioFileSourceID3(source);
  //  id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");
  out = new AudioOutputI2SNoDAC();
  wav = new AudioGeneratorWAV();
  wav->begin(id3, out);
  while (wav->isRunning())
  {
    if (!wav->loop())
      wav->stop();
  }
  Serial.println("End of playback");
  source->close();
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(1000);  // waits for a second
  SD.begin(CS);
  if (!SD.begin(CS))
  {
    Serial.println("Card Mount Failed");
    return;
  }
  else
  {
    Serial.println("SD card is ready!");
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
  }

  File root = SD.open("/");
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }
  File file = root.openNextFile();
  while (file)
  {
    if (String(file.name()).endsWith(".wav"))
    {
      Serial.print("FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.print(file.size() / (1024 * 1024));
      Serial.println(" MB");
      playwav(file.name());
    }
    file.close();
    file = root.openNextFile();
  }
}

void loop(){}