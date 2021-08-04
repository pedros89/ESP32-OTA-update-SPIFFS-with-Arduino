# ESP32 OTA update SPIFFS with Arduino IDE
Use Arduino IDE to upload a file system image (SPIFFS) on your ESP32 from your online https webserver

1) FIRST STEP: GENERATE IMAGE

In my opinion the best way to generate spiffs images for ESP32 is to use Visual Code with the plug in PlatformIO.

See this tutorial to set everything up
https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/
In this tutorial they use the most common "ESP32 Dev module" board with partition scheme default 4MB (1.2MB APP  1.5MB SPIFFS).

Look at this picture to see where to click to generate the image.
It is super quick and you can find the newly generated .bin image in
C:\Users\YOUR_NAME\Documents\PlatformIO\Projects\YOUR_PROJECT_NAME\\.pio\build\esp32doit-devkit-v1\spiffs.bin

![alt text](https://community.mongoose-os.com/uploads/default/original/1X/80a8ea4e793d19d2eec82779fb60aa747a097220.png)

Remember that the spiffs image size is fixed according to the size of the partition of your board, if you have a board with an ESP32 with bigger memory and you want to assign more memory to spiffs partition e.g. "SpurkFun ESP32 Thing plus" you have to select the right board from the PlatformIO to make a suitable spiffs image for that board. If the spiffs image and spiffs partition size do not match they will give you an error when you try to mount that partition with SPIFFS.begin();

2) SECOND STEP: PUT THE GENERATED IMAGE ON YOUR WEBSITE FTP SERVER
3) GET YOUR SERVER CERTIFICATE 
Substitute the certificate with the one of your website in the file CertMern.h
[Link to how to download certificate](https://www.esri.com/arcgis-blog/products/bus-analyst/field-mobility/learn-how-to-download-a-ssl-certificate-for-a-secured-portal/)

6) THIRD STEP: INSTALL THE REQUIRED LIBRARIES
All the .h file missing can be get from here 
https://github.com/espressif/esp-idf

4) FOURTH STEP: UPLOAD THE .ino CODE TO YOUR ESP32

THIS GUIDE IS ONLY TO UPDATE THE SPIFFS .bin OF ESP32, NOT ESP32 FIRMWARE ON THE APP PARTITION. IF YOU NEED TO UPDATE THE ESP32 FIRMWARE VIA OTA WITH ARDUINO use this code:
https://github.com/espressif/arduino-esp32/tree/master/libraries/Update/examples/HTTPS_OTA_Update

5) to enable ESP datalogger settings → code debug level → verbose
 https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/ArduinoBLE.md#switching-on-debugging

6) make sure that when you implement the OTA SPIFFS Upload funciton in your code if you implement the "SPIFFS.h" in your sketch make sure you format spiffs before installing the image otherwise it won't work

```
 #include "SPIFFS.h"

     if(!SPIFFS.begin(true)){    //you have to include this funciton for sure if you want to access spiffs files
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
      }   
      
      SPIFFS.format();           //make sure you run this function before the OTA spiffs update task in your code
      ota_spiffs_task();
```

    
