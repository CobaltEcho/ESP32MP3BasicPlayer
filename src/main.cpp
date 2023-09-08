
/*
//Works without Using Amp - No volume control - no SD Card looping


#include <Arduino.h>
#include "AudioFileSourceID3.h"
#include "AudioOutputI2S.h"
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

// Audio out is pin GPIO 25


AudioFileSourceSD *source = NULL;
AudioFileSourceSD file[100]; // Maximum number of files in the directory
AudioGeneratorWAV *wav = NULL;
AudioOutputI2S *out = NULL;
AudioFileSourceID3 *id3 = NULL;

void playwav(String path)
{
  source = new AudioFileSourceSD();
  if (!source->open(path.c_str()))
  {
    Serial.println("Error opening audio file!");
    return;
  }

  id3 = new AudioFileSourceID3(source);
  out = new AudioOutputI2S(0,1);
  wav = new AudioGeneratorWAV();
  wav->begin(id3, out);

  if (!wav->isRunning())
  {
    Serial.println("Error starting WAV playback!");
    source->close();
    return;
  }

  Serial.println("Playing audio file: " + path);
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
  delay(1000); // waits for a second
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
      String audiofile = String("/" + String(file.name()));
      Serial.println("Playing: " + audiofile);
      Serial.print("FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.print(file.size() / (1024 * 1024));
      Serial.println(" MB");
      playwav(audiofile);
    }
    Serial.println("file closed: " + String(file.name()));
    file.close();
    file = root.openNextFile();
  }
}

void loop()
{
}

//--------------------------------------------------------------------------------------------------
*/

/*

//Works looping files - no volume control

#include <Arduino.h>
#include <AudioFileSourceID3.h>
#include <AudioOutputI2S.h>
#include <AudioGeneratorWAV.h>
#include <AudioFileSourceSD.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Audio.h>

// SPI / SD Card Pins
#define SCK 18    // GPIO 18
#define MISO 19   // GPIO 19
#define MOSI 23   // GPIO 23
#define CS 2      // GPIO 2

// Define the I2S pins
#define I2S_BCLK 26      // GPIO 26
#define I2S_LRCLK 25     // GPIO 25
#define I2S_DATA_OUT 22  // GPIO 22

AudioFileSourceSD *source = NULL;
AudioGeneratorWAV *wav = NULL;
AudioOutputI2S *out = NULL;
AudioFileSourceID3 *id3 = NULL;

void playwav(String path)
{
  source = new AudioFileSourceSD();
  if (!source->open(path.c_str()))
  {
    Serial.println("Error opening audio file: " + path);
    return;
  }

  id3 = new AudioFileSourceID3(source);
  out = new AudioOutputI2S();
  out->SetPinout(I2S_BCLK, I2S_LRCLK, I2S_DATA_OUT);
  wav = new AudioGeneratorWAV();
  wav->begin(id3, out);

  if (!wav->isRunning())
  {
    Serial.println("Error starting WAV playback: " + path);
    source->close();
    return;
  }

  Serial.println("Playing audio file: " + path);
  while (wav->isRunning())
  {
    if (!wav->loop())
    {
      wav->stop();
      Serial.println("End of playback: " + path);
    }
  }
  source->close();
}

void playWavFiles()
{
  File root = SD.open("/");
  if (!root)
  {
    Serial.println("Failed to open directory: /");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory: /");
    return;
  }

  while (true)
  {
    root.rewindDirectory(); // Start over at the beginning of the directory
    while (true)
    {
      File file = root.openNextFile();
      if (!file)
      {
        // No more files in the directory, break the loop
        break;
      }
      if (!file.isDirectory() && String(file.name()).endsWith(".wav"))
      {
        String fileName = String(file.name());
        Serial.println("Playing: " + fileName);
        playwav("/" + fileName);
        delay(1000); // Add a delay between playing files if needed
      }
      file.close();
    }
  }

  root.close();
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(1000); // waits for a second
  Serial.println("Initializing SD Card...");
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

  playWavFiles();
}

void loop()
{
  // Your main loop code, if any
}

*/

