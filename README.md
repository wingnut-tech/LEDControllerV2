# FT Night Radian LED Controller

BMP280 library from <https://github.com/adafruit/Adafruit_BMP280_Library> (v2.3.0)  
FastLED library from <https://github.com/FastLED/FastLED> (v3.4.0)  

## Installation
The easiest way to install this firmware is to use the [LEDController Updater](https://github.com/reyemxela/LEDControllerUpdater) app.  
Just download the latest [release](https://github.com/reyemxela/LEDControllerUpdater/releases).

## Manual installation
- Install the latest version of the [Arduino IDE](https://www.arduino.cc/en/main/software)
- Install the above specified versions of the **Adafruit BMP280** and **FastLED** libraries using the Library Manager in the Arduino IDE
- On Windows, you might need to install the [CH340G drivers](https://github.com/reyemxela/LEDControllerUpdater/releases/download/v1.0.0/CH34x_Install_Windows_v3_4.zip) for the Arduino Nano
- Download/clone this repository, unzip if needed
  - If you downloaded the zip file from Github, you'll probably need to rename the folder. It should be named "LEDController" to match the .ino file.
- Connect the LED Controller to your computer with a USB cable
- In the Arduino IDE, make sure the Board Type is set to "Arduino Nano" and the Port is set to the correct serial port
- Depending on the controller board you have, the "Processor" selection can vary:
  - New v2 boards and some older boards: **"Atmega328P"**
  - Most older boards: **"Atmega328P (Old Bootloader)"**
  - If you have an older board and it won't flash, try the other processor type
- Upload the code to the Arduino!
