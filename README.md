# ESP32 WAV (MP3 to come later) Basic Player From SD Card
ESP32 WAV (MP3 to come later) Basic Player From SD Card

Using the ESP8266Audio Library (which is also for ESP32) which can be found at https://github.com/earlephilhower/ESP8266Audio.


## Current Error:
```Linking .pio\build\esp32dev\firmware.elf
c:/users/echo/.platformio/packages/toolchain-xtensa-esp32@8.4.0+2021r2-patch5/bin/../lib/gcc/xtensa-esp32-elf/8.4.0/../../../../xtensa-esp32-elf/bin/ld.exe: .pio\build\esp32dev\src\main.cpp.o:(.literal._Z5setupv+0x1c): undefined reference to `vtable for AudioOutputULP'
collect2.exe: error: ld returned 1 exit status
*** [.pio\build\esp32dev\firmware.elf] Error 1```