//----------------------------------------------------------------------------------------------

/*
// This code works with manual volume control, No Looping, No playing list, No volume adjusting

#include <Arduino.h>
#include <AudioFileSourceSD.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2S.h>
#include <SD.h> // Include the SD library

// Pin definitions for the I2S DAC
#define I2S_BCLK_PIN  26  // Bit clock pin for I2S
#define I2S_LRC_PIN   25  // Left/right channel clock pin for I2S
#define I2S_DOUT_PIN  22  // Data out pin for I2S

AudioGeneratorWAV *wav;
AudioOutputI2S *out;
AudioFileSourceSD *file;

// Adjust the volume here (0.0 to 1.0)
float volume = 0.5;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");

  // Initialize the SD card
  if (!SD.begin()) {
    Serial.println("SD Card initialization failed");
    return;
  }
  Serial.println("SD Card initialized.");

  // Initialize the I2S output
  out = new AudioOutputI2S();
  out->SetOutputModeMono(true); // Set to true for mono audio
  Serial.println("I2S output initialized.");

  // Set the gain (volume)
  out->SetGain(volume);

  // Specify the file path to your WAV file on the SD card
  const char *filePath = "/CountryRoadsWav2.wav";
  Serial.print("Playing WAV file: ");
  Serial.println(filePath);

  // Open the WAV file on the SD card
  file = new AudioFileSourceSD();
  if (!file->open(filePath)) {
    Serial.println("Could not open WAV file");
    delete out;
    return;
  }
  Serial.println("WAV file opened.");

  // Try to open the WAV file using AudioGeneratorWAV
  wav = new AudioGeneratorWAV();
  if (wav->begin(file, out)) {
    Serial.println("Playing WAV file...");
  } else {
    Serial.println("Could not start WAV file playback");
    delete wav;
    delete out;
    delete file;
    return;
  }
}

void loop() {
  // Check if the WAV file is still playing
  if (wav->isRunning()) {
    if (!wav->loop()) {
      // If the file has finished playing, close it and clean up
      wav->stop();
      delete wav;
      delete out;
      delete file;
      Serial.println("WAV file playback finished.");
      delay(1000); // Delay to prevent looping continuously
    }
  }
}

*/
//----------------------------------------------------------------------------------------------------

// -------------------------- Works looping files-------------------------------------

/*

#include <Arduino.h>
#include "AudioFileSourceSD.h"
#include "AudioOutputI2S.h"
#include "AudioGeneratorWAV.h"

#define SD_CardSelect 4 // Define the SD card select pin

File dir;
AudioFileSourceSD *source = NULL;
AudioOutputI2S *output = NULL;
AudioGeneratorWAV *decoder = NULL;
bool allFilesPlayed = false;

void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(1000);

  audioLogger = &Serial;
  source = new AudioFileSourceSD();
  output = new AudioOutputI2S(0, 1);
  decoder = new AudioGeneratorWAV();

  Serial.println("Initializing SD card...");
  if (SD.begin(SD_CardSelect)) { // Use the defined SD_CardSelect pin
    Serial.println("SD card initialization successful");
  } else {
    Serial.println("SD card initialization failed");
  }

  dir = SD.open("/");
}

void loop() {
  if (!allFilesPlayed) {
    if ((decoder) && (decoder->isRunning())) {
      if (!decoder->loop()) {
        decoder->stop();
        Serial.println("Playback finished");
      }
    } else {
      File file = dir.openNextFile();
      if (file) {
        if (String(file.name()).endsWith(".wav")) {
          source->close();
          String filePath = "/" + String(file.name());  // Add "/" in front of the file name
          if (source->open(filePath.c_str())) {
            Serial.printf("Playing '%s' from SD card...\n", filePath.c_str());
            decoder->begin(source, output);
          } else {
            Serial.printf("Error opening '%s'\n", filePath.c_str());
          }
        }
      } else {
        Serial.println("All files played. Replaying...");
        dir.rewindDirectory(); // Reset the directory to replay files
        allFilesPlayed = true;
      }
    }
  } else {
    dir.rewindDirectory(); // Reset the directory to replay files
    allFilesPlayed = false; // Reset the flag
  }
}
*/

