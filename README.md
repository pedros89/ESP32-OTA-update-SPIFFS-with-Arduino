# ESP32 OTA update SPIFFS with Arduino IDE
Use Arduino IDE to upload a file system image (SPIFFS) on your ESP32 from your online https webserver

1) FIRST STEP: GENERATE IMAGE

The best and quicker way to generate a Spiffs image is to use Ardino IDE, just open your sketch folder and find the folder called data.  Inside data put all the file you need in SPIFFS (e.g. .png images, .css stylesheet, ...)

Make shure you select the right board and you select the partition type you want to work with, in my case as you see in the picture I am working with ESP with 16MB of flash so I selected SparkFun ESP32 Thing Plus as board becasue it has 16MB of flash and I am selecting default partition scheme
![image](https://user-images.githubusercontent.com/30262131/172793718-19beb1cc-f8c9-42ed-8084-74751ff0299f.png)

If you press ESP sketch data Upload a spiffs image will automatically be generated for you according to the selection of the partition you selected above.
C:\Users\YOUR_NAME_HERE\AppData\Local\Temp\arduino_build_526090/YOUR_PROJECT_NAME.spiffs.bin

![image](https://user-images.githubusercontent.com/30262131/172792737-cf27a1d5-5917-4f6c-b5ad-76b4a9403a99.png)

![image](https://user-images.githubusercontent.com/30262131/172794434-e064488a-afc3-44fd-b73f-5d580024031e.png)


Another good way to generate spiffs images for ESP32 is to use Visual Code with the plug in PlatformIO.

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
https://github.com/pedros89/ESP32-update-both-OTA-Firmware-and-OTA-SPIFFS-with-Arduino
You can also use the Arduino ESP32 core library Update but I found that it is not always working, on some websites with redirect it give the error file not found. So I suggest you to use the above link. You can try the following example if you like though.
https://github.com/espressif/arduino-esp32/tree/master/libraries/Update/examples/HTTPS_OTA_Update

5) ENABLE DATALOGGER: to enable ESP datalogger settings → code debug level → verbose
 https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/ArduinoBLE.md#switching-on-debugging

6) BEFORE OTA UPDATE SPIFFS FORMAT SPIFFS: make sure that when you implement the OTA spiffs upload funciton in your code if you implement the "SPIFFS.h" in your sketch make sure you format spiffs before installing the image otherwise it won't work

```
 #include "SPIFFS.h"

 if(!SPIFFS.begin(true)){    //you have to include this funciton for sure if you want to access spiffs files
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }   

  SPIFFS.format();           //make sure you run this function before the OTA spiffs update task in your code
  ota_spiffs_task();
```

    
