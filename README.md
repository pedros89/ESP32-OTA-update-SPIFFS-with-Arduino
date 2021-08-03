# ESP32 OTA update SPIFFS with Arduino IDE
Use Arduino IDE to upload a file system image (SPIFFS) on your ESP32 from your online https webserver

1) FIRST STEP: GENERATE IMAGE

In my opinion the best way to generate spiffs images for ESP32 is to use Visual Code with the plug in PlatformIO.

See this tutorial to set everything up
https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/

Look at this picture to see where to click to generate the image.
It is super quick and you can find the newly generated .bin image in
C:\Users\YOUR_NAME\Documents\PlatformIO\Projects\YOUR_PROJECT_NAME\\.pio\build\esp32doit-devkit-v1\spiffs.bin


![alt text](https://community.mongoose-os.com/uploads/default/original/1X/80a8ea4e793d19d2eec82779fb60aa747a097220.png)

2) SECOND STEP: PUT THE GENERATED IMAGE ON YOUR WEBSITE FTP SERVER
3) GET YOUR SERVER CERTIFICATE  [Link to how to download certificate](https://www.esri.com/arcgis-blog/products/bus-analyst/field-mobility/learn-how-to-download-a-ssl-certificate-for-a-secured-portal/)
5) THIRD STEP: INSTALL THE REQUIRED LIBRARIES
All the .h file missing can be get from here 
https://github.com/espressif/esp-idf

4) FOURTH STEP: UPLOAD THE .ino CODE TO YOUR ESP32

THIS GUIDE IS ONLY TO UPDATE THE SPIFFS .bin OF ESP32, NOT ESP32 FIRMWARE ON THE APP PARTITION. IF YOU NEED TO UPDATE THE ESP32 FIRMWARE VIA OTA WITH ARDUINO use this code:
https://github.com/espressif/arduino-esp32/tree/master/libraries/Update/examples/HTTPS_OTA_Update