//---------------------------------------------------------------------------------



/*

// --------------  Works with volume control without amp, loops files--------

#include <Arduino.h>
#include "AudioFileSourceSD.h"
#include "AudioOutputI2S.h"
#include "AudioGeneratorWAV.h"

#define SD_CardSelect 4      // Define the SD card select pin
#define POTENTIOMETER_PIN A0 // Analog pin where the potentiometer is connected, aka VP pin

File dir;
AudioFileSourceSD *source = NULL;
AudioOutputI2S *output = NULL;
AudioGeneratorWAV *decoder = NULL;
bool allFilesPlayed = false;
float currentVolume = 1.0; // Default volume to maximum (1.0)

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(1000);

  audioLogger = &Serial;
  source = new AudioFileSourceSD();
  output = new AudioOutputI2S(0, 1);
  decoder = new AudioGeneratorWAV();

  Serial.println("Initializing SD card...");
  if (SD.begin(SD_CardSelect))
  { // Use the defined SD_CardSelect pin
    Serial.println("SD card initialization successful");
  }
  else
  {
    Serial.println("SD card initialization failed");
  }

  dir = SD.open("/");
  output->SetGain(0.5);
}

void loop()
{
  int potValue = analogRead(POTENTIOMETER_PIN); // Read the potentiometer value

  // Display the potValue in the serial monitor
  Serial.print("Potentiometer value: ");
  Serial.println(potValue);

  // Map the potentiometer value to volume range (0.0 to 1.0)
  float newVolume = map(potValue, 0, 1023, 0.0, 1.0);

  // Constrain the newVolume to stay within the valid range (0.0 to 1.0)
  newVolume = constrain(newVolume, 0.0, 3.99);

  // Check if the volume has changed
  if (newVolume != currentVolume)
  {
    currentVolume = newVolume;      // Update the current volume
    output->SetGain(currentVolume); // Set the volume

    // Display the volume level in the serial monitor

    Serial.print("Potentiometer value: ");
    Serial.println(potValue);
    Serial.print("New Volume: ");
    Serial.println(newVolume, 2);
    Serial.print("Current Volume: ");
    Serial.println(currentVolume, 2);
  }

  if (!allFilesPlayed)
  {
    if ((decoder) && (decoder->isRunning()))
    {
      if (!decoder->loop())
      {
        decoder->stop();
        Serial.println("Playback finished");
      }
    }
    else
    {
      File file = dir.openNextFile();
      if (file)
      {
        if (String(file.name()).endsWith(".wav"))
        {
          source->close();
          String filePath = "/" + String(file.name()); // Add "/" in front of the file name
          if (source->open(filePath.c_str()))
          {
            Serial.printf("Playing '%s' from SD card...\n", filePath.c_str());
            decoder->begin(source, output);
          }
          else
          {
            Serial.printf("Error opening '%s'\n", filePath.c_str());
          }
        }
      }
      else
      {
        Serial.println("All files played. Replaying...");
        dir.rewindDirectory(); // Reset the directory to replay files
        allFilesPlayed = true;
      }
    }
  }
  else
  {
    // You can add your replay logic here.
    // For example, you can reset 'allFilesPlayed' to false to replay indefinitely,
    // or implement a different replay strategy.
    dir.rewindDirectory();  // Reset the directory to replay files
    allFilesPlayed = false; // Reset the flag
  }
}


*/

//----------------The below code works with amp and all, but pops between songs-------------

