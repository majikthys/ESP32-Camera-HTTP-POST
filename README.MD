ESP32 Camera Ardunio Libarry
====================
Each example is tested using the TTGO T-Journal.

![jpg](./images/TTGOcam.jpg)

The example contains three programs:
- Upload a photo to the BIGIOT platform every 30 seconds
- WiFi Soft-AP mode video stream 
- WiFi Station mode video stream

In order to use the Station mode for video transmission quality, it should be as close as possible to the router, and the Soft-AP mode should be as close as possible to the device.

This library is based on [esp32-cam-demo](https://github.com/igrr/esp32-cam-demo) changes and is used in Arduino.

# Installing
To install this library:
- Install it using the Arduino Library manager ("Sketch" -> "Include Library" -> "Manage Libraries..."), or
- Download a zipfile from github using the "Download ZIP" button and install it using the IDE ("Sketch" -> "Include Library" -> "Add .ZIP Library..."
- Clone this git repository into your sketchbook/libraries folder.

For more info, see https://www.arduino.cc/en/Guide/Libraries

 # How to use
- Change the WiFi ssid and password in the program.
- In Sota-AP you should connect `TTGO-CAMERA` and password is "`12345678`" , enter `192.168.4.1` in the browser to access the video stream.
- In the Station mode, If you use OLED ,you should enter the IP address displayed in the OLED for access. You can also enter the IP address in the log to access