#include <Arduino.h>      // Setup e loop
#include <CertMeren.h>    //in this file put your certificate

#include "esp_ota_ops.h"    
#include <WiFi.h>
#include <WiFiClient.h>
#include "esp_http_client.h"   //it manages the SSL connectin

#define BUFFSIZE 1024   


static char ota_write_data[BUFFSIZE + 1] = { 0 };

#define URL_SPIFFS "https://www.putYourWebsiteHere.com/fileSystemImage.bin"       //here you must put the file image


static void http_cleanup(esp_http_client_handle_t client)
{
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

static void __attribute__((noreturn)) task_fatal_error(void)
{
    ESP_LOGE(TAG, "Exiting task due to fatal error...");
    (void)vTaskDelete(NULL);

    while (1) {
        ;
    }
}



void connectWiFI(){                                                //quick WiFi connection
  const char* ssid = "PUT YOUR WIFI SSID HERE";
  const char* password = "PUT YOUR WIFI PASSWORD HERE";
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.print("connecting to WiFi");
  }
}

void ota_spiffs_task(void)
{
esp_err_t err;

esp_ota_handle_t update_handle = 0 ;
const esp_partition_t *update_partition = NULL;
esp_partition_t *spiffs_partition=NULL;
ESP_LOGI(TAG, "Starting OTA SPIFFS...");
 
const esp_partition_t *configured = esp_ota_get_boot_partition();
const esp_partition_t *running = esp_ota_get_running_partition();
 
/*Update SPIFFS : 1/ First we need to find SPIFFS partition  */
 
esp_partition_iterator_t spiffs_partition_iterator=esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS,NULL);
while(spiffs_partition_iterator !=NULL){
    spiffs_partition = (esp_partition_t *)esp_partition_get(spiffs_partition_iterator);
    printf("main: partition type = %d.\n", spiffs_partition->type);
    printf("main: partition subtype = %d.\n", spiffs_partition->subtype);
    printf("main: partition starting address = %x.\n", spiffs_partition->address);
    printf("main: partition size = %x.\n", spiffs_partition->size);
    printf("main: partition label = %s.\n", spiffs_partition->label);
    printf("main: partition subtype = %d.\n", spiffs_partition->encrypted);
    printf("\n");
    printf("\n");
    spiffs_partition_iterator=esp_partition_next(spiffs_partition_iterator);
}
vTaskDelay(1000/portTICK_RATE_MS);
esp_partition_iterator_release(spiffs_partition_iterator);
 
/* Wait for the callback to set the CONNECTED_BIT in the
   event group.
*/


if(WiFi.status() == WL_CONNECTED){
  Serial.println("Connect to Wifi ! Start to Connect to Server....");
}
else{
   Serial.println("You are not connected to WiFi. You will get a connection error");
}



esp_http_client_config_t config;
  memset(&config, 0, sizeof(esp_http_client_config_t));
  config.url = URL_SPIFFS;
  config.cert_pem = UPGRADE_SERVER_CERT;


esp_http_client_handle_t client = esp_http_client_init(&config);
if (client == NULL) {
    ESP_LOGE(TAG, "Failed to initialise HTTP connection");
    task_fatal_error();
}


err = esp_http_client_open(client, 0);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    esp_http_client_cleanup(client);
    task_fatal_error();
    Serial.println("fatal error");
}
esp_http_client_fetch_headers(client);
 
/* 2: Delete SPIFFS Partition  */
err=esp_partition_erase_range(spiffs_partition,spiffs_partition->address,spiffs_partition->size);
 
 
int binary_file_length = 0;
int offset=0;
/*deal with all receive packet*/
while (1) {
    int data_read = esp_http_client_read(client, ota_write_data, BUFFSIZE);
    if (data_read < 0) {
        ESP_LOGE(TAG, "Error: SSL data read error");
        http_cleanup(client);
        task_fatal_error();
    } else if (data_read > 0) {
        /* 3 : WRITE SPIFFS PARTITION */
        err = esp_partition_write(spiffs_partition,offset,(const void *)ota_write_data, data_read);
 
        if (err != ESP_OK) {
            http_cleanup(client);
            task_fatal_error();
        }
        offset=offset+1024;
        
        binary_file_length += data_read;
        ESP_LOGD(TAG, "Written image length %d", binary_file_length);
        Serial.println("Writing..");
    } else if (data_read == 0) {
        ESP_LOGI(TAG, "Connection closed,all data received");
        Serial.println("Finished to Write");
        break;
    }
}
ESP_LOGI(TAG, "Total Write binary data length : %d", binary_file_length);
 
ESP_LOGI(TAG, "restart!");


//esp_restart();                //you can restart if you want
}


void setup() {
  Serial.begin(115200);
  connectWiFI();
  ota_spiffs_task();
}

void loop() {
  // put your main code here, to run repeatedly:

}