/*

#include <Arduino.h>
#include "AudioFileSourceSD.h"
#include "AudioOutputI2S.h"
#include "AudioGeneratorWAV.h"

#define SD_CardSelect 4      // Define the SD card select pin

File dir;
AudioFileSourceSD *source = NULL;
AudioOutputI2S *output = NULL;
AudioGeneratorWAV *decoder = NULL;
bool allFilesPlayed = false;

void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(1000);

  audioLogger = &Serial;
  source = new AudioFileSourceSD();
  output = new AudioOutputI2S(0, 1);
  decoder = new AudioGeneratorWAV();

  Serial.println("Initializing SD card...");
  if (SD.begin(SD_CardSelect)) { // Use the defined SD_CardSelect pin
    Serial.println("SD card initialization successful");
  } else {
    Serial.println("SD card initialization failed");
  }

  dir = SD.open("/");
  
  // Set the gain to 0.5 (50% volume)
  output->SetGain(0.5);
}

void loop() {
  if (!allFilesPlayed) {
    if ((decoder) && (decoder->isRunning())) {
      if (!decoder->loop()) {
        decoder->stop();
        Serial.println("Playback finished");
      }
    } else {
      File file = dir.openNextFile();
      if (file) {
        if (String(file.name()).endsWith(".wav")) {
          source->close();
          String filePath = "/" + String(file.name()); // Add "/" in front of the file name
          if (source->open(filePath.c_str())) {
            //output->SetGain(1.0);
            Serial.printf("Playing '%s' from SD card...\n", filePath.c_str());
            decoder->begin(source, output);
          } else {
            Serial.printf("Error opening '%s'\n", filePath.c_str());
          }
        }
      } else {
        Serial.println("All files played. Replaying...");
        dir.rewindDirectory(); // Reset the directory to replay files
        allFilesPlayed = true;
      }
    }
  } else {
    // You can add your replay logic here.
    // For example, you can reset 'allFilesPlayed' to false to replay indefinitely,
    // or implement a different replay strategy.
    dir.rewindDirectory();  // Reset the directory to replay files
    allFilesPlayed = false; // Reset the flag
  }
}

*/

//------------------------------------------------------------



#include <Arduino.h>
#include "AudioFileSourceSD.h"
#include "AudioOutputULP.h"
#include "AudioGeneratorWAV.h"

#define SD_CardSelect 4      // Define the SD card select pin

File dir;
AudioFileSourceSD *source = NULL;
AudioOutputULP *output = NULL;
AudioGeneratorWAV *decoder = NULL;
bool allFilesPlayed = false;

void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(1000);

  audioLogger = &Serial;
  source = new AudioFileSourceSD();
  output = new AudioOutputULP(1);
  decoder = new AudioGeneratorWAV();

  Serial.println("Initializing SD card...");
  if (SD.begin(SD_CardSelect)) { // Use the defined SD_CardSelect pin
    Serial.println("SD card initialization successful");
  } else {
    Serial.println("SD card initialization failed");
  }

  dir = SD.open("/");
  
  // Set the gain to 0.5 (50% volume)
  output->SetGain(0.5);
}

void loop() {
  if (!allFilesPlayed) {
    if ((decoder) && (decoder->isRunning())) {
      if (!decoder->loop()) {
        decoder->stop();
        Serial.println("Playback finished");
      }
    } else {
      File file = dir.openNextFile();
      if (file) {
        if (String(file.name()).endsWith(".wav")) {
          source->close();
          String filePath = "/" + String(file.name()); // Add "/" in front of the file name
          if (source->open(filePath.c_str())) {
            Serial.printf("Playing '%s' from SD card...\n", filePath.c_str());
            decoder->begin(source, output);
          } else {
            Serial.printf("Error opening '%s'\n", filePath.c_str());
          }
        }
      } else {
        Serial.println("All files played. Replaying...");
        dir.rewindDirectory(); // Reset the directory to replay files
        allFilesPlayed = true;
      }
    }
  } else {
    // You can add your replay logic here.
    // For example, you can reset 'allFilesPlayed' to false to replay indefinitely,
    // or implement a different replay strategy.
    dir.rewindDirectory();  // Reset the directory to replay files
    allFilesPlayed = false; // Reset the flag
  }
}